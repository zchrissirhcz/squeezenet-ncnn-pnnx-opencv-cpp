@echo off

set BUILD_DIR=vs2022-x64
cmake ^
  -S .. ^
  -B %BUILD_DIR% ^
  -G "Visual Studio 17 2022" -A x64 ^
  -D CMAKE_CXX_FLAGS="/MP"

set UseMultiToolTask=true
cmake --build %BUILD_DIR% --config Release -- /p:CL_MP=true /p:CL_MPCount=4
@REM cmake --build %BUILD_DIR% --config Release -- /p:CL_MP=true /p:CL_MPCount=4
pause
