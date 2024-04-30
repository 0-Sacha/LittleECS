@echo off
call PowerShell -NoProfile -ExecutionPolicy Bypass -Command "%CD%\.buildbuddy\workspace_status.ps1"
