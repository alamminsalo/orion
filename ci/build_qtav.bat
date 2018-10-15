set QTAV_DIR=C:\projects\QtAV\%cc%\%platform%\%configuration%
setlocal enableextensions

set arch=%platform%

if exist %QTAV_DIR%\sdk_install.bat goto :installdeps
mkdir %QTAV_DIR%\..\

git clone --recursive https://github.com/wang-bin/QtAV.git %QTAV_DIR%
::version freeze until fix of https://github.com/wang-bin/QtAV/issues/1149
cd "%QTAV_DIR%" && git reset --hard 34afa14
cd %QTAV_DIR%\..\ && cmd /c "%QTAV_DIR%\tools\ci\win\install_dep.bat"
cd "%QTAV_DIR%"
qmake QtAV.pro "CONFIG+=%configuration%" "CONFIG+=no-examples" "CONFIG+=no-tests"
nmake %configuration%
:goto install

:installdeps
cd %QTAV_DIR%\..\ && cmd /c "%QTAV_DIR%\tools\ci\win\install_dep.bat"

:install
call "%QTAV_DIR%\sdk_install.bat"