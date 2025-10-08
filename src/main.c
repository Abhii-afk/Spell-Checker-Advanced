#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/trie.h"
#include "../include/file_io.h"
#include "../include/spell_check.h"

/**
 * Display usage information
 */
void print_usage(const char* program_name) {
    printf("Simple Spell Checker\n");
    printf("Usage: %s <dictionary_file> <input_file>\n", program_name);
    printf("\nArguments:\n");
    printf("  dictionary_file  Path to dictionary file (one word per line)\n");
    printf("  input_file       Path to text file to spell check\n");
}

/**
 * Load dictionary and show progress
 */
bool load_dictionary_with_progress(const char* filename, Trie* trie) {
    if (load_dictionary(filename, trie)) {
        printf("Dictionary loaded: %d words\n", trie->total_words);
        return true;
    } else {
        printf("Error: Cannot open dictionary file '%s'\n", filename);
        return false;
    }
}

/**
 * Print spell check results
 */
void print_results(SpellCheckResult* result) {
    if (!result) {
        printf("No results to display\n");
        return;
    }
    
    printf("\n=== SPELL CHECK RESULTS ===\n");
    printf("Total words checked: %d\n", result->total_words_checked);
    printf("Errors found: %d\n", result->error_count);
    
    if (result->error_count == 0) {
        printf("No spelling errors found!\n");
        return;
    }
    
    printf("\nErrors:\n");
    for (int i = 0; i < result->error_count; i++) {
        SpellError* error = &result->errors[i];
        printf("%d. Line %d: '%s' (original: '%s')\n", 
               i + 1, error->line_number, error->misspelled_word, error->original_word);
        
        if (error->suggestion_count > 0) {
            printf("   Suggestions: ");
            for (int j = 0; j < error->suggestion_count; j++) {
                printf("%s", error->suggestions[j]);
                if (j < error->suggestion_count - 1) printf(", ");
            }
            printf("\n");
        } else {
            printf("   No suggestions available\n");
        }
    }
    
    // Calculate accuracy
    double accuracy = 100.0;
    if (result->total_words_checked > 0) {
        accuracy = 100.0 - ((double)result->error_count / result->total_words_checked * 100.0);
    }
    printf("\nAccuracy: %.1f%%\n", accuracy);
}

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char* dictionary_file = argv[1];
    const char* input_file = argv[2];
    
    printf("Simple Spell Checker Starting...\n");
    printf("Dictionary: %s\n", dictionary_file);
    printf("Input file: %s\n", input_file);
    printf("\n");
    
    // Create and load dictionary
    Trie* dictionary = trie_create();
    if (!dictionary) {
        printf("Error: Failed to create dictionary\n");
        return 1;
    }
    
    if (!load_dictionary_with_progress(dictionary_file, dictionary)) {
        trie_destroy(dictionary);
        return 1;
    }
    
    // Load and process input file
    printf("Loading input file...\n");
    TextDocument* document = load_text_file(input_file);
    if (!document) {
        printf("Error: Failed to load input file '%s'\n", input_file);
        trie_destroy(dictionary);
        return 1;
    }
    
    printf("Input file loaded: %d tokens, %d words\n", 
           document->token_count, document->total_words);
    
    // Perform spell checking
    printf("Performing spell check...\n");
    SpellCheckResult* result = spell_check_document(document, dictionary);
    if (!result) {
        printf("Error: Spell checking failed\n");
        free_text_document(document);
        trie_destroy(dictionary);
        return 1;
    }
    
    // Display results
    print_results(result);
    
    // Cleanup
    free_spell_check_result(result);
    free_text_document(document);
    trie_destroy(dictionary);
    
    printf("\nSpell check complete!\n");
    return 0;
}