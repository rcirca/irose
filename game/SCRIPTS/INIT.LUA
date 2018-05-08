--=================--
-- init script
--=================--
useVertexShader             ( 1 )
usePixelShader              ( 0 )
useWireMode                 ( 0 )
useCull                     ( 1 )
useObjectSorting            ( 1 )
usePolygonSorting           ( 0 )
useHardwareVertexProcessing ( 1 )
useMultiPass                ( 0 )
useShadowmap                ( 1 )
useLightmap                 ( 1 )
useFileTimeStamp            ( 0 )
useGlow                     ( 1 )
useFullSceneGlow            ( 0 )
useTerrainLOD               ( 0 )

setScreen ( 640, 480, 32, 0 ) -- width, depth, bpp, fullscreen
setBuffer ( 640, 480, 32 ) -- affected only in windowed mode
setTimeDelay ( 0 )

setClearColor ( 0, 0, 0 ) -- r, g, b as [0...1]
setShadowmapSize ( 256 )
setShadowmapBlurType( 1 ) -- none = 0; blur once = 1; blur twice = 2
setShadowmapColor( 0.2, 0.2, 0.2 ) -- shadow color
setGlowmapSize ( 256 )
setGlowColor ( 0.06, 0.06, 0.06 )
glowscale = 0.015
setFullSceneGlowColor ( glowscale, glowscale, glowscale )
setFullSceneGlowType( 0 )
setMipmapFilter ( 2 ) -- none = 0, point = 1, linear = 2, anisotropic = 3 (only minfilter)
setMipmapLevel ( 3 ) -- -1(텍스쳐내부정의), 0 (풀레벨), 1(밉맵없음), 2(두개), 3(세개)...
setMinFilter ( 3 ) -- point = 1, linear = 2 
setMagFilter ( 2 ) -- point = 1, linear = 2
setFullSceneAntiAliasing ( 0 )
setUseTimeWeight ( 1 )
setUseFixedFramerate ( 0 )
setFramerateRange( 15, 1000 ) -- 최소, 최대 프레임률 제한
setLightmapBlendStyle ( 5 ) -- 4 : modulate, 5 : modulate2x, 6 : modulate4x
setCameraTransparency( .3 )
setDrawShadowmapViewport( 0 )
setTextureLoadingScale( 0 ) -- 텍스쳐 로딩시 강제로 텍스쳐 사이즈 변경 : 저사양 그래픽카드에서는 1이나 2의 값을 사용
setTextureLoadingFormat( 0 ) -- 텍스쳐 로싱시 텍스쳐 포맷 강체로 변경 : 0(원래텍스쳐포맷, default), 1(16비트), 2(압축)
setDisplayQualityLevel( 0 ) -- 엔진 퀄리티 레벨 : 0(자동), 1(최상급-설정파일대로), 2, 3(중간품질), 4, 5(최하급품질)
useVSync( 1 ) -- 풀스크린시 수직 동기 여부. 0으로 하면 풀스크린모드에서 모니터 주파수율보다 더 높은 fps가 가능.
---setMonitorRefreshRate( 60 )
---setAdapter( 0 )
---useDebugDisplay( 1 ) -- 디버깅 메세지 화면에 출력 여부

useMotionInterpolation( 1 ) -- 모션 보간 여부
setMotionInterpolationRange( 20 ) -- 모션 보간이 적용되는 범위. 단위 미터. 
---setDataPath( "d:/online2003/data/" )

setLazyBufferSize( 1000, 1000, 1000, 1000 ) -- 점진적 로딩 버퍼 사이즈. 디폴트는 각각 300. 순서:(텍스쳐, 일반메시, 지형메시, 바다메시)
