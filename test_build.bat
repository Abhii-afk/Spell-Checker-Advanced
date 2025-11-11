@echo off
REM Windows build script for Trie Unit Tests

echo Building Trie Unit Tests...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Compile trie.c (library code)
echo Compiling trie.c...
gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/trie.c -o build/trie.o
if %errorlevel% neq 0 (
    echo Compilation of trie.c failed!
    exit /b 1
)

REM Compile test file
echo Compiling test_trie.c...
gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c test/test_trie.c -o build/test_trie.o
if %errorlevel% neq 0 (
    echo Compilation of test_trie.c failed!
    exit /b 1
)

REM Link test executable
echo Linking test executable...
gcc build/trie.o build/test_trie.o -o test_runner.exe -lm
if %errorlevel% neq 0 (
    echo Linking failed!
    exit /b 1
)

echo Test build successful! Running tests...
echo.

REM Run the tests
test_runner.exe

echo.
echo Test execution complete.