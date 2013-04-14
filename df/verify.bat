@echo off

set holo_toolkit_path="..\..\..\holo_toolkit"

rem tento prikaz vyrobi novy riadok (`echo.`)
echo.

rem verify the created hologram
echo Propagating the diffraction pattern to verify results ...
"%holo_toolkit_path%\HoloPropagLarge.exe" "-c.\HoloPropagLarge.xml"
echo.

pause
