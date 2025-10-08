# Spell Checker

A simple, clean implementation of a spell checker in C.

## Features

- **Dictionary-based spell checking** using Trie data structure
- **Text file processing** with tokenization
- **Error detection** with line number and position tracking
- **Suggestion generation** using edit distance algorithms
- **Cross-platform** - works on Windows, Linux, and macOS

## Building

### Windows
```bash
build.bat
```

## Usage

```bash
spell_checker.exe dictionary.txt input.txt
```

### Arguments
- `dictionary.txt` - Dictionary file with one word per line
- `input.txt` - Text file to spell check

## Example

```bash
spell_checker.exe test_data/dictionary.txt test_data/sample_text.txt
```

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