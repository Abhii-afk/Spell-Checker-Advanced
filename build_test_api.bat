@echo off
echo Building API Test Program...

gcc -Wall -Wextra -std=c99 -g -Iinclude -c test_api.c -o obj/test_api.o
if errorlevel 1 goto error

gcc -Wall -Wextra -std=c99 -g -Iinclude -c src/api_client.c -o obj/api_client.o
if errorlevel 1 goto error

gcc obj/test_api.o obj/api_client.o -o test_api.exe -lcurl -lcjson
if errorlevel 1 goto error

echo Build successful! Executable: test_api.exe
echo.
echo Usage: test_api.exe YOUR_API_KEY
goto end

:error
echo Build failed!
echo Make sure libcurl and cJSON are installed.
exit /b 1

:end