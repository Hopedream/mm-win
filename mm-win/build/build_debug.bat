::-----------------build batch--------------
::-----------------version: 1.0.0.0---------
::-----------------author:llzhou------------

@CD ..

::update code
git pull

@set MM_ROOT=%CD%
@CD .\build

::delete older binary app.
@if exist "%MM_ROOT%\bin\MM.exe" del "%MM_ROOT%\bin\MM.exe" /s/q
@if exist "%MM_ROOT%\bin\gloox.dll" del "%MM_ROOT%\bin\gloox.dll" /s/q
@if exist "%MM_ROOT%\bin\DuiLib.dll" del "%MM_ROOT%\bin\DuiLib.dll" /s/q

::compile project.
@call "C:\Program Files\Microsoft Visual Studio 10.0\Common7\Tools\vsvars32.bat"
@echo Building 
devenv "..\gloox-1.0.14\gloox.sln" /rebuild "Debug|Win32" /out gloox_build_log.log
devenv "..\DuiLib\DuiLib.sln" /rebuild "Debug|Win32" /out duilib_build_log.log
devenv "..\MM.sln" /rebuild "Debug|Win32" /out MM_build_log.log

::check compile result.
@if not exist "%MM_ROOT%\bin\gloox.dll" (
		echo "****************************************************" 
		echo "************* ERROR: gloox.dll compile failed ******" 
		echo "****************************************************" 
		pause
		exit )

@if not exist "%MM_ROOT%\bin\DuiLib.dll" (
		echo "****************************************************" 
		echo "************* ERROR: DuiLib.dll compile failed ******" 
		echo "****************************************************" 
		pause
		exit )

@if not exist "%MM_ROOT%\bin\MM.exe" (
		echo "****************************************************" 
		echo "*************   ERROR: MM.exe compile failed  ******" 
		echo "****************************************************" 
		pause
		exit )


::create release package.
@SET YEAR=%date:~0,4%
@SET MONTH=%date:~5,2%
@SET DAY=%date:~8,2%
@SET HOUR=%time:~0,2%
@IF "%HOUR:~0,1%"==" " SET HOUR=0%HOUR:~1,1%
@SET MINUTE=%time:~3,2%
@SET SECOND=%time:~6,2%
@SET ReleaseDate=%YEAR%_%MONTH%_%DAY% %HOUR%;%MINUTE%;%SECOND%
@set RELEASE_PACKAGE_DIR=%MM_ROOT%\MM_Debug %ReleaseDate%
echo %RELEASE_PACKAGE_DIR%
@if not exist "%RELEASE_PACKAGE_DIR%" mkdir "%RELEASE_PACKAGE_DIR%"

::echo %RELEASE_PACKAGE_DIR%
::copy file.
@echo Binary file
SET REALEASE_BIN_DIR=%RELEASE_PACKAGE_DIR%\bin
@if not exist "%REALEASE_BIN_DIR%" mkdir "%REALEASE_BIN_DIR%"
@xcopy /Y /S "%MM_ROOT%\bin\*.*" "%REALEASE_BIN_DIR%"
del "%REALEASE_BIN_DIR%\*.pdb" /s/q


@echo res file
@SET REALEASE_RES_DIR=%RELEASE_PACKAGE_DIR%\MM\res
@if not exist "%REALEASE_RES_DIR%" mkdir "%REALEASE_RES_DIR%"
@xcopy /Y /S "%MM_ROOT%\MM\res\*.*" "%REALEASE_RES_DIR%"






