/** 
 * @file zz_interface.h
 * @brief znzin external interface class 
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    28-nov-2002
 *
 * $Header: /engine/include/zz_interface.h 69    04-06-24 2:12p Zho $
 * $History: zz_interface.h $
 * 
 * *****************  Version 69  *****************
 * User: Zho          Date: 04-06-24   Time: 2:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 68  *****************
 * User: Zho          Date: 04-06-22   Time: 9:50a
 * Updated in $/engine/include
 * 
 * *****************  Version 67  *****************
 * User: Zho          Date: 04-06-09   Time: 2:30p
 * Updated in $/engine/include
 * 
 * *****************  Version 66  *****************
 * User: Zho          Date: 04-06-08   Time: 10:35a
 * Updated in $/engine/include
 * 
 * *****************  Version 65  *****************
 * User: Zho          Date: 04-06-03   Time: 11:18a
 * Updated in $/engine/include
 * glow code updated.
 * 
 * *****************  Version 64  *****************
 * User: Zho          Date: 04-06-02   Time: 8:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 63  *****************
 * User: Zho          Date: 04-06-02   Time: 9:43a
 * Updated in $/engine/include
 * after mesh format changed
 * 
 * *****************  Version 62  *****************
 * User: Zho          Date: 04-05-29   Time: 10:01p
 * Updated in $/engine/include
 * 
 * *****************  Version 61  *****************
 * User: Zho          Date: 04-05-29   Time: 9:28p
 * Updated in $/engine/include
 * 
 * *****************  Version 60  *****************
 * User: Zho          Date: 04-05-28   Time: 4:57p
 * Updated in $/engine/include
 * 
 * *****************  Version 59  *****************
 * User: Zho          Date: 04-05-28   Time: 4:52p
 * Updated in $/engine/include
 * 
 * *****************  Version 58  *****************
 * User: Zho          Date: 04-05-28   Time: 4:32p
 * Updated in $/engine/include
 * 
 * *****************  Version 57  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/include
 * 
 * *****************  Version 56  *****************
 * User: Zho          Date: 04-05-19   Time: 8:13p
 * Updated in $/engine/include
 * 
 * *****************  Version 55  *****************
 * User: Zho          Date: 04-04-27   Time: 6:13p
 * Updated in $/engine/include
 * 
 * *****************  Version 54  *****************
 * User: Zho          Date: 04-04-27   Time: 6:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 53  *****************
 * User: Zho          Date: 04-04-25   Time: 1:18p
 * Updated in $/engine/include
 * 
 * *****************  Version 52  *****************
 * User: Zho          Date: 04-04-22   Time: 9:39p
 * Updated in $/engine/include
 * 
 * *****************  Version 51  *****************
 * User: Zho          Date: 04-04-21   Time: 4:17p
 * Updated in $/engine/include
 * 
 * *****************  Version 50  *****************
 * User: Zho          Date: 04-04-22   Time: 8:09p
 * Updated in $/engine/include
 * 
 * *****************  Version 49  *****************
 * User: Zho          Date: 04-04-19   Time: 3:15p
 * Updated in $/engine/include
 * 
 * *****************  Version 48  *****************
 * User: Zho          Date: 04-04-15   Time: 6:20p
 * Updated in $/engine/include
 * 
 * *****************  Version 47  *****************
 * User: Zho          Date: 04-04-15   Time: 11:22a
 * Updated in $/engine/include
 * 
 * *****************  Version 46  *****************
 * User: Zho          Date: 04-04-09   Time: 2:17p
 * Updated in $/engine/include
 * 
 * *****************  Version 45  *****************
 * User: Zho          Date: 04-04-07   Time: 5:23p
 * Updated in $/engine/include
 * 
 * *****************  Version 44  *****************
 * User: Zho          Date: 04-03-31   Time: 10:23a
 * Updated in $/engine/include
 * 
 * *****************  Version 43  *****************
 * User: Zho          Date: 04-03-25   Time: 11:18a
 * Updated in $/engine/include
 * 
 * *****************  Version 42  *****************
 * User: Zho          Date: 04-03-25   Time: 10:33a
 * Updated in $/engine/include
 * 
 * *****************  Version 41  *****************
 * User: Zho          Date: 04-03-23   Time: 10:46a
 * Updated in $/engine/include
 * Added getTextureSize()
 * 
 * *****************  Version 40  *****************
 * User: Zho          Date: 04-03-19   Time: 5:27p
 * Updated in $/engine/include
 * 
 * *****************  Version 39  *****************
 * User: Zho          Date: 04-03-18   Time: 9:04p
 * Updated in $/engine/include
 * 
 * *****************  Version 38  *****************
 * User: Zho          Date: 04-03-18   Time: 3:18p
 * Updated in $/engine/include
 * 
 * *****************  Version 37  *****************
 * User: Zho          Date: 04-03-16   Time: 5:57p
 * Updated in $/engine/include
 * Added visible range.
 * 
 * *****************  Version 36  *****************
 * User: Zho          Date: 04-03-11   Time: 2:48p
 * Updated in $/engine/include
 * To delayed load ocean textures and all texture with pool
 * 
 * *****************  Version 35  *****************
 * User: Zho          Date: 04-03-05   Time: 1:41p
 * Updated in $/engine/include
 * Redesigned vfs_thread.
 * 
 * *****************  Version 34  *****************
 * User: Zho          Date: 04-02-21   Time: 6:44p
 * Updated in $/engine/include
 * Added vfs_thread statistics.
 * 
 * *****************  Version 33  *****************
 * User: Zho          Date: 04-02-20   Time: 12:17p
 * Updated in $/engine/include
 * 
 * *****************  Version 32  *****************
 * User: Zho          Date: 04-02-17   Time: 2:09p
 * Updated in $/engine/include
 * Added setVisibilityRecursive().
 * 
 * *****************  Version 31  *****************
 * User: Zho          Date: 04-02-09   Time: 8:41p
 * Updated in $/engine/include
 * 
 * *****************  Version 30  *****************
 * User: Zho          Date: 04-02-09   Time: 7:42p
 * Updated in $/engine/include
 * 
 * *****************  Version 29  *****************
 * User: Zho          Date: 04-02-04   Time: 5:08p
 * Updated in $/engine/include
 * 
 * *****************  Version 28  *****************
 * User: Zho          Date: 04-02-03   Time: 6:25p
 * Updated in $/engine/include
 * Added getCollisionLevel and getVisibleAABB
 * 
 * *****************  Version 27  *****************
 * User: Zho          Date: 04-01-27   Time: 7:00p
 * Updated in $/engine/include
 * Added SkyMaterial interface.
 * 
 * *****************  Version 26  *****************
 * User: Zho          Date: 04-01-20   Time: 4:07p
 * Updated in $/engine/include
 * 
 * *****************  Version 25  *****************
 * User: Zho          Date: 04-01-10   Time: 2:56p
 * Updated in $/engine/include
 * Added getCameraXXX ()
 * 
 * *****************  Version 24  *****************
 * User: Zho          Date: 04-01-07   Time: 5:54p
 * Updated in $/engine/include
 * Added polygon normal calculation getting intersection point
 * 
 * *****************  Version 23  *****************
 * User: Zho          Date: 04-01-07   Time: 9:36a
 * Updated in $/engine/include
 * Added 3D sound system.
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 04-01-05   Time: 8:13p
 * Updated in $/engine/include
 * getModelShiftPosition() Added
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 04-01-02   Time: 1:37p
 * Updated in $/engine/include
 * ocean 타입 추가
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 03-12-30   Time: 6:39p
 * Updated in $/engine/include
 * getWorldObjectHeightInScene 수정
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 03-12-30   Time: 12:06p
 * Updated in $/engine/include
 * getWorldObjectHeightInScene() 추가
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 03-12-29   Time: 11:30p
 * Updated in $/engine/include
 * Added getWorldHeightInScene().
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 03-12-26   Time: 6:04p
 * Updated in $/engine/include
 * applygravity () added
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 03-12-24   Time: 4:48p
 * Updated in $/engine/include
 * Added getModelRightPosition()
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 03-12-24   Time: 1:06p
 * Updated in $/engine/include
 * updateSceneTransform() 추가
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 03-12-23   Time: 5:20p
 * Updated in $/engine/include
 * added savePrevPosition(), getPrevPosition(), intersectTriSphereMoving()
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 03-12-22   Time: 9:19p
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 03-12-19   Time: 3:26a
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 03-12-16   Time: 9:22p
 * Updated in $/engine/include
 * added controlTrail comments
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-12-16   Time: 3:21p
 * Updated in $/engine/include
 * renamed blend_duration to interp_interval
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-12-12   Time: 7:50p
 * Updated in $/engine/include
 * color format added
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-11   Time: 11:37a
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-10   Time: 9:48p
 * Updated in $/engine/include
 * loadTextureWithPool added
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/include
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-03   Time: 12:34p
 * Updated in $/engine/include
 * added getFontHeight()
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 4:24p
 * Updated in $/engine/include
 * drawFontLater(), drawFontLaterf() added
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-02   Time: 12:06a
 * Updated in $/engine/include
 * fileReadFrom(), fileReadWord(), fileClose() added for script file
 * loading
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-01   Time: 10:01a
 * Updated in $/engine/include
 * some comments added
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 157  *****************
 * User: Zho          Date: 03-11-29   Time: 6:25a
 * Updated in $/znzin11/engine/include
 * loadText(), unloadText(), unloadAllText() added
 * 
 * *****************  Version 156  *****************
 * User: Zho          Date: 03-11-28   Time: 1:43a
 * Updated in $/znzin11/engine/include
 * added file system setting
 * 
 * *****************  Version 155  *****************
 * User: Zho          Date: 03-11-27   Time: 5:45p
 * Updated in $/znzin11/engine/include
 * getTexturePointer() 추가
 * 
 * *****************  Version 154  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/include
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 * 
 * *****************  Version 153  *****************
 * User: Zho          Date: 03-11-26   Time: 9:12p
 * Updated in $/znzin11/engine/include
 * added font & sprite thing
 * 
 * *****************  Version 152  *****************
 * User: Zho          Date: 03-11-25   Time: 10:55p
 * Updated in $/znzin11/engine/include
 * 
 * *****************  Version 151  *****************
 * User: Zho          Date: 03-11-25   Time: 2:08p
 * Updated in $/znzin11/engine/include
 * first added sprite, font, texture external interface
 * 
 * *****************  Version 150  *****************
 * User: Zho          Date: 03-11-19   Time: 4:39p
 * Updated in $/znzin11/engine/include
 *
 */

#pragma warning(disable : 4786)

#ifndef __ZZ_INTERFACE_H__
#define __ZZ_INTERFACE_H__

#include "zz_type.h"

// windows and d3d type things
#ifdef WIN32
#include <windows.h>
#include <d3dx9math.h>

typedef RECT ZZ_RECT;
typedef D3DVECTOR ZZ_VECTOR;
typedef D3DCOLOR ZZ_COLOR;
typedef DWORD ZZ_DWORD;
typedef SIZE ZZ_SIZE;

#else // WIN32

// same as WIN32 RECT and zz_rect in zz_type.h
typedef struct _ZZ_RECT {
  long left; 
  long top; 
  long right; 
  long bottom; 
} ZZ_RECT; 

// same as DIRECTX D3DVECTOR and vec3 in zz_algebra.h
typedef struct _ZZ_VECTOR {
    float x;
    float y;
    float z;
} ZZ_VECTOR;

// same as D3DCOLOR
typedef unsigned long ZZ_COLOR; // same as WIN32 DWORD
typedef unsigned long ZZ_DWORD;
typedef zz_size ZZ_SIZE;
#endif // WIN32

