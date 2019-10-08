; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

#define MyAppName "自动改价软件Noon版"
#define MyAppVersion "V1.0-M"
#define MyAppPublisher "EchizenRyoma"
#define MyAppExeName "AutoMachine.exe"

[Setup]                              
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。   
; (生成新的GUID，点击 工具|在IDE中生成GUID。)
AppId={{DA254EE4-CE65-4385-BDA4-816485DA3124}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={pf}\{#MyAppName}  
DisableDirPage=yes
DisableFinishedPage=yes
DisableProgramGroupPage=yes
DisableReadyMemo=yes
DisableStartupPrompt =yes
DisableWelcomePage=yes
DisableReadyPage=yes
OutputDir=.\
OutputBaseFilename=自动改价软件Noon版补丁包
SetupIconFile=..\Lemon01\lemon.ico
Compression=lzma
SolidCompression=yes


[Messages]
SetupAppTitle=安装补丁
SetupWindowTitle=安装补丁
WizardReady=准备安装补丁
ReadyLabel1=补丁程序准备在你的电脑上安装。
ReadyLabel2b=点击“安装”继续。


[Files]
Source: "..\x64\Release\AutoMachine.exe"; DestDir: "{app}\"; Flags: ignoreversion
Source: "..\x64\Release\deprecated\*"; DestDir: "{app}\deprecated\"; Flags: ignoreversion recursesubdirs createallsubdirs
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”



