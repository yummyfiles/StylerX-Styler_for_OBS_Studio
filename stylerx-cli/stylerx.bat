@echo off
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PYTHON="

where python3 >nul 2>&1
if %errorlevel% equ 0 (
    set "PYTHON=python3"
) else (
    where python >nul 2>&1
    if %errorlevel% equ 0 (
        set "PYTHON=python"
    )
)

if not defined PYTHON (
    echo [^✗^] Python is not installed. Please install Python 3.
    echo [^•^] https://www.python.org/downloads/
    pause
    exit /b 1
)

"%PYTHON%" "%SCRIPT_DIR%stylerx.py" %*
exit /b %errorlevel%
