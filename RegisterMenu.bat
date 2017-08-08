

rem ---------------------------------------------------------------------------------------------------
rem massive props to mb for this post (registry cascading menus):
rem http://io-repo.blogspot.pt/2011/05/cascading-context-menus-via-static.html
rem ----------
rem massive props to Ben Gripka for this other post (admin permissions):
rem https://stackoverflow.com/questions/1894967/how-to-request-administrator-access-inside-a-batch-file
rem ---------------------------------------------------------------------------------------------------


@echo off

:: BatchGotAdmin
:-------------------------------------
REM  --> Check for permissions
    IF "%PROCESSOR_ARCHITECTURE%" EQU "amd64" (
>nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) ELSE (
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params = %*:"=""
    echo UAC.ShellExecute "cmd.exe", "/c ""%~s0"" %params%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"
:--------------------------------------
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject" /t REG_SZ /v "ExtendedSubCommandsKey" /d "dllfile\shell\Inject"   /f
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject\Shell"         /t REG_SZ /v "" /f
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject\Shell\cmd1"         /t REG_SZ /v "MUIVerb" /d "LoadLibrary"   /f
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject\Shell\cmd1\command" /t REG_SZ /v "" /d "\"%CD%\Injectah.exe\" \"%%1\" /l" /f
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject\Shell\cmd2"         /t REG_SZ /v "MUIVerb" /d "Manual Mapping"   /f
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject\Shell\cmd2\command" /t REG_SZ /v "" /d "\"%CD%\Injectah.exe\" \"%%1\" /m" /f