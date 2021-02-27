@echo off
Echo.%3 | findstr /C:"Jenkins">nul && (
   REM Don't copy
   Echo "Don't copy on Jenkins!"
   goto:eof
) || (
    goto copy_files
)

:copy_files

SET "solutionDir=%1"
SET "outDir=%2"
SET "projectName=%3"
SET "configuration=%4"
echo -- Copy build resources --
echo solution: %solutionDir%
echo outdir: %outDir%
echo projectname: %projectName%
echo "from: %solutionDir%Helios\Libs"
echo "to: %solutionDir%bin\%outDir%\%projectName%\ on configuration %configuration%"

Echo.%configuration% | findstr /C:"Debug">nul && (
    xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmodL.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
    xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmodstudioL.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
) || (
    xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmod.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
    xcopy "%solutionDir%Helios\Libs\FMOD\lib\fmodstudio.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
)

xcopy "%solutionDir%Helios\Libs\Optick_\lib\x64\debug\OptickCore.dll" %solutionDir%bin\%outDir%\%projectName%\ /y /d /r /I
xcopy "%solutionDir%%projectName%\Assets" %solutionDir%bin\%outDir%\%projectName%\Assets\ /y /r /s /i
@echo off