#ifdef __cplusplus
extern "C" {
#endif
 
#ifdef ZZ_DLL_EXPORT // define this for znzin project to be exported
#define ZZ_DLL __declspec(dllexport)
#elif ZZ_STATIC_USE  // define this for project which uses znzin as static library
#define ZZ_DLL       // no define macros needed for project which uses znzin as dll library
#else // import
#define ZZ_DLL __declspec(dllimport)
#endif

// 스크립트에 노출시킬 함수는 ZZ_SCRIPT 를 사용
#define ZZ_SCRIPT ZZ_DLL

//--------------------------------------------------------------------------------
// 작성시 주의 사항:
// - 스크립트와 DLL 모두 사용할 함수는 ZZ_SCRIPT 를 사용
// - DLL 에만 사용할 함수는 ZZ_DLL 을 사용
// - 함수의 ( 와 ); 기호는 좌우에 공백을 꼭 삽입
// - 스크립트에 사용할 수 있는 함수의 인자와 리턴타입은 다음으로 제한
//   - (void, int, float, HNODE, ZSTRING)
// - ZZ_SCRIPT 는 라인의 제일 처음에 위치해야 함
// - 이 파일이 갱신되면, export_interface.lua 스크립트를 실행시켜야 함
//--------------------------------------------------------------------------------

// 포인터를 unsigned int로 사용하기 위해
// 주의! 32-bit 머신에서만 동작함
typedef unsigned int HNODE;

typedef const char * ZSTRING;

// 비져블 클래스 타입 핸들
enum zz_class_type {
	ZZ_TYPE_NONE       = 0,
	ZZ_TYPE_VISIBLE    = (1 << 0),
	ZZ_TYPE_ANIMATABLE = (1 << 1),
	ZZ_TYPE_MORPHER    = (1 << 2),
	ZZ_TYPE_SKELETON   = (1 << 3),
	ZZ_TYPE_MODEL      = (1 << 4),
	ZZ_TYPE_PARTICLE   = (1 << 5),
	ZZ_TYPE_TRAIL      = (1 << 6),
	ZZ_TYPE_TERRAIN    = (1 << 7),
	ZZ_TYPE_OCEAN = (1 << 8)
};

// 파라미터 타입
enum zz_param_type {
	ZZ_PARAM_NONE = 0,
	ZZ_PARAM_INT = 1,
	ZZ_PARAM_FLOAT = 2,
	ZZ_PARAM_FLOAT3 = 3,
	ZZ_PARAM_STRING = 4,
	ZZ_PARAM_UINT = 5, // unsigned int
	ZZ_PARAM_HNODE = 6,
	ZZ_PARAM_END = 999  // TERMINATOR
};

// 키정의
enum zz_key_type {
	ZZ_KEY_NONE = 0,
	ZZ_KEY_ESC = 1,
	ZZ_KEY_LEFT,
	ZZ_KEY_RIGHT,
	ZZ_KEY_UP,
	ZZ_KEY_DOWN,
	ZZ_KEY_INSERT,
	ZZ_KEY_DELETE,
	ZZ_KEY_HOME,
	ZZ_KEY_END,
	ZZ_KEY_PAGEUP,
	ZZ_KEY_PAGEDOWN,
	ZZ_KEY_F1,
	ZZ_KEY_F2,
	ZZ_KEY_F3,
	ZZ_KEY_F4,
	ZZ_KEY_F5,
	ZZ_KEY_F6,
	ZZ_KEY_F7,
	ZZ_KEY_F8,
	ZZ_KEY_F9,
	ZZ_KEY_F10,
	ZZ_KEY_F11,
	ZZ_KEY_F12,
	ZZ_KEY_SHIFT,
	ZZ_KEY_CTRL,
	ZZ_KEY_ALT,

	ZZ_KEY_0,
	ZZ_KEY_1,
	ZZ_KEY_2,
	ZZ_KEY_3,
	ZZ_KEY_4,
	ZZ_KEY_5,
	ZZ_KEY_6,
	ZZ_KEY_7,
	ZZ_KEY_8,
	ZZ_KEY_9,
	
	ZZ_KEY_A,
	ZZ_KEY_B,
	ZZ_KEY_C,
	ZZ_KEY_D,
	ZZ_KEY_E,
	ZZ_KEY_F,
	ZZ_KEY_G,
	ZZ_KEY_H,
	ZZ_KEY_I,
	ZZ_KEY_J,
	ZZ_KEY_K,
	ZZ_KEY_L,
	ZZ_KEY_M,
	ZZ_KEY_N,
	ZZ_KEY_O,
	ZZ_KEY_P,
	ZZ_KEY_Q,
	ZZ_KEY_R,
	ZZ_KEY_S,
	ZZ_KEY_T,
	ZZ_KEY_U,
	ZZ_KEY_V,
	ZZ_KEY_W,
	ZZ_KEY_X,
	ZZ_KEY_Y,
	ZZ_KEY_Z,
};

// 
ZZ_SCRIPT
int setScreen (
	int Width,
	int Height,
	int Depth,
	int bUseFullScreen
);

ZZ_SCRIPT
void setBuffer (
	int Width,
	int Height,
	int Depth
);

//// resize back buffer
//ZZ_SCRIPT
//int resizeBuffer ( int Width, int Height );

ZZ_SCRIPT
int useHardwareVertexProcessing ( int bUse );

ZZ_SCRIPT
int useVertexShader ( int bUse );

ZZ_SCRIPT
int usePixelShader ( int bUse );

ZZ_SCRIPT
int useWireMode ( int bUse );

ZZ_SCRIPT
void setClearColor ( float fClearColorX, float fClearColorY, float fClearColorZ );

/// 쉐도우맵의 가로세로 크기
/// @return 이전값
ZZ_SCRIPT
int setShadowmapSize ( int iSize );

/// 쉐도우맵 블러 타입
/// 주의: attachWindow() 전에 호출되어야 함(init.lua 권장)
/// @iBlurType : 0(블러사용안함), 1(블러 한번 적용), 2(블러 두번 적용)...
/// @return : 이전 타입값
ZZ_SCRIPT
int setShadowmapBlurType ( int iBlurType );

ZZ_SCRIPT
void useShadowmap ( int bUse );

/// 글로우효과를 사용할지 여부
/// @bUse : 0(사용안함), 1(사용함)
ZZ_SCRIPT
void useGlow ( int bUse );

/// 글로우 효과의 렌더타겟 텍스쳐 해상도
/// @iSize : 텍스쳐 해상도(픽셀). 가로 = 세로
ZZ_SCRIPT
void setGlowmapSize ( int iSize );

/// 글로우 효과의 블러링시 혼합 농도 색상. 범위 (0, 1.f)
/// 이 값이 크면, 글로우가 밝아진다.
/// 디폴트 : 0.01f, 0.01f, 0.01f
ZZ_SCRIPT
void setGlowColor ( float fColorR, float fColorG, float fColorB );

/// 그림자맵의 색상
/// 0에서 1 사이의 RGB 값
ZZ_SCRIPT
void setShadowmapColor ( float fColorR, float fColorG, float fColorB );

ZZ_SCRIPT
int setMipmapFilter ( int FilterType );

/// 로딩될 텍스쳐의 밉맵 레벨 설정하기.
/// @Level -1이면 텍스쳐(dds)에 지정된 그대로 사용하기. 0이면 가능한한 많이. 1이면 하나만. 1이상이면 개수대로
/// 리턴값은 이전에 저장되어 있는 레벨값
ZZ_SCRIPT
int setMipmapLevel ( int Level );

/// 축소필터 타입 설정하기.
/// 리턴값은 이전에 저장되어 있는 레벨값
ZZ_SCRIPT
int setMinFilter ( int FilterType );

/// 확대필터 타입 설정하기.
/// 리턴값은 이전에 저장되어 있는 레벨값
ZZ_SCRIPT
int setMagFilter ( int FilterType );

ZZ_SCRIPT
int useObjectSorting ( int bUse );

ZZ_SCRIPT
int usePolygonSorting ( int bUse );

ZZ_SCRIPT
void useFog ( int bUse );

ZZ_SCRIPT
int setFullSceneAntiAliasing ( int Type ); // None = 0, TwoSample = 2, FourSample = 4

// whether use octree cull or not
// returns old value (true : 1, false : 0)
ZZ_SCRIPT
int useCull ( int bCull );

// whether draw bounding volume or not, in cull mode 
// returns old value
ZZ_SCRIPT
int useDrawBoundingVolume ( int bDraw );

// whether draw view frustum or not. in cull mode
// returns old value
// default is false
ZZ_SCRIPT
int useDrawViewFrustum ( int bDraw );

/// 그림자맵 뷰포트를 표시할 것인지 여부
ZZ_SCRIPT
int setDrawShadowmapViewport ( int bDraw );

// whether clear cull-list and recalc or not. in cull mode
// returns old value
// default is true
ZZ_SCRIPT
int useRefreshCull ( int bRefresh );

// set delay time per every frame
ZZ_SCRIPT
int setTimeDelay ( int Delay );

/// 텍스쳐 로딩시 텍스쳐 사이즈에 오른쪽 쉬프트(2로 나눌)시킬 값. 기본값 0
/// 텍스쳐 메모리가 모자를 시에, iScale 을 1이나 2으로 하면
/// 로딩하는 텍스쳐의 크기를 2배또는 4배 더 적은 크기로 로딩하므로,
/// 메모리를 적게 차지한다. 단, 텍스쳐 해상도가 현저히 떨어진다.
/// 텍스쳐 크기는 기본적으로 2의 배수를 따르기 때문에, 쉬프트연산을 사용한다.
ZZ_SCRIPT
int setTextureLoadingScale ( int iScale );

//--------------------------------------------------------------------------------
// functions
//--------------------------------------------------------------------------------

/// 해당 이름의 노드를 찾아 핸들을 넘겨준다.
/// @param pNodeName 가져올 노드의 이름.
/// @return          찾은 노드의 핸들. 그 노드가 없을 때에는 0을 리턴한다.
ZZ_SCRIPT
HNODE findNode ( ZSTRING pNodeName );

/// 메쉬파일을 메모리로 로드한다.
/// @param pMeshName 이 이름으로 메쉬가 등록된다.
/// @param pMeshPath 로드할 메쉬 파일 이름.
/// @return          성공하면 메쉬의 핸들, 실패하면 0.
ZZ_SCRIPT
HNODE loadMesh (
	ZSTRING pMeshName,
	ZSTRING pMeshPath
);

/// 메쉬를 메모리로부터 언로드한다.
/// @param hMesh 기존의 메쉬에 대한 핸들.
/// @return      성공하면 1, 실패하면 0.
ZZ_SCRIPT
int unloadMesh ( HNODE hMesh );

/// @bMeshType 0 : static mesh, 1 : dynamic mesh, 2 : system mesh
ZZ_SCRIPT
void setMeshType ( HNODE hMesh, int bDynamic );

/// 컬러맵 재질을 읽어들인다.
/// @pMaterialName : 등록될 재질 이름
/// @hShader       : 등록된 쉐이더 핸들
/// @pMapFileName  : 텍스쳐 파일 경로 및 이름
/// @리턴값        : 성공하면 재질핸들, 실패하면 0
ZZ_SCRIPT 
HNODE loadColormapMaterial (
	ZSTRING pMaterialName,
	HNODE hShader,
	ZSTRING pMapFileName
);

/// 컬러맵 재질을 생성한다.
/// @pMaterialName : 등록될 재질 이름
/// @hShader       : 등록된 쉐이더 핸들
/// @iWidth        : 넓이
/// @iHeight       : 높이
/// @리턴값        : 성공하면 재질핸들, 실패하면 0
ZZ_SCRIPT
HNODE loadNullColormapMaterial ( 
								ZSTRING pMaterialName,
								HNODE hShader,
								int iWidth, 
								int iHeight
								);

/// 재질을 등록해제한다.
/// @param hMaterial 재질 핸들
/// @return          성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadMaterial ( HNODE hMaterial );

/// 재질의 알파 속성을 설정한다.
/// @param hMaterial 재질 핸들
/// @param bUseAlpha 1이면 알파값 사용, 0이면 알파값 사용 안함.
/// @return          성공하면 1, 실패하면 0
ZZ_SCRIPT 
int setMaterialUseAlpha (
	HNODE hMaterial,
	int bUseAlpha
);

/// 재질의 양면 속성을 설정한다.
/// @param hMaterial   재질 핸들
/// @param bUseTwoSide 1이면 양면 사용, 0이면 단면만 사용.
/// @result            성공하면 1, 실패하면 0
ZZ_SCRIPT 
int setMaterialUseTwoSide (
	HNODE hMaterial,
	int bUseTwoSide
);

/// 재질의 알파 테스트 속성을 설정한다.
ZZ_SCRIPT
int setMaterialUseAlphaTest (
	HNODE hMaterial,
	int bUseAlphaTest
);

/// 재질의 알파 레퍼런스 값을 설정한다.
ZZ_SCRIPT
int setMaterialAlphaRef (
	 HNODE hMaterial,
	 int iAlphaRef
);

/// 비져블을 로딩한다.
/// @param pVisibleName 등록될 비져블 이름
/// @param hMesh        등록된 메쉬 핸들
/// @param hMaterial    등록된 재질 핸들
/// @param hLight       등록된 라이트 핸들
/// @return             비져블 핸들. 실패하면 0
ZZ_SCRIPT 
HNODE loadVisible (
	ZSTRING pVisibleName,
	HNODE hMesh,
	HNODE hMaterial,
	HNODE hLight
);

/// 애니메이터블을 로딩한다.
/// @param pAnimatableName 등록될 애니메이터블 이름
/// @param hMesh        등록될 메쉬 핸들
/// @param hMaterial    등록될 재질 핸들
/// @param hLight       등록될 라이트 핸들
/// @param hMotion      등록될 라이트 핸들
/// @return             애니메이터블 핸들. 실패하면 0
ZZ_SCRIPT
HNODE loadAnimatable (
	ZSTRING pAnimatableName,
	HNODE hMesh,
	HNODE hMotion,
	HNODE hMaterial,
	HNODE hLight
);

/// 비져블을 언로드한다.
/// @param hVisible 비져블 핸들
/// @return         성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadVisible ( HNODE hVisible );

ZZ_SCRIPT
int unloadAnimatable ( HNODE hAnimatable );

/// 검잔상 삭제.
ZZ_SCRIPT
int unloadTrail ( HNODE hTrail );

/// 비져블의 OBB를 설정한다.
/// @param hVisible 비져블 핸들
/// @param LocalCenterXYZ 로컬 센터의 xyz(절대좌표)
/// @param HalfLengthXYZ  박스의 넓이/높이/길이의 반
/// @return               성공하면 1, 실패하면 0
ZZ_SCRIPT 
int setObbox (
	HNODE hVisible,
	float LocalCenterX,
	float LocalCenterY,
	float LocalCenterZ,
	float HalfLengthX,
	float HalfLengthY,
	float HalfLengthZ
);

/// 모퍼(버텍스 애니메이션이 되는 객체)를 로드한다.
/// @param pMorpherName 등록할 모퍼 이름
/// @param hMesh        메쉬 핸들
/// @param hMotion      모션 핸들
/// @param hMaterial    재질 핸들
/// @param hLight       라이트 핸들
/// @return             모퍼 핸들을 리턴, 실패하면 0
ZZ_SCRIPT 
HNODE loadMorpher (
	ZSTRING pMorpherName,
	HNODE hMesh,
	HNODE hMotion,
	HNODE hMaterial,
	HNODE hLight
);

/// 모퍼 언로드.
/// @param hMorpher 등록해제할 모퍼 핸들
/// @return         성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadMorpher ( HNODE hMorpher );

/// 지형 블로커 로드.
/// @param pTerrainName   등록될 지형이름
/// @param pHeightmapName 하이트맵으로 사용될 파일이름
/// @param hLight         사용할 라이트 핸들
/// @param BlockSize      블록 사이즈
/// @param BlockScale3    블록 스케일
/// @return               등록된 지형핸들, 실패하면 0
//ZZ_SCRIPT 
//HNODE loadTerrainBlocker (
//	ZSTRING pTerrainName,
//	ZSTRING pHeightmapFile,
//	HNODE hLight,
//	int BlockSize,
//	ZPFLOAT BlockScale3
//);

/// 지형 블로커를 해제.
/// @param hTerrain 지형 블로커 핸들
/// @return         성공하면 1, 실패하면 0
//ZZ_SCRIPT
//int unloadTerrainBlocker ( HNODE hTerrain );

/// 스켈리톤 로드.
ZZ_SCRIPT
HNODE loadSkeleton (
	ZSTRING pSkeletonName,
	ZSTRING pSkeletonPath
);

/// 모델 로드.
/// @param pModelName  모델 이름
/// @param hSkeleton   스켈리톤 핸들
/// @param hMotion     모션 핸들
/// @param ScaleInLoad 로딩시 적용될 스케일
/// @return            성공하면 1, 실패하면 0
ZZ_SCRIPT
HNODE loadModel (
	ZSTRING pModelName,
	HNODE hSkeleton,
	HNODE hMotion,
	float ScaleInLoad
);

// 스켈리톤 해제
// 해제시에는 이 스켈리톤을 사용하는 모든 모델들이 먼저
// 해제되어 있어야 한다.
ZZ_SCRIPT
int unloadSkeleton (
	HNODE hSkeleton
);

/// 모델 언로드
/// @param hModel 모델 핸들
/// @result       성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadModel ( HNODE hModel );

/// 렌더유닛 추가
/// @param hVisible  비져블 핸들
/// @param hMesh     메쉬 핸들
/// @param hMaterial 재질 핸들
/// @param hLight    라이트 핸들
/// @return          성공하면 1, 실패하면 0
ZZ_SCRIPT 
int addRenderUnit (
	HNODE hVisible,
	HNODE hMesh,
	HNODE hMaterial,
	HNODE hLight
);

/// 렌더유닛 전부 삭제
/// @param hVisible 렌더유닛들을 삭제할 비져블의 핸들
/// @result         성공하면 1, 실패하면 0
ZZ_SCRIPT
int clearRenderUnit ( HNODE hVisible );

/// 렌더유닛 개수 얻기
ZZ_SCRIPT
int getNumRenderUnit ( HNODE hVisible );

/// 노드를 링크시킨다.
/// @param hParent 부모 노드의 핸들 (없으면 최상위 노드로 만듦)
/// @param hNode   부모 노드에 연결시킬 자식노드의 핸들
/// @return        성공하면 1, 실패하면 0
ZZ_SCRIPT 
int linkNode (
	HNODE hParent,
	HNODE hNode
);

/// 노드의 링크를 해제시킨다.
/// 해당 노드를 부모로부터 링크해제한다.
/// @param hNode   자식 노드의 핸들
/// @return        성공하면 1, 실패하면 0
ZZ_SCRIPT 
int unlinkNode (
	HNODE hNode
);

/// 모델의 본에 아이템을 직접 링크시킨다.
/// 링크를 해제할 때에는 unlinkNode () 사용
/// @param hParentModel 링크될 모델의 핸들
/// @param hNode        링크시킬 아이템의 핸들
/// @param iSkel        모델의 본 번호(현재로서는 로그파일 참조)
/// @return             성공하면 1, 실패하면 0
ZZ_SCRIPT
int linkBone (
	HNODE hParentModel,
	HNODE hNode,
	int iSkel
);

/// 모델의 더미노드에 아이템을 링크시킨다.
/// 링크를 해제할 때에는 unlinkNode () 사용
/// @param hParentModel 더미노드를 소유하는 모델의 핸들
/// @param hNode        링크되어지는 아이템의 핸들
/// @param iDummy       모델의 더미 노드 인덱스
/// @return             성공하면 1, 실패하면 0
ZZ_SCRIPT
int linkDummy (
	HNODE hParentModel,
	HNODE hNode,
	int iDummy
);

/// 라이트 로드.
/// @param pLightName 라이트 파일 이름
/// @result           라이트 핸들, 실패하면 0
ZZ_SCRIPT 
HNODE loadLight ( ZSTRING pLightName );	

/// 라이트 언로드.
/// @param hLight 라이트 핸들
/// @result       성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadLight ( HNODE hLight );

/// 포인트 라이트 로드
ZZ_SCRIPT
HNODE loadLightPoint ( ZSTRING pLightName );

/// 라이트 속성 설정
/// @param hLight 라이트 핸들
/// @param pProperty 속성 스트링("ambient", "diffuse", "specular", "direction", "position", "attenuation")
/// @param Value3    속성 값
/// @return          성공하면 1, 실패하면 0
ZZ_SCRIPT 
int setLight (
	HNODE hLight,
	ZSTRING pProperty,
	float fValue1,
	float fValue2,
	float fValue3
);

ZZ_SCRIPT 
int setQuaternion (
	HNODE hVisible,
	float qW,
	float qX,
	float qY,
	float qZ
);

ZZ_SCRIPT 
int setAxisAngle (
	HNODE hVisible,
	float AngleDegree,
	float AxisX,
	float AxisY,
	float AxisZ
);

ZZ_SCRIPT 
int setPosition (
	HNODE hVisible,
	float PositionX,
	float PositionY,
	float PositionZ
);

ZZ_DLL
int setPositionVec3 ( HNODE hVisible, float vPosition[3] );

ZZ_SCRIPT 
int setScale (
	HNODE hVisible,
	float ScaleX,
	float ScaleY,
	float ScaleZ
);

/// 화면에 보일지의 여부 및 투명도 얻기
ZZ_SCRIPT
float getVisibility ( HNODE hVisible );

/// 비져블의 투명도 세팅.
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setVisibility ( HNODE hVisible, float fVisibility );

/// 비져블의 투명도 세팅. 모든 하위 자식노드들까지 적용됨.
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setVisibilityRecursive ( HNODE hVisible, float fVisibility );

/// 쉐이더에 특정 포맷에 사용할 쉐이더를 설정한다.
/// @hShader : 쉐이더 핸들(loadShader()를 통해 얻은값)
/// @pVertexShaderPath : 버텍스 쉐이더 경로명
/// @pPixelShaderPath : 픽셀 쉐이더 경로명
/// @iFormat : 포맷 번호(주의: 순차적으로 지정해야 한다. 즉, 0 다음에, 1. 1 다음에 2 식으로. 스킵 가능)
ZZ_SCRIPT
int setShaderFormat (
	HNODE hShader,
	ZSTRING pVertexShaderPath,
	ZSTRING pPixelShaderPath,
	int iFormat
);

/// @iVertexFormat : use combination of ZZ_VF_XXXXs, in zz_mesh.h
ZZ_SCRIPT 
HNODE loadShader (
	ZSTRING pShaderName,
	ZSTRING pVertexShaderPath,
	ZSTRING pPixelShaderPath,
	int bUseBinary,
	int iVertexFormat
);

ZZ_SCRIPT
int unloadShader ( HNODE hShader );

/// 모션 로딩
/// @pMotionName			name of motion object
/// @pMotionFileName		file path to load into motion
/// @bUseLoop				loop motion or not
/// @InterpTypePosition		interpolation method for position. for now.
/// @InterpTypeRotation		interpolation method for rotation. for now.
/// @Scale					motion loading scale
/// @bForModel				set true if this motion will be used in zz_model
///
/// interpolation type :
/// ZZ_INTERP_NONE = 0,
/// ZZ_INTERP_LINEAR = 1,
/// ZZ_INTERP_SLERP = 2, // for rotation only. aka. LINEAR
/// ZZ_INTERP_SQUAD = 3, // for rotation only
/// ZZ_INTERP_CATMULLROM = 4,
/// ZZ_INTERP_TCB = 5 // not used yet
ZZ_SCRIPT 
HNODE loadMotion (
	ZSTRING pMotionName,
	ZSTRING pMotionFileName,
	int bUseLoop,
	int InterpTypePosition,
	int InterpTypeRotation,
	float Scale,
	int bForModel
);

ZZ_SCRIPT 
HNODE loadMotionMixer (
	ZSTRING pMixMotionName,
	HNODE hMotionA,
	HNODE hMotionB
);

ZZ_SCRIPT
int unloadMotion ( HNODE hMotion );

ZZ_SCRIPT 
int attachMotion (
	HNODE hNode,
	HNODE hMotion
);

ZZ_SCRIPT 
int doScript ( ZSTRING pScriptName );


ZZ_SCRIPT 
void doLog ( ZSTRING pLogMessage );

ZZ_DLL
void doLogf ( const char * pMsgFormat, ... );

// hModel 모델을 시계반대방향으로 Angle 각도만큼 회전시킴
// 주의: Model 객체만 가능함(Visible 또는 Morpher 등은 안됨)
ZZ_SCRIPT 
int turnModel ( HNODE hModel, float Angle );

//--------------------------------------------------------------------------------
// camera section
//--------------------------------------------------------------------------------
ZZ_SCRIPT 
HNODE loadCamera (
	ZSTRING pCameraName,
	ZSTRING pCameraPath,
	HNODE hCameraMotion
);

ZZ_SCRIPT
int unloadCamera ( HNODE hCamera );

ZZ_SCRIPT
int saveCamera ( HNODE hCamera, ZSTRING pCameraPath );

ZZ_SCRIPT 
int setCameraAspectRatio (
	HNODE hCamera,
	float AspectRatio
);

/// Set default camera
/// @return old default camera
ZZ_SCRIPT
HNODE setCameraDefault ( HNODE hCamera );

/// Set shadow light camera
/// @return old shadow camera
ZZ_SCRIPT
HNODE setCameraLight ( HNODE hCamera );

ZZ_SCRIPT 
int rotateCamera ( HNODE hCamera, int AxisType, float AngleDegree );

ZZ_SCRIPT 
int dollyCamera ( HNODE hCamera, float StepSize );

ZZ_SCRIPT 
int beginTrackball ( HNODE hCamera, float ScreenX, float ScreenY );

ZZ_SCRIPT 
int rotateTrackball ( HNODE hCamera, float ScreenX, float ScreenY );

ZZ_SCRIPT 
int endTrackball ( HNODE hCamera, float ScreenX, float ScreenY );

ZZ_SCRIPT 
int beginPan ( HNODE hCamera, float ScreenX, float ScreenY );

ZZ_SCRIPT 
int doPan ( HNODE hCamera, float ScreenX, float ScreenY, float PanSize );

ZZ_SCRIPT 
int endPan ( HNODE hCamera, float ScreenX, float ScreenY );

ZZ_SCRIPT 
int moveCamera ( HNODE hCamera, float deviationX, float deviationY, float deviationZ );

//--------------------------------------------------------------------------------

ZZ_SCRIPT
float getFps ( void );

ZZ_SCRIPT 
void setWindowText ( ZSTRING pTitle );

ZZ_SCRIPT 
int traverseNode ( HNODE hNode );

//ZZ_SCRIPT 
//HNODE createIkChain (HNODE hIk, ZSTRING pRootName, ZSTRING pMiddleName, ZSTRING pEndName);
//
//ZZ_SCRIPT 
//int setIkTarget (HNODE hIk, ZPFLOAT TargetPosWorld);

ZZ_SCRIPT 
void buildScene ( void );

ZZ_SCRIPT 
void cullScene ( void );

ZZ_SCRIPT
int insertToScene ( HNODE hVisible );

ZZ_SCRIPT
int removeFromScene ( HNODE hVisible );

ZZ_SCRIPT
int getScreenWidth ( void );

ZZ_SCRIPT
int getScreenHeight ( void );

ZZ_SCRIPT
int getFullScreen ( void );

ZZ_SCRIPT 
void setFogColor ( float colorR, float colorG, float colorB );

ZZ_SCRIPT 
void setFogRange ( float rangeStart, float rangeEnd );

ZZ_SCRIPT
int getUseFog ( void );

ZZ_SCRIPT
int getUseWireMode ( void );

//--------------------------------------------------------------------------------
// DLL only interfaces
//--------------------------------------------------------------------------------
ZZ_DLL
void initZnzin ( void );

ZZ_DLL
void destZnzin ( void );

ZZ_DLL
int callScriptFunc ( ZSTRING funcName, ... );

ZZ_DLL
int attachWindow ( const void * window_handle );

ZZ_DLL
int detachWindow ( void );

//--------------------------------------------------------------------------------

ZZ_SCRIPT
int loadCharacter ( ZSTRING characterFileName );

//TODO: unloadCharacter()

// function wrappers

ZZ_SCRIPT
int callInt ( ZSTRING funcString );

ZZ_SCRIPT
HNODE callHNODE ( ZSTRING funcString );

ZZ_SCRIPT
ZSTRING callString ( ZSTRING funcString );

ZZ_SCRIPT
float callFloat ( ZSTRING funcString );

ZZ_SCRIPT
ZSTRING getName ( HNODE hNode );

ZZ_SCRIPT
int setName ( HNODE hNode, ZSTRING pNewName );

/// 2-3 인칭 모드 카메라를 사용할 때 사용한다.
/// 주의: cameraUpdate() 를 매 프레임마다 호출해야 한다.
ZZ_SCRIPT
int cameraAttachTarget ( HNODE hCamera, HNODE hModel );

ZZ_SCRIPT
int cameraDetachTarget ( HNODE hCamera );

ZZ_SCRIPT
int cameraUpdate ( HNODE hCamera );

//ZZ_SCRIPT
//int cameraRotateAroundTarget ( HNODE hCamera );

ZZ_SCRIPT
int lookAt (
			HNODE hCamera,
			float eyeX, float eyeY, float eyeZ,
			float centerX, float centerY, float centerZ,
            float upX, float upY, float upZ );

ZZ_SCRIPT
int setCameraPerspective ( HNODE hCamera, float fFovY, float fAspectRatio, float fNear, float fFar );

/// 지형용 메쉬(패치) 생성
/// 주의: 스크립트에서 지원되지 않음(floa * 때문)
/// fOrigX, fOrigY : 월드좌표상의 기준점(지형메쉬의 왼쪽 위)
/// iDetailLevel   : 하나의 패치 기준 가로(또는 세로) 면 개수(예. 1, 2, 4 등등)
/// iType0         : 기본 텍스쳐의 텍스쳐좌표 타입 (zz_mesh_tool::zz_uv_type 참조)
/// iType1         : 상위 블렌딩 텍스쳐의 텍스쳐좌표 타입 (zz_mesh_tool::zz_uv_type 참조)
/// pHeightList    : float 배열의 포인터(배열의 개수는 버텍스 개수와 동일. (iDetailLevel+1)^2
/// iMapSize       : 라이트맵의 크기. 예) 16000
/// iBlockSize     : 하나의 블럭 사이즈. 예) 1000
ZZ_DLL
HNODE loadTerrainMesh (
					   ZSTRING pMeshName,
					   float fOrigX, float fOrigY,
					   int iDetailLevel,
					   int iUVType0, int iUVType1,
					   float * pHeightList,
					   int iMapSize,
					   int iBlockSize
					   );

// 지형용 재질 생성
// 주의: 스크립트에서 지원되지 않음(loadTerrainMesh가 지원되지 않기 때문에)
ZZ_DLL
HNODE loadTerrainMaterial (
	ZSTRING pMatName,
	HNODE hShader,
	HNODE pFirstTexture,
	HNODE pSecondTexture,
	HNODE pLightTexture
);

// 주어진 노드 핸들(hNode)이 iType의 클래스 타입인지 여부를 리턴.
// 1이면 참. 0이면 거짓
// hNode : 노드 핸들
// iType : 클래스 타입 핸들(ex. ZZ_TYPE_VISIBLE)
ZZ_SCRIPT
int isA ( HNODE hNode, int iType );

// hNode 노드의 타입을 리턴한다.
int getTypeOf ( HNODE hNode );

// 씬그래프에서 노드들이 현재 몇 개 있는지
ZZ_SCRIPT
int getNumSceneNode ( void );

// 씬그래프에서 노드들 중 iIndex번째 노드를 얻어옴.
ZZ_SCRIPT
HNODE getSceneNode ( int iIndex );

/// 해당 노드가 뷰프러스텀 안에 존재하는가.
/// 지형이나 물 오브젝트는 대상에서 제외된다.
/// 존재하면 1을 리턴, 아니면 0을 리턴.
ZZ_SCRIPT
int inViewfrustum ( HNODE hVisible );

// 모션의 총 시간 얻어오기
ZZ_SCRIPT
int getMotionTotalTime ( HNODE hMotion );

// 모션의 총 프레임 개수 얻어오기
// return : 프레임 번호: (0, .... ,n-1)
ZZ_SCRIPT
int getMotionTotalFrame ( HNODE hMotion );

// 노드 하나만 렌더링하기.
ZZ_SCRIPT
int renderNode ( HNODE hNode );

// 렌더링 시작하기 전에 호출
ZZ_SCRIPT
void beginScene ( void );

// 렌더링 후에 호출
ZZ_SCRIPT
void endScene ( void );

/// 씬에 대한 전처리 작업. 
/// beginScene() 이전에 호출
ZZ_SCRIPT
void preProcessScene ( void );

/// 씬에 대한 후처리 작업.
/// 예를 들면, 글로우 효과 처리
/// endScene()  이전에 호출.
/// 일반적으로 beginSprite() 이전에 호출해준다. 그렇지 않으면 스프라이트로 출력된 인터페이스도 후처리가 적용되기 때문.
ZZ_SCRIPT
void postProcessScene ( void );

ZZ_SCRIPT
void swapBuffers ( void );

//// 현재 씬그래프의 모든 노드들을 렌더링한다.
//// return : 렌더링한 노드들의 개수
//ZZ_SCRIPT
//int renderSceneTime (int iTime);

// 화면을 모두 지움
ZZ_SCRIPT
void clearScreen ( void );

// D3D 디바이스를 얻음
// 없으면 NULL을 리턴
ZZ_SCRIPT
HNODE getDevice ( void );

// 페이드-인 효과
// zeroToOne 을 0에서 1로 주면 점진적으로 검은색에서 원래색으로 변환한다
ZZ_SCRIPT
void fadeIn ( float fZeroToOne );

/// 감마 컨트롤 값 지정.
/// fGammaValue : -4 에서 8까지 지정 가능. 디폴트 0
ZZ_SCRIPT
void setGammaValue ( float fGammaValue );

ZZ_SCRIPT
void renderScene ( void );

/// 현재 비저블의 위치 얻어오기.
ZZ_SCRIPT
float getPositionX ( HNODE hVisible );

ZZ_SCRIPT
float getPositionY ( HNODE hVisible );

ZZ_SCRIPT
float getPositionZ ( HNODE hVisible );


/// 비져블의 월드좌표를 얻는다.
/// 성공하면 1, 실패하면 0을 리턴.
/// 예)
/// D3DXVECTOR3 pos;
/// getPosition( model, 0, pos ); 

ZZ_DLL
int getPosition ( HNODE hVisible, float fPositionXYZ[3] );

// 첫번째 실수리턴값 : x좌표
// 두번째 실수리턴값 : y좌표
// 세번째 실수리턴값 : z좌표
ZZ_SCRIPT
int getPositionScript ( HNODE hVisible );

/// 모델의 속도 지정
/// @fVelocity : distance(cm) of the model per second.
/// @return : 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setModelMoveVelocity ( HNODE hModel, float fVelocity );

/// 모델의 속도 얻기
/// @return : distance(cm) of the model per second.
ZZ_SCRIPT
float getModelMoveVelocity ( HNODE hModel );

/// 모델의 방향을 설정
/// @fAngleDegree : 각도(단위:degree). 기준(월드의 -Y축이 0도), 방향(시계반대방향이 양의 방향)
/// @bImmediate : 1이면 즉각 회전, 0이면 점진적으로 회전
/// @return       : 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setModelDirection ( HNODE hModel, float fAngleDegree, int bImmediate );

/// 위치좌표를 통해 모델의 이동 방향 설정하기
/// @return : 성공하면 1, 실패면 0
ZZ_SCRIPT
int setModelDirectionByPosition ( HNODE hModel, float fX, float fY );

/// 모델의 방향 얻기
ZZ_SCRIPT
float getModelDirection ( HNODE hModel );

/// 모델의 방향 벡터 얻기
ZZ_DLL
int getModelDirectionVector ( HNODE hModel, float fDirectionXYZ[3] );

/// getModelDirectionVector()의 스크립트 버전
// 첫번째 실수리턴값 : x좌표
// 두번째 실수리턴값 : y좌표
// 세번째 실수리턴값 : z좌표
ZZ_SCRIPT
int getModelDirectionVectorScript ( HNODE hModel );

/// 비저블 회전
/// 로컬 축을 기준으로 지정된 (상대적인) 각도만큼 시계반대방향으로 회전시킴
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT int rotateAxis ( HNODE hVisible, float fAngleDegree, float fAxisX, float fAxisY, float fAxisZ );

/// start 와 end 를 (월드좌표의) 시작 및 종료 벡터로 하여 (절대)회전으로 적용시킨다.
ZZ_DLL
int rotateByVectorAbsolute ( HNODE hVisible, const float * fStartXYZ, const float * fEndXYZ );

ZZ_SCRIPT
int rotateByVectorAbsoluteScript ( HNODE hVisible,
						  float fStartX, float fStartY, float fStartZ, 
						  float fEndX, float fEndY, float fEndZ );

/// 상대적 벡터 회전 함수.
/// 비져블 오브젝트의 로컬 벡터(fLocalStartAxisXYZ)를
/// 비져블의 원점으로부터 월드 목표 위치(fWorldEndTargetXYZ)을 가리키는 벡터로 
/// 회전시키는 회전을 적용한다.
/// 예)
/*
    HNODE model = getBone(findNode("00300_0", 0);
    FLOAT pos[3];
	FLOAT axis[] = {1, 0, 0};
	getPosition( model, pos )
    
	// 화살의 나가는 방향(1, 0, 0)을 모델의 중심을 가리키도록 회전시킴
    rotateByVectorRelative( sword[1], axis, pos );
*/
ZZ_DLL
int rotateByVectorRelative ( HNODE hVisible,
							const float fLocalStartAxisXYZ[3],
							const float fWorldEndTargetXYZ[3] );

/// Similar to rotateByVectorRelative, but this API uses WorldEndTargetXYZ as world vector
ZZ_DLL
int rotateByVectorRelativeVector ( HNODE hVisible,
							const float fLocalStartAxisXYZ[3],
							const float fWorldEndTargetXYZ[3] );

/// 상대적 벡터 회전 함수의 스크립트용 버전
ZZ_SCRIPT
int rotateByVectorRelativeScript (
	HNODE hVisible,
	float fLocalStartAxisX, float fLocalStartAxisY, float fLocalStartAxisZ,
	float fWorldEndTargetX, float fWorldEndTargetY, float fWorldEndTargetZ );

/// 로그를 남길 것인지 말것인지
/// @bTrueFalse : 1이면 로그 남김. 0이면 로그 안 남김
/// @return     : 이전값 리턴
ZZ_SCRIPT
int activateLog ( int bTrueFalse );

/// 노드를 언로드한다. 모든 노드 타입에 대해 가능
/// 자동적으로 링크가 깨진다. unlinkBone()이나 unlinkDummy(), unlinkNode() 등을 따로 호출할 필요 없음.
/// @return   : 성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadNode ( HNODE hModel );

/// 노드를 언로드하는 동시에 차일드 노드까지 언로드한다.
/// 자동적으로 링크가 깨진다.
/// @return   : 성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadNodeRecursive ( HNODE hNode );

/// 텍스쳐 갱신(시간을 체크해서 바뀐 텍스쳐는 갱신함)
/// @terun
ZZ_SCRIPT
int reloadTextures ( void );

/// 시간차 얻기
/// @return 시간차(단위는 milisecond)
ZZ_SCRIPT
int getTimeDiff ( void );

/// 지형 객체 생성
/// 설명 : loadTerrainMesh 참조.
ZZ_DLL
HNODE loadTerrainBlock (
	ZSTRING pTerrainBlockName,
	float fOrigX, float fOrigY,
	int iDetailLevel,
	int iUVType0, int iUVType1,
	float * pHeightList,
	int iMapSize,
	int iBlockSize,
	HNODE hFirstMat,
	HNODE hSecondMat,
	HNODE hLightMat,
	HNODE hTerrainShader,
	HNODE hLight
);

/// 노드 선택하기. 엔진에서 사용
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int selectNode ( HNODE hNode );

// float 형으로 스크립트 글로벌 변수 값 얻어오기
ZZ_SCRIPT
float getGlobalFloat ( ZSTRING pName );

ZZ_SCRIPT
int setCameraFollowYaw ( HNODE hCamera, float fYawDeg );

/// @fPitch 0 (-60 도) - 1.0 (80 도)
ZZ_SCRIPT
int setCameraFollowPitch ( HNODE hCamera, float fPitch );

ZZ_SCRIPT
int setCameraFollowDistance ( HNODE hCamera, float fDistance );

ZZ_SCRIPT
float getCameraFollowYaw ( HNODE hCamera );

ZZ_SCRIPT
float getCameraFollowPitch ( HNODE hCamera );

ZZ_SCRIPT
float getCameraFollowDistance ( HNODE hCamera );

ZZ_DLL
int getCameraEye ( HNODE hCamera, float posEye[3] );

ZZ_DLL
int getCameraDir ( HNODE hCamera, float dirFront[3] );

ZZ_DLL
int getCameraUp ( HNODE hCamera, float dirUp[3] );

/// 비저블 노드의 포그 상태 설정
/// 주의: 비저블의 모든 렌더유닛을 추가한 다음 호출해야 한다. 즉, 모든 유닛들을 addRenderUnit() 후에 호출
///       그렇지 않으면, 현재 등록된 렌더유닛의 재질에만 적용된다.
/// 할일: 재질의 상태를 변경하지 말고, 재질에 포그받을 여부를 설정하거나, 비저블 자체에 포그받을 여부를 설정토록.
ZZ_SCRIPT
int setReceiveFog     ( HNODE hVisible, int hUseFog );

ZZ_SCRIPT
int setReceiveShadow  ( HNODE hVisible, int hReceiveShadow );

ZZ_SCRIPT
int setCastShadow     ( HNODE hVisible, int hCastShadow );

ZZ_SCRIPT
int setCameraFollowMode ( HNODE hCamera, int mode_Look0_Back1 );

ZZ_SCRIPT
int setUseTimeWeight ( int bTrueOrFalse );

ZZ_SCRIPT
int setUseFixedFramerate ( int bTrueOrFalse );

ZZ_SCRIPT
int enableRenderState ( int bTrueOrFalse );

/// same as D3D's SetRenderState()
ZZ_DLL
void setRenderState ( ZZ_DWORD dwState, ZZ_DWORD dwValue ); 

/// iRefValue :(0-255)
/// if iRefValue is not in range (0-255), then ALPHATEST will is ignored.
/// alphatest function is greater-equal.
ZZ_DLL
void setAlphaTest ( int iRefValue );

ZZ_SCRIPT
HNODE loadSky ( ZSTRING pSkyName, HNODE hMesh, HNODE hMaterial, HNODE hLight );

ZZ_SCRIPT
int unloadSky ( HNODE hSky );

/// 하늘의 회전 속도 지정
/// iSkyUnit : 하늘에 지정된 렌더 유닛 중 하나 선택
/// fAngleDegreePerSecond : 초당 회전 각도
/// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setSkyRotationSpeed ( HNODE hSky, int iSkyUnit, float fAngleDegreePerSecond );

/// 객체(Visible 이나 Model)의 현재 프레임을 얻는다.
/// 실패하면, -1
/// 성공하면, 현재 프레임 번호(0부터 시작)
ZZ_SCRIPT
int getMotionFrame ( HNODE hNode );

/// 스크린 좌표를 월드 좌표로 변환
/// 월드 좌표의 z는 0으로 가정함
/// X, Y : 스크린좌표. 예) (100, 100)
/// 주의: x와 y를 각각 따로 뽑아야 함
ZZ_SCRIPT
float pickScreenX ( int screenX, int screenY );

ZZ_SCRIPT
float pickScreenY ( int screenX, int screenY );

ZZ_DLL
bool pickScreenXY ( int screenX, int screenY, float * worldX, float * worldY );

/// 노드 대 반직선 충돌 체크
/// 가능한 노드는 Visible, Model, Morpher, TerrainBlock 등.
/// 반직선 : 시작점(originX, originY, originZ), 방향벡터(dirX, dirY, dirZ)
/// pContactPointX/Y/Z : 접촉 지점 위치를 받을 포인터.
/// pDistance : origin과 충돌지점 사이의 거리를 받을 포인터. 필요없으면 0을 입력해도 됨.
/// 충돌하면 1, 그렇지 않으면 0
/// 충돌했다면, 충돌지점이 pfContactPointX/Y/Z에 저장되고, pfDistance 에 origin과의 거리가 저장됨
ZZ_DLL
int intersectRay (
	HNODE hNode,
	float originX, float originY, float originZ,
	float dirX, float dirY, float dirZ,
	float * pContactPointX, float * pContactPointY, float * pContactPointZ,
	float * pDistance
 );

/// 충돌하면 1, 그렇지 않으면 0
/// 충돌했다면, 충돌지점이 FloatArray의 0, 1, 2 인덱스에, origin과의 거리가 인덱스 3에 들어간다.
/// 사용하는 실수형 인덱스 개수 : 4
ZZ_SCRIPT
int intersectRayScript ( HNODE hNode, float originX, float originY, float originZ, float dirX, float dirY, float dirZ );

/// 노드대 노드 충돌 체크(obb-obb 까지)
/// 폴리곤단위가 아닌 바운딩박스까지만.
/// 노드는 Visible, Model, Morpher 등이 될 수 있다.
/// 충돌하면 1, 그렇지 않으면 0
ZZ_SCRIPT
int intersectNode ( HNODE hNodeA, HNODE hNodeB );

/// 노드대 노드 충돌 체크(sphere-sphere 까지)
/// 폴리곤단위가 아닌 바운딩박스까지만.
/// 노드는 Visible, Model, Morpher 등이 될 수 있다.
/// 충돌하면 1, 그렇지 않으면 0
ZZ_SCRIPT
int intersectNodeSphereSphere ( HNODE hNodeA, HNODE hNodeB );

/// 노드(폴리곤)대 노드(구) 충돌 체크
ZZ_DLL
int intersectNodeTriSphere ( HNODE hNodeByTri_In, float vSphereCenter_In[3], float fSphereRadius_In );

/// Triangle vs. moving Sphere intersection test
/// @hNodeByTri_In node for triangle
/// @vSphereCenter_In the center of sphere
/// @fSphereRadius_In the radius of sphere
/// @vClosestCenter_Out the closest center position
/// @fMaxDistance_In valid maximum distance. If exceed this, intersection test will fail.
ZZ_DLL
int intersectNodeTriSphereMoving ( HNODE hNodeByTri_In, float vSphereCenter_In[3], float fSphereRadius_In,  float vPrevCenter_In[3], float vClosestCenter_Out[3], float fMaxDistanceSquare_In );

ZZ_SCRIPT
float getCameraEyeX ( HNODE hCamera );

ZZ_SCRIPT
float getCameraEyeY ( HNODE hCamera );

ZZ_SCRIPT
float getCameraEyeZ ( HNODE hCamera );

/// 씬 내에서 노드 픽킹하기(Visible 류의 객체들만 픽킹된다)
/// screenX, screenY : 화면 스크린 좌표(예. 200, 100)
/// fContactPointX/Y/Z : 접촉지점 월드 좌표를 받을 포인터변수.
/// fDistance : 카메라와 접촉지점 사이의 거리를 받을 포인터변수. 필요없으면 0을 입력
/// 접촉하는 노드가 있다면, 그 노드를 리턴, 없다면 0을 리턴
ZZ_DLL
HNODE pickNode ( int screenX, int screenY, float * pContactPointX, float * pContactPointY, float * pContactPointZ, float * pDistance );

// pickNode() 와 동일한 스크립트전용 함수
/// 성공한다면, float_array의 처음 네 개를 각각 fContactPointX, fContactPointY, fContactPointZ, fDistance 로 사용한다.
ZZ_SCRIPT
HNODE pickNodeScript ( int screenX, int screenY );

// 카메라 타겟의 높이값 보정
// fHeight : 높이값(cm)
// 실패하면 0, 성공하면 1 리턴
ZZ_SCRIPT
int setCameraTargetHeight ( HNODE hCamera, float fHeight );

// float 형 배열을 설정한다.
// 배열의 크기는 256
// 엔진 함수와 스크립트가 서로 두 개 이상의 실수형 변수를 주고 받을 때 사용한다.
// 모든 함수들이 이 임시변수 배열을 사용하는 것은 아니다.
ZZ_SCRIPT
float getFloatArray ( int iIndex );

ZZ_SCRIPT
float setFloatArray ( int iIndex, float fNewFloat );

ZZ_SCRIPT
int unloadTerrainBlock ( HNODE hTerrainBlock );

ZZ_DLL
int getRay (
	int screenX, int screenY,
	float * pOriginX, float * pOriginY, float * pOriginZ,
	float * pDirectionX, float * pDirectionY, float * pDirectionZ );

// 동시사용 텍스쳐가 4 미만인 하드웨어에서는,
// seMultiPass( 1 )로 설정한 것과 동일한 효과를 갖음.
// 지형 렌더링 시 두패스로 렌더링 함.
ZZ_SCRIPT
int useMultiPass ( int bUseMultiPass );

ZZ_SCRIPT
int useLightmap ( int bUseLightmap );

ZZ_SCRIPT
int weldMesh ( HNODE hMesh );

ZZ_SCRIPT
int clearMeshMatid ( HNODE hMesh );

ZZ_SCRIPT
int generateMeshStrip ( HNODE hMesh );

/// bTextMode : 텍스트모드로 저장하려면 1, 바이너리는 0
ZZ_SCRIPT
int saveMesh ( HNODE hMesh, ZSTRING pSavePath, int bTextMode );

// fPointX/Y 로 지정된 월드좌표로부터 위방향(z방향)으로의 최대 높이값을 리턴한다.
// hNode : 높이를 비교할 오브젝트 핸들(비져블, 모델, 모퍼, 지형 등)
// 만약, 충돌되지 않는다면, -999999999.f (9 아홉개)를 리턴함.
// 사용예. [ test/test_update_collision.lua ]
// 주의: 이전 인터페이스(getPointHeight)에는 float fPointZ 가 있었으나, 이 함수가 현재 2D좌표 상에서 최대 높이를 구하는 것이기 때문에,
//       높이값을 입력으로 넣어 줄 필요가 없으며, 그로 인한 버그의 위험이 있으므로 생략했다.
ZZ_SCRIPT
float getWorldHeight ( HNODE hNode, float fPointX, float fPointY );

// Origin 에서 Dir로 향하는 레이를 쏘아 지정된 오브젝트와 
// 가장 가까운(또는 먼) 충돌점까지의 거리를 리턴한다.
// hVisible : 레이를 쏠 대상 오브젝트 핸들(비져블, 모델, 모퍼, 지형 등)
// iNearest : 1(가장 가까운 거리를 선택), 0(가장 먼 거리를 선택)
// 오브젝트에 충돌하지 않는 경우에는 999999999.f (9 아홉개)를 리턴함.
ZZ_SCRIPT
float shootRay ( HNODE hVisible, int iNearest,
				float fOriginX, float fOriginY, float fOriginZ, 
				float fDirX, float fDirY, float fDirZ );

/// shootRay 의 축약 버전.
ZZ_DLL
float shootRayVec3 ( HNODE hVisible, int iNearest, float vOrigin[3], float vDirection[3] );

// 비져블의 충돌 레벨을 정의한다. (0 은 충돌 고려 안함. 현재 4(폴리곤단위로 충돌)가 최대)
// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setCollisionLevel ( HNODE hVisible, int iLevel );

/// 비져블의 충돌 레벨을 얻어온다.
ZZ_SCRIPT
int getCollisionLevel ( HNODE hVisible );

// 비져블의 라이트맵을 설정한다. (실제로는 렌더유닛에 등록된 재질들의 속성을 변경한다)
// pLightmapFilename : 라이트맵 파일 이름. 컬러맵 재질을 따로 만들 필요 없이 경로명만 지정하면 된다.
// iX, iY : 하나의 이미지 파일에 여러 라이트맵을 공유할 경우, 가로 세로 인덱스
// iWidth, iHeight : 하나의 이미지 파일에 들어가는 라이트맵의 가로 및 세로 개수
//                   (예. 총 12개(4x3)의 맵이 포함된다면 iWidth = 4, iHeight = 3 이 되어야 한다.
// 주의. 재질을 공유하는 비져블들이 있다면, 그 비져블들에는 모두 동일한 라이트맵이 적용된다.
ZZ_SCRIPT
int setMaterialLightmap ( HNODE hMaterial, ZSTRING pLightmapFilename, int iX, int iY, int iWidth, int iHeight );

/// hChild 노드가 hParent 노드의 자식 또는 후손일 경우 1을 리턴, 아니면 0을 리턴
/// hChild와 hParent 가 동일한 경우에도 1을 리턴.
ZZ_SCRIPT
int isDescendantOf ( HNODE hChild, HNODE hParent );

/// hChild 노드가 iType(예, ZZ_TYPE_MODEL)의 타입인 오브젝트의 자식(또는 후손)이라면 1을, 아니라면 0을 리턴한다.
/// 예를 들어, 현재 hChild 노드가 iType타입 그 차일드(또는 그 후손)일 경우 1을 리턴한다.
/// 주의: 자기 자신이 iType인 경우에도 1을 리턴.
ZZ_SCRIPT
int isDescendantOfType ( HNODE hChild, int iType );

/// 부모 노드를 얻는다.
/// 부모 노드가 없으면 0을 리턴
ZZ_SCRIPT
HNODE getParent ( HNODE hChild );

ZZ_SCRIPT
HNODE loadParticle ( ZSTRING pParticleName, ZSTRING pParticleFilename );

// 메모리로부터 파티클 로딩하기
// pParticleName : 파티클 이름
// pParticleMem  : 파티클 내용이 들어있는 메모리 포인터
// iMemSize : 메모리 크기
ZZ_SCRIPT
HNODE loadParticleFromMem ( ZSTRING pParticleName, ZSTRING pParticleMem, int iMemSize );

ZZ_SCRIPT
int unloadParticle ( HNODE hParticle );

// 파티클 제어
// iControl : 0(Stop), 1(Start)
// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int controlParticle ( HNODE hParticle, int iControl ); 

// min-max 안에 있는 모든 Visible 노드들을 찾아 구성하고,
// 그 개수를 리턴
// 주의 : collectByMinMax() 와 collectNode() 사이에는 다른 collectByMinMax()나 collectByNodeBBox()를 호출할 수 없음.
ZZ_SCRIPT
int collectByMinMax ( float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ );

ZZ_DLL
int collectByMinMaxVec3 ( float fMin[3], float fMax[3] );

// hVisible의 바운딩박스와 충돌하는 모든 노드들을 찾아 구성하고, 그 개수를 리턴
// 주의 : collectByNodeBBox() 와 collectNode() 사이에는 다른 collectByMinMax()나 collectByNodeBBox()를 호출할 수 없음.
ZZ_SCRIPT
int collectByNodeBBox ( HNODE hVisible );

/// 구로 정의된 영역에 있는 노드들을 찾아 구성하고, 그 개수를 리턴
ZZ_DLL
int collectBySphere ( float vCenter[3], float fDistance );

// collectByMinMax로 구성된 iNode 번째 노드 얻기
ZZ_SCRIPT
HNODE getCollectNode ( int iNode );

// 파티클의 현재 상태 리턴
// 0 : 파티클 정지. 1 : 파티클 플레이 중
ZZ_SCRIPT
int getParticleState ( HNODE hParticle );

// 재질의 쉐이더 세팅
// 실패하면 0, 성공하면 1
ZZ_SCRIPT
int setMaterialShader ( HNODE hMaterial, HNODE hShader );

ZZ_SCRIPT
HNODE getMaterialShader ( HNODE hMaterial );

// 비져블에 등록된 재질 얻기
ZZ_SCRIPT
HNODE getVisibleMaterial ( HNODE hVisible, int iRenderUnit );

ZZ_SCRIPT
int setVisibleMaterial ( HNODE hVisible, int iRenderUnit, HNODE hMaterial );

ZZ_SCRIPT
HNODE getVisibleMesh ( HNODE hVisible, int iRenderUnit );

ZZ_SCRIPT
int setVisibleMesh ( HNODE hVisible, int iRenderUnit, HNODE hMesh );

ZZ_SCRIPT
HNODE getVisibleLight ( HNODE hVisible, int iRenderUnit );

ZZ_SCRIPT
int setVisibleLight ( HNODE hVisible, int iRenderUnit, HNODE hLight );

// 라이트맵에 적용되는 합성모드 설정
// iBlendStyle = { 4 : ZZ_TOP_MODULATE, 5 : ZZ_TOP_MODULATE2X, 6 : ZZ_TOP_MODULATE4X }
ZZ_SCRIPT
int setLightmapBlendStyle ( int iBlendStyle );

// 씬 업데이트.
// beginScene() 이전에 호출
// 주의 : updateScene() 호출 후에는 씬내에 포함된 오브젝트를 삭제하면 안됨.
// 즉, 오브젝트 삭제후 updateScene()을 호출해야 함.
// 뿐만 아니라, beginScene()->renderScene()->endScene() 구간에도 오브젝트 삭제 금지
ZZ_SCRIPT
void updateScene ( void );

/// 씬 변환정보 업데이트. 모델의 이동 및 회전 갱신용.
/// endScene() 이후에 호출.
/// 이 함수를 호출한 다음, 모델의 좌표를 점검하고, 알맞지 않다면, 다시 직접 갱신할 수 있다(setPosition()...)
ZZ_SCRIPT
void updateSceneTransform ( void );

// 오브젝트 하나만 개별적으로 애니메이션 갱신하기
// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int applyMotion ( HNODE hAnimatable );

ZZ_SCRIPT
int setMaterialZTest ( HNODE hMaterial, int bZTest );

ZZ_SCRIPT
int setMaterialZWrite ( HNODE hMaterial, int bZWrite );

ZZ_SCRIPT
int setMaterialObjectAlpha ( HNODE hMaterial, float fAlpha );

ZZ_SCRIPT
int setCameraTransparency ( float fTransparency );

// 재질의 블렌딩 타입. 0 : 일반, 1 : 밝게
// (ZZ_BLEND_TYPE)
ZZ_SCRIPT
int setMaterialBlendType ( HNODE hMaterial, int iBlendType );

/// 재질의 알파 블렌딩 세부 지정
/// setMaterialUseAlpha() 에 의해 알파블렌딩 지정된 오브젝트만 영향받음.
/// @iBlendSrc : ZZ_BLEND ( in renderer.h )
/// @iBlendDest : ZZ_BLEND ( in renderer.h )
/// @iBlendOp : ZZ_BLENDOP ( in renderer.h )
ZZ_SCRIPT
int setMaterialBlendTypeCustom ( HNODE hMaterial, int iBlendSrc, int iBlendDest, int iBlendOp );

// 모델의 애니메이션 속도 조절.
// fSpeed : 원본 애니메이션의 속도에 곱해질 값. 1.0이면 원래 모션의 속도 그대로 유지. 0.5 면 두 배 느리게, 2.0이면 두 배 빠르게
// 리턴값 : 1이면 성공. 0이면 실패
// 주의 : 
//   첫번째 인자가 Motion이 아니고, 모델이나 모퍼인 것을 주의
//   setVisibleVelocity()는 이동 속도인 반면, 이 함수는 애니메이션의 속도
ZZ_SCRIPT
int setAnimatableSpeed ( HNODE hAnimatable, float fSpeed );

// 애니메이션 속도 얻기
// 리턴값 : 속도를 리턴, 실패하면, 0보다 작은 값을 리턴
ZZ_SCRIPT
float getAnimatableSpeed ( HNODE hAnimatable );


// 애니메이터블(모델 포함)의 애니메이션 컨트롤
// hAnimatable : ex) 모델
// iControl : 0 : Stop, 1 : Start, 2 : Pause
// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int controlAnimatable ( HNODE hAnimatable, int iControl );

// 애니메이터블의 애니메이션 상태 얻기
// @return : 0(Stopped), 1(Started), 2(Paused)
ZZ_SCRIPT
int getAnimatableState ( HNODE hAnimatable );

// 검잔상 객체 생성(스크립트 전용)
// fDistancePerPoint : 1개의 포인트가 새로 생성되는 거리(cm 단위). 즉, fDistancePerPoint 거리만큼 이동되면 새로 포인트가 생성된다.
// iDurationInMilliSecond : 밀리 세컨드 단위의 라이프 타임. 포인트들은 이 시간만큼 살아있다 죽는다.
// pTexturePath : 사용될 텍스쳐 경로
// fColorRed/Green/Blue : 텍스쳐에 블렌딩될 색상값(0 ~ 1.0)
// fSP_X/Y/Z : 검의 원점에 대한 시작 포인트의 로컬 좌표
// fEP_X/Y/Z : 검의 원점에 대한 종료 포인트의 로컬 좌표
ZZ_SCRIPT
HNODE loadTrailScript ( ZSTRING pName, 
				 float fDistancePerPoint,
				 int iDurationInMilliSecond,
				 int bUseCurve,
				 ZSTRING pTexturePath, 
				 float fColorRed, float fColorGreen, float fColorBlue, float fColorAlpha,
				 float fSP_X, float fSP_Y, float fSP_Z,
				 float fEP_X, float fEP_Y, float fEP_Z );

// 검잔상 객체 생성
// fDistancePerPoint : 1개의 포인트가 새로 생성되는 거리(cm 단위). 즉, fDistancePerPoint 거리만큼 이동되면 새로 포인트가 생성된다.
// iDurationInMilliSecond : 밀리 세컨드 단위의 라이프 타임. 포인트들은 이 시간만큼 살아있다 죽는다.
// pTexturePath : 사용될 텍스쳐 경로
// Color : 텍스쳐에 블렌딩될 색상값. D3DCOLOR. ARGB
// vStrat : 검의 원점에 대한 시작 포인트의 로컬 좌표
// vEnd : 검의 원점에 대한 종료 포인트의 로컬 좌표
ZZ_DLL
HNODE loadTrail ( ZSTRING pName, 
				 float fDistancePerPoint,
				 int iDurationInMilliSecond,
				 int bUseCurve,
				 ZSTRING pTexturePath, 
				 ZZ_COLOR Color,
				 ZZ_VECTOR vStart,
				 ZZ_VECTOR vEnd );

/// 검잔상 컨트롤
/// @iControl
///   0 : 정지. 정지하는 즉시 화면에 검잔상이 사라짐. 즉, 부드럽게 점진적으로 사라지지 않음)
///   1 : 시작
///   2 : 일시정지. 검잔상은 화면에 그대로 정지해 있음
///   3 : 점진적 정지. 포인트를 새로 생성하지 않음. 시간에 따라 검잔상이 점차적으로 사라짐.
ZZ_SCRIPT
int controlTrail ( HNODE hTrail, int iControl );

/// 검잔상 상태 얻기
/// @return 
///   0 : 정지
///   1 : 시작
///   2 : 일시정지
///   3 : 점진적 정지
ZZ_SCRIPT
int getTrailState ( HNODE hTrail );

/// 없어진 인터페이스. 
/// obsolete interface
ZZ_SCRIPT
void setDataPath ( ZSTRING pDataPath );

// 투명한 오브젝트의 렌더링 우선순위.
// iPriority : 	ZZ_DP_HIGHEST, ZZ_DP_HIGH, ZZ_DP_NORMAL(디폴트), ZZ_DP_LOW, ZZ_DP_LOWEST
// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setDrawPriority ( HNODE hVisible, int iPriority );

// 렌더링 우선순위 얻기
// 실패하면 -1을 리턴
ZZ_SCRIPT
int getDrawPriority ( HNODE hVisible );

// 모델에 연결된 더미 개수 리턴
ZZ_SCRIPT
int getNumDummies ( HNODE hModel );

// 모델에 연결된 본 개수 리턴
ZZ_SCRIPT
int getNumBones ( HNODE hModel );

// iBoneIndex 번째 본 오브젝트 얻기
ZZ_SCRIPT
HNODE getBone ( HNODE hModel, int iBoneIndex );

// 애니메이터블에서 사용하는 모션의 시작 지연시간(밀리세컨드) 얻기
ZZ_SCRIPT
int getStartDelay ( HNODE hAnimatable );

// 애니메이터블에서 사용하는 모션의 시작 지연시간(밀리세컨드) 설정
// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setStartDelay ( HNODE hAnimatable, int iDelayMilliSecond );

// 애니메이터블의 애니메이션 반복 회수 설정
// iRepeatCount : 0이면 무한반복, 지정된 개수만큼만 애니메이션 반복
ZZ_SCRIPT
int setRepeatCount ( HNODE hAnimatable, int iRepeatCount );

/// 반복 회수 얻어 오기
ZZ_SCRIPT
int getRepeatCount ( HNODE hAnimatable );

/// 비져블 노드를 링크시킴. 단, 현재 월드상의 좌표는 그대로 유지한다.
/// 즉, 로컬 좌표가 월드에 기준으로 변경된다.
/// 성공하면 1, 실패하면 0
ZZ_SCRIPT
int linkVisibleWorld ( HNODE hParent, HNODE hChild );
					 
/// 비져블 노드의 링크를 해제시킨다. 단, 해제된 노드는 현재 위치상에 그대로 남는다.
/// @param hNode   자식 노드의 핸들
/// @return        성공하면 1, 실패하면 0
ZZ_SCRIPT 
int unlinkVisibleWorld (
	HNODE hVisible
);

/// 애니메이터블 객체의 모션을 iFrame으로 지정된 프레임의 모션으로 설정한다.
/// @iFrame : 프레임 번호
ZZ_SCRIPT
int setAnimatableFrame ( HNODE hAnimatable, int iFrame );

/// 애니메이터블 객체의 모션을 iTimeMilliSecond으로 지정된 시간의 모션으로 설정한다.
// @iTimeMilliSecond : 밀리세컨드 단위의 시간
ZZ_SCRIPT
int setAnimatableTime ( HNODE hAnimatable, int iTimeMilliSecond );

// 현재 어태치 되어 있는 모션을 얻어옴
ZZ_SCRIPT
HNODE getMotion ( HNODE hAnimatable );

/// 가장 마지막에 추가된 렌더유닛 삭제
/// 대응되는 함수(addRenderUnit())
/// hVisible : 비져블 또는 모델
ZZ_SCRIPT
int popRenderUnit ( HNODE hVisible );

// 충돌검사시 고려할 대상 클래스들을 정의
// pickNode() 등에 사용됨
ZZ_SCRIPT
int setIncludeClasses ( int iIncludeClasses );

/// 충돌검사시 고려되는 대상 클래스들을 얻는다.
ZZ_SCRIPT
int getIncludeClasses ( void );

// 충돌검사시 제외할 대상 클래스들을 정의
// pickNode() 등에 사용됨
ZZ_SCRIPT
int setExcludeClasses ( int iExcludeClasses );

/// 충돌검사시 제외되는 대상 클래스들을 얻는다.
ZZ_SCRIPT
int getExcludeClasses ( void );

/// worldToScreen() 의 스크립트용 버전
/// 실수형배열 0, 1, 2에 변환된 스크린 좌표를 저장한다.
ZZ_SCRIPT
void worldToScreenScript ( float fWorldX, float fWorldY, float fWorldZ );

/// 3차원 월드좌표를 스크린 좌표로 변환
/// fWorldX/Y/Z : 월드좌표
/// fScreenX/Y/Z : 변환된 스크린 좌표(0부터 시작. 스크린 오른쪽과 아래쪽 방향으로 증가)
ZZ_DLL
void worldToScreen ( float fWorldX, float fWorldY, float fWorldZ, float * fScreenX, float * fScreenY, float * fScreenZ );

/// Model 의 메시 높이를 리턴함
/// 더 엄밀하게 말하자면, 모델에 렌더유닛으로 등록된 메시들의 바운딩 박스를 얻어,
/// 그 높이를 리턴한다. 따라서, 링크로 연결된 머리나, 모자 등의 객체는 고려하지 않는다.
ZZ_SCRIPT
float getModelHeight ( HNODE hModel );

/// 바다 객체 생성
/// @fOrigX/Y/Z : 바다블록의 월드상의 원점(왼쪽 상단)
/// @fWidth/fHeight : 넓이 및 높이
/// @iRepeatU/V : 텍스쳐좌표 반복 개수(가로/세로)
/// @hShader : 쉐이더 핸들(shader_ocean 으로 지정해야 함)
/// @hLight : 라이트 핸들(0 가능)
/// 설명 : 내부적으로 메시와 재질을 생성한다.
///        삭제시에는 unloadOceanBlock()을 호출(자동적으로 메시와 재질도 삭제됨)
ZZ_SCRIPT
HNODE loadOceanBlock (
	ZSTRING pOceanBlockName,
	float fOrigX, float fOrigY, float fOrigZ,
	float fWidth, float fHeight,
	int iRepeatU, int iRepeatV,
	HNODE hShader,
	HNODE hLight
);

// 이 인터페이스는 사용할 필요 없음. loadOceanBlock에서(만) 직접 호출
ZZ_SCRIPT
HNODE loadOceanMaterial (
	ZSTRING pMatName,
	HNODE hShader,
	ZSTRING pFirstTexture
);

/// OceanBlock 객체는 꼭 이 함수로 삭제해야 함
ZZ_SCRIPT
int unloadOceanBlock ( HNODE hOceanBlock );

/// 재질에 텍스쳐를 추가적으로 등록함
/// 텍스쳐 목록의 가장 마지막에 추가됨.
/// 텍스쳐 애니메이션을 위해 사용됨
ZZ_SCRIPT
int pushTexture ( HNODE hMaterial, ZSTRING pTexturePath );

/// 바다 블럭의 텍스쳐 애니메이션 시간간격을 설정
/// @iIntervalMilliSecond : 밀리세컨드 단위의 시간 간격. 이 시간만큼 뒤에 다음 텍스쳐로
///                         변경된다.
ZZ_SCRIPT
int setOceanBlockInterval ( HNODE hOceanBlock, int iIntervalMilliSecond );

ZZ_SCRIPT
int setMeshNumClipFaces ( HNODE hMesh, int iNumClipFaces );

ZZ_SCRIPT
int getMeshNumClipFaces ( HNODE hMesh );

/// 얼굴의 눈깜박임을 위해 사용.
/// @hVisible  : 얼굴 비져블
/// @bClipFace : 0(사용안함, 기본값), 1(눈뜨기), 2(눈감기)
/// @return    : 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setVisibleClipFace ( HNODE hVisible, int iClipFace );

ZZ_SCRIPT
int getVisibleClipFace ( HNODE hVisible );

/// DDS 파일로 현재 화면 저장하기
ZZ_SCRIPT
int saveToDDS ( ZSTRING pFileNameDDS );

/// BMP 파일로 현재 화면 저장하기
ZZ_SCRIPT
int saveToBMP ( ZSTRING pFileNameBMP );

/// 사용자 데이터 설정하기
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setUserData ( HNODE hVisible, HNODE hData );

ZZ_SCRIPT
HNODE getUserData ( HNODE hVisible );

// 오브젝트의 현재 위치에 대한 미끄럼 벡터 얻기
// fMin = 미끄럼 적용될 최소 기울기. 기울기는 0일때 지면과 평행, 1일때 수직. [0..1]
// fMax = 미끄럼 적용될 최대 기울기. [0..1]
// fNeighborDistance = ex) 30.0f
// fVelocity = 미끄럼 벡터가 적용되는 크기. 수직 경사면(절벽)에서 1초당 이동되는 거리(cm)
//             ex) 2000.0f (1초 당 2000cm. )
// vSliding = 얻어질 미끄럼 벡터
// @return = 기울기값. 0.0(평지)-1.0(90도 경사)
/*
ex)
    if (getModelVelocity( model ) > 0.1) then
        applySliding( model, 0.3, 0.7, 20, 2000.0 )
    end
*/
ZZ_DLL
float getSliding ( HNODE hVisible, float fMin, float fMax, float fNeighborDistance, float fVelocity, float vSliding[3] );

// float_array 3개 사용
ZZ_SCRIPT
float getSlidingScript ( HNODE hVisible, float fMin, float fMax, float fNeighborDistance, float fVelocity );

/// 더미의 월드좌표 위치 얻기
/// @hModel : 더미가 포함된 모델의 핸들
/// @iDummyIndex : 더미의 인덱스
/// @fPosXYZ : 결과로 더미의 월드 위치가 저장됨. D3DXVECTOR3 타입과 호환됨.
/// 예)
/// D3DXVECTOR3 pos;
/// getDummyPosition( model, 0, pos ); 
ZZ_DLL
int getDummyPosition ( HNODE hModel, int iDummyIndex, float fPosXYZ[3] );

ZZ_SCRIPT
int getDummyPositionScript ( HNODE hModel, int iDummyIndex );

/// 2차원 좌표에 대응하는 지형의 높이 얻기
/// 만약, 해당 좌표에 지형이 존재하지 않거나 문제가 발생하면 -ZZ_INFINITE 가 리턴된다.
/// @return : 높이 좌표
ZZ_SCRIPT
float getTerrainHeight ( float fPositionX, float fPositionY );

ZZ_SCRIPT
float setBlendFactor ( float fFactor );

/// 메시의 정보 얻기.
/// iInfoType = {1(면 개수), 2(정점 개수), 3(버텍스포맷), 4(재질개수), 5(맵채널개수),
// 6(본인덱스개수)}
ZZ_SCRIPT
int getMeshInfo ( HNODE hMesh, int iInfoType );

/// 메시에 노말 정보 추가
/// 기존의 노말정보는 무시됨.
ZZ_SCRIPT
int buildNormal ( HNODE hMesh );

/// 이펙트 파일 로딩
/// .eft 파일 테스트용 이펙트 파일 로딩 스크립트
/// 주의: 로딩 테스트용으로만 사용하고, 실제 게임에서는 사용하지 말것.
ZZ_SCRIPT
HNODE loadEffect ( ZSTRING pEffectName, ZSTRING pEffectFilePath );

/// 이펙트 파일 언로드.
/// 주의: 이펙트 생성시 자동 생성된 더미 비저블 및 모션들도 함께 언로드된다.
///          테스트용으로만 사용하고, 실제 게임에서는 사용하지 말것.
ZZ_SCRIPT
int unloadEffect ( HNODE hNode );

/// 삭제된 함수.
/// 실제 경로 얻기
/// 주의: 내부적으로 static 문자열을 사용하기 때문에 복사해서 사용요.
///       즉, 다음번 호출시 이전 값 무효화 됨. 255자 이하의 문자열만 가능.
ZZ_SCRIPT
ZSTRING getRealPath ( ZSTRING pPath );

/// 스페큘러가 들어가는 재질로 설정
/// 주의: 스페큘러 재질은 setMaterialShader()로 스페큘러를 사용하는 쉐이더를 지정해야 한다.
ZZ_SCRIPT
int setMaterialUseSpecular ( HNODE hMaterial, int bUseSpecular );

/// 빌보드(billboard) 효과
/// 비저블의 특정 축을 카메라로 향하도록 회전시킴. 이후로 지속적으로 적용됨.
/// bUseBillboard : 빌보드를 사용할지 여부. 0이면 이전속성 해제(빌보딩하지 않음). 1이면 사용함
ZZ_SCRIPT
int setBillboard ( HNODE hVisible, int bUseBillboard );

/// 한축을 중심으로 회전하는 빌보드(billboard) 효과
/// 비저블의 특정 축을 카메라로 향하도록 회전시킴. 이후로 지속적으로 적용됨.
/// bUseBillboard : 빌보드를 사용할지 여부. 0이면 이전속성 해제(빌보딩하지 않음). 1이면 사용함
/// iRotationAxis : 회전의 기준이 되는 비저블의 로컬 축. 값 (0(x), 1(y), 2(z))
/// iDirectionAxis : 카메라를 향하게 되는 비저블의 로클 축. 값 (0(x), 1(y), 2(z))
/// iRotationAxis와 iDirectionAxis 가 같으면 해제하는 효과.
ZZ_SCRIPT
int setBillboardOneAxis ( HNODE hVisible, int bUseBillboard, int iRotationAxis, int iDirectionAxis );

/// hMaterial 재질의 iTextureIndex (디폴트 0)번째의 텍스쳐객체를 리턴한다.
// 리턴값을 LPDIRECT3DTEXTURE9 로 형변환하여 사용해야 함.
// 사용법은 view.cpp의 test_LockTexture() 함수 참고.
// 주의: 재질에 밉맵이 포함되어 있다면, setMipmapLevel()로 밉맵을 1로 세팅해 주어야 한다.
ZZ_SCRIPT
HNODE getTexture ( HNODE hMaterial, int iTextureIndex );

ZZ_SCRIPT
void resetScreen ( void );

/// 비저블의 수평 반지름을 리턴한다.
/// 내부적으로는 AABB바운딩박스의 x 또는 y값을 리턴한다.
/// bSelectSmaller 1이면 x와 y 중 작은 값을, 0이면 큰 값을 리턴한다.
ZZ_SCRIPT
float getVisibleRadius ( HNODE hVisible, int bSelectSmaller );

// 비저블의 수직 높이를 리턴한다.
// 내부적으로는 AABB바운딩박스의 z 값의 두 배를 리턴한다.
// 참고: getModelHeight()
ZZ_SCRIPT
float getVisibleHeight ( HNODE hVisible );

// 모델의 정면 위치를 계산하여 fPositionXYZ에 저장한다.
// 정면 위치란 모델의 경계구와 진행방향벡터와의 교차지점에서의 월드 좌표를 말한다.
// 실패하면 0, 성공하면 1을 리턴
ZZ_DLL
int getModelFront ( HNODE hModel, float fPositionXYZ_Out[3] );

// getModelFront() 의 스크립트 버전
// 첫번째 실수리턴값 : x좌표
// 두번째 실수리턴값 : y좌표
// 세번째 실수리턴값 : z좌표
ZZ_SCRIPT
int getModelFrontScript ( HNODE hModel );

/// 카메라에 의해 가려지는 오브젝트인지 판별
/// 카메라에 가려지면 1을 리턴, 아니면 0을 리턴
ZZ_SCRIPT
int getVisibleSeethruMode ( HNODE hVisible );

ZZ_SCRIPT
void beginProfiler ( void );

ZZ_SCRIPT
void endProfiler ( ZSTRING pProfilerName );

/// 쿼터니온을 모델 방향값으로 변환한다.
/// @return setModelDirection()에 사용할 수 있는 회전각. 단위는 degree.
ZZ_SCRIPT
float quaternionToModelDirection ( float fQuatW, float fQuatX, float fQuatY, float fQuatZ );

/// 지형 로딩 테스트용 인터페이스
// iType : 0이면 평면, 1이면 울퉁불퉁한면(아직 구현 안됨)
ZZ_SCRIPT
HNODE loadTerrainBlockTest (
	ZSTRING pTerrainBlockName,
	int iX,
	int iY,
	int iType,
	HNODE hFirstMat,
	HNODE hSecondMat,
	HNODE hLightMat,
	HNODE hTerrainShader,
	HNODE hLight
);

/// 메시와 텍스쳐를 점진적으로(1) 또는 즉시(0) 로딩할지 여부 결정
/// 디폴트 : 1(점진적)
/// @iDelayedLoad : 0(즉시 로딩), 1(지연 로딩), 2(현재 지연 로딩된 상태를 플러시하고, 현재 상태 유지)
/// 점진적로딩모드 : 일정 프레임간격으로 메시와 텍스쳐를 파일에서 읽는다.
/// 즉시로딩모드 : loadMesh() 또는 loadColormapMaterial() 등의 호출시 즉시 파일을 읽는다.
ZZ_SCRIPT
int setDelayedLoad ( int iDelayedLoad );

/// 텍스쳐 로딩 강제 포맷 지정
/// iFormat : 0(원래텍스쳐포맷, default), 1(16비트), 2(압축)
ZZ_SCRIPT
int setTextureLoadingFormat ( int iFormat );

/// 한 버텍스당 영향주는 본의 최대 개수 설정
/// iMaxBone : 1, 2, 4(default)
/// 현재 아직 구현 안됨(셰이더 lod 구현 필요)
ZZ_SCRIPT
int setMaxSimultaneousBone ( int iMaxBone );

/// 엔진의 디스플레이 컬리티 단계 설정
/// 0(자동설정,default), 1(최상급품질), 2, 3(중급), 4, 5(최하급품질)
/// 주의 : 이 함수는 다른 set*() 또는 use*() 인터페이스 함수들보다 나중에 사용되어야 한다.
ZZ_SCRIPT
int setDisplayQualityLevel ( int iLevel );

/// 최소 및 최대 프레임율 제한(디폴트=[15,60])
ZZ_SCRIPT
int setFramerateRange ( int iMinFramerate, int iMaxFramerate );

/// 파일 갱신 시간 비교여부
ZZ_SCRIPT
int useFileTimeStamp ( int bUse );

/// 모션 보간 여부
ZZ_SCRIPT
int useMotionInterpolation ( int bUse );

/// 모션 보간이 적용되는 비져블과 카메라 간의 최대 거리. 단위 미터
/// useMotionInterpolation()가 0이라면 적용안됨.
/// 기본값 = 10 (미터)
ZZ_SCRIPT
int setMotionInterpolationRange ( float fDistanceFromCamera );

/// 풀스크린시 수직동기여부
/// 1이면 수직동기함. 0이면 안함
ZZ_SCRIPT
int useVSync ( int bUseVSync );

/// 재질의 특정 스테이지의 어드레싱 모드 설정
/// iStage : 텍스쳐 스테이지 번호(0부터 4번까지)
/// iTextureAddress :
//   ZZ_TADDRESS_WRAP = 1, (디폴트값)
//   ZZ_TADDRESS_MIRROR = 2,
//   ZZ_TADDRESS_CLAMP = 3,
//   ZZ_TADDRESS_BORDER = 4,
//   ZZ_TADDRESS_MIRRORONCE = 5,
ZZ_SCRIPT
int setMaterialTextureAddress ( HNODE hMaterial, int iStage, int iTextureAddress );

/// 라이팅의 영향을 받을 것인지의 여부 설정
ZZ_SCRIPT
int setMaterialUseLight ( HNODE hMaterial, int bUseLight );

/// 엔진 디버깅용 메세지 출력 여부
ZZ_SCRIPT
int useDebugDisplay ( int bUseDebugDisplay );

/// 디폴트값 각각 300
ZZ_SCRIPT
int setLazyBufferSize ( int iTextureSize, int iNormalMeshSize, int iTerrainMeshSize, int iOceanMeshSize );


//--------------------------------------------------------------------------------
// 텍스쳐 관련
//--------------------------------------------------------------------------------

/// 텍스쳐 로딩
/// 텍스쳐 포맷은 텍스쳐(.dds)의 기본 포맷으로 생성된다.
/// @pTextureName 텍스쳐 오브젝트명(텍스쳐 경로명과 달라도 된다)
/// @pTextureFileName 텍스쳐 경로명
/// @iMipLevels 밉맵레벨. -1 이면 파일(dds)에 지정된 대로, 0이면 가능한한 많이 생성. 1이면 밉맵 사용 안함.
/// @bUseFilter 필터링 적용 여부. 0이면 필터링 사용 안함.
/// @return 성공하면 텍스쳐 핸들, 실패하면 0
ZZ_SCRIPT
HNODE loadTexture ( 	ZSTRING pTextureName, ZSTRING pTextureFileName, int iMipLevels, int bUseFilter );

/// Pool 을 지정할 수 있는 텍스쳐 로딩
/// 텍스쳐 포맷은 텍스쳐(.dds)의 기본 포맷으로 생성된다.
/// @pTextureName 텍스쳐 오브젝트명(텍스쳐 경로명과 달라도 된다)
/// @pTextureFileName 텍스쳐 경로명
/// @iMipLevels 밉맵레벨. -1 이면 파일(dds)에 지정된 대로, 0이면 가능한한 많이 생성. 1이면 밉맵 사용 안함.
/// @bUseFilter 필터링 적용 여부. 0이면 필터링 사용 안함.
/// @iPoolType 0 : Default Pool(그래픽카드에 생성), 1 : Managed Pool(DX가 관리), 2 : SysetmMem Pool(시스템 메모리에 생성)
/// @return 성공하면 텍스쳐 핸들, 실패하면 0
ZZ_SCRIPT
HNODE loadTextureWithPool ( ZSTRING pTextureName, ZSTRING pTextureFileName, int iMipLevels, int bUseFilter, int iPoolType );

/// 텍스쳐 해제
/// @hTexture 텍스쳐 핸들
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int unloadTexture ( HNODE hTexture );

/// 텍스쳐의 LPDIRECT3DTEXTURE9 를 얻는다.
/// @hTexture 텍스쳐 핸들
/// @return 그 텍스쳐의 LPDIRECT3DTEXTURE9 값. 캐스팅해서 사용해야 함.
ZZ_SCRIPT
HNODE getTexturePointer ( HNODE hTexture );

//--------------------------------------------------------------------------------
// 폰트 관련
//--------------------------------------------------------------------------------

/// 폰트 객체 생성
/// @pFontIDName 폰트 이름. 이 폰트를 접근하기 위해 사용하는 사용자 지정 이름
/// @pFontFileName 폰트 이름(실제 시스템 폰트명). 예) "굴림체"
/// @iFontSize 폰트 크기.
/// @iBold 폰트 굵기. 1이면 bold.
/// @iItalic 이탤릭 속성 여부. 1이면 이탤릭체.
/// @iColorR/G/B/A 폰트 색상(0-255). A가 1이면 완전불투명. A가 0이면 완전투명
ZZ_SCRIPT
HNODE loadFont (
				ZSTRING pFontIDName,
				ZSTRING pFontName,
				int iFontSize,
				int iBold,
				int iItalic,
				int iColorR,
				int iColorG,
				int iColorB,
				int iColorA
				);

/// 폰트 객체 해제
ZZ_SCRIPT
int unloadFont ( HNODE hFont );

/// 화면에 폰트를 렌더링
/// @hFont 폰트 핸들. 0이면 시스템 폰트.
ZZ_SCRIPT
int drawFontScript ( HNODE hFont, int iX, int iY, ZSTRING pMessage );

/// 그려질 폰트 색상 설정
/// @iRed/iGreen/iBlue/iAlpha 0-255 사이의 정수값
ZZ_SCRIPT
int setFontColor ( HNODE hFont, int iRed, int iGreen, int iBlue, int iAlpha );

/// 해당 폰트에 대해 문자열의 픽셀 단위 높이/넓이를 얻는다.
/// @hFont 폰트핸들
/// @return 문자열의 전체 높이/넓이(픽셀 단위). WIN32 의 SIZE와 호환됨.
ZZ_DLL
ZZ_SIZE getFontTextExtent ( HNODE hFont, ZSTRING pText );

/// 해당 폰트의 픽셀 단위 높이를 얻는다.
/// @hFont 폰트핸들
/// @return 픽셀 단위 높이
ZZ_SCRIPT
int getFontHeight ( HNODE hFont );

/// 해당 폰트를 사용하여 텍스트를 생성한다.
/// drawFontXXX 와 다른 점은, 한번만 생성하면, 다음 프레임에 다시 그려주지 않아도 된다.
/// unloadText 나 unloadAllText 를 호출하지 않으면 계속 화면에 남는다.
/// @return 생성된 텍스트의 핸들. 0부터 시작한다. 0도 유효한 값.
ZZ_SCRIPT
HNODE loadText ( HNODE hFont, int iX, int iY, ZSTRING pMessage );

/// 폰트를 사용하는 텍스트를 삭제한다.
ZZ_SCRIPT
int unloadText ( HNODE hFont, HNODE hText );

/// 이 폰트를 사용하는 모든 텍스트를 삭제한다.
ZZ_SCRIPT
int unloadAllText ( HNODE hFont );

//--------------------------------------------------------------------------------
// 스프라이트 관련
//--------------------------------------------------------------------------------

/// 스프라이트 Begin
/// @iFlag ID3DXSPRITE 의 Begin() 메소드의 파라미터와 동일
ZZ_SCRIPT
int beginSprite ( int iFlag ); // iFlag in ZZ_SPRITE_XXXXXX zz_renderer.h

/// 스프라이트 End
ZZ_SCRIPT
int endSprite ( void );

/// 스프라이트를 렌더링. ID3DXSPRITE 의 Draw() 메소드와 유사함.
/// @hTexture 텍스쳐 객체의 핸들. loadTexture() 로 얻을 수 있다.
/// @pSrcRect 원본 사각형 영역. RECT 의 포인터를 전달하면 된다.
/// @pCenter 스프라이트의 중앙 위치. D3DVECTOR(또는 D3DXVECTOR3) 의 포인터를 전달하면 된다.
/// @pPosition 스프라이트가 그려질 좌표. D3DVECTOR(또는 D3DXVECTOR3) 의 포인터를 전달하면 된다.
/// @Color 텍스쳐에 곱해질 색상. D3DCOLOR와 호환됨.
ZZ_DLL
int drawSprite ( HNODE hTexture, 
    const ZZ_RECT * pSrcRect,
    const ZZ_VECTOR *pCenter,
    const ZZ_VECTOR *pPosition,
    const ZZ_COLOR Color
);

/// 스프라이트를 플러시
ZZ_SCRIPT
int flushSprite ( void );

/// 스프라이트의 변환행렬 얻기
/// @d3d_tm_4x4 얻어올 4x4 행렬(D3DMATRIX와 동일한 구조)
/// ex)
/// D3DXMATRIX m;
/// getTransformSprite(m); // uses casting operator FLOAT* ();
ZZ_DLL
int getTransformSprite ( float * d3d_tm_4x4 );

/// 스프라이트의 변환행렬 설정
/// @d3d_tm_4x4 설정할 4x4 행렬(D3DMATRIX와 동일한 구조)
/// ex)
/// D3DXMATRIX m;
/// setTransformSprite(m); // uses casting operator CONST FLOAT* () const;
ZZ_DLL
int setTransformSprite ( const float * d3d_tm_4x4 );


//--------------------------------------------------------------------------------
// file sytem 관련
//--------------------------------------------------------------------------------

/// 파일 시스템 핸들을 설정
/// @hVFS 파일 시스템 핸들(OpenVFS()로 얻은)
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setFileSytem ( HNODE hVFS );

/// 파일 시스템 열기
/// @pIndexFileName 인덱스 파일명
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int openFileSystem ( ZSTRING pIndexFileName );

/// 파일 시스템 닫기. openFileSystem() 으로 호출한 파일 시스템 닫기.
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int closeFileSystem ( void );

/// 파일 읽기모드로 열기
/// @pFilename 파일 이름
/// @return 파일 핸들, 실패하면 0
ZZ_SCRIPT
HNODE fileReadFrom ( ZSTRING pFilename );

/// 파일의 한 낱말 읽기
/// @return 읽은 문자열. 파일의 끝이라면, ""을 리턴. 사이즈가 0인 문자열.
ZZ_SCRIPT
ZSTRING fileReadWord ( HNODE hFile );

/// 파일 닫기
/// @hFile 파일 핸들
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int fileClose ( HNODE hFile );

/// 모션 보간 시간 지정
/// 이전 모션과 지정된 시간 간격만큼 보간되어 애니메이션된다.
/// @hMotion 모션 핸들
/// @iMilliSeconds 지연 시간. 단위) 밀리세컨드
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setMotionInterporationInterval ( HNODE hMotion, int iMilliSeconds );

/// 이전의 월드 공간 위치를 저장
/// @hVisible 비져블 노드 핸들
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int savePrevPosition ( HNODE hVisible );

/// 이전 프레임의 월드 공간 위치를 얻어옴
/// @hVisible 비져블 노드 핸들
/// @fPositionXYZ 얻어올 3차원 좌표
/// @return 성공하면 1, 실패하면 0
ZZ_DLL
int getPrevPosition ( HNODE hVisible, float fPositionXYZ[3] );

ZZ_SCRIPT
float getPrevPositionX ( HNODE hVisible );

ZZ_SCRIPT
float getPrevPositionY ( HNODE hVisible );

ZZ_SCRIPT
float getPrevPositionZ ( HNODE hVisible );

/// 모델을 좌 또는 우 방향으로 fDistance 거리만큼 이동시킨 월드 위치를 얻는다.
/// 좌 또는 우 방향은 ShiftDirection에 의해 결정된다.
/// @fDistance 이동될 거리. 항상 양수이어야 함.
/// @fWorldPosOut 구해질 월드 좌표
ZZ_DLL
int getModelShiftPosition ( HNODE hModel, float fCmPerSecond, float fWorldPosOut[3] );

/// 모델의 이동 선호 방향을 반대로 설정한다.
/// @iShiftLeftDirection 1이면 왼쪽, 0이면 오른쪽
ZZ_DLL
int flipModelShiftDirection ( HNODE hModel );

/// 비져블에 중력을 적용한 위치를 리턴한다.
/// 현재 속도 및 질량은 적용되지 않음.
ZZ_DLL
float applyGravity ( HNODE hVis );

/// 현재 씬에 등록된 모든 오브젝트들(지형 및 바다오브젝트는 제외)에 대해,
/// 지정한 2차원 점에 해당하는 가장 높은 높이 얻기.
/// 지형을 포함하여 씬 안의 모든 오브젝트들의 높이 중에 가장 높은 것을 선택한다.
/// @fWorldX/Y 높이를 얻고자하는 2D 좌표
/// @fDefaultHeight 충돌하는 오브젝트가 없을 때 리턴하는 높이값. 일반적으로 지형의 높이로 입력.
/// @return 해당 위치의 높이
ZZ_SCRIPT
float getWorldObjectHeightInScene ( float fWorldX, float fWorldY, float fDefaultHeight );

//--------------------------------------------------------------------------------
// drawFontLater ()
//--------------------------------------------------------------------------------
/// 지정한 문자열을 렌더링의 제일 마지막 부분에 렌더링하도록 요청한다.
/// 이 문자열은 렌더링된 뒤에 바로 삭제된다.
/// 이 인터페이스는 drawFont()와는 달리, beginScene()/endScene() 내에 존재하지 않아도 된다.
/// @hFont loadFont()로부터 얻은 핸들.
/// @iX 스크린 x좌표. 0부터 시작.
/// @iY 스크린 y좌표. 0부터 시작. 아래 방향으로 증가.
/// @pMsg 출력할 메세지.
/// @return 1이면 성공. 0이면 실패.
ZZ_SCRIPT
int drawFontLater ( HNODE hFont, int iX, int iY, ZSTRING pMsg );

/// pushFont()에 포맷 문자열이 적용된 버전
ZZ_DLL
int drawFontLaterf ( HNODE hFont, int iX, int iY, const char * pMsgFormat, ... );


//--------------------------------------------------------------------------------
// 3d sound system 관련
//--------------------------------------------------------------------------------

/// 사운드 시스템 초기화
/// attachWindow() 호출 후에 호출해야 함. (윈도우 핸들을 사용하기 때문)
ZZ_SCRIPT
int initSoundSystem ( void );

/// 사운드 시스템 해제
/// destZnzin() 호출 전에 호출해야 함.
/// 현재 아무것도 안함.
ZZ_SCRIPT
int destSoundSystem ( void );

/// 사운드 시스템 업데이트
/// 실질적인 리스너 위치 갱신이 여기에서 이루어짐.
ZZ_SCRIPT
int updateSoundSystem ( void );

/// 사운드 로딩
/// @strSoundName 사운드 객체 이름
/// @strWaveFileName .WAV 파일 이름
/// @iNumBuffers 버퍼 개수
ZZ_SCRIPT
HNODE loadSound ( ZSTRING strSoundName, ZSTRING strWaveFileName, int iNumBuffers );

/// 사운드 해제
ZZ_SCRIPT
int unloadSound ( HNODE hSound );

/// 사운드의 3차원 위치 지정.
/// 월드 공간 좌표와 동일
ZZ_DLL
int setSoundPosition ( HNODE hSound, float fPosition[3] );

/// 사운드 플레이
/// @bLoop 1이면 반복
ZZ_SCRIPT
int playSound ( HNODE hSound, int bLoop );

ZZ_SCRIPT
int stopSound ( HNODE hSound );

// @return : 0(Stopped), 1(Started)
ZZ_SCRIPT
int getSoundState ( HNODE hSound );

//--------------------------------------------------------------------------------

/// 두 벡터간 사이각을 구한다.
/// 음수가 될 수 있음.
/// @return 사이각. 단위. 도(degree)
ZZ_DLL
float getVectorAngle ( float vVector1[3], float vVector2[3] );

/// 하늘 재질을 생성한다.
/// @pMaterialName : 등록될 재질 이름
/// @hShader       : 등록된 쉐이더 핸들
/// @pMapFileName1  : 첫번째 텍스쳐 파일 경로 및 이름
/// @pMapFileName2  : 두번째 텍스쳐 파일 경로 및 이름
/// @리턴값        : 성공하면 재질핸들, 실패하면 0
ZZ_SCRIPT 
HNODE loadSkyMaterial (
	ZSTRING pMaterialName,
	HNODE hShader,
	ZSTRING pMapFileName1,
	ZSTRING pMapFileName2
);

/// 하늘 재질의 멀티텍스쳐 블렌딩 비율을 설정한다.
/// @hMat 재질 핸들
/// @fBlendRatio 블렌딩 비율. 1이면 첫번째 텍스쳐가 100%. 0이면 두번째 텍스쳐가 100%임.
/// @return 성공하면 1. 실패하면 0
ZZ_SCRIPT 
int setSkyMaterialBlendRatio ( HNODE hMat, float fBlendRatio );

/// 비져블의 AABB를 얻는다.
/// @hVisible_In 비져블 핸들
/// @fMin_Out 바운딩 박스의 최저 지점
/// @fMax_Out 바운딩 박스의 최고 지점
ZZ_DLL
int getVisibleAABB ( HNODE hVisible_In, float vMin_Out[3], float vMax_Out[3] );

/// 비져블 노드의 바운딩 구를 얻는다.
/// @hVisible_In 비져블 핸들
/// @vCenter_Out 바운딩 구의 중심점
/// @fRadius_Out 바운딩 구의 반지름
ZZ_DLL
int getVisibleSphere ( HNODE hVisible_In, float vCenter_Out[3], float * fRadius_Out );

/// 반직선 대 삼각형 교차 판정. 충돌 법선도 구해진다.
/// 판정이 성공하는 경우에만, vContactPoint_Out 와 vContactNormal_Out 가 갱신된다.
ZZ_DLL
int intersectRayNormal ( HNODE hVisible_In, float vRayOrigin_In[3], float vRayDirection_In[3], float vContactPoint_Out[3], float vContactNormal_Out[3] );

/// 현재 등록되어 있는 모든 텍스쳐들의 경로를 로그파일에 출력한다.
/// @return 텍스쳐들의 개수를 리턴
ZZ_SCRIPT
int logTextures ( void );

/// 모델의 무게 중심 위치를 얻는다. (월드 좌표 중심)
/// @vCenterOfMass 월드 상의 중심 위치
/// @return 성공하면, 1, 실패하면 0
ZZ_DLL
int getModelCOMPositionWorld ( HNODE hModel, float vCenterOfMass[3] );

/// 모델의 무게 중심 위치를 얻는다. (로컬 좌표 중심)
/// @vCenterOfMass 월드 상의 중심 위치
/// @return 성공하면, 1, 실패하면 0
ZZ_DLL
int getModelCOMPositionLocal ( HNODE hModel, float vCenterOfMass[3] );

/// 비져블의 렌더링 영역을 설정한다.
/// @hVisible 영역을 설정할 비져블 노드의 핸들
/// @fDistanceFromCameraStart 렌더링의 시작거리(카메라로부터의 거리)
/// @fDistanceFromCameraEnd 렌더링이 끝거리(카메라로부터의 거리)
ZZ_SCRIPT
int setVisibleRangeFromCamera ( HNODE hVisible, float fDistanceFromCameraStart, float fDistanceFromCameraEnd );

ZZ_SCRIPT
int pushSpecialEffect ( int iEffectID );

ZZ_SCRIPT
int popSpecialEffect ( void );

ZZ_SCRIPT
int clearSpecialEffect ( void );

/// 텍스쳐의 가로 세로 사이즈를 얻는다.
/// @hTexture 텍스쳐 핸들
/// @iWidth_Out 넓이
/// @iHeight_Out 높이
/// @return 성공하면 1, 실패하면 0
ZZ_DLL
int getTextureSize ( HNODE hTexture, int& iWidth_Out, int& iHeight_Out );

/// 모델의 법선벡터를 설정한다. 지정된 법선벡터가 모델의 z축이 된다.
/// @hModel 모델 핸들
/// @vNormal 법선벡터
/// @retrurn 성공하면 1, 실패하면 0을 리턴
ZZ_DLL
int setModelNormal ( HNODE hModel, float vNormal[3] );

/// 카메라를 지정된 시간동안 지정된 범위내에서 무작위로 이동시킨다.
/// @hCamera 카메라 핸들
/// @vMin 범위 지정 박스의 최소 위치(예, -10, -10, -10)
/// @vMax 범위 지정 박스의 최대 위치(예, 10, 10, 10)
/// @iTimeMSEC 밀리세컨드 단위의 지속 시간. 
/// @return 성공하면 1, 실패하면 0을 리턴
ZZ_DLL
int shakeCamera ( HNODE hCamera, float vMin[3], float vMax[3], int iTimeMSEC );

ZZ_DLL
int getVisibleVelocity ( HNODE hVisible, float vVelocityVector[3] );

ZZ_DLL
int setVisibleVelocity ( HNODE hVisible, float vVelocityVector[3] );

ZZ_SCRIPT
int zeroVisibleVelocityZ ( HNODE hVisible );

ZZ_DLL
int setGravity ( float vGravityVector[3] );

ZZ_SCRIPT
int setGravityScript ( float fGravityX, float fGravityY, float fGravityZ );

/// 모델의 초기 모션 위치를 얻는다. 모델좌표계의 상대좌표임.
ZZ_DLL
int getModelInitialCOMPosition ( HNODE hModel, float vPositionOut[3] );

ZZ_SCRIPT
int useGravity ( HNODE hVisible, int bUseOrNot );

ZZ_DLL
int setVisibleForce ( HNODE hVisible, float vForceVector[3] );

// fJumpForce = difference of velocity per seconds
ZZ_SCRIPT
int setVisibleJump ( HNODE hVisible, float fJumpForce );

ZZ_SCRIPT
float getGravityVelocityZ ( HNODE hVisible );

ZZ_SCRIPT
int setAdapter ( int iAdapterIndex );

ZZ_SCRIPT
int getNumAdapter ( void );

ZZ_SCRIPT
int setMonitorRefreshRate ( int iRefreshRate );

ZZ_SCRIPT
int getMonitorRefreshRate ( void );

ZZ_SCRIPT
int dumpResourcesInViewstrum ( int bTerrain, int bOcean, int bOpaque, int bTransparent );

/// 로그 내용을 문자열로 얻어오기.
/// @iNumLatest 얻어올 로그 라인 개수. 0이면 전체
/// @return 로그 문자열.
/// 주의: 한번 얻어오면 다음번 호출시에는 이전 내용은 제외된다.
ZZ_SCRIPT
ZSTRING readLogString ( int iNumLatest, ZSTRING pStrLineBreakingMsg );

/// HNODE 타입을 리턴함
ZZ_SCRIPT
HNODE returnHNODE ( HNODE hNode );

//--------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------------------
//               for cpp only interfaces (ex. overloading functions)
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// drawFont()
// : 폰트를 렌더링.
//--------------------------------------------------------------------------------
/// @hFont loadFont() 로부터 얻은 핸들.
/// @bUseSprite 스프라이트 사용 여부. 1이면, 스프라이트의 beginSprite()/endSprite() 블록 안에서 호출되어야 한다.
/// @iX 스크린 x좌표. 0부터 시작.
/// @iY 스크린 y좌표. 0부터 시작. 아래 방향으로 증가.
/// @pMsg 출력할 메세지.
/// @return 1이면 성공, 0이면 실패.
ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, int iX, int iY, const char * pMsg);

/// @Color D3DCOLOR 와 동일한 타입
ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, int iX, int iY, ZZ_COLOR Color, const char * pMsg);

/// @pRect ID3DXFONT 의 DrawText() 메소드의 RECT와 동일
ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, const char * pMsg);

/// @dwFormat ID3DXFONT 의 DrawText() 메소드의 Format과 동일
ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_DWORD dwFormat, const char * pMsg);

ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_COLOR Color, ZZ_DWORD dwFormat, const char * pMsg);

//--------------------------------------------------------------------------------
// drawFontf()
// : drawFont()에 포맷 문자열이 적용된 버전
//--------------------------------------------------------------------------------
ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, int iX, int iY, const char * pMsgFormat, ... );

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, int iX, int iY, ZZ_COLOR Color, const char * pMsgFormat, ... );

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, const char * pMsgFormat, ... );

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_DWORD dwFormat, const char * pMsgFormat, ... );

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_COLOR Color, ZZ_DWORD dwFormat, const char * pMsgFormat, ... );



#endif //__ZZ_INTERFACE_H__
