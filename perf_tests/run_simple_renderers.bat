rem setup environment
@set PATH=%MY_LIB_PATH%\libxml2-2.7.8.win32\bin;%MY_LIB_PATH%\SDL_image-1.2.12\lib\x86;%MY_LIB_PATH%\SDL-1.2.15\lib\x86;%MY_LIB_PATH%\iconv-1.9.2.win32\bin;%PATH%;
cd D:\AC601\VirtualBOXShare\Ubuntu\IBP\code\v4\build\msvc2012\IBP_holoren\Release
D:

set EXE_NAME=IBP_holoren
set OF_W=1024
set OF_H=1024
set INPUT="D:\AC601\VirtualBOXShare\Ubuntu\IBP\code\v4\input\img\FIT-logo_160x160.png"
set KERNEL="D:\AC601\VirtualBOXShare\Ubuntu\IBP\code\v4\src\OpenCL\holoren_obj_wave.cl"


rem ===================================================================================================================
rem run the first set of tests
echo "Running simple SW rendering tests"
set PERF_LOG="..\..\..\..\perf_tests\Windows7\perf_SinglePass_1024x1024.txt"
set OUTPUT="..\..\..\..\perf_tests\Windows7\SinglePass_1024x1024.df"
set ALG=SinglePass

for /l %%x in (1, 1, 5) do (
  %EXE_NAME% -z 0.5 -s 0.00002 -w %OF_W% -h %OF_H% -i %INPUT% -a %ALG% -r simple -o %OUTPUT% -f %KERNEL% -p %PERF_LOG%
)

pause
