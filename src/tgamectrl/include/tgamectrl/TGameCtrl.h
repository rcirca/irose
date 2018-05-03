/// 다음 ifdef 블록은 DLL에서 내보내기하는 작업을 쉽게 해 주는 매크로를 만드는 
/// 표준 방식입니다. 이 DLL에 들어 있는 파일은 모두 명령줄에 정의된 TGAMECTRL_EXPORTS 기호로
/// 컴파일되며, 동일한 DLL을 사용하는 다른 프로젝트에서는 이 기호를 정의할 수 없습니다.
/// 이렇게 하면 소스 파일에 이 파일이 들어 있는 다른 모든 프로젝트에서는 
/// TGAMECTRL_API 함수를 DLL에서 가져오는 것으로 보고,
/// 이 DLL은 해당 매크로로 정의된 기호가 내보내지는 것으로 봅니다.
#pragma warning( disable : 4251 )
#pragma warning( disable : 4100 )
//#pragma warning( disable : 4786 )

#ifdef TGAMECTRL_DLL
#define TGAMECTRL_API __declspec(dllexport)
#elif TGAMECTRL_STATIC
#define TGAMECTRL_API
#else
#define TGAMECTRL_API __declspec(dllimport)
#endif

#include <D3DX9.h>
class ITDraw;
class ITSound;
class ITFont;
/// 초기화
TGAMECTRL_API bool it_Init( HWND hWnd ,ITDraw* pDrawMgr , ITSound* pSoundMgr , ITFont* pFontMgr );

TGAMECTRL_API void it_Destroy();

TGAMECTRL_API void it_SetCodePage( unsigned uiCodePage );

TGAMECTRL_API void it_SetKeyboardInputType( short nInputType );
TGAMECTRL_API short it_GetKeyboardInputType();

TGAMECTRL_API bool	it_IsVisibleImeCandidateWindow();
//void	TWriteConsole(const char *fmt,...);