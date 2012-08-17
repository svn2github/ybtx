#include "stdafx.h"
#include "CRenderStyleCoder.h"
#include "CRenderStyleEx.h"
#include "CTexture.h"

namespace 
{
}

namespace sqr
{
	BEGIN_VERSION_MAP(CRenderStyleCoder)
		ON_VERSION(1, DeCode1)
	END_VERSION_MAP()

	FourCC CRenderStyleCoder::GetChunkName()
	{
		return 'RSEX';
	}

	uint16 CRenderStyleCoder::GetNewVer()
	{
		return 1;
	}

	bool CRenderStyleCoder::Code( WriteDataInf& wdi, OBJ_HANDLE handle )
	{
		CRenderStyleEx* rs = static_cast<CRenderStyleEx*>(handle);

		// write texture info
		wdi << rs->m_DefaultTexture.size();
		
		TexturesPoolIter tex_it		= rs->m_DefaultTexture.begin();
		TexturesPoolIter tex_eit	= rs->m_DefaultTexture.end();
		for ( ; tex_it != tex_eit; ++tex_it )
		{
			wdi << (*tex_it)->GetName();
		}

		// write color info
		wdi.WriteMap( rs->m_ColorParams.m_Ambient );
		wdi.WriteMap( rs->m_ColorParams.m_Diffuse );
		wdi.WriteMap( rs->m_ColorParams.m_Specular );
		wdi.WriteMap( rs->m_ColorParams.m_Emissive );
		wdi.WriteMap( rs->m_ColorParams.m_Specularpower );
		wdi.WriteMap( rs->m_ColorParams.m_Alpharef );
		wdi.WriteMap( rs->m_ColorParams.m_Texturefactor );
		
		// write blend info
		wdi << rs->m_BlendParams;

		return true;
	}

	bool CRenderStyleCoder::DeCode1( const CDataChunk& chunk, OBJ_HANDLE handle )
	{
		CRenderStyleEx* rs = static_cast<CRenderStyleEx*>(handle);
		CBufFile buf( chunk.GetBuffer(), chunk.GetSize() );

		// read texture info
		size_t texNameNum = 0;
		buf >> texNameNum;

		vector<GString> texNameList(texNameNum);
		for ( size_t i = 0; i<texNameNum; ++i )
		{
			size_t texNameLen = 0;
			buf >> texNameLen;
			texNameList[i].resize(texNameLen);
			buf.SafeRead( &texNameList[i][0], texNameLen );
		}
		
		// read color info
		buf.SafeReadMap( rs->m_ColorParams.m_Ambient );
		buf.SafeReadMap( rs->m_ColorParams.m_Diffuse );
		buf.SafeReadMap( rs->m_ColorParams.m_Specular );
		buf.SafeReadMap( rs->m_ColorParams.m_Emissive );
		buf.SafeReadMap( rs->m_ColorParams.m_Specularpower );
		buf.SafeReadMap( rs->m_ColorParams.m_Alpharef );
		buf.SafeReadMap( rs->m_ColorParams.m_Texturefactor );

		// read blend info
		buf >> rs->m_BlendParams;

		return true;
	}
}