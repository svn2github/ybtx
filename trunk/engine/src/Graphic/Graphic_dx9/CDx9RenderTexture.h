#pragma once
#include "CRenderTexture.h"
#include "CDx9DepthTexture.h"
#include "Dx9Base.h"

namespace sqr
{

//------------------------------------------------------------------------------
class DX9RenderTexture : public	CRenderTexture
{
public:
	DX9RenderTexture();
	~DX9RenderTexture();

	bool		IsAntiAlias() const;
	void		SetAntiAlias(bool isUseAntiAlias);
	
	bool		IsValid();

	void		Begin(bool needClear = true);
	void		End();

	void*		GetBuffer() const;
	bool		ReCreate(void);

	void		release();
	void		Reset();
	HRESULT		SaveToFile(const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS );

	CRenderTexture*		GetDepthBuffer(void)
	{
		return	&m_DepthBuffer;
	};

	const char*	GetName()	const;

	HRESULT		Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 ) { return GERROR; };//暂时用不上懒得写
	HRESULT		Unlock( uint32 level, uint32 Num = 0 ) { return GERROR; };//暂时用不上懒得写
	bool		UpdateDefaultTex(void);
protected:
	bool		_Create();
private:
	DX9DepthTexture		m_DepthBuffer;

	ID3DSurfacePtr		m_renderTarget;
	ID3DTexturePtr		m_renderTexture;
	ID3DSurfacePtr		m_oldRenderTarget;

	D3DVIEWPORT9		m_oldViewport;
	float				m_oldAspectRatio;
};
//------------------------------------------------------------------------------
inline const char*	DX9RenderTexture::GetName() const
{
	return "DX9RenderTexture";
}

//------------------------------------------------------------------------------
inline void*	DX9RenderTexture::GetBuffer() const
{
	return m_renderTexture.get_unsafe();
}

//------------------------------------------------------------------------------
inline bool	DX9RenderTexture::IsValid()
{
	return NULL != m_renderTexture;
}

}// namespace sqr
