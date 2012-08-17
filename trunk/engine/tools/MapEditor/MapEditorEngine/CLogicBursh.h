#pragma once

namespace sqr
{
	class ITexture;
	struct RenderState;

	class CLogicBrush
	{

		DWORD m_dwBrushSizeInX;
		DWORD m_dwBrushSizeInZ;
		DWORD m_dwColor;

		//ª≠À¢Œ∆¿Ì∂‘œÛ
		ITexture	*m_pBrushTexture;
		RenderState *m_pRS;

	public:
		CLogicBrush();
		~CLogicBrush();

		void  InitBrushParams( const DWORD dwBurshInX, const DWORD dwBrushInZ, const DWORD color, const string& strBurshTexName );
		void  SetBrushTextureName( const string& strTexName );

		DWORD GetBrushSizeInX();
		void  SetBrushSizeInX(DWORD dw);
		void  IncBrushSizeInX();
		void  DecBrushSizeInX();

		DWORD GetBrushSizeInZ();
		void  SetBrushSizeInZ(DWORD dw);
		void  IncBrushSizeInZ();
		void  DecBrushSizeInZ();

		vector<int>& GetBrushGridIndices();
		vector<int>& GetBrushSubRegionIndices();

		void  Render();
	};
}