#include "api_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <time.h>

// API configuration
#define API_BASE_URL "https://www.dictionaryapi.com/api/v3/references/collegiate/json/"
#define MAX_URL_LENGTH 512
#define MAX_RESPONSE_SIZE 65536

// Global API state
static char* g_api_key = NULL;
static bool g_initialized = false;
static APIStats g_stats = {0};

/**
 * @brief Structure to hold response data from curl
 */
typedef struct {
    char* data;
    size_t size;
} MemoryStruct;

/**
 * @brief Callback function for curl to write response data
 */
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    MemoryStruct* mem = (MemoryStruct*)userp;

    char* ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "Error: Not enough memory for API response\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

/**
 * @brief Initialize the API client with your API key
 */
bool api_client_init(const char* api_key) {
    if (!api_key || strlen(api_key) == 0) {
        fprintf(stderr, "Error: Invalid API key\n");
        return false;
    }

    // Initialize curl globally
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        return false;
    }

    // Store API key
    g_api_key = strdup(api_key);
    if (!g_api_key) {
        curl_global_cleanup();
        return false;
    }

    g_initialized = true;
    memset(&g_stats, 0, sizeof(APIStats));

    printf("✅ API Client initialized successfully\n");
    return true;
}

/**
 * @brief Check if API client is initialized
 */
bool is_api_initialized(void) {
    return g_initialized;
}

/**
 * @brief Parse JSON response to check if word exists
 */
static bool parse_json_response(const char* json_str, char** definition) {
    if (!json_str) return false;

    cJSON* json = cJSON_Parse(json_str);
    if (!json) {
        fprintf(stderr, "Error: Failed to parse JSON response\n");
        return false;
    }

    bool word_found = false;

    // Check if response is an array
    if (cJSON_IsArray(json)) {
        int array_size = cJSON_GetArraySize(json);
        
        if (array_size > 0) {
            cJSON* first_item = cJSON_GetArrayItem(json, 0);
            
            // Check if first item is an object (valid word entry)
            if (cJSON_IsObject(first_item)) {
                word_found = true;
                
                // Try to extract definition if requested
                if (definition) {
                    cJSON* shortdef = cJSON_GetObjectItem(first_item, "shortdef");
                    if (cJSON_IsArray(shortdef) && cJSON_GetArraySize(shortdef) > 0) {
                        cJSON* first_def = cJSON_GetArrayItem(shortdef, 0);
                        if (cJSON_IsString(first_def)) {
                            *definition = strdup(first_def->valuestring);
                        }
                    }
                }
            }
            // If first item is a string, it's a suggestion (word not found)
            else if (cJSON_IsString(first_item)) {
                word_found = false;
            }
        }
    }

    cJSON_Delete(json);
    return word_found;
}

/**
 * @brief Make HTTP request to Merriam-Webster API
 */
