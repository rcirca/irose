#include "stdafx.h"
#include ".\skilltreedlg.h"

#include "icon/ciconskilldummy.h"
#include "../../object.h"
#include "../../gamecommon/skill.h"

#include "xmlparse.h"
#include "../CInfo.h"
#include "../CToolTipMgr.h"
#include "../interfacetype.h"

void node_baseskill::clear()
{
	SAFE_DELETE( m_icon );
	if( m_texture )
		unloadTexture( m_texture );
	
	std::list<node_skill>::iterator iter;
	for( iter = m_children.begin(); iter != m_children.end(); )
	{
		SAFE_DELETE( iter->m_icon );
		iter = m_children.erase( iter );
	}
}

void startElement(void *userData, const char *name, const char **atts)
{
	s_userdata* p = (s_userdata*)userData;

	if( p->depth == 1 )///base skill
	{
		node_baseskill node_base;
		node_base.m_index   = 0;
		node_base.m_texture = 0;
		node_base.m_icon    = 0;
		node_base.m_level   = 1;

		for( int index = 0; atts[index] != NULL; index += 2 )
		{
			if( strcmpi( atts[index],"INDEX" ) == 0 )
			{
				node_base.m_index = atoi(atts[index + 1]);
			}
			else if( strcmpi( atts[index],"OFFSETX" ) == 0 )
			{
				node_base.m_offset_x = atoi( atts[index+1] );
			}
			else if( strcmpi( atts[index],"OFFSETY" ) == 0 )
			{
				node_base.m_offset_y = atoi( atts[index+1] );
			}
			else if( strcmpi( atts[index],"IMAGE" ) == 0 )
			{
				std::string file = "3DData\\Control\\Res\\";
				file.append( atts[index+1] );

				//setDelayedLoad(0);
				node_base.m_texture = loadTexture ( file.c_str(), 
											file.c_str(),
											1, 
											0 );
				//		ShowChild( IID_TEXT_HOWKER );setDelayedLoad(1);
			}
		}
		
		node_base.m_icon = CSkillTreeDlg::MakeIcon( node_base.m_index , node_base.m_level , 0 ) ;

		p->base_skills.push_back( node_base );
	}
	else if( p->depth > 1 && !(p->base_skills.empty()) )
	{
		node_skill node;

		node.m_index       = 0;
		node.m_offset_x    = 0;
		node.m_offset_y    = 0;
		node.m_level       = 1;
		node.m_level_limit = 0;

		for( int index = 0; atts[index] != NULL; index += 2 )
		{
			if( strcmpi( atts[index],"INDEX" ) == 0 )
			{
				node.m_index = atoi(atts[index + 1]);
			}
			else if( strcmpi( atts[index],"OFFSETX" ) == 0 )
			{
				node.m_offset_x = atoi( atts[index+1] );
			}
			else if( strcmpi( atts[index],"OFFSETY" ) == 0 )
			{
				node.m_offset_y = atoi( atts[index+1] );
			}
			else if( strcmpi( atts[index],"LEVEL" ) == 0 )
			{
				node.m_level = atoi( atts[index+1] );
			}
			else if( strcmpi( atts[index],"LIMITLEVEL") == 0 )
			{
				node.m_level_limit = atoi( atts[index+1] );
			}
		}

		node.m_icon = CSkillTreeDlg::MakeIcon( node.m_index, node.m_level, node.m_level_limit ) ;
		p->base_skills.back().m_children.push_back( node );
	}

	p->depth += 1;
}

void endElement(void *userData, const char *name)
{
	s_userdata* p = (s_userdata*)userData;
	p->depth -= 1;
}


CSkillTreeDlg::CSkillTreeDlg(void)
{
	m_userdata.clear();
}

CSkillTreeDlg::~CSkillTreeDlg(void)
{
}

void CSkillTreeDlg::Show()
{
	CTDialog::Show();

	char buf[MAX_PATH];
	XML_Parser parser = XML_ParserCreate(NULL);
	int done;


	XML_SetUserData(parser, &m_userdata);
	XML_SetElementHandler(parser, startElement, endElement);
	HideChild( IID_TEXT_SOLDIER	);
	HideChild( IID_TEXT_MUSE );
	HideChild( IID_TEXT_HOWKER );
	HideChild( IID_TEXT_DEALER );
	

	FILE* pFile = NULL;
	switch( g_pAVATAR->Get_JOB() / 100 )
	{
	case 1:
		ShowChild( IID_TEXT_SOLDIER	);
		pFile = fopen( "3DData\\Control\\xml\\skilltree_soldier.xml","r" );
		break;
	case 2:
		ShowChild( IID_TEXT_MUSE );
		pFile = fopen( "3DData\\Control\\xml\\skilltree_muse.xml","r" );
		break;
	case 3:
		ShowChild( IID_TEXT_HOWKER );
		pFile = fopen( "3DData\\Control\\xml\\skilltree_howker.xml","r" );
		break;
	case 4:
		ShowChild( IID_TEXT_DEALER );
		pFile = fopen( "3DData\\Control\\xml\\skilltree_dealer.xml","r" );
		break;
	default:
		break;
	}
	assert( pFile );
	if( pFile == NULL )
		return ;

	do 
	{
		size_t len = fread(buf, 1, sizeof(buf), pFile );
		done = len < sizeof(buf);
		if (!XML_Parse(parser, buf, len, done)) 
		{
			assert( 0 && "XML parse error" );
			return ;
		}
	} while (!done);

	XML_ParserFree(parser);

	fclose( pFile );

	///Make Skill Icons from m_userdata;


	MoveWindow( m_sPosition );
}

