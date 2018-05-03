# Microsoft Developer Studio Project File - Name="SHO_GS_DLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SHO_GS_DLL - Win32 Compaq Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SHO_GS_DLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SHO_GS_DLL.mak" CFG="SHO_GS_DLL - Win32 Compaq Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SHO_GS_DLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SHO_GS_DLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SHO_GS_DLL - Win32 Debug2" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SHO_GS_DLL - Win32 Xeon Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SHO_GS_DLL - Win32 Compaq Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SHO_GS_DLL", LGAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SHO_GS_DLL - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /ZI /I "../../LIB_Util" /I "../../LIB_Server" /I "../../Client/Common" /I "../../Client/Util" /I "../Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"Release/SHO_GS.dll" /libpath:"../../../Client/Util" /libpath:"../../../LIB_Server/VC_LIB"

!ELSEIF  "$(CFG)" == "SHO_GS_DLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../LIB_Util" /I "../../LIB_Server" /I "../../Client/Common" /I "../../Client/Util" /I "../Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"..\Debug\SHO_GS.dll" /pdbtype:sept /libpath:"../../../Client/Util" /libpath:"../../../LIB_Server/VC_LIB"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=import gs_dll.ib
PostBuild_Cmds=..\dll2lib.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SHO_GS_DLL - Win32 Debug2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SHO_GS_DLL___Win32_Debug2"
# PROP BASE Intermediate_Dir "SHO_GS_DLL___Win32_Debug2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug2"
# PROP Intermediate_Dir "Debug2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../LIB_Util" /I "../../LIB_Server" /I "../../Client/Common" /I "../../Client/Util" /I "../Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"../SHO_GS2.dll" /pdbtype:sept /libpath:"../../../Client/Util" /libpath:"../../../LIB_Server/VC_LIB"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=dll2lib2.bat
PostBuild_Cmds=..\dll2lib2.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SHO_GS_DLL - Win32 Xeon Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SHO_GS_DLL___Win32_Xeon_Debug"
# PROP BASE Intermediate_Dir "SHO_GS_DLL___Win32_Xeon_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xeon_Debug"
# PROP Intermediate_Dir "Xeon_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../LIB_Util" /I "../../LIB_Server" /I "../../Client/Common" /I "../../Client/Util" /I "../Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"Xeon_Debug/SHO_GS.dll" /pdbtype:sept /libpath:"../../../Client/Util" /libpath:"../../../LIB_Server/VC_LIB"

!ELSEIF  "$(CFG)" == "SHO_GS_DLL - Win32 Compaq Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SHO_GS_DLL___Win32_Compaq_Debug"
# PROP BASE Intermediate_Dir "SHO_GS_DLL___Win32_Compaq_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Compaq_Debug"
# PROP Intermediate_Dir "Compaq_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SHO_GS_DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /dll /debug /machine:I386 /out:"Compaq_Debug/SHO_GS.dll" /pdbtype:sept /libpath:"../../../Client/Util" /libpath:"../../../LIB_Server/VC_LIB"

!ENDIF 

# Begin Target

# Name "SHO_GS_DLL - Win32 Release"
# Name "SHO_GS_DLL - Win32 Debug"
# Name "SHO_GS_DLL - Win32 Debug2"
# Name "SHO_GS_DLL - Win32 Xeon Debug"
# Name "SHO_GS_DLL - Win32 Compaq Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SHO_GS_DLL.cpp
# End Source File
# Begin Source File

SOURCE=.\SHO_GS_DLL.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
