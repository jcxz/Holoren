@echo off

set holo_toolkit_path="..\..\..\holo_toolkit"

rem tento prikaz vyrobi novy riadok (`echo.`)
echo.

echo Creating a hologram from diffraction patern ...
"%holo_toolkit_path%\DFtoHologram.exe" "-c.\DFtoHologram.xml"
echo.

pause
