@echo off
REM Build script for HelloWorld FMU on Windows
REM Usage: build.cmd [windows|clean|help]

setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
set BUILD_DIR=%SCRIPT_DIR%build

if "%1"=="" goto usage
if "%1"=="windows" goto build_windows
if "%1"=="clean" goto clean
if "%1"=="help" goto usage
goto unknown

:build_windows
echo =========================================
echo Building for Windows...
echo =========================================
if exist "%BUILD_DIR%-windows" rmdir /s /q "%BUILD_DIR%-windows"
mkdir "%BUILD_DIR%-windows"
cd /d "%BUILD_DIR%-windows"
cmake ..
cmake --build . --config Release
echo.
echo FBU build complete: %BUILD_DIR%-windows\binaries\HelloWorld.dll
goto end

:clean
echo Cleaning build directories...
for /d %%D in ("%BUILD_DIR%"*) do (
    rmdir /s /q "%%D"
)
echo Clean complete
goto end

:usage
echo Usage: %0 [command]
echo.
echo Commands:
echo   windows   Build for Windows (native MSVC)
echo   clean     Remove all build directories
echo   help      Show this message
echo.
echo Examples:
echo   build.cmd windows
goto end

:unknown
echo Unknown command: %1
goto usage

:end
endlocal
