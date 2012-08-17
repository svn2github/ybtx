#pragma once
#include "GraphicBase.h"
#include "../../src/Graphic/Graphic_dx9/Dx9Base.h"

class CTexture 
{
public:
	enum	ESaveType { eSTY_TEX = 1, eSTY_DDS, eSTY_COUNT };

public:
	static sqr::eTextureFormat	  TransToLocalFormat( D3DFORMAT fmt );

public:
	CTexture(void);
	~CTexture(void);

	BOOL            IsValid()			const	{ return m_pTextures != NULL; }
	UINT            GetWidth()			const	{ return m_Head.uWidth;  }
	UINT			GetOriginWidth()	const	{ return m_OldHead.uWidth; };
	UINT            GetHeight()			const	{ return m_Head.uHeight; }
	UINT			GetOriginHeight()	const	{ return m_OldHead.uHeight; }
	UINT			GetMipMapCount()	const	{ return m_Head.uMipMap;  }
	UINT			GetFrameCount()		const	{ return m_Head.uFrameCount; }
	UINT			GetOriginMipMapCount() const { return m_OldHead.uMipMap; }
	const string&	GetFileName()		const	{ return m_Filename; }
	int				GetFrameTime()		const	{ return m_Head.uFrameCircle; }
	LPDIRTEX		GetD3DTexture()		const;												//渲染时设定贴图
    sqr::eTextureFormat   GetFormat()	const	{ return m_Head.eFormat;   }
	sqr::eTextureFormat   GetOriginFormat()	const	{ return m_OldHead.eFormat;   }
	void			AddExFrame( LPCTSTR szFileName );

	int  LoadPic( LPDIRDEV pD3dDev, LPCTSTR TextName );                                     //载入文件
	int  SaveTEX( LPDIRDEV pD3dDev, LPCTSTR TextName, sqr::eTextureFormat Format, UINT nMipMap, int nFrameTime ); //保存文件
	int  SaveAsTga( LPCTSTR TextName );
	int  Free();						//释放资源										
	void SetUseOriginSize( BOOL bUseOriginSize ) { m_bUseOriginSize = bUseOriginSize; }

private:
	sqr::sTextureInfo	m_Head;						// 文件头
	sqr::sTextureInfo	m_OldHead;					// 原始文件信息
	string				m_Filename;
	LPDIRECT3DTEXTURE9*	m_pTextures;				// 贴图
	LPDIRDEV			m_pD3dDev;

	ESaveType			m_SaveType;
	BOOL				m_bSizeChanged;				// 纹理尺寸是否被改变
	BOOL				m_bUseOriginSize;			// 是否使用原始纹理尺寸

	int  LoadCOM( LPDIRDEV pD3dDev, LPCTSTR TextName );	//载入标准文件，bmp，tga，jpg
	int  LoadTEX( LPDIRDEV pD3dDev, LPCTSTR TextName );	//载入tex（暂定）文件
};