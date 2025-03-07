::©2025 Wizurth, tous droits réservés.
::Github link: https://github.com/Wizurth

set SOLUTION_NAME=Engine-Project
set ROOT_DIRECTORY_NAME=lyo-t2-amiga-p7-09
set bAutoOpenSLN=true
set bCopyVisualFilters=false

set bDebugMode=false

if not "%bDebugMode%" == "true" (
	@echo off
)
@echo off

:: Copy Visual Studio filters
if /i "%bCopyVisualFilters%" == "true" (
	if not exist "%SOLUTION_NAME%.vcxproj.filters" (
		echo .
		echo [Information] Les filtres personnalisés de Visual Studio sont introuvables, récupération des filtres par défaut.
		echo .
		
		:: Copy Visual Folder => Temp
		xcopy /i /c /q /y "ide\vs\%SOLUTION_NAME%\%SOLUTION_NAME%.vcxproj.filters"
		
		if errorlevel 1 (
			echo .
			echo [Warning] Les filtres de Visual Studio sont introuvables.
			echo
		)
	)
	
	::Make project
    "SolutionGenerator.exe" -make ../%ROOT_DIRECTORY_NAME%
	
	::Copy Temp => Visual folder
    xcopy /i /c /q /y "%SOLUTION_NAME%.vcxproj.filters" "ide\vs\%SOLUTION_NAME%\"
	
	if errorlevel 1 (
		echo .
		echo [Warning] Les filtres de Visual Studio n'ont pas pu être copiés.
		echo .
	)
) else ( ::No Visual Studio Filters backup
	"SolutionGenerator.exe" -make ../%ROOT_DIRECTORY_NAME%
)

:: Open the solution
if /i "%bAutoOpenSLN%" == "true" (
	start "" "ide\vs\%SOLUTION_NAME%.sln"
)

:: Debug mode
if /i "%bDebugMode%" == "true" (
	echo.
	echo ======[DEBUG MODE]======
	echo SOLUTION_NAME=%SOLUTION_NAME%
	echo ROOT_DIRECTORY_NAME=%ROOT_DIRECTORY_NAME%
	echo bAutoOpenSLN=%bAutoOpenSLN%
	echo bCopyVisualFilters=%bCopyVisualFilters%
	echo ========================
	echo.

	pause
)