#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/trie.h"
#include "../include/file_io.h"
#include "../include/spell_check.h"
#include "../include/api_client.h"

/**
 * Display usage information
 */
void print_usage(const char* program_name) {
    printf("Advanced Spell Checker with API Integration\n");
    printf("Usage: %s [OPTIONS] <dictionary_file> <input_file>\n", program_name);
    printf("\nArguments:\n");
    printf("  dictionary_file  Path to dictionary file (one word per line)\n");
    printf("  input_file       Path to text file to spell check\n");
    printf("\nOptions:\n");
    printf("  --api-key KEY    Enable Merriam-Webster API with your API key\n");
    printf("  --api-stats      Show API statistics after spell checking\n");
    printf("  -h, --help       Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s dict.txt input.txt\n", program_name);
    printf("  %s --api-key YOUR_KEY dict.txt input.txt\n", program_name);
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
    // Parse command line arguments
    const char* api_key = NULL;
    bool show_api_stats = false;
    const char* dictionary_file = NULL;
    const char* input_file = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--api-key") == 0 && i + 1 < argc) {
            api_key = argv[++i];
        } else if (strcmp(argv[i], "--api-stats") == 0) {
            show_api_stats = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (!dictionary_file) {
            dictionary_file = argv[i];
        } else if (!input_file) {
            input_file = argv[i];
        }
    }
    
    // Validate required arguments
    if (!dictionary_file || !input_file) {
        fprintf(stderr, "Error: Missing required arguments\n\n");
        print_usage(argv[0]);
        return 1;
    }
    
    printf("🔍 Advanced Spell Checker Starting...\n");
    printf("Dictionary: %s\n", dictionary_file);
    printf("Input file: %s\n", input_file);
    
    // Initialize API if key provided
    if (api_key) {
        printf("API Key: %s\n", api_key);
        if (api_client_init(api_key)) {
            printf("✅ Merriam-Webster API enabled\n");
        } else {
            fprintf(stderr, "⚠️  Failed to initialize API, continuing with local dictionary only\n");
        }
    } else {
        printf("ℹ️  API not enabled (use --api-key to enable)\n");
    }
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
    
    // Show API statistics if requested
    if (show_api_stats && is_api_initialized()) {
        print_api_stats();
    }
    
    // Cleanup
    free_spell_check_result(result);
    free_text_document(document);
    trie_destroy(dictionary);
    
    // Cleanup API if initialized
    if (is_api_initialized()) {
        api_client_cleanup();
    }
    
    printf("\n✅ Spell check complete!\n");
    return 0;
}