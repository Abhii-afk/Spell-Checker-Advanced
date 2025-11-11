@echo off
REM Windows build script for File I/O Tests

echo Building File I/O Tests...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Compile source files (excluding main.c)
echo Compiling trie.c...
gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/trie.c -o build/trie.o
if %errorlevel% neq 0 (
    echo Compilation of trie.c failed!
    exit /b 1
)

echo Compiling file_io.c...
gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/file_io.c -o build/file_io.o
if %errorlevel% neq 0 (
    echo Compilation of file_io.c failed!
    exit /b 1
)

REM Compile test file
echo Compiling test_file_io.c...
gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c test/test_file_io.c -o build/test_file_io.o
if %errorlevel% neq 0 (
    echo Compilation of test_file_io.c failed!
    exit /b 1
)

REM Link test executable
echo Linking test executable...
gcc build/trie.o build/file_io.o build/test_file_io.o -o test_file_io.exe -lm
if %errorlevel% neq 0 (
    echo Linking failed!
    exit /b 1
)

echo Build successful! Running File I/O tests...
echo.

REM Run the tests
test_file_io.exe

echo.
echo Test execution complete.