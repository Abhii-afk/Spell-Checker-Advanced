#include "spell_check.h"
#include "edit_distance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Safe string duplication function
 */
static char* safe_strdup(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}

/**
 * Helper function to check if a character is alphabetic
 */
static bool is_alpha_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * Helper function to check if a word contains only alphabetic characters
 */
static bool is_alphabetic_word(const char* word) {
    if (!word || strlen(word) == 0) {
        return false;
    }
    
    for (int i = 0; word[i] != '\0'; i++) {
        if (!is_alpha_char(word[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Check if a word is likely a proper noun (simple heuristic)
 */
bool is_likely_proper_noun(const char* word, const TextToken* token) {
    if (!word || !token || !token->original_word) {
        return false;
    }
    
    // If original word starts with uppercase and rest is lowercase
    if (isupper(token->original_word[0])) {
        // Skip if it's at the beginning of a sentence
        if (token->position == 0) {
            return false;
        }
        return true;
    }
    
    return false;
}

/**
 * Generate suggestions using edit distance
 */
char** generate_suggestions(const char* misspelled_word, Trie* dictionary, int* count) {
    if (!misspelled_word || !dictionary || !count) {
        return NULL;
    }
    
    *count = 0;
    
    // Get all words from dictionary
    char** all_words;
    int total_words;
    trie_get_all_words(dictionary, &all_words, &total_words);
    
    if (!all_words || total_words == 0) {
        return NULL;
    }
    
    // Structure to hold word-distance pairs for sorting
    typedef struct {
        char* word;
        int distance;
    } WordDistance;
    
    WordDistance* candidates = malloc(total_words * sizeof(WordDistance));
    if (!candidates) {
        // Free all_words
        for (int i = 0; i < total_words; i++) {
            free(all_words[i]);
        }
        free(all_words);
        return NULL;
    }
    
    int candidate_count = 0;
    
    // Find words with edit distance <= 2
    for (int i = 0; i < total_words; i++) {
        int distance = compute_edit_distance(misspelled_word, all_words[i]);
        if (distance <= 2 && distance > 0) { // distance > 0 to exclude exact matches
            candidates[candidate_count].word = safe_strdup(all_words[i]);
            candidates[candidate_count].distance = distance;
            candidate_count++;
        }
    }
    
    // Free all_words as we've copied what we need
    for (int i = 0; i < total_words; i++) {
        free(all_words[i]);
    }
    free(all_words);
    
    if (candidate_count == 0) {
        free(candidates);
        return NULL;
    }
    
    // Sort candidates by edit distance (bubble sort for simplicity)
    for (int i = 0; i < candidate_count - 1; i++) {
        for (int j = 0; j < candidate_count - i - 1; j++) {
            if (candidates[j].distance > candidates[j + 1].distance) {
                WordDistance temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }
    
    // Return top 5 suggestions
    int max_suggestions = (candidate_count < 5) ? candidate_count : 5;
    char** suggestions = malloc(max_suggestions * sizeof(char*));
    if (!suggestions) {
        // Free candidates
        for (int i = 0; i < candidate_count; i++) {
            free(candidates[i].word);
        }
        free(candidates);
        return NULL;
    }
    
    for (int i = 0; i < max_suggestions; i++) {
        suggestions[i] = candidates[i].word; // Transfer ownership
    }
    
    // Free remaining candidates
    for (int i = max_suggestions; i < candidate_count; i++) {
        free(candidates[i].word);
    }
    free(candidates);
    
    *count = max_suggestions;
    return suggestions;
}

/**
 * Simple spell checking function
 */
SpellCheckResult* spell_check_document(TextDocument* doc, Trie* dictionary) {
    if (!doc || !dictionary) {
        return NULL;
    }
    
    // Allocate result structure
    SpellCheckResult* result = malloc(sizeof(SpellCheckResult));
    if (!result) {
        return NULL;
    }
    
    // Initialize result
    result->errors = NULL;
    result->error_count = 0;
    result->total_words_checked = 0;
    result->processing_time = 0.0;
    result->memory_used = 0;
    
    // Allocate initial error array
    int max_errors = 100;
    result->errors = malloc(max_errors * sizeof(SpellError));
    if (!result->errors) {
        free(result);
        return NULL;
    }
    
    // Process each token in the document
    for (int i = 0; i < doc->token_count; i++) {
        TextToken* token = &doc->tokens[i];
        
        // Skip invalid words
        if (!is_valid_word(token->word) || !is_alphabetic_word(token->word)) {
            continue;
        }
        
        result->total_words_checked++;
        
        // Check if word is in dictionary
        if (!trie_search(dictionary, token->word)) {
            // Check if it's likely a proper noun (skip if so)
            if (is_likely_proper_noun(token->word, token)) {
                continue;
            }
            
            // Expand error array if needed
            if (result->error_count >= max_errors) {
                max_errors *= 2;
                SpellError* new_errors = realloc(result->errors, max_errors * sizeof(SpellError));
                if (!new_errors) {
                    break; // Memory allocation failed, return partial results
                }
                result->errors = new_errors;
            }
            
            // Create new error entry
            SpellError* error = &result->errors[result->error_count];
            
            // Copy word information
            error->misspelled_word = safe_strdup(token->word);
            error->original_word = safe_strdup(token->original_word);
            error->line_number = token->line_number;
            error->position = token->position;
            
            // Generate suggestions
            error->suggestions = generate_suggestions(token->word, dictionary, &error->suggestion_count);
            
            // Simple scoring (if suggestions exist)
            if (error->suggestion_count > 0) {
                error->suggestion_scores = malloc(error->suggestion_count * sizeof(float));
                if (error->suggestion_scores) {
                    for (int j = 0; j < error->suggestion_count; j++) {
                        error->suggestion_scores[j] = (float)j; // Simple scoring
                    }
                }
            } else {
                error->suggestion_scores = NULL;
            }
            
            result->error_count++;
        }
    }
    
    return result;
}

/**
 * Free all memory associated with a SpellCheckResult
 */
void free_spell_check_result(SpellCheckResult* result) {
    if (!result) {
        return;
    }
    
    // Free each error and its associated data
    for (int i = 0; i < result->error_count; i++) {
        SpellError* error = &result->errors[i];
        
        if (error->misspelled_word) {
            free(error->misspelled_word);
        }
        
        if (error->original_word) {
            free(error->original_word);
        }
        
        if (error->suggestions) {
            for (int j = 0; j < error->suggestion_count; j++) {
                if (error->suggestions[j]) {
                    free(error->suggestions[j]);
                }
            }
            free(error->suggestions);
        }
        
        if (error->suggestion_scores) {
            free(error->suggestion_scores);
        }
    }
    
    // Free errors array
    if (result->errors) {
        free(result->errors);
    }
    
    // Free result structure
    free(result);
}