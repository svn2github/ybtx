#pragma once
#include "BaseDefs.h"

	class IIndexGenerator;
	
	class CImageSplitter
	{
		static const uint32		DELTA_X		= 256;
		static const uint32		DELTA_Y		= 256;
		static const D3DFORMAT	SAVE_FORMAT = D3DFMT_DXT1;

		uint32			m_uWidth;
		uint32			m_uHeight;
		string			m_szTgtPath;
		string			m_szSrcPath;
		string			m_szSrcFile;
		IIndexGenerator	*m_pIndexGenerator;

		LPDIRECT3DTEXTURE9	m_pTexOld;

		CImageSplitter(const CImageSplitter&);
		CImageSplitter& operator = (const CImageSplitter&);

		// ---- 读取并将原图片缩放到256的n次方尺寸
		bool	_ReadOldPicture();

		// ---- 生成目标图片
		bool	_GenerateTargetPicture( void *pSrcBuf, uint32 uPitch, 
										RECT &rectSrc, const string &szTarget);

		// ---- 执行切分
		bool	_DoCut();

	public:
		CImageSplitter(IIndexGenerator *pIndexGenerator);
		~CImageSplitter();

		// ---- 设置原图片路径
		void	SetSourceFile(const string &szSrcFile);

		// ---- 设置切分后的图片的存储目录
		void	SetTargetPath(const string &szTargetPath);
		
		// ---- 设置区域信息
		void	SetRgnInfo(uint32 uWidth, uint32 uHeight);

		// ---- 执行切分
		bool	Run();
	};