/**
 * @file test_api.c
 * @brief Test program for Merriam-Webster API integration
 * 
 * This program demonstrates the API client functionality
 * and tests various scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include "include/api_client.h"

/**
 * Test basic API functionality
 */
void test_basic_api(void) {
    printf("\n=== Test 1: Basic API Functionality ===\n");
    
    const char* test_words[] = {
        "hello",      // Common word (should be found)
        "computer",   // Technical word (should be found)
        "xyzabc",     // Nonsense word (should not be found)
        "algorithm",  // Technical word (should be found)
        "wrld"        // Misspelled word (should not be found)
    };
    
    int num_words = sizeof(test_words) / sizeof(test_words[0]);
    
    for (int i = 0; i < num_words; i++) {
        printf("\nTesting word: '%s'\n", test_words[i]);
        int result = fetch_from_api(test_words[i]);
        
        switch (result) {
            case 1:
                printf("  ✅ Word found in dictionary\n");
                break;
            case 0:
                printf("  ❌ Word not found in dictionary\n");
                break;
            case -1:
                printf("  ⚠️  API error occurred\n");
                break;
        }
    }
}

/**
 * Test detailed word information
 */
void test_detailed_info(void) {
    printf("\n\n=== Test 2: Detailed Word Information ===\n");
    
    const char* word = "computer";
    printf("\nFetching detailed information for: '%s'\n", word);
    
    APIResponse* response = fetch_word_details(word);
    if (response) {
        printf("\nResults:\n");
        printf("  Word Found: %s\n", response->word_found ? "Yes" : "No");
        printf("  HTTP Status: %d\n", response->http_status);
        printf("  Response Time: %d ms\n", response->response_time_ms);
        
        if (response->definition) {
            printf("  Definition: %s\n", response->definition);
        }
        
        if (response->error_message) {
            printf("  Error: %s\n", response->error_message);
        }
        
        free_api_response(response);
    } else {
        printf("  ⚠️  Failed to fetch word details\n");
    }
}

/**
 * Test error handling
 */
void test_error_handling(void) {
    printf("\n\n=== Test 3: Error Handling ===\n");
    
    // Test with empty string
    printf("\nTesting with empty string:\n");
    int result = fetch_from_api("");
    printf("  Result: %d (expected: -1)\n", result);
    
    // Test with NULL
    printf("\nTesting with NULL:\n");
    result = fetch_from_api(NULL);
    printf("  Result: %d (expected: -1)\n", result);
    
    // Test with very long word
    printf("\nTesting with very long word:\n");
    char long_word[1000];
    for (int i = 0; i < 999; i++) {
        long_word[i] = 'a';
    }
    long_word[999] = '\0';
    result = fetch_from_api(long_word);
    printf("  Result: %d\n", result);
}

/**
 * Test performance
 */
void test_performance(void) {
    printf("\n\n=== Test 4: Performance Testing ===\n");
    
    const char* words[] = {
        "test", "example", "word", "dictionary", "spell"
    };
    int num_words = sizeof(words) / sizeof(words[0]);
    
    printf("\nTesting %d words...\n", num_words);
    
    for (int i = 0; i < num_words; i++) {
        fetch_from_api(words[i]);
    }
    
    printf("\nPerformance Statistics:\n");
    print_api_stats();
}

/**
 * Main test function
 */
int main(int argc, char* argv[]) {
    printf("╔════════════════════════════════════════╗\n");
    printf("║  Merriam-Webster API Test Suite       ║\n");
    printf("║  Team Ctrl Alt Defeat                 ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    // Check for API key
    if (argc != 2) {
        printf("\nUsage: %s <API_KEY>\n", argv[0]);
        printf("\nGet your API key from: https://dictionaryapi.com/\n");
        return 1;
    }
    
    const char* api_key = argv[1];
    
    // Initialize API client
    printf("\n🔧 Initializing API client...\n");
    if (!api_client_init(api_key)) {
        fprintf(stderr, "❌ Failed to initialize API client\n");
        return 1;
    }
    
    // Run tests
    test_basic_api();
    test_detailed_info();
    test_error_handling();
    test_performance();
    
    // Final statistics
    printf("\n\n=== Final Statistics ===\n");
    print_api_stats();
    
    // Cleanup
    printf("\n🧹 Cleaning up...\n");
    api_client_cleanup();
    
    printf("\n✅ All tests completed!\n");
    return 0;
}