void CSkillTreeDlg::Hide()
{
	CTDialog::Hide();
	///clear skill icons
	m_userdata.clear();

}

void CSkillTreeDlg::Draw()
{
	if( !IsVision() ) return;
	CTDialog::Draw();
	if( !m_userdata.base_skills.empty() )
	{
		std::list< node_baseskill >::iterator iter_base = m_userdata.base_skills.begin();

		assert( iter_base->m_texture );
		assert( iter_base->m_icon );

		D3DXMATRIX mat;	
		D3DXMatrixTranslation( &mat, (float)m_sPosition.x + 20, (float)m_sPosition.y + 75, 0 );

		::setTransformSprite( mat );
		::drawSprite( iter_base->m_texture, 
						NULL,
						NULL,
						&D3DXVECTOR3( 0, 0, 0 ),								
						D3DCOLOR_RGBA( 255, 255, 255, 255 ) );

		for( iter_base = m_userdata.base_skills.begin(); iter_base != m_userdata.base_skills.end(); ++iter_base )
		{
			assert( iter_base->m_icon );
			iter_base->m_icon->Draw();
		}

		iter_base = m_userdata.base_skills.begin();
		std::list<node_skill>::iterator iter_children;
		for( iter_children = iter_base->m_children.begin(); iter_children != iter_base->m_children.end(); ++iter_children )
		{
			assert( iter_children->m_icon );
			iter_children->m_icon->Draw();
		}
	}
}

CIconSkill* CSkillTreeDlg::MakeIcon( int skillindex, int level, int level_limit )
{
	if( !g_pAVATAR ) return NULL;
	if( CSkillSlot* pSlot = g_pAVATAR->GetSkillSlot() )
	{
		CSkill* pSkill = pSlot->GetSkillByBaseSkillIDX( skillindex );
		if( pSkill && level_limit && pSkill->GetSkillLevel() > level_limit )
		{
			CIconSkillDummy* pIcon = new CIconSkillDummy( skillindex + level_limit - 1 );		
			pIcon->SetDrawColor( D3DCOLOR_ARGB( 255,255,255,255 ) );
			return pIcon;
		}
		else if( pSkill && pSkill->GetSkillLevel() >= level )///이미 배운 스킬이라면 SkillSlot의 CSkill로 CIconSkill를 생성해서 리턴한다.
			return new CIconSkill( pSkill->GetSkillSlot() );
		else///안배운 스킬이라면 CIconSkillDummy를 리턴한다.
			return new CIconSkillDummy( skillindex + level - 1 );
	}

	return NULL;
}
void CSkillTreeDlg::MoveWindow( POINT pt )
{
	CTDialog::MoveWindow( pt );
	std::list< node_baseskill >::iterator iter_base;
	std::list<node_skill>::iterator iter_children;
	POINT ptDraw;
	for( iter_base = m_userdata.base_skills.begin(); iter_base != m_userdata.base_skills.end(); ++iter_base )
	{
		ptDraw.x = m_sPosition.x + iter_base->m_offset_x;
		ptDraw.y = m_sPosition.y + iter_base->m_offset_y;
		if( iter_base->m_icon )
			iter_base->m_icon->SetPosition( ptDraw );

		for( iter_children = iter_base->m_children.begin(); iter_children != iter_base->m_children.end(); ++iter_children )
		{
			ptDraw.x = m_sPosition.x + iter_children->m_offset_x;
			ptDraw.y = m_sPosition.y + iter_children->m_offset_y;

			if( iter_children->m_icon )
				iter_children->m_icon->SetPosition( ptDraw );
		}
	}
}

