-- functions

-- eDialogType
-- {
-- 	DLG_TYPE_MENU
-- 	DLG_TYPE_CHAR
-- 	DLG_TYPE_ITEM
-- 	DLG_TYPE_SKILL
-- 	DLG_TYPE_QUEST
-- 	DLG_TYPE_COMMUNITY
-- 	DLG_TYPE_CLAN
-- 	DLG_TYPE_HELP
-- 	DLG_TYPE_OPTION
-- }
-- eButton
-- {
--	MENU_BTN_CHAR
--	MENU_BTN_ITEM
--	MENU_BTN_SKILL
--	MENU_BTN_QUEST
--	MENU_BTN_COMMUNITY
--	MENU_BTN_CLAN
--	MENU_BTN_HELP
--	MENU_BTN_INFO
--	MENU_BTN_OPTION
--	MENU_BTN_EXIT
-- }
--
-- SC_OpenDialog ( eDialogType )
-- SC_SetButtonBlink( eDialogType, eButton )
-- SC_AddNpcIndicator ( npcno, auto_remove ) --npcno(Áßº¹ °¡´É), auto_remove: 1(´ëÈ­½Ãµµ½Ã ÀÚµ¿»èÁ¦:Áßº¹µÈ¸ðµç), 0(½ºÅ©¸³Æ® È£Ãâ·Î ¼öµ¿»èÁ¦)
-- SC_RemoveNpcIndicator ( npcno ) -- Áßº¹½Ã Áßº¹ °³¼ö¸¸Å­ È£Ãâ ÇÊ¿ä
-- SC_AddCoordinatesIndicator ( index, zoneno, x, y ) -- index Áßº¹ºÒ°¡
-- SC_RemoveCoordinatesIndicator ( index ) -- Add½Ã ¼³Á¤µÈ index



-- Tutorial 01
-- Ã³À½ Á¢¼Ó½Ã
function Tutorial01( iOwnerIndex )	
	SC_ShowNotifyMessage( 1 );
end

-- Tutorial 02
-- 2 ·¹º§·Î ½Â±Þ
function Tutorial02( iOwnerIndex )	
	SC_ShowNotifyMessage( 2 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_CHAR )
end

-- Tutorial 03
-- 2·¹º§ °æÇèÄ¡ 150 ÀÌ»óÀÌ µÇ¾úÀ»¶§
function Tutorial03( iOwnerIndex )	
	SC_ShowNotifyMessage( 3 );
end

-- Tutorial 04
-- 2·¹º§ °æÇèÄ¡ 220 ÀÌ»óÀÌ µÇ¾úÀ»¶§
function Tutorial04( iOwnerIndex )	
	SC_ShowNotifyMessage( 4 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_ITEM )
end

-- Tutorial 05
-- 3·¹º§·Î ½Â±Þ
function Tutorial05( iOwnerIndex )	
	SC_ShowNotifyMessage( 5 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_SKILL )
end

-- Tutorial 06
-- 3·¹º§ °æÇèÄ¡ 100 ÀÌ»óÀÌ µÇ¾úÀ»¶§
function Tutorial06( iOwnerIndex )	
	SC_ShowNotifyMessage( 6 );
end

-- 5051 Äù½ºÆ®¸¦ ¹Þ¾ÒÀ»¶§ ¹öÆ° »ý¼º
function Tutorial07button()	
	SC_CreateEventButton( 7 )
end

-- Tutorial 07
-- 5051 Äù½ºÆ®¸¦ ¹Þ¾ÒÀ»¶§
function Tutorial07( iOwnerIndex )	
	SC_ShowNotifyMessage( 7 );
	SC_OpenDialog ( DLG_TYPE_MENU )
	SC_SetButtonBlink( DLG_TYPE_MENU, MENU_BTN_QUEST )
end

-- Tutorial 08
-- 4·¹º§·Î ½Â±Þ
function Tutorial08( iOwnerIndex )	
	SC_ShowNotifyMessage( 8 );
end

-- T-warp Æ®¸®°Å ¼öÇàÈÄ ¹öÆ° »ý¼º
function Tutorial09button()	
	SC_CreateEventButton( 9 )
end

-- Tutorial 09
-- T-warp Æ®¸®°Å ¼öÇàÈÄ Ú
function Tutorial09( iOwnerIndex )	
	SC_ShowNotifyMessage( 9 );
