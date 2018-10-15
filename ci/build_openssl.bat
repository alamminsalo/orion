set OPENSSL_DIR=C:\projects\OpenSSL\%cc%\%platform%
set "PATH=C:\Program Files (x86)\nasm;%PATH%"
setlocal enableextensions

if exist %OPENSSL_DIR% goto :eof

if not exist "C:\projects\OpenSSL" mkdir "C:\projects\OpenSSL"
cd "C:\projects\OpenSSL"
if not exist ssl.zip ( appveyor DownloadFile "https://codeload.github.com/openssl/openssl/zip/OpenSSL_1_0_2p" -FileName ssl.zip || goto :eof )
if not exist build ( 7z x ssl.zip > NUL && move openssl* build || goto :eof )

cd build

if %platform%==x64 ( 
    cmd /c "perl Configure VC-WIN64A && ms\do_win64a"
) else ( 
    curl -L -o nasminst.exe http://libgd.blob.core.windows.net/nasm/nasm-2.07-installer.exe
    start /wait nasminst.exe /S
    cmd /c "perl Configure VC-WIN32 && ms\do_nasm"
)

nmake -f ms\ntdll.mak || goto :eof

mkdir %OPENSSL_DIR%\..\
move /Y out32dll %OPENSSL_DIR%
