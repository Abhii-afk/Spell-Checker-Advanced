@echo off
echo Building Report Generation Tests...

REM Compile the test executable
gcc -Wall -Wextra -std=c99 -Iinclude -I. -o test_report_generation.exe ^
    test_report_generation.c ^
    src/report.c ^
    src/spell_check.c ^
    src/trie.c ^
    src/file_io.c ^
    src/edit_distance.c ^
    src/similarity_matrix.c

if %ERRORLEVEL% neq 0 (
    echo Compilation failed!
    exit /b 1
)

echo Compilation successful!
echo Running tests...
echo.

REM Run the tests
test_report_generation.exe

echo.
echo Test execution completed.