end

-- Tutorial 10
-- 10·¹º§·Î ½Â±ÞÚ
function Tutorial10( iOwnerIndex )	
	SC_ShowNotifyMessage( 10 );
end



-- Notify 01
function Notify01( iOwnerIndex )	
	SC_ShowNotifyMessage( 3 );
end








function InitTutorialEvent()
	
end


iPrevEXP = 0;
iInitPrevEXP = 0;
iStartLevel1Event = 0;	-- 1·¾ °æÇèÄ¡0 ÀÏ¶§ ÀÌº¥Æ®¸¦ ¿©·¯¹ø ¹ß»ý½ÃÅ°Áö ¾Ê±â À§ÇØ.

-- CheckTutorialEvent
function CheckTutorialEvent()

	if iInitPrevEXP == 0 then
		iPrevEXP = SC_GetAvatarEXP();
		iInitPrevEXP = 1;
	end

	iAvatarExp 	= SC_GetAvatarEXP();
	iAvatarLEVEL 	= SC_GetAvatarLEVEL();
	
	-- Ã³À½ Á¢¼Ó½Ã( 1·¹º§ °æÇèÄ¡ 0 )ÀÏ°æ¿ì
	if ( iStartLevel1Event == 0 ) and ( iAvatarLEVEL == 1 ) and ( iAvatarExp == 0 ) then	
		SC_CreateEventButton( 1 )
		iStartLevel1Event = 1
	end
	
	--2 ·¹º§ °æÇèÄ¡ 150 ÀÌ»ó µÇ¾úÀ»¶§
	if ( iAvatarLEVEL == 2 ) and ( iAvatarExp >= 150 ) and ( iPrevEXP < 150 ) then
		SC_CreateEventButton( 3 )
	end
	
	--2 ·¹º§ °æÇèÄ¡ 220 ÀÌ»ó µÇ¾úÀ»¶§
	if ( iAvatarLEVEL == 2 ) and ( iAvatarExp >= 220 ) and ( iPrevEXP < 220 ) then
		SC_CreateEventButton( 4 )
	end
	
	--3 ·¹º§ °æÇèÄ¡ 220 ÀÌ»ó µÇ¾úÀ»¶§
	if ( iAvatarLEVEL == 3 ) and ( iAvatarExp >= 100 ) and ( iPrevEXP < 100 ) then
		SC_CreateEventButton( 6 )
	end
	
	
		
	-- ¸ðµç Ã³¸®ÈÄ iPrevEXP¸¦ ÇöÀç °æÇèÄ¡·Î ¼¼ÆÃÇØ¼­ ´ÙÀ½ ÇÁ·¹ÀÓ¿¡¼­ °æÇèÄ¡ÀÇ º¯È­¸¦ °üÂûÇÑ´Ù.
	iPrevEXP = SC_GetAvatarEXP();
end




--SC_RunEvent : NPC ´ëÈ­Ã¢
--SC_RunEventFieldWarp : Warp gate
--SC_RunEventObjectEvent


-- ¸ÞÀÎ¿¡ÇÇ¼Òµå Äù½ºÆ® ÀÌº¥Æ® ¿ÀºêÁ§Æ®

-- ¹ö¼¸ ¾Õ ÀÌº¥Æ®
function mushroom( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object002.con", -1 );
end

-- ¸ð·¡½Ã°è ÀÌº¥Æ®
function sandglass( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object003.con", -1 );
end

-- ¹«¼­¿î Ã¥ ÀÌº¥Æ®
function horriblebook( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object004.con", -1 );
end

-- ÇÇ¶ó¹Ìµå ÀÌº¥Æ®01
function piramid01( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object005.con", -1 );	
end

-- ÇÇ¶ó¹Ìµå ÀÌº¥Æ®02
function piramid02( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object006.con", -1 );
end

-- ¿Ã»©¹ÌÀÇ ´«
function owl( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object007.con", -1 );
end

-- ÇÇ¶ó¹Ìµå ÀÌº¥Æ®03
function piramid03( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object005.con", -1 );
end

-- °í¿äÇÑ Çù°î ¸¶³ª
function mana( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object008.con", -1 );
end

-- °ÕÁö½ºÅæ
function genzistone( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventObjectEvent( iIndex, "3Ddata\\Event\\Object009.con", -1 );
end

