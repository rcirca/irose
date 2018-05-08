-- light
light = loadLight( "light_01" )
setLight( light, "specular", .5, .5, .5 )

-- addsign
---setLight( light, "ambient", .4, .4, .4 )
---setLight( light, "diffuse", .3, .3, .3 )
---setLight( light, "direction", .5, .5, .5 )
-- modulate



---setLight( light, "ambient", .95 , .92, .88 )

setLight( light, "ambient", .86 , .83 , .8  )

setLight( light, "diffuse", 1, 1, 1 )
setLight( light, "direction", .5, .5, .5 )

setDefaultLight ( light )




--/////////////////////////////////////////////////////
-- character light
-- light
char_light = loadLight( "light_02" )
setLight( char_light, "specular", .5, .5, .5 )

setLight( char_light, "ambient", .86 , .83 , .8  )

setLight( char_light, "diffuse", 1, 1, 1 )
setLight( char_light, "direction", .5, .5, .5 )
