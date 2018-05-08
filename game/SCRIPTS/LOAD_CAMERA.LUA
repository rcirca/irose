-----------------------------------
-- camera script
--
-- 2004.06.24 zho
-----------------------------------

-- load light camera for shadow casting
light_camera = loadCamera( ".camera_light", "cameras/light_camera.zca", 0 )
setCameraOrthogonal( light_camera, 4000, 4000, 100, 10000 )

-- load avatar follow camera
avatar_camera = loadCamera( "avatar_camera", "cameras/camera01.zca", 0 )

-- load motion camera
camera_motion = loadMotion( ".camera_motion", "cameras/camera01.zmo", 1, 4, 3, 1, 0 )
motion_camera = loadCamera( "motion_camera", "cameras/camera01.zca", camera_motion )
controlAnimatable( motion_camera, 1 )

-- set default/light camera
setCameraDefault( motion_camera )
setCameraLight( light_camera )
