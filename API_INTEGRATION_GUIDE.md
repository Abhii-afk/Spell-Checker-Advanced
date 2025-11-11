# 🌐 Merriam-Webster API Integration Guide

## 📋 Overview

This guide explains how to integrate and use the Merriam-Webster Dictionary API with your spell checker project.

## ✨ Features

- **Hybrid Dictionary System**: Local Trie + Online API fallback
- **Performance Logging**: Track API response times and statistics
- **Error Handling**: Graceful fallback to local dictionary on API failures
- **JSON Parsing**: Lightweight cJSON library for response parsing
- **HTTP Requests**: libcurl for reliable API communication

## 🔑 Getting Your API Key

1. Visit [Merriam-Webster Developer Center](https://dictionaryapi.com/)
2. Sign up for a free account
3. Register for the **Collegiate Dictionary API**
4. Copy your API key (format: `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`)

## 📦 Prerequisites

### Required Libraries:

#### 1. **libcurl** (HTTP requests)
```bash
# Windows (MSYS2)
pacman -S mingw-w64-x86_64-curl

# Linux (Ubuntu/Debian)
sudo apt-get install libcurl4-openssl-dev

# macOS
brew install curl
```

#### 2. **cJSON** (JSON parsing)
```bash
# Windows (MSYS2)
pacman -S mingw-w64-x86_64-cjson

# Linux (Ubuntu/Debian)
sudo apt-get install libcjson-dev

# macOS
brew install cjson
```

## 🔧 Building with API Support

### Windows:
```bash
.\build_with_api.bat
```

### Linux/macOS:
```bash
gcc -Wall -Wextra -std=c99 -g -Iinclude \
    src/main.c src/spell_check.c src/api_client.c \
    src/trie.c src/file_io.c src/edit_distance.c \
    -o spell_checker_api \
    -lcurl -lcjson
```

## 🚀 Usage

### Basic Usage (Local Dictionary Only):
```bash
./spell_checker_api test_data/dictionary.txt test_data/sample_text.txt
```

### With API Integration:
```bash
./spell_checker_api --api-key YOUR_API_KEY test_data/dictionary.txt test_data/sample_text.txt
```

### With API Statistics:
```bash
./spell_checker_api --api-key YOUR_API_KEY --api-stats test_data/dictionary.txt test_data/sample_text.txt
```

## 📊 How It Works

### Spell Checking Flow:

```
1. Check word in local Trie dictionary
   ├─ Found? → Mark as correct
   └─ Not found? → Continue to step 2

2. Check if API is enabled
   ├─ No? → Mark as misspelled, generate suggestions
   └─ Yes? → Continue to step 3

3. Query Merriam-Webster API
   ├─ Word found? → Mark as correct
   ├─ Word not found? → Mark as misspelled
   └─ API error? → Fall back to local dictionary

4. Generate suggestions using Edit Distance algorithm
```

### API Request Example:

```c
// Initialize API client
api_client_init("your-api-key-here");

// Check a word
int result = fetch_from_api("hello");
// Returns: 1 (found), 0 (not found), -1 (error)

// Get detailed information
APIResponse* response = fetch_word_details("hello");
if (response) {
    printf("Word found: %s\n", response->word_found ? "Yes" : "No");
    printf("Definition: %s\n", response->definition);
    printf("Response time: %d ms\n", response->response_time_ms);
    free_api_response(response);
}

// Cleanup
api_client_cleanup();
```

## 📈 Performance Statistics

The API client tracks:
- **Total Requests**: Number of API calls made
- **Successful Requests**: Successful API responses
- **Failed Requests**: Failed API calls
- **Words Found**: Words validated by API
- **Words Not Found**: Words not in API dictionary
- **Average Response Time**: Mean API response time in ms

### Example Output:
```
=== API STATISTICS ===
Total Requests:      15
Successful:          14
Failed:              1
Words Found:         10
Words Not Found:     4
Total Response Time: 3450 ms
Avg Response Time:   230.00 ms
======================
```

## 🔍 API Response Format

### Successful Response (Word Found):
```json
[
  {
    "meta": {
      "id": "hello",
      "uuid": "...",
      "sort": "...",
      "src": "collegiate",
      "section": "alpha",
      "stems": ["hello", "hellos"],
      "offensive": false
    },
    "hwi": {
      "hw": "hel*lo",
      "prs": [{"mw": "he-ˈlō", "sound": {"audio": "hello001"}}]
    },
    "fl": "interjection",
    "def": [...],
    "shortdef": [
      "an expression or gesture of greeting",
      "an expression of surprise"
    ]
  }
]
```

### Word Not Found (Suggestions):
```json
["hallo", "hullo", "hell", "jello"]
```

## 🛡️ Error Handling

The API client handles various error scenarios:

### 1. **Network Errors**
```c
// Timeout, connection refused, DNS failure
if (api_result == -1) {
    fprintf(stderr, "API Error: Network issue\n");
    // Falls back to local dictionary
}
```

### 2. **HTTP Errors**
```c
// 401 Unauthorized, 403 Forbidden, 404 Not Found, 500 Server Error
response->http_status = 401;
response->error_message = "HTTP error: 401";
```

### 3. **JSON Parsing Errors**
```c
// Invalid JSON response
cJSON* json = cJSON_Parse(response_data);
if (!json) {
    fprintf(stderr, "Failed to parse JSON\n");
}
```

### 4. **API Rate Limiting**
```c
// Too many requests (429)
if (response->http_status == 429) {
    fprintf(stderr, "Rate limit exceeded\n");
    // Wait and retry or fall back to local
}
```

## 🎯 Code Structure

### Files Added:

```
include/
└── api_client.h          # API client header

src/
└── api_client.c          # API client implementation

Modified Files:
├── src/spell_check.c     # Added API integration
└── src/main.c            # Added API initialization
```

### Key Functions:

#### `api_client.h`:
```c
bool api_client_init(const char* api_key);
int fetch_from_api(const char* word);
APIResponse* fetch_word_details(const char* word);
const APIStats* get_api_stats(void);
void print_api_stats(void);
void api_client_cleanup(void);
```

#### `spell_check.c` (Modified):
```c
// Check local dictionary first
bool word_found = trie_search(dictionary, token->word);

// If not found, try API
if (!word_found && is_api_initialized()) {
    int api_result = fetch_from_api(token->word);
    if (api_result == 1) {
        word_found = true; // API validated the word
    }
}

// If still not found, mark as misspelled
if (!word_found) {
    // Generate suggestions using edit distance
}
```

## 📝 Example Usage

### Example 1: Basic API Integration
```c
#include "api_client.h"

int main() {
    // Initialize API
    if (!api_client_init("your-api-key")) {
        return 1;
    }
    
    // Check words
    printf("Checking 'hello': %d\n", fetch_from_api("hello"));
    printf("Checking 'wrld': %d\n", fetch_from_api("wrld"));
    
    // Show statistics
    print_api_stats();
    
    // Cleanup
    api_client_cleanup();
    return 0;
}
```

### Example 2: Detailed Word Information
```c
APIResponse* response = fetch_word_details("computer");
if (response && response->word_found) {
    printf("Word: computer\n");
    printf("Definition: %s\n", response->definition);
    printf("Response time: %d ms\n", response->response_time_ms);
}
free_api_response(response);
```

## 🔒 Security Best Practices

1. **Never commit API keys to version control**
   ```bash
   # Add to .gitignore
   echo "*.key" >> .gitignore
   echo "config.ini" >> .gitignore
   ```

2. **Use environment variables**
   ```bash
   export MW_API_KEY="your-api-key"
   ./spell_checker_api --api-key $MW_API_KEY dict.txt input.txt
   ```

3. **Store keys in config files**
   ```ini
   # config.ini
   [API]
   merriam_webster_key=your-api-key-here
   ```

## 🚦 Rate Limits

Merriam-Webster API limits:
- **Free Tier**: 1,000 requests per day
- **Rate**: Reasonable request rate (no hard limit specified)

**Tip**: Use local dictionary for common words, API for rare/technical terms.

## 🐛 Troubleshooting

### Issue: "Failed to initialize curl"
**Solution**: Install libcurl
```bash
pacman -S mingw-w64-x86_64-curl  # Windows
sudo apt-get install libcurl4-openssl-dev  # Linux
```

### Issue: "Failed to parse JSON"
**Solution**: Install cJSON
```bash
pacman -S mingw-w64-x86_64-cjson  # Windows
sudo apt-get install libcjson-dev  # Linux
```

### Issue: "HTTP error: 401"
**Solution**: Check your API key is correct

### Issue: "HTTP error: 429"
**Solution**: Rate limit exceeded, wait before retrying

### Issue: Slow performance
**Solution**: 
- Use local dictionary for common words
- Cache API results
- Reduce API calls with batch processing

## 📚 Additional Resources

- [Merriam-Webster API Documentation](https://dictionaryapi.com/products/api-collegiate-dictionary)
- [libcurl Documentation](https://curl.se/libcurl/)
- [cJSON GitHub](https://github.com/DaveGamble/cJSON)

## 🎓 Credits

**Team Ctrl Alt Defeat**
- Spell Checker Project with API Integration
- Built with C, Trie, Edit Distance & Merriam-Webster API

---

**Happy Spell Checking! 🚀✨**