@echo off

rem create a hologram from diffraction pattern
call post_process.bat

rem verify the created hologram
call verify.bat
