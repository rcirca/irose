//==========================================================================//
// 이 파일은 export_interface.lua 스크립트에 의해 자동생성된 코드입니다.
// 직접 수정하지 마십시오.
// 05/03/18 12:49:19
//==========================================================================//


ZL_FUNC_BEGIN( SC_AddCoordinatesIndicator, RETURNS_NONE )
	         ZL_INT,
	         ZL_INT,
	       ZL_FLOAT,
	       ZL_FLOAT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_AddCreateAVT, RETURNS_HNODE )
	      ZL_STRING,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_AddNpcIndicator, RETURNS_NONE )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_AddSelectItem4CreateAvatar, RETURNS_NONE )
	         ZL_INT,
	      ZL_STRING,
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_AttachMotion, RETURNS_NONE )
	       ZL_HNODE,
	       ZL_HNODE
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_ChangeState, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_ClearSelectItem4CreateAvatar, RETURNS_NONE )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_ControlAnimation, RETURNS_NONE )
	       ZL_HNODE,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_CreateAVT, RETURNS_HNODE )
	      ZL_STRING,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_CreateEventButton, RETURNS_NONE )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_DestroyAVT, RETURNS_NONE )
	       ZL_HNODE
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_DoScript, RETURNS_INT )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_EffectOnChar, RETURNS_NONE )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_EffectOnObject, RETURNS_NONE )
	       ZL_HNODE,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_FindNode, RETURNS_HNODE )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GF_getVariable, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GF_setRevivePosition, RETURNS_NONE )
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetAvatarEXP, RETURNS_INT )
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetAvatarLEVEL, RETURNS_INT )
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetBGZoneNO, RETURNS_INT )
	               
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetEffectUseFile, RETURNS_INT )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetEffectUseIndex, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetEventObject, RETURNS_HNODE )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetEventObjectIndex, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetMotion, RETURNS_HNODE )
	       ZL_HNODE
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetSkeleton, RETURNS_HNODE )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_GetTotalFrame, RETURNS_INT )
	       ZL_HNODE
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_InitGame, RETURNS_NONE )
	               
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_LoadCamera, RETURNS_HNODE )
	      ZL_STRING,
	      ZL_STRING,
	       ZL_HNODE
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_LoadModel, RETURNS_HNODE )
	      ZL_STRING,
	       ZL_HNODE,
	       ZL_HNODE,
	       ZL_FLOAT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_LoadMotion, RETURNS_HNODE )
	      ZL_STRING,
	      ZL_STRING,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT,
	       ZL_FLOAT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_LoadSkeleton, RETURNS_HNODE )
	      ZL_STRING,
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_LogString, RETURNS_NONE )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_OpenDialog, RETURNS_NONE )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_OpenMsgbox, RETURNS_NONE )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_checkQuestCondition, RETURNS_INT )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_doQuestTrigger, RETURNS_NONE )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_findQuest, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getEpisodeVAR, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getJobVAR, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getNpcQuestZeroVal, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getPlanetVAR, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getQuestItemQuantity, RETURNS_INT )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getQuestSwitch, RETURNS_INT )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getQuestVar, RETURNS_INT )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getUnionVAR, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_QF_getUserSwitch, RETURNS_INT )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_RemoveCoordinatesIndicator, RETURNS_NONE )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_RemoveCreateAVT, RETURNS_NONE )
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_RemoveNpcIndicator, RETURNS_NONE )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_RunEvent, RETURNS_NONE )
	         ZL_INT,
	      ZL_STRING,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_RunEventFieldWarp, RETURNS_NONE )
	         ZL_INT,
	      ZL_STRING,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_RunEventObjectEvent, RETURNS_NONE )
	         ZL_INT,
	      ZL_STRING,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_ScreenFadeInStart, RETURNS_INT )
	       ZL_FLOAT,
	       ZL_FLOAT,
	       ZL_FLOAT,
	         ZL_INT,
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SelectItem4CreateAvatar, RETURNS_NONE )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SendMsgToWeb, RETURNS_NONE )
	      ZL_STRING,
	      ZL_STRING,
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetAvatarFace, RETURNS_NONE )
	      ZL_STRING,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetAvatarHair, RETURNS_NONE )
	      ZL_STRING,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetAvatarMotionByIndex, RETURNS_NONE )
	      ZL_STRING,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetAvatarMotionByName, RETURNS_NONE )
	      ZL_STRING,
	      ZL_STRING
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetButtonBlink, RETURNS_NONE )
	         ZL_INT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetCameraDefault, RETURNS_HNODE )
	       ZL_HNODE
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetModelDirection, RETURNS_NONE )
	       ZL_HNODE,
	       ZL_FLOAT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetModelPosition, RETURNS_NONE )
	       ZL_HNODE,
	       ZL_FLOAT,
	       ZL_FLOAT,
	       ZL_FLOAT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetMotionFrame, RETURNS_NONE )
	       ZL_HNODE,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetPosition, RETURNS_NONE )
	       ZL_HNODE,
	       ZL_FLOAT,
	       ZL_FLOAT,
	       ZL_FLOAT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetRepeatCount, RETURNS_INT )
	       ZL_HNODE,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetScale, RETURNS_NONE )
	       ZL_HNODE,
	       ZL_FLOAT,
	       ZL_FLOAT,
	       ZL_FLOAT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_SetVisible, RETURNS_NONE )
	       ZL_HNODE,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_ShowNotifyMessage, RETURNS_NONE )
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_ShowTutorialImage, RETURNS_NONE )
	      ZL_STRING,
	         ZL_INT,
	         ZL_INT,
	       ZL_FLOAT,
	       ZL_FLOAT,
	       ZL_FLOAT,
	         ZL_INT
ZL_FUNC_END

ZL_FUNC_BEGIN( SC_UnloadNode, RETURNS_NONE )
	       ZL_HNODE
ZL_FUNC_END

// end of file
