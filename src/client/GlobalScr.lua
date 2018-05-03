-- 파일 이름으로 이펙트를 얻는다.
-- int	GF_GetEffectUseFile ( ZSTRING szEffectFile )

-- LIST_EFFECT.stb 의 index로 이펙트를 얻는다.
-- int	GF_GetEffectUseIndex ( int iEffectIndex )

-- iObjec위에  effect를 표시한다.
-- void GF_EffectOnObject ( int iObject, int iEffectHash )

-- 케릭터 생성시 호출되는곳
function ON_CHAR_APPEAR ( iObject )
	--iEffect = GF_GetEffectUseFile( "3DDATA/Effect/apear_01.eft" )
	--GF_EffectOnObject( iObject, iEffect );
end

-- 레벨없이 호출되는곳
function ON_CHAR_LEVEL_UP ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/levelup_01.eft" )
	GF_playSound( "Sound/System/Levelup_1.wav", 1, 1 );
	GF_EffectOnObject( iObject, iEffect );
end
	
-- 소모 아이템 사용할때...
function ON_USE_ITEM ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/recover_01.eft" )
	GF_EffectOnObject( iObject, iEffect );
end


-- 스킬 실행하는곳( 1~5 )...
function DO_SKILL ( iObject, iSkillIDX )
	--iEffect = GF_GetEffectUseFile( "3DDATA/Effect/apear_01.eft" )

	--iTarget = GF_GetTarget( iObject );

	--GF_EffectOnObject( iTarget, iEffect );
	--GF_EffectOnObject( iObject, iEffect );

	--	iMotion = GF_GetMotionUseFile( "3DDATA/MOTION/AVATAR/empty_jump2_m1.ZMO" )

	--GF_SetMotion( iObject, iMotion );
end


