-- Event_Object script

-- @iObject 오브젝트 번호
-- @iState  현재 상태
-- @bJustEnd 모션없이 바로 모션의 끝으로 갈것인가?
function Warp_Gate01 ( iObject, iState, bJustEnd )
	hObj = SC_GetEventObject( iObject, 1 );
	if hObj ~= nil then
		hMotion = 0;
	
		if iState == 0 then
			hMotion = SC_FindNode( "Warp_Gate01_01" );
			if hMotion == 0 then
				hMotion = SC_LoadMotion( "Warp_Gate01_01", 
						"3DData\\Special\\warp_gate01\\warp_gate01.ZMO",
						0, 4, 3, 1, 0 );
			end
		end
		
		if iState == 1 then		
			hMotion = SC_FindNode( "Warp_Gate01_02" );
			if hMotion == 0 then
				hMotion = SC_LoadMotion( "Warp_Gate01_02", 
						"3DData\\Special\\warp_gate01\\warp_gate02.ZMO",
						0, 4, 3, 1, 0 );			
			end
		end
		
		if hMotion ~= nil then
			SC_AttachMotion( hObj, hMotion );	
			SC_ControlAnimation( hObj, 1 );
			SC_SetRepeatCount( hObj, 1 );
			
			if bJustEnd == 1 then
				iTotalFrame = SC_GetTotalFrame( hMotion );
				SC_SetMotionFrame( hObj, iTotalFrame - 1 );
				SC_LogString( "bJustEnd is true[Warp_Gate01]" );
			end
		end
		
	end	
end

-- 루나에서 마을중앙에서 필드로 워프하는 이벤트 오브젝트
function Lunar_Warp_Gate01( iObject, iState, bJustEnd )
	iIndex = SC_GetEventObjectIndex( iObject );
	SC_RunEventFieldWarp( iIndex, "3Ddata\\Event\\Object001.con", -1 );
end