---============================
--- shader description file
---
--- by zho
--- 2004.06.03 zho added terrain shader format with first + second + shadowmap
---============================

---============================
--- shader path
---============================
shader_path = "shaders/"

---============================
--- for lit skined mesh
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_BLEND_WEIGHT + ZZ_VF_BLEND_INDEX + ZZ_VF_UV0;
shader_lit_skin         = loadShader( "shader_lit_skin", shader_path.."lit_skin.vso", shader_path.."simple.pso", 1, vertex_format )
shader_specular_skin    = loadShader( "shader_specular_skin", shader_path.."specular_skin.vso", shader_path.."simple.pso", 1, vertex_format )
shader_nolit_skin       = loadShader( "shader_nolit_skin", shader_path.."lit_skin.vso", shader_path.."simple.pso", 1, vertex_format )
setShaderFormat( shader_lit_skin, shader_path.."lit_skin.vso", 0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_nolit_skin, shader_path.."lit_skin.vso", 0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_specular_skin, shader_path.."lit_skin.vso", 0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_specular_skin, shader_path.."specular_skin.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SPECULARMAP )

vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_UV0;
shader_lit              = loadShader( "shader_lit", shader_path.."simple_lit.vso", shader_path.."simple.pso", 1, vertex_format )
shader_specular         = loadShader( "shader_specular", shader_path.."specular.vso", shader_path.."simple.pso", 1, vertex_format )
shader_nolit            = loadShader( "shader_nolit", shader_path.."simple_nolit.vso", shader_path.."simple.pso", 1, vertex_format )
setShaderFormat( shader_lit, shader_path.."simple_lit.vso", 0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_lit, shader_path.."simple_lit_shadow.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_nolit, shader_path.."simple_nolit.vso", 0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_nolit, shader_path.."simple_nolit_shadow.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_specular, shader_path.."simple_lit.vso", 0, SHADER_FORMAT_FIRSTMAP)
setShaderFormat( shader_specular, shader_path.."specular.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SPECULARMAP)

---============================
--- lightmap shader
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_UV0 + ZZ_VF_UV1
shader_lightmap_nolit   = loadShader( "shader_lightmap_nolit", shader_path.."lightmap_nolit_shadow.vso", shader_path.."simple.pso", 1, vertex_format )

---vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_UV0 + ZZ_VF_UV1
---shader_lightmap_nolit_shadow   = loadShader( "shader_lightmap_nolit_shadow", shader_path.."lightmap_nolit_shadow.vso", shader_path.."simple.pso", 1, vertex_format )

setShaderFormat( shader_lightmap_nolit, shader_path.."lightmap_nolit_first.vso", 0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_lightmap_nolit, shader_path.."lightmap_nolit_shadow1.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_LIGHTMAP )
setShaderFormat( shader_lightmap_nolit, shader_path.."lightmap_nolit_shadow2.vso", 0, SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_lightmap_nolit, shader_path.."lightmap_nolit_first_shadow.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_lightmap_nolit, shader_path.."lightmap_nolit_shadow.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_SHADOWMAP )

---============================
--- glow shader
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_UV0;
shader_glow             = loadShader( "shader_glow", shader_path.."glow.vso", shader_path.."simple.pso", 1, vertex_format )

vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_BLEND_WEIGHT + ZZ_VF_BLEND_INDEX + ZZ_VF_UV0;
shader_glow_skin        = loadShader( "shader_glow_skin", shader_path.."glow_skin.vso", shader_path.."simple.pso", 1, vertex_format )

---============================
--- shadowmap shader
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_UV0
shader_shadowmap        = loadShader( "shader_shadowmap", shader_path.."shadowmap.vso", shader_path.."shadowmap.pso", 1, vertex_format )

vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_BLEND_WEIGHT + ZZ_VF_BLEND_INDEX + ZZ_VF_UV0;
shader_shadowmap_skin   = loadShader( "shader_shadowmap_skin", shader_path.."shadowmap_skin.vso", shader_path.."shadowmap.pso", 1, vertex_format )

---============================
--- rough terrain shader
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_UV0 + ZZ_VF_UV1 + ZZ_VF_UV2
shader_terrain_rough    = loadShader( "shader_terrain_rough", shader_path.."terrain_rough.vso", shader_path.."terrain.pso", 1, vertex_format )
setShaderFormat( shader_terrain_rough, shader_path.."terrain_rough.vso", 0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_terrain_rough, shader_path.."terrain_rough.vso", 0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_FOG )

---============================
--- terrain shader uses diffusemap and lightmap
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_UV0 + ZZ_VF_UV1 + ZZ_VF_UV2
shader_terrain          = loadShader( "shader_terrain", shader_path.."terrain_first_second_light_shadow.vso", shader_path.."terrain.pso", 1, vertex_format )

setShaderFormat( shader_terrain, shader_path.."terrain_pos.vso",                        0, SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first.vso",                      0, SHADER_FORMAT_FIRSTMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second.vso",               0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second_light.vso",         0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP + SHADER_FORMAT_LIGHTMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second_light_shadow.vso",  0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP + SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second_shadow.vso",        0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_first_light.vso",                0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_LIGHTMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_first_light_shadow.vso",         0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_first_shadow.vso",               0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_light.vso",                      0, SHADER_FORMAT_LIGHTMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_light_shadow.vso",               0, SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_SHADOWMAP )
setShaderFormat( shader_terrain, shader_path.."terrain_shadow.vso",                     0, SHADER_FORMAT_SHADOWMAP )

setShaderFormat( shader_terrain, shader_path.."terrain_first_fog.vso",                      0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second_fog.vso",               0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second_light_fog.vso",         0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP + SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second_light_shadow_fog.vso",  0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP + SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_SHADOWMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first_second_shadow_fog.vso",        0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SECONDMAP + SHADER_FORMAT_SHADOWMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first_light_fog.vso",                0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first_light_shadow_fog.vso",         0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_SHADOWMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_first_shadow_fog.vso",               0, SHADER_FORMAT_FIRSTMAP + SHADER_FORMAT_SHADOWMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_light_fog.vso",                      0, SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_FOG )
setShaderFormat( shader_terrain, shader_path.."terrain_light_shadow_fog.vso",               0, SHADER_FORMAT_LIGHTMAP + SHADER_FORMAT_SHADOWMAP + SHADER_FORMAT_FOG )


---============================
--- terrain shader uses diffusemap and lightmap
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_UV0
shader_ocean            = loadShader( "shader_ocean", shader_path.."ocean.vso", 0, 1, vertex_format )

---============================
--- sky shader
---============================
vertex_format           = ZZ_VF_POSITION + ZZ_VF_NORMAL + ZZ_VF_UV0
shader_sky            = loadShader( "shader_sky", shader_path.."sky.vso", 0, 1, vertex_format )
