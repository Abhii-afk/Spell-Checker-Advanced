@echo off
echo Building Spell Checker...

REM Create obj directory if it doesn't exist
if not exist obj mkdir obj

REM Compile source files
echo Compiling source files...
gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/main.c -o obj/main.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/spell_check.c -o obj/spell_check.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/trie.c -o obj/trie.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/file_io.c -o obj/file_io.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/edit_distance.c -o obj/edit_distance.o
if errorlevel 1 goto error

REM Link executable
echo Linking executable...
gcc obj/main.o obj/spell_check.o obj/trie.o obj/file_io.o obj/edit_distance.o -o spell_checker.exe
if errorlevel 1 goto error

echo Build successful! Executable: spell_checker.exe
echo.
echo Usage: spell_checker.exe dictionary.txt input.txt
goto end

:error
echo Build failed!
exit /b 1

:end