@echo off
REM Windows build script for Integration Tests
echo Building Integration Tests...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Compile library source files (excluding main.c)
echo Compiling library files...
gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/trie.c -o build/trie.o
if %errorlevel% neq 0 (
    echo Compilation of trie.c failed!
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/file_io.c -o build/file_io.o
if %errorlevel% neq 0 (
    echo Compilation of file_io.c failed!
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/edit_distance.c -o build/edit_distance.o
if %errorlevel% neq 0 (
    echo Compilation of edit_distance.c failed!
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/similarity_matrix.c -o build/similarity_matrix.o
if %errorlevel% neq 0 (
    echo Compilation of similarity_matrix.c failed!
    exit /b 1
)

gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c src/spell_check.c -o build/spell_check.o
if %errorlevel% neq 0 (
    echo Compilation of spell_check.c failed!
    exit /b 1
)

REM Compile integration test
echo Compiling integration test...
gcc -Wall -Wextra -std=c99 -pedantic -O2 -g -Iinclude -c test_integration_spell_check.c -o build/test_integration_spell_check.o
if %errorlevel% neq 0 (
    echo Compilation of integration test failed!
    exit /b 1
)

REM Link integration test executable
echo Linking integration test executable...
gcc build/test_integration_spell_check.o build/trie.o build/file_io.o build/edit_distance.o build/similarity_matrix.o build/spell_check.o -o test_integration_spell_check.exe -lm
if %errorlevel% neq 0 (
    echo Linking integration test failed!
    exit /b 1
)

echo Integration test build successful!
echo.
echo Running integration tests...
test_integration_spell_check.exe