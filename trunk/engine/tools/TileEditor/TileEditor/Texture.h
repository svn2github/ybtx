#pragma once

#include "BluelyG.h"

#define TEXTURE_VERSION 10003

class CTexture 
{
	TEXTINFO	m_Head;                 //文件头
	string		m_Filename;
	LPDIRTEX	m_Texture;				//贴图

	LPDIRDEV	m_pD3dDev;

	int  LoadCOM( LPDIRDEV pD3dDev, LPCTSTR TextName );                         //载入标准文件，bmp，tga，jpg
	int  LoadTEX( LPDIRDEV pD3dDev, LPCTSTR TextName );                         //载入tex（暂定）文件

	int  SaveCompress( LPDIRDEV pD3dDev, LPCTSTR TextName, TEXTINFO& SaveHead );//保存为压缩格式
	int  SaveHighColor( LPCTSTR TextName, TEXTINFO& SaveHead );          //保存为高颜色格式

public:
	CTexture(void);
	~CTexture(void);

	BOOL            IsValid()		const	{ return m_Texture != NULL; }
	UINT            GetWidth()		const	{ return m_Head.dwWidth;  }
	UINT            GetHeight()		const	{ return m_Head.dwHeight; }
	TEXTUREFORMAT   GetFormat()		const	{ return m_Head.Format;   }
	const string&	GetFileName()	const	{ return m_Filename; }
	LPDIRTEX		GetD3DTexture() const	{ return m_Texture; }                                  //渲染时设定贴图

	int  LoadPic( LPDIRDEV pD3dDev, LPCTSTR TextName );                                    //载入文件
	int  SaveTEX( LPDIRDEV pD3dDev, LPCTSTR TextName, TEXTUREFORMAT Format = TF_UNKNOW );  //保存文件
	int  Free();                                                                    //释放资源
	int  AlphaImport( CTexture& AlphaCannel );										//将指定图片作为ALPHA通道
};
