#include "../include/file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Maximum line length for dictionary and text files
 */
#define MAX_LINE_LENGTH 1024

bool load_dictionary(const char* filename, Trie* trie) {
    if (filename == NULL || trie == NULL) {
        fprintf(stderr, "Error: Invalid parameters - filename or trie is NULL\n");
        return false;
    }
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open dictionary file '%s'\n", filename);
        fprintf(stderr, "Suggestion: Check that the file exists and you have read permissions\n");
        return false;
    }
    
    char line[MAX_LINE_LENGTH];
    int words_loaded = 0;
    int line_number = 0;
    int corrupted_lines = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        
        // Check for line length overflow (corrupted file indicator)
        size_t len = strlen(line);
        if (len == MAX_LINE_LENGTH - 1 && line[len - 1] != '\n') {
            fprintf(stderr, "Warning: Line %d exceeds maximum length, truncating\n", line_number);
            corrupted_lines++;
            // Skip rest of this line
            int c;
            while ((c = fgetc(file)) != EOF && c != '\n');
        }
        
        // Remove newline character if present
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        
        // Skip empty lines
        if (len == 0) {
            continue;
        }
        
        // Check for non-printable characters (corrupted file indicator)
        bool has_invalid_chars = false;
        for (size_t i = 0; i < len; i++) {
            if (!isprint(line[i]) && !isspace(line[i])) {
                has_invalid_chars = true;
                break;
            }
        }
        
        if (has_invalid_chars) {
            fprintf(stderr, "Warning: Line %d contains invalid characters, skipping\n", line_number);
            corrupted_lines++;
            continue;
        }
        
        // Normalize the word (convert to lowercase, validate characters)
        char* normalized = normalize_word(line);
        if (normalized != NULL && is_valid_word(normalized)) {
            if (trie_insert(trie, normalized)) {
                words_loaded++;
            } else {
                fprintf(stderr, "Warning: Failed to insert word '%s' at line %d (possible memory issue)\n", 
                       normalized, line_number);
            }
            free(normalized);
        } else {
            // Skip invalid words but don't treat as error
            if (normalized != NULL) {
                free(normalized);
            }
        }
    }
    
    // Check for file read errors
    if (ferror(file)) {
        fprintf(stderr, "Error: File read error occurred while processing '%s'\n", filename);
        fprintf(stderr, "Suggestion: Check file integrity and disk space\n");
        fclose(file);
        return words_loaded > 0; // Return success if we loaded some words
    }
    
    fclose(file);
    
    if (corrupted_lines > 0) {
        printf("Warning: Skipped %d corrupted/invalid lines in dictionary '%s'\n", 
               corrupted_lines, filename);
    }
    
    if (words_loaded == 0) {
        fprintf(stderr, "Error: No valid words loaded from dictionary '%s'\n", filename);
        fprintf(stderr, "Suggestion: Check file format - dictionary should contain one word per line\n");
        return false;
    }
    
    printf("Successfully loaded %d words from dictionary '%s'\n", words_loaded, filename);
    return true;
}

char* normalize_word(const char* word) {
    if (word == NULL) {
        return NULL;
    }
    
    size_t len = strlen(word);
    if (len == 0) {
        return NULL;
    }
    
    // Check for extremely long words (potential corruption)
    if (len > 100) {
        return NULL;
    }
    
    // Allocate memory for normalized word (worst case: same length)
    char* normalized = (char*)malloc((len + 1) * sizeof(char));
    if (normalized == NULL) {
        return NULL;
    }
    
    size_t write_pos = 0;
    
    // Process each character
    for (size_t i = 0; i < len; i++) {
        char c = word[i];
        
        // Convert to lowercase if alphabetic
        if (isalpha(c)) {
            normalized[write_pos++] = tolower(c);
        }
        // Skip punctuation and other non-alphabetic characters
    }
    
    // Null terminate
    normalized[write_pos] = '\0';
    
    // If no valid characters found, return NULL
    if (write_pos == 0) {
        free(normalized);
        return NULL;
    }
    
    return normalized;
}

bool is_valid_word(const char* word) {
    if (word == NULL) {
        return false;
    }
    
    size_t len = strlen(word);
    if (len == 0) {
        return false;
    }
    
    // Check if word contains only alphabetic characters
    for (size_t i = 0; i < len; i++) {
        if (!isalpha((unsigned char)word[i])) {
            return false;
        }
    }
    
    // Reject very short words (single characters) and very long words
    if (len < 2 || len > 50) {
        return false;
    }
    
    // Additional validation: reject words with repeated characters (potential corruption)
    int consecutive_count = 1;
    for (size_t i = 1; i < len; i++) {
        if (word[i] == word[i-1]) {
            consecutive_count++;
            if (consecutive_count > 3) {
                return false; // Reject words like "aaaaaaa"
            }
        } else {
            consecutive_count = 1;
        }
    }
    
    return true;
}

