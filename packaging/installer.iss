#define Version Trim(FileRead(FileOpen("..\VERSION")))
#define PluginName "ZL E Compressor"
#define ProjectName "ZLEComp"
#define Publisher "ZL"
#define Year GetDateTimeString("2023","","")

[Setup]
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
AppName={#PluginName}
OutputBaseFilename={#ProjectName}-{#Version}
AppCopyright=Copyright (C) {#Year} {#Publisher}
AppPublisher={#Publisher}
AppVersion={#Version}
DefaultDirName="{commoncf64}\VST3\{#PluginName}.vst3"
DisableDirPage=yes
LicenseFile="..\LICENSE"
UninstallFilesDir="{commonappdata}\{#PluginName}\uninstall"

[UninstallDelete]
Type: filesandordirs; Name: "{commoncf64}\VST3\{#PluginName}Data"

; MSVC adds a .ilk when building the plugin. Let's not include that.
[Files]
Source: "..\Builds\{#ProjectName}_artefacts\Release\VST3\{#PluginName}.vst3\*"; DestDir: "{commoncf64}\VST3\{#PluginName}.vst3\"; Excludes: *.ilk; Flags: ignoreversion recursesubdirs;

[Run]
Filename: "{cmd}"; \
    WorkingDir: "{commoncf64}\VST3"; \
    Parameters: "/C mklink /D ""{commoncf64}\VST3\{#PluginName}Data"" ""{commonappdata}\{#PluginName}"""; \
    Flags: runascurrentuser;