static APIResponse* make_api_request(const char* word) {
    if (!g_initialized || !g_api_key) {
        fprintf(stderr, "Error: API client not initialized\n");
        return NULL;
    }

    // Allocate response structure
    APIResponse* response = calloc(1, sizeof(APIResponse));
    if (!response) return NULL;

    // Build API URL
    char url[MAX_URL_LENGTH];
    snprintf(url, sizeof(url), "%s%s?key=%s", API_BASE_URL, word, g_api_key);

    // Initialize curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        response->error_message = strdup("Failed to initialize curl");
        return response;
    }

    // Setup response buffer
    MemoryStruct chunk = {0};
    chunk.data = malloc(1);
    chunk.size = 0;

    // Configure curl
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "SpellChecker/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 second timeout
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Measure response time
    clock_t start_time = clock();
    
    // Perform request
    CURLcode res = curl_easy_perform(curl);
    
    clock_t end_time = clock();
    response->response_time_ms = (int)(((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000);

    // Get HTTP status code
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    response->http_status = (int)http_code;

    // Check for errors
    if (res != CURLE_OK) {
        response->error_message = strdup(curl_easy_strerror(res));
        response->word_found = false;
    } else if (http_code == 200) {
        // Parse JSON response
        response->word_found = parse_json_response(chunk.data, &response->definition);
    } else {
        char error_buf[256];
        snprintf(error_buf, sizeof(error_buf), "HTTP error: %ld", http_code);
        response->error_message = strdup(error_buf);
        response->word_found = false;
    }

    // Cleanup
    free(chunk.data);
    curl_easy_cleanup(curl);

    return response;
}

/**
 * @brief Fetch word validation from Merriam-Webster API
 */
int fetch_from_api(const char* word) {
    if (!word || strlen(word) == 0) {
        return -1;
    }

    // Update statistics
    g_stats.total_requests++;

    // Make API request
    APIResponse* response = make_api_request(word);
    if (!response) {
        g_stats.failed_requests++;
        return -1;
    }

    // Update statistics
    g_stats.total_response_time += response->response_time_ms;
    g_stats.avg_response_time = (double)g_stats.total_response_time / g_stats.total_requests;

    int result;
    if (response->error_message) {
        // API call failed
        g_stats.failed_requests++;
        fprintf(stderr, "API Error for '%s': %s\n", word, response->error_message);
        result = -1;
    } else {
        // API call succeeded
        g_stats.successful_requests++;
        
        if (response->word_found) {
            g_stats.words_found++;
            printf("✓ API: Word '%s' found (response time: %dms)\n", 
                   word, response->response_time_ms);
            result = 1;
        } else {
            g_stats.words_not_found++;
            printf("✗ API: Word '%s' not found (response time: %dms)\n", 
                   word, response->response_time_ms);
            result = 0;
        }
    }

    free_api_response(response);
    return result;
}

/**
 * @brief Fetch detailed word information from API
 */
APIResponse* fetch_word_details(const char* word) {
    if (!word || strlen(word) == 0) {
        return NULL;
    }

    // Update statistics
    g_stats.total_requests++;

    // Make API request
    APIResponse* response = make_api_request(word);
    if (!response) {
        g_stats.failed_requests++;
        return NULL;
    }

    // Update statistics
    g_stats.total_response_time += response->response_time_ms;
    g_stats.avg_response_time = (double)g_stats.total_response_time / g_stats.total_requests;

    if (response->error_message) {
        g_stats.failed_requests++;
    } else {
        g_stats.successful_requests++;
        if (response->word_found) {
            g_stats.words_found++;
        } else {
            g_stats.words_not_found++;
        }
    }

    return response;
}

/**
 * @brief Get current API statistics
 */
const APIStats* get_api_stats(void) {
    return &g_stats;
}

/**
 * @brief Reset API statistics
 */
void reset_api_stats(void) {
    memset(&g_stats, 0, sizeof(APIStats));
}

/**
 * @brief Print API statistics to stdout
 */
void print_api_stats(void) {
    printf("\n=== API STATISTICS ===\n");
    printf("Total Requests:      %d\n", g_stats.total_requests);
    printf("Successful:          %d\n", g_stats.successful_requests);
    printf("Failed:              %d\n", g_stats.failed_requests);
    printf("Words Found:         %d\n", g_stats.words_found);
    printf("Words Not Found:     %d\n", g_stats.words_not_found);
    printf("Total Response Time: %ld ms\n", g_stats.total_response_time);
    printf("Avg Response Time:   %.2f ms\n", g_stats.avg_response_time);
    printf("======================\n\n");
}

/**
 * @brief Free APIResponse structure
 */
void free_api_response(APIResponse* response) {
    if (!response) return;

    if (response->definition) {
        free(response->definition);
    }
    if (response->error_message) {
        free(response->error_message);
    }
    free(response);
}

/**
 * @brief Cleanup API client resources
 */
void api_client_cleanup(void) {
    if (g_api_key) {
        free(g_api_key);
        g_api_key = NULL;
    }
    
    curl_global_cleanup();
    g_initialized = false;
    
    printf("✅ API Client cleaned up\n");
}