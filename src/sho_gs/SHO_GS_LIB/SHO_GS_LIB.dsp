# Microsoft Developer Studio Project File - Name="SHO_GS_LIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SHO_GS_LIB - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SHO_GS_LIB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SHO_GS_LIB.mak" CFG="SHO_GS_LIB - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SHO_GS_LIB - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SHO_GS_LIB - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SHO_GS_LIB - Win32 Debug2" (based on "Win32 (x86) Static Library")
!MESSAGE "SHO_GS_LIB - Win32 Xeon Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SHO_GS_LIB - Win32 Compaq Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SHO_GS_LIB - Win32 Debug_AllInONE" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SHO_GS_LIB", OBAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SHO_GS_LIB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VC_Lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /ZI /I "..\Client\Util" /I "..\LIB_Util\\" /I "..\LIB_Server" /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /I "./" /I "../../../LIB_Server" /I "../../../Client/Util" /I "../../../LIB_Util" /I "../../../client/common" /I "../../../server/common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SHO_GS_LIB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "VC_Lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /I "./" /I "../../../LIB_Server" /I "../../../Client/Util" /I "../../../LIB_Util" /I "../../../client/common" /I "../../../server/common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /FR /YX"LIB_gsMAIN.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"VC_Lib\d_SHO_GS_LIB.lib"

!ELSEIF  "$(CFG)" == "SHO_GS_LIB - Win32 Debug2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SHO_GS_LIB___Win32_Debug2"
# PROP BASE Intermediate_Dir "SHO_GS_LIB___Win32_Debug2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug2"
# PROP Intermediate_Dir "Debug2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /FR /YX"LIB_gsMAIN.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /I "./" /I "../../../LIB_Server" /I "../../../Client/Util" /I "../../../LIB_Util" /I "../../../client/common" /I "../../../server/common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "_DEBUG" /D "_WIN32_WINNT 0x0500" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /FR /YX"LIB_gsMAIN.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC_Lib\d_SHO_GS_LIB.lib"
# ADD LIB32 /nologo /out:"VC_Lib\d_SHO_GS_LIB.lib"

!ELSEIF  "$(CFG)" == "SHO_GS_LIB - Win32 Xeon Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SHO_GS_LIB___Win32_Xeon_Debug0"
# PROP BASE Intermediate_Dir "SHO_GS_LIB___Win32_Xeon_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xeon_Debug"
# PROP Intermediate_Dir "Xeon_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /FR /YX"LIB_gsMAIN.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /I "./" /I "../../../LIB_Server" /I "../../../Client/Util" /I "../../../LIB_Util" /I "../../../client/common" /I "../../../server/common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /FR /YX"LIB_gsMAIN.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC_Lib\d_SHO_GS_LIB.lib"
# ADD LIB32 /nologo /out:"VC_Lib\d_SHO_GS_LIB.lib"

!ELSEIF  "$(CFG)" == "SHO_GS_LIB - Win32 Compaq Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SHO_GS_LIB___Win32_Compaq_Debug"
# PROP BASE Intermediate_Dir "SHO_GS_LIB___Win32_Compaq_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Compaq_Debug"
# PROP Intermediate_Dir "Compaq_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /FR /YX"LIB_gsMAIN.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "./" /I "../../../LIB_Server" /I "../../../Client/Util" /I "../../../LIB_Util" /I "../../../client/common" /I "../../../server/common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /FR /YX"LIB_gsMAIN.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC_Lib\d_SHO_GS_LIB.lib"
# ADD LIB32 /nologo /out:"VC_Lib\d_SHO_GS_LIB.lib"

!ELSEIF  "$(CFG)" == "SHO_GS_LIB - Win32 Debug_AllInONE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SHO_GS_LIB___Win32_Debug_AllInONE"
# PROP BASE Intermediate_Dir "SHO_GS_LIB___Win32_Debug_AllInONE"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug_AllInONE"
# PROP Intermediate_Dir "Debug_AllInONE"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /ZI /I "..\Client\Util" /I "..\LIB_Util\\" /I "..\LIB_Server" /I ".\\" /I "..\..\LIB_Server" /I "..\..\Client\Util" /I "..\..\LIB_Util\\" /I "..\..\client\common" /I "..\..\server\common" /I "./" /I "../../../LIB_Server" /I "../../../Client/Util" /I "../../../LIB_Util" /I "../../../client/common" /I "../../../server/common" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /ZI /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB" /I "D:\Project2003\OnlineRPG\Client\Util" /I "D:\Project2003\OnlineRPG\LIB_Util" /I "D:\Project2003\OnlineRPG\LIB_Server" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Common" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Network" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "D:\Project2003\OnlineRPG\Server\SHO_GS\SHO_GS_LIB\Ai_lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__SERVER" /D "__USE_CHEAT" /D "_WIN32_WINNT 0x0500" /D "__INC_WORLD" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SHO_GS_LIB - Win32 Release"
# Name "SHO_GS_LIB - Win32 Debug"
# Name "SHO_GS_LIB - Win32 Debug2"
# Name "SHO_GS_LIB - Win32 Xeon Debug"
# Name "SHO_GS_LIB - Win32 Compaq Debug"
# Name "SHO_GS_LIB - Win32 Debug_AllInONE"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ZONE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GS_ThreadZONE.cpp
# End Source File
# Begin Source File

SOURCE=.\GS_ThreadZONE.h
# End Source File
# Begin Source File

SOURCE=.\ZoneFILE.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoneFILE.h
# End Source File
# Begin Source File

SOURCE=.\ZoneLIST.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoneLIST.h
# End Source File
# Begin Source File

SOURCE=.\ZoneSECTOR.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoneSECTOR.h
# End Source File
# End Group
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCharDATA.cpp
# End Source File
# Begin Source File

