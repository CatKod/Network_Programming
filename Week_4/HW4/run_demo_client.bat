@echo off
title IT4062 HW4 Demo - HoangKimVinh 20235876
color 0E
cd /d D:\GitHub\Network_Programming\Week_4\HW4
echo ==========================================================
echo   IT4062 - Homework 4 (HW4) - TCP Socket Applications
echo   Student: HoangKimVinh - 20235876
echo ==========================================================
echo.
echo [INFO] Make sure both servers are already running:
echo        - bin\server.exe      5500  (Bai 1 - String Splitter)
echo        - bin\server_b2.exe   5600  (Bai 2 - File Transfer)
echo.
echo --- Bai 1: String Splitter Client ---
echo.
bin\client.exe 127.0.0.1 5500 < data\b1_input.txt
echo.
echo ----------------------------------------------------------
echo.
echo --- Bai 2: File Transfer Client ---
echo.
bin\client_b2.exe 127.0.0.1 5600
echo.
echo ==========================================================
echo   Demo finished. Press any key to close.
echo ==========================================================
pause >nul