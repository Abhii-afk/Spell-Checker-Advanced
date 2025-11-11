@echo off
echo Building Spell Checker with API Integration...

REM Create obj directory if it doesn't exist
if not exist obj mkdir obj

REM Compile source files
echo Compiling source files...
gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/main.c -o obj/main.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/spell_check.c -o obj/spell_check.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/api_client.c -o obj/api_client.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/trie.c -o obj/trie.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/file_io.c -o obj/file_io.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/edit_distance.c -o obj/edit_distance.o
if errorlevel 1 goto error

REM Link executable with libcurl and cJSON
echo Linking executable...
gcc obj/main.o obj/spell_check.o obj/api_client.o obj/trie.o obj/file_io.o obj/edit_distance.o -o spell_checker_api.exe -lcurl -lcjson
if errorlevel 1 goto error

echo Build successful! Executable: spell_checker_api.exe
echo.
echo Usage: spell_checker_api.exe [--api-key YOUR_KEY] dictionary.txt input.txt
echo.
echo Note: To use API features, you need:
echo   - libcurl installed
echo   - cJSON library installed
echo   - Merriam-Webster API key from https://dictionaryapi.com/
goto end

:error
echo Build failed!
echo.
echo Make sure you have installed:
echo   - libcurl: pacman -S mingw-w64-x86_64-curl
echo   - cJSON: pacman -S mingw-w64-x86_64-cjson
echo.
echo Or download from:
echo   - libcurl: https://curl.se/download.html
echo   - cJSON: https://github.com/DaveGamble/cJSON
exit /b 1

:end