SOURCE=.\CCharDATA.h
# End Source File
# Begin Source File

SOURCE=.\CGameOBJ.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameOBJ.h
# End Source File
# Begin Source File

SOURCE=.\CIngSTATUS.cpp
# End Source File
# Begin Source File

SOURCE=.\CIngSTATUS.h
# End Source File
# Begin Source File

SOURCE=.\CObjCHAR.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjCHAR.h
# End Source File
# Begin Source File

SOURCE=.\CObjEVENT.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjEVENT.h
# End Source File
# Begin Source File

SOURCE=.\CObjITEM.cpp
# End Source File
# Begin Source File

SOURCE=.\CObjITEM.h
# End Source File
# Begin Source File

SOURCE=.\GS_ListUSER.cpp
# End Source File
# Begin Source File

SOURCE=.\GS_ListUSER.h
# End Source File
# Begin Source File

SOURCE=.\GS_ObjPOOL.cpp
# End Source File
# Begin Source File

SOURCE=.\GS_ObjPOOL.h
# End Source File
# Begin Source File

SOURCE=.\GS_USER.cpp
# End Source File
# Begin Source File

SOURCE=.\GS_USER.h
# End Source File
# Begin Source File

SOURCE=.\OBJECT.cpp
# End Source File
# Begin Source File

SOURCE=.\OBJECT.h
# End Source File
# End Group
# Begin Group "srv_COMMON"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\srv_COMMON\CDB_Socket.cpp
# End Source File
# Begin Source File

SOURCE=.\srv_COMMON\CDB_Socket.h
# End Source File
# Begin Source File

SOURCE=.\srv_COMMON\CObjVAR.h
# End Source File
# Begin Source File

SOURCE=.\srv_COMMON\CWorldVAR.h
# End Source File
# Begin Source File

SOURCE=.\srv_COMMON\NET_Prototype.h
# End Source File
# Begin Source File

SOURCE=.\srv_COMMON\SRV_Prototype.h
# End Source File
# End Group
# Begin Group "cli_COMMON"

# PROP Default_Filter ""
# Begin Group "AI_LIB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ai_lib\AI_Action.cpp
# End Source File
# Begin Source File

SOURCE=.\Ai_lib\AI_Condition.cpp
# End Source File
# Begin Source File

SOURCE=.\Ai_lib\CAI_FILE.CPP
# End Source File
# Begin Source File

SOURCE=.\Ai_lib\CAI_FILE.H
# End Source File
# Begin Source File

SOURCE=.\Ai_lib\CAI_LIB.H
# End Source File
# End Group
# Begin Source File

SOURCE=.\Common\Calculation.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\Calculation.h
# End Source File
# Begin Source File

SOURCE=.\Common\CEconomy.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CEconomy.h
# End Source File
# Begin Source File

SOURCE=.\Common\CHotICON.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CHotICON.h
# End Source File
# Begin Source File

SOURCE=.\Common\CInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CInventory.h
# End Source File
# Begin Source File

SOURCE=.\Common\CItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CItem.h
# End Source File
# Begin Source File

SOURCE=.\Common\CObjAI.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CObjAI.h
# End Source File
# Begin Source File

SOURCE=.\Common\CQuest.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CQuest.h
# End Source File
# Begin Source File

SOURCE=.\Common\CRegenAREA.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CRegenAREA.h
# End Source File
# Begin Source File

SOURCE=.\Common\CUserDATA.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\CUserDATA.h
# End Source File
# Begin Source File

SOURCE=.\Common\DataTYPE.h
# End Source File
# Begin Source File

SOURCE=.\Common\IO_AI.CPP
# End Source File
# Begin Source File

SOURCE=.\Common\IO_AI.H
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Motion.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Motion.h
# End Source File
# Begin Source File

SOURCE=.\Common\IO_PAT.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_PAT.H
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Quest.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Quest.h
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Skill.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_Skill.h
# End Source File
# Begin Source File

SOURCE=.\Common\IO_STB.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\IO_STB.h
# End Source File
# End Group
# Begin Group "DB_SQL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GS_ThreadSQL.cpp
# End Source File
# Begin Source File

SOURCE=.\GS_ThreadSQL.h
# End Source File
# End Group
# Begin Group "Grouping"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GS_GUILD.CPP
# End Source File
# Begin Source File

SOURCE=.\GS_GUILD.H
# End Source File
# Begin Source File

SOURCE=.\GS_PARTY.CPP
# End Source File
# Begin Source File

SOURCE=.\GS_PARTY.H
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GS_SocketLOG.cpp
# End Source File
# Begin Source File

SOURCE=.\GS_SocketLOG.h
# End Source File
# Begin Source File

SOURCE=.\GS_SocketLSV.cpp
# End Source File
# Begin Source File

SOURCE=.\GS_SocketLSV.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CGS_API.h
# End Source File
# Begin Source File

SOURCE=.\ETC_Math.cpp
# End Source File
# Begin Source File

SOURCE=.\ETC_Math.h
# End Source File
# Begin Source File

SOURCE=.\LIB_gsMAIN.cpp
# End Source File
# Begin Source File

SOURCE=.\LIB_gsMAIN.H
# End Source File
# Begin Source File

SOURCE=.\stdAFX.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Common\CObjVAR.h
# End Source File
# Begin Source File

SOURCE=".\DB구조.TXT"
# End Source File
# Begin Source File

SOURCE=".\구조설정서.txt"
# End Source File
# Begin Source File

SOURCE=".\버그수정사항.txt"
# End Source File
# Begin Source File

SOURCE=".\사용안하는 소스.txt"
# End Source File
# End Target
# End Project