unsigned CSkillTreeDlg::Process( unsigned uiMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsVision() ) return 0;
	if( unsigned uiProcID = CTDialog::Process( uiMsg, wParam ,lParam ) )
	{
		switch( uiMsg )
		{
		case WM_LBUTTONUP:
			switch( uiProcID )
			{
			case IID_BTN_ICONIZE:
				break;
			case IID_BTN_CLOSE:
				Hide();
				break;
			default:
				break;
			}
			break;
		case WM_LBUTTONDOWN:
			{
				POINT pt = { LOWORD( lParam ), HIWORD( lParam ) };
				std::list< node_baseskill >::iterator iter_base, iter_begin ;
				for( iter_base = m_userdata.base_skills.begin(); iter_base != m_userdata.base_skills.end(); ++iter_base )
				{
					assert( iter_base->m_icon );
					if( iter_base->m_icon->IsInside( pt ) )
					{
						iter_begin = m_userdata.base_skills.begin();
						int offset_x = iter_base->m_offset_x;
						int offset_y = iter_base->m_offset_y;
						
						iter_base->m_offset_x = iter_begin->m_offset_x;
						iter_base->m_offset_y = iter_begin->m_offset_y;

						iter_begin->m_offset_x = offset_x;
						iter_begin->m_offset_y = offset_y;
						
						node_baseskill node = *iter_base;

						m_userdata.base_skills.erase( iter_base );
						m_userdata.base_skills.push_front( node );
						MoveWindow( m_sPosition );
						return uiMsg;
					}
				}		
			}
		default:
			break;
		}
		return uiMsg;
	}
	return 0;
}

void CSkillTreeDlg::Update( POINT ptMouse )
{
	if( !IsVision() ) return;

	CTDialog::Update( ptMouse );

	if( CTDialog* pDlg = CTDialog::GetProcessMouseOverDialog() )
		if( pDlg != (CTDialog*)this )
			return;


	CInfo	ToolTip;
	ToolTip.Clear();
	

	std::list< node_baseskill >::iterator iter_base;

	for( iter_base = m_userdata.base_skills.begin();iter_base != m_userdata.base_skills.end(); ++iter_base )
	{
		assert( iter_base->m_icon );
		if( iter_base->m_icon->IsInside( ptMouse ) )
		{
			iter_base->m_icon->GetToolTip( ToolTip ,DLG_TYPE_SKILLTREE, INFO_STATUS_DETAIL );
			ptMouse.x += 20;
			ToolTip.SetPosition( ptMouse );

			if( !ToolTip.IsEmpty() )
				CToolTipMgr::GetInstance().RegistInfo( ToolTip );

			return;
		}
	}

	
	if( !m_userdata.base_skills.empty() )
	{
		iter_base = m_userdata.base_skills.begin();
		std::list<node_skill>::iterator iter_children;
		for( iter_children = iter_base->m_children.begin(); iter_children != iter_base->m_children.end(); ++iter_children )
		{
			assert( iter_children->m_icon );
			if( iter_children->m_icon->IsInside( ptMouse ) )
			{
				iter_children->m_icon->GetToolTip( ToolTip, DLG_TYPE_SKILLTREE, INFO_STATUS_DETAIL );
				ptMouse.x += 20;
				ToolTip.SetPosition( ptMouse );
				if( !ToolTip.IsEmpty() )
					CToolTipMgr::GetInstance().RegistInfo( ToolTip );

				return;
			}
		}

	}
}

void CSkillTreeDlg::Update( CObservable* pObservable, CTObject* pObj )
{
	assert( pObservable );
	assert( pObj );

	if( pObj == NULL || strcmp( pObj->toString(), "CTEventSkill" ) )
	{
		assert( 0 && "Invalid Event Type or Null" );
		return;
	}

	CTEventSkill* pEvent = (CTEventSkill*)pObj;

	int iSkillSlotIndex = pEvent->GetIndex();
	int iPage = iSkillSlotIndex / MAX_LEARNED_SKILL_PER_PAGE;


	switch( pEvent->GetID() )
	{
	case CTEventSkill::EID_ADD_SKILL :
		{
			AddSkill( iSkillSlotIndex );
			break;
		}
	case CTEventSkill::EID_DEL_SKILL :
		{
			RemoveSkill( iSkillSlotIndex );
			break;
		}
	case CTEventSkill::EID_LEVELUP:
		LevelupSkill( iSkillSlotIndex );
		break;
	default:
		assert( 0 && "Invalid Event Skill Type");
		break;
	}
}

