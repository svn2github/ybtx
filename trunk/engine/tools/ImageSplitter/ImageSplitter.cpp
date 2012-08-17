#include "stdafx.h"
#include "ImageSplitter.h"
#include "IIndexGenerator.h"
#include "PathHelper.h"
#include "DirectX9.h"

	CImageSplitter::CImageSplitter(IIndexGenerator *pIndexGenerator)
		: m_uWidth(0)
		, m_uHeight(0)
		, m_pIndexGenerator(0)
		, m_pTexOld(0)
	{
		assert(pIndexGenerator);

		m_pIndexGenerator = pIndexGenerator;
	}

	CImageSplitter::~CImageSplitter()
	{
		if ( m_pIndexGenerator )
		{
			m_pIndexGenerator->Release();
			m_pIndexGenerator = 0;
		}

		if ( m_pTexOld )
		{
			m_pTexOld->Release();
			m_pTexOld = 0;
		}
	}

	void CImageSplitter::SetTargetPath( const string &szTargetPath )
	{
		m_szTgtPath = szTargetPath;

		// 创建目标路径
		::MakeDirectory(m_szTgtPath.c_str());
	}

	void CImageSplitter::SetRgnInfo( uint32 uWidth, uint32 uHeight )
	{
		assert(m_pIndexGenerator);

		m_uWidth	= uWidth;
		m_uHeight	= uHeight;
		m_pIndexGenerator->SetRaginInfo(m_uWidth, m_uHeight);
	}

	bool CImageSplitter::Run()
	{
		assert(m_pIndexGenerator);

		cout << "正在转换图片 " << m_szSrcFile << "...\n";

		bool bRet = true;

		bRet &= _ReadOldPicture();
		bRet &= _DoCut();

		return bRet;
	}

	void CImageSplitter::SetSourceFile( const string &szSrcFile )
	{
		m_szSrcFile = szSrcFile;
	}

	bool CImageSplitter::_ReadOldPicture()
	{
		assert(m_szSrcFile != "");

		uint32 uTotalWidth	= m_uWidth * DELTA_X;
		uint32 uTotalHeight = m_uHeight * DELTA_Y;

		// 读取并缩放原始图片到256的整次幂
		HRESULT hr = D3DXCreateTextureFromFileEx( 
			GetGraphicsDeviceDX9(),
			m_szSrcFile.c_str(),
			uTotalWidth,
			uTotalHeight,
			1,
			0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			D3DX_FILTER_TRIANGLE,
			D3DX_FILTER_NONE,
			0,
			0,
			0,
			&m_pTexOld
			);

		if ( FAILED(hr) )
		{
			cout << "读取原始图片 " << m_szSrcFile << "失败\n";
			return false;
		}
		
		return true;
	}

	bool CImageSplitter::_DoCut()
	{
		IIndexGenerator::IndexStruct indexStruct;
		
		HRESULT				hr = E_FAIL;
		RECT				rectSrc;
		D3DLOCKED_RECT		lockedRect;
		
		hr = m_pTexOld->LockRect(0, &lockedRect, NULL, D3DLOCK_READONLY);
		while ( m_pIndexGenerator->GetNextIndex(&indexStruct) )
		{
			rectSrc.left	= indexStruct.x * DELTA_X;
			rectSrc.top		= indexStruct.y * DELTA_Y;
			rectSrc.right	= rectSrc.left  + DELTA_X;
			rectSrc.bottom	= rectSrc.top   + DELTA_Y;

			
			if ( FAILED(hr) )
			{
				cout << "切分图片 " << m_szSrcFile << " 失败\n";
				return false;
			}

			if ( !_GenerateTargetPicture(lockedRect.pBits, lockedRect.Pitch, 
										 rectSrc, indexStruct.name) )
			{
				return false;
			}
		}
		m_pTexOld->UnlockRect(0);

		return true;
	}

	bool CImageSplitter::_GenerateTargetPicture( 
		void *pSrcBuf, uint32 uPitch, RECT &rectSrc, const string &szTarget )
	{
		HRESULT				hr			= E_FAIL;
		LPDIRECT3DTEXTURE9	pNewTex		= 0;
		LPDIRECT3DSURFACE9	pSurface	= 0;

		// 创建目标贴图
		hr = D3DXCreateTexture(
			GetGraphicsDeviceDX9(),
			DELTA_X,
			DELTA_Y,
			1,
			0,
			SAVE_FORMAT,
			D3DPOOL_MANAGED,
			&pNewTex
			);
		if ( FAILED(hr) )
		{
			cout << "切分图片 " << m_szSrcFile << " 失败\n";
			return false;
		}

		pNewTex->GetSurfaceLevel(0, &pSurface);
		hr = D3DXLoadSurfaceFromMemory(
			pSurface, 
			0, 
			0, 
			pSrcBuf, 
			D3DFMT_A8R8G8B8, 
			uPitch, 
			0, 
			&rectSrc,
			D3DX_FILTER_TRIANGLE,
			0
			);
		if ( FAILED(hr) )
		{
			cout << "切分图片 " << m_szSrcFile << " 失败\n";
			return false;
		}
		pSurface->Release();
		
		// 保存图片
		string szTgtPath = m_szTgtPath + "/" + szTarget + ".dds";
		hr = D3DXSaveTextureToFile(szTgtPath.c_str(), D3DXIFF_DDS, pNewTex, 0);
		if ( FAILED(hr) )
		{
			cout << "切分图片 " << m_szSrcFile << " 失败\n";
			return false;
		}

		pNewTex->Release();
		pNewTex = 0;
		return true;
	}