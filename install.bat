set QT=G:\application/Qt/Qt6
set QTSDK=%QT%\6.6.3\msvc2019_64
set CMAKE=%QT%\Tools\CMake_64\bin\cmake
set OUTPATH=.\bin
@echo cmake to Vs2022 
%CMAKE% -S . -B build -DCMAKE_PREFIX_PATH=%QTSDK%
@echo cmake build
%CMAKE% --build build
@echo install DonutQMLPlayer
md %OUTPATH%
copy .\bin\*  %OUTPATH%
%QTSDK%\bin\windeployqt.exe %OUTPATH%\DonutQMLPlayer.exe --qmldir ./src