TextDocument* load_text_file(const char* filename) {
    if (filename == NULL) {
        fprintf(stderr, "Error: Invalid parameter - filename is NULL\n");
        return NULL;
    }
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open text file '%s'\n", filename);
        fprintf(stderr, "Suggestion: Check that the file exists and you have read permissions\n");
        return NULL;
    }
    
    // Allocate TextDocument structure
    TextDocument* doc = (TextDocument*)malloc(sizeof(TextDocument));
    if (doc == NULL) {
        fclose(file);
        return NULL;
    }
    
    // Initialize document
    doc->tokens = NULL;
    doc->token_count = 0;
    doc->total_words = 0;
    doc->filename = (char*)malloc((strlen(filename) + 1) * sizeof(char));
    if (doc->filename == NULL) {
        free(doc);
        fclose(file);
        return NULL;
    }
    strcpy(doc->filename, filename);
    
    // Initial capacity for tokens array
    int capacity = 1000;
    doc->tokens = (TextToken*)malloc(capacity * sizeof(TextToken));
    if (doc->tokens == NULL) {
        free(doc->filename);
        free(doc);
        fclose(file);
        return NULL;
    }
    
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int corrupted_lines = 0;
    int memory_failures = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        
        // Check for line length overflow (corrupted file indicator)
        size_t len = strlen(line);
        if (len == MAX_LINE_LENGTH - 1 && line[len - 1] != '\n') {
            fprintf(stderr, "Warning: Line %d exceeds maximum length, truncating\n", line_number);
            corrupted_lines++;
            // Skip rest of this line
            int c;
            while ((c = fgetc(file)) != EOF && c != '\n');
        }
        
        // Remove newline character if present
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        
        // Check for non-printable characters (corrupted file indicator)
        bool has_invalid_chars = false;
        for (size_t i = 0; i < len; i++) {
            if (!isprint(line[i]) && !isspace(line[i])) {
                has_invalid_chars = true;
                break;
            }
        }
        
        if (has_invalid_chars) {
            fprintf(stderr, "Warning: Line %d contains invalid characters, skipping\n", line_number);
            corrupted_lines++;
            continue;
        }
        
        // Tokenize the line
        char* line_copy = (char*)malloc((len + 1) * sizeof(char));
        if (line_copy == NULL) {
            fprintf(stderr, "Warning: Memory allocation failed for line %d, skipping\n", line_number);
            memory_failures++;
            continue; // Skip this line on memory allocation failure
        }
        strcpy(line_copy, line);
        
        // Simple tokenization by whitespace and punctuation
        char* token_start = line_copy;
        int position = 0;
        
        for (size_t i = 0; i <= len; i++) {
            char c = (i < len) ? line_copy[i] : '\0';
            
            // Check if we've reached end of a token
            if (isspace(c) || ispunct(c) || c == '\0') {
                if (token_start < line_copy + i) {
                    // Extract token
                    char original_char = line_copy[i];
                    line_copy[i] = '\0'; // Temporarily null-terminate
                    
                    // Create normalized version
                    char* normalized = normalize_word(token_start);
                    
                    if (normalized != NULL && is_valid_word(normalized)) {
                        // Resize tokens array if needed
                        if (doc->token_count >= capacity) {
                            capacity *= 2;
                            TextToken* new_tokens = (TextToken*)realloc(doc->tokens, 
                                                                      capacity * sizeof(TextToken));
                            if (new_tokens == NULL) {
                                fprintf(stderr, "Warning: Memory allocation failed, stopping at line %d\n", line_number);
                                memory_failures++;
                                free(normalized);
                                break; // Stop processing on memory allocation failure
                            }
                            doc->tokens = new_tokens;
                        }
                        
                        // Store token
                        TextToken* token = &doc->tokens[doc->token_count];
                        token->word = normalized;
                        
                        // Store original word
                        size_t orig_len = strlen(token_start);
                        token->original_word = (char*)malloc((orig_len + 1) * sizeof(char));
                        if (token->original_word != NULL) {
                            strcpy(token->original_word, token_start);
                        }
                        
                        token->line_number = line_number;
                        token->position = position;
                        
                        doc->token_count++;
                        doc->total_words++;
                    } else {
                        if (normalized != NULL) {
                            free(normalized);
                        }
                    }
                    
                    line_copy[i] = original_char; // Restore character
                }
                
                // Move to next potential token start
                if (i < len) {
                    while (i < len && (isspace(line_copy[i]) || ispunct(line_copy[i]))) {
                        i++;
                        position++;
                    }
                    if (i < len) {
                        token_start = line_copy + i;
                        position = i;
                        i--; // Adjust for loop increment
                    }
                }
            }
        }
        
        free(line_copy);
    }
    
    // Check for file read errors
    if (ferror(file)) {
        fprintf(stderr, "Error: File read error occurred while processing '%s'\n", filename);
        fprintf(stderr, "Suggestion: Check file integrity and disk space\n");
        // Continue with partial results rather than failing completely
    }
    
    fclose(file);
    
    // Report processing summary
    if (corrupted_lines > 0) {
        printf("Warning: Skipped %d corrupted/invalid lines in text file '%s'\n", 
               corrupted_lines, filename);
    }
    
    if (memory_failures > 0) {
        printf("Warning: %d memory allocation failures occurred during processing\n", 
               memory_failures);
    }
    
    if (doc->token_count == 0) {
        fprintf(stderr, "Warning: No valid tokens found in file '%s'\n", filename);
        fprintf(stderr, "Suggestion: Check file format and content\n");
    } else {
        printf("Successfully loaded %d tokens from text file '%s'\n", 
               doc->token_count, filename);
    }
    
    return doc;
}

void free_text_document(TextDocument* doc) {
    if (doc == NULL) {
        return;
    }
    
    // Free all tokens
    if (doc->tokens != NULL) {
        for (int i = 0; i < doc->token_count; i++) {
            if (doc->tokens[i].word != NULL) {
                free(doc->tokens[i].word);
            }
            if (doc->tokens[i].original_word != NULL) {
                free(doc->tokens[i].original_word);
            }
        }
        free(doc->tokens);
    }
    
    // Free filename
    if (doc->filename != NULL) {
        free(doc->filename);
    }
    
    // Free document structure
    free(doc);
}