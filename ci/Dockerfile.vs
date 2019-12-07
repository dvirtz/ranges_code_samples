# escape=`

FROM mcr.microsoft.com/dotnet/framework/runtime:4.8-windowsservercore-ltsc2019

LABEL maintainer="Luis Martinez de Bartolome <luism@jfrog.com>"

SHELL ["powershell.exe", "-ExecutionPolicy", "Bypass", "-Command"]

ENV chocolateyUseWindowsCompression=false `
    PYTHONIOENCODING=UTF-8

RUN iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1')); `
    $env:Path += '";C:\tools\python3;C:\tools\python3\Scripts"'; `
    choco install --no-progress --yes git --version=2.19.0 --params '"/InstallDir:C:\tools\git"'; `
    choco install --no-progress --yes svn --version=1.8.17 --params '"/InstallDir:C:\tools\svn"'; `
    choco install --no-progress --yes cmake --version=3.15.4 --params '"/InstallDir:C:\tools\cmake"' --installargs 'ADD_CMAKE_TO_PATH=""System""'; `
    choco install --no-progress --yes python3 --version=3.7.0 --params '"/InstallDir:C:\tools\python3"'
    
ADD https://aka.ms/vscollect.exe C:\TEMP\collect.exe

COPY Install.cmd C:\TEMP\

RUN C:\TEMP\Install.cmd choco install --no-progress --yes visualstudio2019buildtools --version=16.3.2.0
RUN C:\TEMP\Install.cmd choco install --no-progress --yes visualstudio2019-workload-vctools --version=1.0.0
RUN C:\TEMP\Install.cmd choco install --no-progress --yes --execution-timeout=0 visualstudio2019-workload-manageddesktop --version=1.0.1

RUN python -m pip install --quiet --upgrade pip; `
    python -m pip install win-unicode-console --quiet --upgrade --force-reinstall --no-cache; `
    python -m pip install conan --quiet --upgrade --force-reinstall --no-cache; `
    python -m pip install conan_package_tools --quiet --upgrade --force-reinstall --no-cache

ADD VsDevCmdPowerShell.bat C:\TEMP\
ENTRYPOINT C:\TEMP\VsDevCmdPowerShell.bat
