; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

#define MyAppName "�Զ��ļ����Noon��"
#define MyAppVersion "V1.0-M"
#define MyAppPublisher "EchizenRyoma"
#define MyAppExeName "AutoMachine.exe"

[Setup]                              
; ע: AppId��ֵΪ������ʶ��Ӧ�ó���
; ��ҪΪ������װ����ʹ����ͬ��AppIdֵ��   
; (�����µ�GUID����� ����|��IDE������GUID��)
AppId={{DA254EE4-CE65-4385-BDA4-816485DA3124}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={pf}\{#MyAppName}  
DisableProgramGroupPage=yes
DisableDirPage=yes
OutputDir=.\
OutputBaseFilename=�Զ��ļ����Noon�� V1.0
SetupIconFile=..\Lemon01\lemon.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone;   

[Files]
Source: "..\x64\Release\AutoMachine.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\Release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\vcredist_x64.2013.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\ChromeSetup.exe"; DestDir: "{app}"; Flags: ignoreversion
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\vcredist_x64.2013.exe"; Description: "vs2013���п�"; Flags: nowait postinstall skipifsilent
Filename: "{app}\ChromeSetup.exe"; Description: "chrome��װ����"; Flags: nowait postinstall skipifsilent

