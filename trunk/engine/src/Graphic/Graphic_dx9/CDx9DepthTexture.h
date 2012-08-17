#pragma once
#include "CRenderTexture.h"
#include "Dx9Base.h"

namespace sqr
{
	class DX9DepthTexture : public	CRenderTexture
	{
	public:
		DX9DepthTexture();
		~DX9DepthTexture();

	
		void		Begin(bool needClear = true);
		void		End();
		
		bool		ReCreate(void);
		void		release();
		void		Reset();
		
		bool		IsValid()					{ return NULL != m_depthStencil; };
		const char*	GetName()	const		{ return "DX9DepthTexture";		 };
		void*		GetBuffer() const			{ return m_depthTexture;		 };
		CRenderTexture*		GetDepthBuffer(void) { return this; }
		HRESULT		SaveToFile(const string& filename,IMAGE_FILEFORMAT Format = IFF_DDS ){return true;};

		//true是深度Surface false为深度纹理
		void		SetUseDepthBuffer(bool b)	{ m_bDepthBuffer = b; };

		HRESULT		Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 ) { return GERROR; };//暂时用不上懒得写
		HRESULT		Unlock( uint32 level, uint32 Num = 0 ) { return GERROR; };//暂时用不上懒得写

	protected:
	bool		_Create();
		///做为场景承载体 而必须的
	private:
		ID3DTexturePtr		m_depthTexture;
		ID3DSurfacePtr		m_depthStencil;
		ID3DSurfacePtr		m_oldDepthStencil;
	};
}