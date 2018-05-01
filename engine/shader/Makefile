# Makefile for shader compile
# by zho@korea.com
# $Header: $

OUTPUTDIR=../../data/shaders

VMACROS=\
/DV_POSITION=v0 \
/DV_NORMAL=v1 \
/DV_COLOR=v2 \
/DV_BLENDWEIGHT=v3 \
/DV_BLENDINDICES=v4 \
/DV_TANGENT=v5 \
/DV_TEXCOORD0=v6 \
/DV_TEXCOORD1=v7 \
/DV_TEXCOORD2=v8 \
/DV_TEXCOORD3=v9 \
/DC_WORLDVIEW_TM=c0 \
/DC_PROJECTION_TM=c4 \
/DC_LIGHT_TEX_TM=c8 \
/DC_LIGHT_DIFFUSE=c12 \
/DC_LIGHT_AMBIENT=c13 \
/DC_LIGHT_DIRECTION=c14 \
/DC_CAMERA_POSITION=c15 \
/DC_ONE_HALF_ZERO_MAXPOWER=c16 \
/DC_ALPHAFOG_START_END_FACTOR=c17 \
/DC_LIGHTMAP_TRANSFORM=c18 \
/DC_FOG_START_END_FACTOR=c19 \
/DC_LIGHT_POSITION=c22 \
/DC_LIGHT_ATTENUATION=c23 \
/DC_WORLD_TM=c24 \
/DC_BONESTART=30

DXSDKPATH=../DX90CSDK
VCOMP=${DXSDKPATH}/Utilities/vsa.exe /Zi ${VMACROS}
PCOMP=${DXSDKPATH}/Utilities/psa.exe 
	
all : \
${OUTPUTDIR}/glow.vso \
${OUTPUTDIR}/glow_skin.vso \
${OUTPUTDIR}/ocean.vso \
${OUTPUTDIR}/terrain.pso \
${OUTPUTDIR}/terrain_pos.vso \
${OUTPUTDIR}/terrain_rough.vso \
${OUTPUTDIR}/terrain_first.vso \
${OUTPUTDIR}/terrain_first_second.vso \
${OUTPUTDIR}/terrain_first_second_light.vso \
${OUTPUTDIR}/terrain_first_second_light_shadow.vso \
${OUTPUTDIR}/terrain_first_second_shadow.vso \
${OUTPUTDIR}/terrain_first_light.vso \
${OUTPUTDIR}/terrain_first_light_shadow.vso \
${OUTPUTDIR}/terrain_first_shadow.vso \
${OUTPUTDIR}/terrain_light.vso \
${OUTPUTDIR}/terrain_light_shadow.vso \
${OUTPUTDIR}/terrain_shadow.vso \
${OUTPUTDIR}/terrain_rough.vso \
${OUTPUTDIR}/terrain_first_fog.vso \
${OUTPUTDIR}/terrain_first_second_fog.vso \
${OUTPUTDIR}/terrain_first_second_light_fog.vso \
${OUTPUTDIR}/terrain_first_second_light_shadow_fog.vso \
${OUTPUTDIR}/terrain_first_second_shadow_fog.vso \
${OUTPUTDIR}/terrain_first_light_fog.vso \
${OUTPUTDIR}/terrain_first_light_shadow_fog.vso \
${OUTPUTDIR}/terrain_first_shadow_fog.vso \
${OUTPUTDIR}/terrain_light_fog.vso \
${OUTPUTDIR}/terrain_light_shadow_fog.vso \
${OUTPUTDIR}/bumpmap.vso ${OUTPUTDIR}/shadowmap_skin.vso \
${OUTPUTDIR}/bumpmap.vso ${OUTPUTDIR}/shadowmap.vso \
${OUTPUTDIR}/bumpmap.vso ${OUTPUTDIR}/shadowmap.pso \
${OUTPUTDIR}/bumpmap.vso ${OUTPUTDIR}/bumpmap_skin.vso \
${OUTPUTDIR}/bumpmap.pso ${OUTPUTDIR}/simple.vso \
${OUTPUTDIR}/lit_skin.vso \
${OUTPUTDIR}/simple_skin.vso \
${OUTPUTDIR}/simple.pso \
${OUTPUTDIR}/simple_skin2.vso \
${OUTPUTDIR}/simple_nolit.vso \
${OUTPUTDIR}/simple_nolit_shadow.vso \
${OUTPUTDIR}/simple_lit.vso \
${OUTPUTDIR}/simple_lit_shadow.vso \
${OUTPUTDIR}/bumpmap_tangentspace.vso \
${OUTPUTDIR}/lightmap_nolit.vso \
${OUTPUTDIR}/lightmap_nolit_first.vso \
${OUTPUTDIR}/lightmap_nolit_first_shadow.vso \
${OUTPUTDIR}/lightmap_nolit_shadow.vso \
${OUTPUTDIR}/lightmap_nolit_shadow1.vso \
${OUTPUTDIR}/lightmap_nolit_shadow2.vso \
${OUTPUTDIR}/specular_skin.vso ${OUTPUTDIR}/specular.vso \
${OUTPUTDIR}/lit_skin1.vso \
${OUTPUTDIR}/sky.vso \
${OUTPUTDIR}/terrain_plight.vso ${OUTPUTDIR}/terrain_plight.vso \

