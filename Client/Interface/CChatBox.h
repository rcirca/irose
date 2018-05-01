#ifndef _CHATBOX_
#define _CHATBOX_

#include <string>
#include <list>


const int	CHAT_DISPLAY_TIME = 270;
const int	MAX_CHAT_BOX = 100;


class CChatBox;

enum 
{
	CHATBOX_BIG = 0,
	CHATBOX_SMALL,
	CHATBOX_MAX,
};

/// Chatbox manager
class CChatBoxManager
{
private:
	std::list< CChatBox* >	m_ChatBoxPool;
	std::list< CChatBox* >	m_ActiveChatBoxList;

	/// Background		
	HNODE					m_BackGroundTex[ CHATBOX_MAX ];
	

public:
	CChatBoxManager();
	~CChatBoxManager();

	bool					Init( );
	void					FreeResource();
	void					Clear();

	void					AddChat( int iCharIndex, const char* Msg, DWORD Color );

	void					Draw();
};





/// Avata ¸Ó¸®À§ Ãª ¹Ú½º
class CChatBox
{
private:
	int					m_iCharIndex;

	/// Display time    
	int					m_iDisplayTime;

	/// Ãª ´ëÈ­..
	std::string			m_strText;
	DWORD				m_Color;

public:
	CChatBox();
	~CChatBox();	

	int					GetChatIndex(){ return m_iCharIndex; }

	void				SetMember( int iCharIndex, const char* Msg, DWORD Color = 0 );

	void				SetText( const char* pStr ){ m_strText = std::string( pStr ); m_iDisplayTime = CHAT_DISPLAY_TIME; }
	void				SetText( std::string str ){ m_strText = str; m_iDisplayTime = CHAT_DISPLAY_TIME; }

	bool				Draw( HNODE* backTexture );
};

#endif //_CHATBOX_