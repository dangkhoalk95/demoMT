The .rule.mk has been updated to remove the pthyon dependencie for regression testing.
To generate the vc_proj_generator.exe from the Python script (vc_proj_generator.py):
(Assuming you have pip installed which you should after installing Python inside folder Scripts) 
Install PyInstaller using pip, type in the command prompt the following.

C:\> pip install pyinstaller

After you install PyInstaller locate where your pyinstaller files are (they should be where your 
pip files are inside the Scripts folder) and go to the command prompt and type the following.

c:\python27\Scripts> pyinstaller --onefile %IOT_SDK_PATH%\tools\scripts\build\vc_proj_generator.py

The above command will create a folder called “dist” inside the Scripts folder, 
this will contain your single executable file “yourscript.exe”.
Copy the generated executable to the git repository:

C:\Python27\Scripts\dist> copy .\vc_proj_generator.exe %IOT_SDK_PATH%\tools\scripts\build\ 