void CSkillTreeDlg::LevelupSkill( int skillslotindex )
{
	if( !g_pAVATAR ) return ;
	CSkillSlot* pSlot = g_pAVATAR->GetSkillSlot();
	assert( pSlot );
	if( pSlot == NULL ) return;

	CSkill* pSkill = pSlot->GetSkill( skillslotindex );

	assert( pSkill );
	if( pSkill == NULL ) 
		return;
	
	int base_skillindex = SKILL_1LEV_INDEX ( pSkill->GetSkillIndex() );
	
	std::list< node_baseskill >::iterator iter_base;
	std::list< node_skill >::iterator iter_child;
	
	for( iter_base = m_userdata.base_skills.begin();iter_base != m_userdata.base_skills.end(); ++iter_base )
	{
		if( iter_base->m_index == base_skillindex && iter_base->m_level == pSkill->GetSkillLevel())
		{
			assert( iter_base->m_icon );
			SAFE_DELETE( iter_base->m_icon );
			iter_base->m_icon = new CIconSkill( skillslotindex );
			MoveWindow( m_sPosition );
		}

		for( iter_child = iter_base->m_children.begin(); iter_child != iter_base->m_children.end(); ++iter_child )
		{
			if( iter_child->m_index == base_skillindex )
			{
				if( iter_child->m_level == pSkill->GetSkillLevel() )
				{
					assert( iter_child->m_icon );
					SAFE_DELETE( iter_child->m_icon );
					iter_child->m_icon = new CIconSkill( skillslotindex );
					MoveWindow( m_sPosition );
				}
				else if( iter_child->m_level_limit && iter_child->m_level_limit + 1 == pSkill->GetSkillLevel() )
				{
					assert( iter_child->m_icon );
					SAFE_DELETE( iter_child->m_icon );
					iter_child->m_icon = new CIconSkillDummy( pSkill->GetSkillIndex() - 1 );
					iter_child->m_icon->SetDrawColor( D3DCOLOR_ARGB( 255,255,255,255 ) );
					MoveWindow( m_sPosition );
				}
			}
		}
	}
}
void CSkillTreeDlg::AddSkill( int skillslotindex )
{
	if( !g_pAVATAR ) return ;
	CSkillSlot* pSlot = g_pAVATAR->GetSkillSlot();
	assert( pSlot );
	if( pSlot == NULL ) return;

	CSkill* pSkill = pSlot->GetSkill( skillslotindex );

	assert( pSkill );
	if( pSkill == NULL ) 
		return;
	
	int base_skillindex = SKILL_1LEV_INDEX ( pSkill->GetSkillIndex() );
	
	std::list< node_baseskill >::iterator iter_base;
	std::list< node_skill >::iterator iter_child;
	
	for( iter_base = m_userdata.base_skills.begin();iter_base != m_userdata.base_skills.end(); ++iter_base )
	{
		if( iter_base->m_index == base_skillindex && iter_base->m_level <= pSkill->GetSkillLevel() )
		{
			assert( iter_base->m_icon );
			SAFE_DELETE( iter_base->m_icon );
			iter_base->m_icon = new CIconSkill( skillslotindex );
			MoveWindow( m_sPosition );
		}

		for( iter_child = iter_base->m_children.begin(); iter_child != iter_base->m_children.end(); ++iter_child )
		{
			if( iter_child->m_index == base_skillindex )
			{
				if( iter_child->m_level <= pSkill->GetSkillLevel() )
				{
					assert( iter_child->m_icon );
					SAFE_DELETE( iter_child->m_icon );
					iter_child->m_icon = new CIconSkill( skillslotindex );
					MoveWindow( m_sPosition );
				}
			}
		}
	}
}

void CSkillTreeDlg::RemoveSkill( int skillslotindex )
{
	if( !g_pAVATAR ) return ;
	CSkillSlot* pSlot = g_pAVATAR->GetSkillSlot();
	assert( pSlot );
	if( pSlot == NULL ) return;

	CSkill* pSkill = pSlot->GetSkill( skillslotindex );

	assert( pSkill );
	if( pSkill == NULL ) 
		return;
	
	int base_skillindex = SKILL_1LEV_INDEX ( pSkill->GetSkillIndex() );
	
	std::list< node_baseskill >::iterator iter_base;
	std::list< node_skill >::iterator iter_child;
	
	for( iter_base = m_userdata.base_skills.begin();iter_base != m_userdata.base_skills.end(); ++iter_base )
	{
		if( iter_base->m_index == base_skillindex )
		{
			assert( iter_base->m_icon );
			SAFE_DELETE( iter_base->m_icon );
			iter_base->m_icon = new CIconSkillDummy( iter_base->m_index );
			MoveWindow( m_sPosition );
		}

		for( iter_child = iter_base->m_children.begin(); iter_child != iter_base->m_children.end(); ++iter_child )
		{
			if( iter_child->m_index == base_skillindex )
			{
				assert( iter_child->m_icon );
				SAFE_DELETE( iter_child->m_icon );
				iter_child->m_icon = new CIconSkillDummy( iter_child->m_index );
				MoveWindow( m_sPosition );
			}
		}
	}
}
