@echo off
setlocal

:: Loop through subdirectories (1 level deep) in the current directory
for /d %%D in (* ) do (
    if exist "%%D\Intermediate" (
        echo Deleting folder: %%D\Intermediate
        rd /s /q "%%D\Intermediate"
    )
)

endlocal
