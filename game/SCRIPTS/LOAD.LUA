---------------------------
-- lua script to load scene
-- 
-- written by zho@korea.com
-- 2002.10.23
-- version : 02-11-29-01
---------------------------

doScript( "scripts/load_shader.lua" )
doScript( "scripts/load_light.lua" )
doScript( "scripts/load_camera.lua" )
doScript( "scripts/load_fog.lua" )

setGammaValue( 0.5 )
---doScript( "testscripts/test_scene_one.lua" )

---doScript( "scripts/load_sky.lua" )

---doScript( "scripts/load_scene.lua" )
---doScript( "test/test_tree.lua" )
---doScript( "test/test_pick.lua" )
---doScript( "test/test_load.lua" )
