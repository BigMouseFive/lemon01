; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

#define MyAppName "�Զ��ļ����Noon&Souq��"
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
OutputBaseFilename=Noon��۰油����
SetupIconFile=..\Lemon01\lemon.ico
Compression=lzma
SolidCompression=yes

[Messages]
SetupAppTitle=��װ����
SetupWindowTitle=��װ����
WizardReady=׼����װ����
ReadyLabel1=��������׼������ĵ����ϰ�װ��
ReadyLabel2b=�������װ��������

[Files]
Source: "..\x64\Release\AutoMachine.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\Release\windows.storage"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\Release\helper.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\x64\Release\deprecated\*"; DestDir: "{app}\deprecated\"; Flags: ignoreversion recursesubdirs createallsubdirs
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��
