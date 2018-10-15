set MPVDIR=C:\projects\mpv\%cc%\%platform%
setlocal enableextensions

if %platform%==x86 set mpv_platform=i686
if %platform%==x64 set mpv_platform=x86_64

if not exist %MPVDIR% mkdir %MPVDIR%
cd %MPVDIR%

if exist %MPVDIR%\mpv-1.dll goto :def
appveyor DownloadFile "https://kent.dl.sourceforge.net/project/mpv-player-windows/libmpv/mpv-dev-%mpv_platform%-20181007-git-2b0b9bb.7z" -FileName mpv-dev.7z
7z x -y mpv-dev.7z

:def
if exist %MPVDIR%\mpv.def goto :lib
appveyor DownloadFile "https://raw.githubusercontent.com/mpv-player/mpv/master/libmpv/mpv.def" -FileName mpv_dl.def
echo EXPORTS > mpv.def
type mpv_dl.def >> mpv.def

:lib
if exist %MPVDIR%\mpv.lib goto :install
lib /def:mpv.def /name:mpv-1.dll /out:mpv.lib /MACHINE:%platform%

:install
copy /y %MPVDIR%\mpv.lib %QTDIR%\lib\mpv.lib
copy /y %MPVDIR%\mpv-1.dll %QTDIR%\bin\mpv-1.dll
xcopy %MPVDIR%\include %QTDIR%\include\mpv /y /s /e /h /i