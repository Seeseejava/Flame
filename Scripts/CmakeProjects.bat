@echo off
pushd ..\
call cmake -B build
call cmake --build build --parallel 4
popd
PAUSE