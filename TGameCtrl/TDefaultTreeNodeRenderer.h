#ifndef _CTDEFAULTTREENODERENDERER_
#define _CTDEFAULTTREENODERENDERER_

#include "itreenoderenderer.h"
#include "TStatic.h"
class CTImage;
class TGAMECTRL_API CTDefaultTreeNodeRenderer : public CTStatic, public ITreeNodeRenderer
{
public:
	CTDefaultTreeNodeRenderer(void);
	virtual ~CTDefaultTreeNodeRenderer(void);

	///ITreeNodeRenderer
	virtual CWinCtrl* getTreeCellRendererComponent(CTree* tree, CTObject* value, bool selected, bool expanded, bool leaf, int row, bool hasFocus);

	/// CWinCtrl
	virtual void CTDefaultTreeNodeRenderer::Draw( POINT ptDraw );

	void	setOpenImage( CTImage* pImage );
	void	setCloseImage( CTImage* pImage );

	void	setParentImage( CTImage* pImage);
	void	setLeafImage( CTImage* pImage);

protected:
	CTImage*		m_pCloseImage;
	CTImage*		m_pOpenImage;
	CTImage*		m_pLeafImage;
	CTImage*		m_pParentImage;

	CTImage*		m_pNodeImage;

	D3DCOLOR		m_color;
	SIZE			m_sizeImage;///close or open image¿« size
};
#endif