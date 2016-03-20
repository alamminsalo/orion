; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=Orion
AppVersion=1.0
DefaultDirName={pf}\Orion
DefaultGroupName=Orion
UninstallDisplayIcon={app}\orion.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.

[Files]
Source: "release\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\Orion"; Filename: "{app}\bin\orion.exe"
