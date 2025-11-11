#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @file api_client.h
 * @brief Merriam-Webster Dictionary API client for word validation
 * 
 * This module provides integration with the Merriam-Webster Dictionary API
 * to validate words that are not found in the local Trie dictionary.
 * Uses libcurl for HTTP requests and cJSON for JSON parsing.
 */

/**
 * @brief API response structure containing validation results
 */
typedef struct {
    bool word_found;           // Whether the word exists in the dictionary
    char* definition;          // First definition (if found)
    int response_time_ms;      // API response time in milliseconds
    int http_status;           // HTTP status code
    char* error_message;       // Error message (if any)
} APIResponse;

/**
 * @brief API statistics for performance monitoring
 */
typedef struct {
    int total_requests;        // Total API requests made
    int successful_requests;   // Successful API responses
    int failed_requests;       // Failed API requests
    int words_found;           // Words found in API
    int words_not_found;       // Words not found in API
    long total_response_time;  // Total response time in ms
    double avg_response_time;  // Average response time in ms
} APIStats;

/**
 * @brief Initialize the API client with your API key
 * 
 * @param api_key Your Merriam-Webster API key
 * @return true if initialization successful, false otherwise
 */
bool api_client_init(const char* api_key);

/**
 * @brief Fetch word validation from Merriam-Webster API
 * 
 * Makes an HTTP GET request to the Merriam-Webster Dictionary API
 * to check if a word exists and retrieve its definition.
 * 
 * @param word The word to look up (must not be NULL)
 * @return 1 if word exists in dictionary, 0 if not found, -1 on error
 * 
 * @note This function is thread-safe
 * @note Response time is logged for performance statistics
 */
int fetch_from_api(const char* word);

/**
 * @brief Fetch detailed word information from API
 * 
 * Returns a complete APIResponse structure with word validation,
 * definition, and performance metrics.
 * 
 * @param word The word to look up (must not be NULL)
 * @return APIResponse structure (caller must free with free_api_response)
 */
APIResponse* fetch_word_details(const char* word);

/**
 * @brief Get current API statistics
 * 
 * @return Pointer to current API statistics (do not free)
 */
const APIStats* get_api_stats(void);

/**
 * @brief Reset API statistics
 */
void reset_api_stats(void);

/**
 * @brief Print API statistics to stdout
 */
void print_api_stats(void);

/**
 * @brief Free APIResponse structure
 * 
 * @param response APIResponse to free (can be NULL)
 */
void free_api_response(APIResponse* response);

/**
 * @brief Cleanup API client resources
 */
void api_client_cleanup(void);

/**
 * @brief Check if API client is initialized
 * 
 * @return true if initialized, false otherwise
 */
bool is_api_initialized(void);

#endif // API_CLIENT_H