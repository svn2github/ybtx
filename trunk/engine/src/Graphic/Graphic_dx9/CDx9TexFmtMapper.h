#pragma once
#include "GraphicBase.h"
#include "CTexFmtMapper.h"
#include "Dx9Base.h"


namespace sqr
{
	class CDx9TexFmtMapper
		: public CTexFmtMapper
	{
	public:
		CDx9TexFmtMapper();
		~CDx9TexFmtMapper();

		static CDx9TexFmtMapper* GetInst()
		{
			return static_cast<CDx9TexFmtMapper*>(CTexFmtMapper::GetInst());
		}

		D3DFORMAT
				GetSuitableTextureFormat( eTextureFormat fmt );

		D3DFORMAT 
				GetDx9Format( eTextureFormat fmt );

		uint32	GetBitsPerPixel( eTextureFormat fmt );

		uint32	GetLinesPerPitch( eTextureFormat fmt );

		uint32	GetPitch( eTextureFormat fmt, uint32 nWidth );

		D3DFORMAT	
				GetDataFormat( eTextureFormat fmt );

		D3DFORMAT
				GetUseFormat( eTextureFormat fmt );

	private:
		void	RegisterTexureFormat( 
					eTextureFormat fmt, 
					int32 nBitsPerPixel,
					int32 nLinePerPitch,
					int32 nPitch,
					D3DFORMAT dataFormat,
					D3DFORMAT useFormamt 
					);

		HRESULT	EnumSuitableTextureFormat();

	private:
		struct TexFmtInfo 
		{
			int32		nBitsPerPixel;
			int32		nLinePerPitch;
			int32		nPitch;
			D3DFORMAT	dataFormat;
			D3DFORMAT	useFormat;
		};

		typedef map<eTextureFormat, TexFmtInfo> FmtMap;

		FmtMap		m_fmtMap;
		D3DFORMAT	m_TextFmt[2][2];
		D3DFORMAT	m_DepthTexFormat;
	};
}