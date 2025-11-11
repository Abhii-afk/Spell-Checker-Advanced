@echo off
echo Building Comprehensive System Tests...

REM Compile the system test executable
gcc -Wall -Wextra -std=c99 -Iinclude ^
    test_system_comprehensive.c ^
    src/trie.c ^
    src/file_io.c ^
    src/spell_check.c ^
    src/edit_distance.c ^
    src/similarity_matrix.c ^
    src/report.c ^
    -o test_system_comprehensive.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Running system tests...
    echo.
    test_system_comprehensive.exe
) else (
    echo Build failed!
    exit /b 1
)