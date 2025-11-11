# 🔍 Advanced Spell Checker

A powerful spell checker with both **command-line interface** and **modern web interface** built with C backend and React frontend.

## ✨ Features

### Core Engine (C):
- **🏗️ Trie Data Structure** - O(m) dictionary lookup performance
- **📝 Smart Text Processing** - Advanced tokenization with position tracking
- **🎯 Intelligent Error Detection** - Line numbers and context preservation
- **💡 Suggestion Generation** - Edit distance algorithms with ranking
- **🚀 High Performance** - Optimized C implementation
- **🌐 API Integration** - Merriam-Webster Dictionary API support
- **📊 Performance Tracking** - API response time and statistics logging

### Web Interface (React):
- **🌐 Modern Web UI** - Responsive React-based interface
- **⚡ Real-time Processing** - Instant spell checking via REST API
- **📊 Visual Analytics** - Statistics dashboard with accuracy metrics
- **🎨 Interactive Design** - Click-to-replace suggestions
- **📱 Mobile Friendly** - Responsive design for all devices

## 🚀 Quick Start

### Option 1: Premium Web Interface (Recommended)
```bash
# Launch the premium UI with backend
./launch_premium_ui.bat

# Opens automatically in your browser
```

### Option 2: Command Line (Basic)
```bash
# Build the CLI version
./build.bat

# Run spell checker
./spell_checker.exe test_data/dictionary.txt test_data/sample_text.txt
```

### Option 3: Command Line with API
```bash
# Build with API support
./build_with_api.bat

# Run with Merriam-Webster API
./spell_checker_api.exe --api-key YOUR_KEY test_data/dictionary.txt test_data/sample_text.txt

# Show API statistics
./spell_checker_api.exe --api-key YOUR_KEY --api-stats test_data/dictionary.txt test_data/sample_text.txt
```

## 📁 Project Structure

```
Spell-Checker-Advanced/
├── 🖥️ CLI Version
│   ├── src/           # C source files
│   ├── include/       # Header files
│   └── build.bat      # Build script
├── 🌐 Web Interface
│   ├── backend/       # C API server
│   ├── frontend/      # React app
│   └── start_dev.bat  # Development server
└── 📊 Test Data
    └── test_data/     # Sample files & dictionary
```

## 🌐 Web Interface Setup

See [WEB_SETUP.md](WEB_SETUP.md) for detailed setup instructions.

**Quick Setup:**
1. Install dependencies (Node.js, GCC, libmicrohttpd, json-c)
2. Run `./start_dev.bat`
3. Open `http://localhost:3000`

## Files

### Core Files
- `src/main.c` - Main program entry point
- `src/spell_check.c` - Core spell checking logic
- `include/spell_check.h` - Spell checking interface

### Supporting Files
- `src/trie.c` - Trie data structure implementation
- `src/file_io.c` - File reading and text processing
- `src/edit_distance.c` - Edit distance calculations

### Build Files
- `build.bat` - Windows build script