${OUTPUTDIR}/glow.vso : glow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/glow.vso glow.vsh
	
${OUTPUTDIR}/glow_skin.vso : glow_skin.vsh
	${VCOMP} /Fo${OUTPUTDIR}/glow_skin.vso glow_skin.vsh
	
${OUTPUTDIR}/ocean.vso : ocean.vsh
	${VCOMP} /Fo${OUTPUTDIR}/ocean.vso ocean.vsh	
	
${OUTPUTDIR}/terrain_pos.vso : terrain_pos.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_pos.vso terrain_pos.vsh
	
${OUTPUTDIR}/terrain_rough.vso : terrain_rough.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_rough.vso terrain_rough.vsh
	
${OUTPUTDIR}/terrain_first.vso : terrain_first.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first.vso terrain_first.vsh

${OUTPUTDIR}/terrain_first_second.vso : terrain_first_second.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second.vso terrain_first_second.vsh

${OUTPUTDIR}/terrain_first_second_light.vso : terrain_first_second_light.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second_light.vso terrain_first_second_light.vsh
		
${OUTPUTDIR}/terrain_first_second_light_shadow.vso : terrain_first_second_light_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second_light_shadow.vso terrain_first_second_light_shadow.vsh

${OUTPUTDIR}/terrain_first_second_shadow.vso : terrain_first_second_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second_shadow.vso terrain_first_second_shadow.vsh

${OUTPUTDIR}/terrain_first_light.vso : terrain_first_light.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_light.vso terrain_first_light.vsh

${OUTPUTDIR}/terrain_first_light_shadow.vso : terrain_first_light_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_light_shadow.vso terrain_first_light_shadow.vsh

${OUTPUTDIR}/terrain_first_shadow.vso : terrain_first_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_shadow.vso terrain_first_shadow.vsh

${OUTPUTDIR}/terrain_light.vso : terrain_light.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_light.vso terrain_light.vsh

${OUTPUTDIR}/terrain_light_shadow.vso : terrain_light_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_light_shadow.vso terrain_light_shadow.vsh

${OUTPUTDIR}/terrain_shadow.vso : terrain_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_shadow.vso terrain_shadow.vsh

${OUTPUTDIR}/terrain_first_fog.vso : terrain_first_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_fog.vso terrain_first_fog.vsh

${OUTPUTDIR}/terrain_first_second_fog.vso : terrain_first_second_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second_fog.vso terrain_first_second_fog.vsh

${OUTPUTDIR}/terrain_first_second_light_fog.vso : terrain_first_second_light_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second_light_fog.vso terrain_first_second_light_fog.vsh
		
${OUTPUTDIR}/terrain_first_second_light_shadow_fog.vso : terrain_first_second_light_shadow_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second_light_shadow_fog.vso terrain_first_second_light_shadow_fog.vsh

${OUTPUTDIR}/terrain_first_second_shadow_fog.vso : terrain_first_second_shadow_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_second_shadow_fog.vso terrain_first_second_shadow_fog.vsh

${OUTPUTDIR}/terrain_first_light_fog.vso : terrain_first_light_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_light_fog.vso terrain_first_light_fog.vsh

${OUTPUTDIR}/terrain_first_light_shadow_fog.vso : terrain_first_light_shadow_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_light_shadow_fog.vso terrain_first_light_shadow_fog.vsh

${OUTPUTDIR}/terrain_first_shadow_fog.vso : terrain_first_shadow_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_first_shadow_fog.vso terrain_first_shadow_fog.vsh

${OUTPUTDIR}/terrain_light_fog.vso : terrain_light_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_light_fog.vso terrain_light_fog.vsh

${OUTPUTDIR}/terrain_light_shadow_fog.vso : terrain_light_shadow_fog.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_light_shadow_fog.vso terrain_light_shadow_fog.vsh

${OUTPUTDIR}/terrain.pso : terrain.psh
	${VCOMP} /Fo${OUTPUTDIR}/terrain.pso terrain.psh

${OUTPUTDIR}/shadowmap.vso : shadowmap.vsh
	${VCOMP} /Fo${OUTPUTDIR}/shadowmap.vso shadowmap.vsh

${OUTPUTDIR}/shadowmap_skin.vso : shadowmap_skin.vsh
	${VCOMP} /Fo${OUTPUTDIR}/shadowmap_skin.vso shadowmap_skin.vsh

