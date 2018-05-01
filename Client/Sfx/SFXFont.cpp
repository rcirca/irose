#include "stdafx.h"
#include ".\sfxfont.h"
#include "../Util/Localizing.h"
#include "../interface/controls/EffectString.h"

CSFXFont::CSFXFont(void)
{

}

CSFXFont::~CSFXFont(void)
{
}

bool CSFXFont::Init()
{
	return true;
}

void CSFXFont::Release()
{
	std::list<CEffectString*>::iterator iter;

	for( iter = m_children.begin(); iter != m_children.end(); )
	{
		delete *iter;
		iter = m_children.erase( iter );
	}
}

void CSFXFont::Update()
{
	std::list<CEffectString*>::iterator iter;
	int ret = 0;
	for( iter = m_children.begin(); iter != m_children.end(); )
	{
		ret = (*iter)->Update();
		if( ret == -1 )//time_over
		{
			delete *iter;
			iter = m_children.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void CSFXFont::Draw()
{
	std::list<CEffectString*>::iterator iter;
	for( iter = m_children.begin(); iter != m_children.end(); ++iter )
		(*iter)->Draw();
}

void CSFXFont::AddEffectString( CEffectString* child )
{
	assert( child );
	if( child )
		m_children.push_back( child );
}

void CSFXFont::RemoveEffectStringsByType( int type )
{
	std::list<CEffectString*>::iterator iter;
	for( iter = m_children.begin(); iter != m_children.end();)
	{
		if( (*iter)->GetType() == type )
		{
			delete *iter;
			iter = m_children.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}