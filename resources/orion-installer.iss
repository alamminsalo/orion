; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#ifndef Platform
  #error Platform undefined. Pass /DPlatform={x86|x64}
#endif

#if "x86" == Platform
#elif "x64" == Platform
#else
  #error Unknown platform. Must be x86 or x64
#endif

#define AppVersion GetFileVersion("release\orion.exe")

[Setup]
AppName=Orion
AppVersion={#AppVersion}
DefaultDirName={pf}\Orion
DefaultGroupName=Orion
UninstallDisplayIcon={app}\orion.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
#if "x64" == Platform
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
#endif

[Files]
Source: "release\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs
#ifdef AdditionalRedist
Source: "{#AdditionalRedist}"; DestDir: "{app}\AdditionalRedist"; Flags: ignoreversion
#endif

[Icons]
Name: "{group}\Orion"; Filename: "{app}\bin\orion.exe"

[Run]
#ifdef AdditionalRedist
#define AdditionalRedistProper ExtractFileName(AdditionalRedist)
Filename: "{app}\AdditionalRedist\{#AdditionalRedistProper}"; Parameters: "/install /passive /norestart"
#endif
