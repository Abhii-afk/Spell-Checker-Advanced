# 🌐 Merriam-Webster API Integration

## 🎯 Overview

Your spell checker now features **hybrid dictionary validation** combining:
- **Local Trie Dictionary** (fast, offline)
- **Merriam-Webster API** (comprehensive, online)

## ✨ Key Features

✅ **Automatic Fallback**: Local → API → Suggestions
✅ **Performance Tracking**: Response times and statistics
✅ **Error Handling**: Graceful degradation on API failures
✅ **JSON Parsing**: Lightweight cJSON library
✅ **HTTP Requests**: Reliable libcurl implementation

## 🚀 Quick Start

### 1. Get API Key
Visit [dictionaryapi.com](https://dictionaryapi.com/) and sign up for free

### 2. Install Dependencies
```bash
# Windows (MSYS2)
pacman -S mingw-w64-x86_64-curl mingw-w64-x86_64-cjson

# Linux
sudo apt-get install libcurl4-openssl-dev libcjson-dev
```

### 3. Build with API Support
```bash
.\build_with_api.bat
```

### 4. Run with API
```bash
.\spell_checker_api.exe --api-key YOUR_KEY test_data/dictionary.txt test_data/sample_text.txt
```

## 📊 How It Works

```
┌─────────────────────────────────────────┐
│  Word: "algorithm"                      │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│  Step 1: Check Local Trie Dictionary   │
│  Result: Not Found                      │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│  Step 2: Query Merriam-Webster API     │
│  GET /api/v3/.../algorithm?key=...     │
│  Result: Found ✓                        │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│  Output: Word is CORRECT                │
│  (No suggestions needed)                │
└─────────────────────────────────────────┘
```

## 📁 Files Added

```
include/
└── api_client.h              # API client interface

src/
└── api_client.c              # API implementation

Modified:
├── src/spell_check.c         # Added API integration
└── src/main.c                # Added CLI options

Documentation:
├── API_INTEGRATION_GUIDE.md  # Detailed guide
└── API_README.md             # This file

Testing:
├── test_api.c                # API test suite
└── build_test_api.bat        # Test build script
```

## 🔧 API Functions

### Initialize API
```c
bool api_client_init(const char* api_key);
```

### Check Word
```c
int fetch_from_api(const char* word);
// Returns: 1 (found), 0 (not found), -1 (error)
```

### Get Details
```c
APIResponse* fetch_word_details(const char* word);
```

### Statistics
```c
const APIStats* get_api_stats(void);
void print_api_stats(void);
```

### Cleanup
```c
void api_client_cleanup(void);
```

## 📈 Example Output

```
🔍 Advanced Spell Checker Starting...
Dictionary: test_data/dictionary.txt
Input file: test_data/sample_text.txt
✅ Merriam-Webster API enabled

Loading dictionary...
Dictionary loaded: 356 words

🔍 Checking 'algorithm' via API...
✓ API: Word 'algorithm' found (response time: 245ms)
✅ Word 'algorithm' validated by API

=== SPELL CHECK RESULTS ===
Total words checked: 77
Errors found: 25
Accuracy: 67.5%

=== API STATISTICS ===
Total Requests:      12
Successful:          11
Failed:              1
Words Found:         8
Words Not Found:     3
Total Response Time: 2890 ms
Avg Response Time:   240.83 ms
======================
```

## 🎯 Use Cases

### 1. Technical Documents
```bash
# Validate programming terms, technical jargon
./spell_checker_api --api-key KEY technical_doc.txt
```

### 2. Academic Papers
```bash
# Check scientific terminology
./spell_checker_api --api-key KEY research_paper.txt
```

### 3. General Text
```bash
# Standard spell checking with API backup
./spell_checker_api --api-key KEY essay.txt
```

## 🔒 Security

**Never commit API keys!**

```bash
# Use environment variables
export MW_API_KEY="your-key"
./spell_checker_api --api-key $MW_API_KEY dict.txt input.txt

# Or use config file (add to .gitignore)
echo "*.key" >> .gitignore
```

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| Build fails | Install libcurl and cJSON |
| HTTP 401 | Check API key is correct |
| HTTP 429 | Rate limit exceeded, wait |
| Slow performance | Use local dictionary for common words |
| Network timeout | Check internet connection |

## 📚 API Limits

- **Free Tier**: 1,000 requests/day
- **Response Time**: ~200-300ms average
- **Rate Limit**: Reasonable usage

## 🎓 Testing

Run the test suite:
```bash
.\build_test_api.bat
.\test_api.exe YOUR_API_KEY
```

Tests include:
- ✅ Basic word validation
- ✅ Detailed word information
- ✅ Error handling
- ✅ Performance metrics

## 📖 Documentation

- **Full Guide**: See `API_INTEGRATION_GUIDE.md`
- **API Docs**: [dictionaryapi.com](https://dictionaryapi.com/)
- **libcurl**: [curl.se/libcurl](https://curl.se/libcurl/)
- **cJSON**: [github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON)

## 🎉 Benefits

| Feature | Local Only | With API |
|---------|-----------|----------|
| Speed | ⚡ Fast | 🐢 Slower |
| Coverage | 📚 Limited | 🌍 Comprehensive |
| Offline | ✅ Yes | ❌ No |
| Technical Terms | ❌ Limited | ✅ Excellent |
| Cost | 💰 Free | 💰 Free (1K/day) |

## 🏆 Best Practices

1. **Use local dictionary for common words**
2. **Enable API for rare/technical terms**
3. **Cache API results to reduce calls**
4. **Monitor API statistics**
5. **Handle errors gracefully**
6. **Respect rate limits**

## 👥 Credits

**Team Ctrl Alt Defeat**
- Advanced Spell Checker with API Integration
- C + Trie + Edit Distance + Merriam-Webster API

---

**Happy Spell Checking with API Power! 🚀✨**