${OUTPUTDIR}/shadowmap.pso : shadowmap.psh
	${VCOMP} /Fo${OUTPUTDIR}/shadowmap.pso shadowmap.psh
		
${OUTPUTDIR}/bumpmap_tangentspace.vso : bumpmap_tangentspace.vsh
	${VCOMP} /Fo${OUTPUTDIR}/bumpmap_tangentspace.vso bumpmap_tangentspace.vsh

${OUTPUTDIR}/bumpmap_skin.vso : bumpmap_skin.vsh
	${VCOMP} /Fo${OUTPUTDIR}/bumpmap_skin.vso bumpmap_skin.vsh

${OUTPUTDIR}/bumpmap.vso : bumpmap.vsh
	${VCOMP} /Fo${OUTPUTDIR}/bumpmap.vso bumpmap.vsh

${OUTPUTDIR}/bumpmap.pso : bumpmap.psh
	${PCOMP} /Fo${OUTPUTDIR}/bumpmap.pso bumpmap.psh

${OUTPUTDIR}/lit_skin.vso : lit_skin.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lit_skin.vso lit_skin.vsh

${OUTPUTDIR}/lit_skin1.vso : lit_skin1.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lit_skin1.vso lit_skin1.vsh
	
${OUTPUTDIR}/simple_skin.vso : simple_skin.vsh
	${VCOMP} /Fo${OUTPUTDIR}/simple_skin.vso simple_skin.vsh
	
${OUTPUTDIR}/simple_skin2.vso : simple_skin2.vsh
	${VCOMP} /Fo${OUTPUTDIR}/simple_skin2.vso simple_skin2.vsh
	
${OUTPUTDIR}/simple.vso : simple.vsh
	${VCOMP} /Fo${OUTPUTDIR}/simple.vso simple.vsh

${OUTPUTDIR}/simple.pso : simple.psh
	${PCOMP} /Fo${OUTPUTDIR}/simple.pso simple.psh

${OUTPUTDIR}/simple_nolit.pso : simple_nolit.psh
	${PCOMP} /Fo${OUTPUTDIR}/simple_nolit.pso simple_nolit.psh

${OUTPUTDIR}/simple_nolit.vso : simple_nolit.vsh
	${VCOMP} /Fo${OUTPUTDIR}/simple_nolit.vso simple_nolit.vsh

${OUTPUTDIR}/simple_nolit_shadow.vso : simple_nolit_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/simple_nolit_shadow.vso simple_nolit_shadow.vsh

${OUTPUTDIR}/simple_lit.vso : simple_lit.vsh
	${VCOMP} /Fo${OUTPUTDIR}/simple_lit.vso simple_lit.vsh

${OUTPUTDIR}/simple_lit_shadow.vso : simple_lit_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/simple_lit_shadow.vso simple_lit_shadow.vsh

${OUTPUTDIR}/lightmap_nolit.vso : lightmap_nolit.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lightmap_nolit.vso lightmap_nolit.vsh

${OUTPUTDIR}/lightmap_nolit_first.vso : lightmap_nolit_first.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lightmap_nolit_first.vso lightmap_nolit_first.vsh

${OUTPUTDIR}/lightmap_nolit_first_shadow.vso : lightmap_nolit_first_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lightmap_nolit_first_shadow.vso lightmap_nolit_first_shadow.vsh

${OUTPUTDIR}/lightmap_nolit_shadow.vso : lightmap_nolit_shadow.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lightmap_nolit_shadow.vso lightmap_nolit_shadow.vsh

${OUTPUTDIR}/lightmap_nolit_shadow1.vso : lightmap_nolit_shadow1.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lightmap_nolit_shadow1.vso lightmap_nolit_shadow1.vsh

${OUTPUTDIR}/lightmap_nolit_shadow2.vso : lightmap_nolit_shadow2.vsh
	${VCOMP} /Fo${OUTPUTDIR}/lightmap_nolit_shadow2.vso lightmap_nolit_shadow2.vsh

${OUTPUTDIR}/specular_skin.vso : specular_skin.vsh
	${VCOMP} /Fo${OUTPUTDIR}/specular_skin.vso specular_skin.vsh

${OUTPUTDIR}/specular.vso : specular.vsh
	${VCOMP} /Fo${OUTPUTDIR}/specular.vso specular.vsh

${OUTPUTDIR}/sky.vso : sky.vsh
	${VCOMP} /Fo${OUTPUTDIR}/sky.vso sky.vsh

${OUTPUTDIR}/terrain_plight.vso : terrain_plight.vsh
	${VCOMP} /Fo${OUTPUTDIR}/terrain_plight.vso terrain_plight.vsh
	
clean :
	rm -f ${OUTPUTDIR}/*.vso
	rm -f ${OUTPUTDIR}/*.pso


