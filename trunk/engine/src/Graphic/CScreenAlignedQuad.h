#pragma once
//------------------------------------------------------------------------------
/**
	@class CScreenAlignedQuad

	全屏幕矩形, 后处理用.
 */
#include "TSingleton.h"
#include "CRenderOperation.h"
namespace sqr
{
class CVertexDeclaration;
class ITexture;

class CScreenAlignedQuad 
	: public Singleton<CScreenAlignedQuad>
	, public CGraphicMallocObject
{
	CScreenAlignedQuad();
	~CScreenAlignedQuad();
	friend class Singleton<CScreenAlignedQuad>;
public:
	void Render();
private:
	CRenderOperation	m_Op;
};

}// namespace sqr
