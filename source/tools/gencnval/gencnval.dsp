# Microsoft Developer Studio Project File - Name="gencnval" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=gencnval - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gencnval.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gencnval.mak" CFG="gencnval - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gencnval - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gencnval - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "gencnval - Win64 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gencnval - Win64 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gencnval - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo /MD /Za /W3 /GX /O2 /I "..\..\common" /I "..\toolutil" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 icuuc.lib icutu.lib /nologo /subsystem:console /machine:I386 /libpath:"..\toolutil\Release" /libpath:"..\..\..\lib"
# Begin Custom Build
TargetPath=.\Release\gencnval.exe
InputPath=.\Release\gencnval.exe
InputName=gencnval
SOURCE="$(InputPath)"

"..\..\..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) ..\..\..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "gencnval - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo /MDd /Za /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\toolutil" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR  /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icuucd.lib icutud.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\toolutil\Debug" /libpath:"..\..\..\lib"
# Begin Custom Build
TargetPath=.\Debug\gencnval.exe
InputPath=.\Debug\gencnval.exe
InputName=gencnval
SOURCE="$(InputPath)"

"..\..\..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) ..\..\..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "gencnval - Win64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN64" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo /MD /Za /W3   /I "..\..\common" /I "..\toolutil" /D"WIN64" /D"NDEBUG" /D"_CONSOLE" /D"_MBCS"  /FD /c /O2 /GX /Op /QIA64_fmaopt /D"_IA64_" /Zi /D"WIN64" /D"WIN32" /D"_AFX_NO_DAO_SUPPORT" /Wp64 /Zm600
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:IA64
# ADD LINK32 icuuc.lib icutu.lib /nologo /subsystem:console /machine:IA64 /libpath:"..\toolutil\Release" /libpath:"..\..\..\lib" /incremental:no
# Begin Custom Build
TargetPath=.\Release\gencnval.exe
InputPath=.\Release\gencnval.exe
InputName=gencnval
SOURCE="$(InputPath)"

"..\..\..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) ..\..\..\bin

# End Custom Build

!ELSEIF  "$(CFG)" == "gencnval - Win64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN64" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo /MDd /Za /W3 /Gm    /I "..\..\common" /I "..\toolutil" /D"WIN64" /D"_DEBUG" /D"_CONSOLE" /D"_MBCS" /FR  /FD /GZ /c /Od /GX /Op /QIA64_fmaopt /D"_IA64_" /Zi /D"WIN64" /D"WIN32" /D"_AFX_NO_DAO_SUPPORT" /Wp64 /Zm600
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:IA64 /pdbtype:sept
# ADD LINK32 icuucd.lib icutud.lib /nologo /subsystem:console /debug /machine:IA64 /pdbtype:sept /libpath:"..\toolutil\Debug" /libpath:"..\..\..\lib" /incremental:no
# Begin Custom Build
TargetPath=.\Debug\gencnval.exe
InputPath=.\Debug\gencnval.exe
InputName=gencnval
SOURCE="$(InputPath)"

"..\..\..\bin\$(InputName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) ..\..\..\bin

# End Custom Build

!ENDIF 

# Begin Target

# Name "gencnval - Win32 Release"
# Name "gencnval - Win32 Debug"
# Name "gencnval - Win64 Release"
# Name "gencnval - Win64 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gencnval.c
# End Source File
# Begin Source File

SOURCE=..\..\common\ucnv_io.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
