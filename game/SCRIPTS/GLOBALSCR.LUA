


-- 스킬 실행하는곳( 1~5 )...
function DO_SKILL ( iObject, iSkillIDX )	
end


-- 아바타 처음 생성할 때 생기는 효과
function SE_CharCreate  ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_warp_join_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- 아바타가 처음 월드에 조인할때
function SE_CharJoinZone( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_warp_join_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- 아바타 사망시
function SE_CharDie		( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_gost_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- 아바타 워프
function SE_CharWarp	( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_warp_join_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- 아바타 레벨업
function SE_CharLevelUp ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/levelup_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- 파티 레벨업
function SE_PartyLevelUp ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/party_up_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

-- 몬스터 죽을때
function SE_MobDie ( iObject )
	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_gost_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end


-- 제조 시작
function SE_StartMake ( iObject )
end
-- 제련 시작
function SE_StartUpgrade ( iObject )
end
-- 제조 성공
function SE_SuccessMake ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_success_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end


-- 제련 성공
function SE_SuccessUpgrade ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_success_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );
end

	
-- 분리/분해 성공
function SE_SuccessSeparate ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_success_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );

end 

-- 제조 실패
function SE_FailMake ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_failed_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );

end

-- 제련 실패
function SE_FailUpgrade ( iObject )

	iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_failed_01.eft" )	
	GF_EffectOnObject( iObject, iEffect );

end


-- 스킬레벨업
function SE_SkillLevelUp ( iObject )
end




-- 날씨 효과
function SE_WeatherEffect( iObject, iWeatherType )
	GF_DeleteEffectFromObject( iObject );
	-- 눈
	if iWeatherType == 1 then
		iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_snow_01.eft" )	
		GF_WeatherEffectOnObject( iObject, iEffect );
	end
	
	-- 벚꽃
	if iWeatherType == 2 then
		iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_flower_01.eft" )	
		GF_WeatherEffectOnObject( iObject, iEffect );
	end

           -- 엘던 낙엽
	if iWeatherType == 3 then
		iEffect = GF_GetEffectUseFile( "3DDATA/Effect/_ed_leaf01.eft" )	
		GF_WeatherEffectOnObject( iObject, iEffect );
	end

end