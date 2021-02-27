@echo off
echo "Start build..."
SET MSBuild= "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\amd64\msbuild.exe"
%MSBuild% "%WORKSPACE%\GradiusVI-Team1.sln" /p:Configuration=%1
@echo %off