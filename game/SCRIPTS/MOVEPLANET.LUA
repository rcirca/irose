

bFadeIn = 0;
bWhiteFadeIn = 0;
bEndFadeOut = 0;



-- 기본 비공정에 모션 세팅 및 카메라에 모션 세팅
function InitPlanetMoveWorld( iPlanet )
	ClearMovePlanetCutScene();
	
	-- 각생성별 세팅할 내용들
	
	-- 쥬논
	if iPlanet == 0 then
	end
	
	-- 루나
	if iPlanet == 1 then	
	end
	
	-- 공통적으로 설정할 내용들
	AttachMotionToEnvObject();
end

function AttachMotionToEnvObject()
	
	-- Camera
	hCameraMotion = SC_FindNode( "MovePlanet_Camera01" );	
	if hCameraMotion == 0 then
		hCameraMotion = SC_LoadMotion( "MovePlanet_Camera01", "3DData\\CUTSCENE\\WARP01\\Camera01.ZMO", 1, 4, 3, 1, 0 );
	end
	
	camera  = SC_FindNode( "motion_camera")
	if camera ~= 0 then
		SC_AttachMotion( camera, hCameraMotion );
		SC_ControlAnimation( camera, 1 );
		SC_SetCameraDefault( camera );
		SC_LogString( "SetCameraDefault [ motion_camera ]" );
	end

	
	
end

function ProcMovePlanetCutScene( iPlayTime )
	
	if iPlayTime > 560 then
		SC_ChangeState( 12 );
		bFadeIn = 0;
		bWhiteFadeIn = 0;
		bEndFadeOut = 0;	
	end
		
	if iPlayTime > 0 then
		if bFadeIn == 0 then
			SC_ScreenFadeInStart( 0, 0, 2, 0, 0, 0 ); 	
			bFadeIn = 1;
		end
	end	
	
	if bWhiteFadeIn == 0 then
		if iPlayTime > 122 then		
			SC_ScreenFadeInStart( 2.2, 1.6, 2.2, 255, 255, 255 ); 	
			bWhiteFadeIn = 1;
		end
	end
	
	if bEndFadeOut == 0 then
		if iPlayTime > 496 then		
			SC_ScreenFadeInStart( 5.4, 1, 0, 255, 255, 255 ); 	
			bEndFadeOut = 1;
		end
	end
	
	--====================================================================
	-- 블랙홀 이펙트
	--====================================================================
	
	if bAttachBlackHall == 0 then
		if iPlayTime > 210 then
			bAttachBlackHall = 1;
			hBlackHallObj = SC_GetEventObject( 8, 1 );
			if hBlackHallObj ~= 0 then
				iEffect = SC_GetEffectUseFile( "3DDATA/Effect/_blackholl_01.eft" )	
				SC_EffectOnObject( hBlackHallObj, iEffect );

			end

		end
	end 
end


function ClearMovePlanetCutScene()
	bFadeIn = 0;
	bWhiteFadeIn = 0;
	bEndFadeOut = 0;
	bAttachBlackHall = 0;
	--SC_LogString( "FadeIn"..bFadeIn.."WhiteFadeIn"..bWhiteFadeIn.."EndFadeOut"..bEndFadeOut.."\n" );
end