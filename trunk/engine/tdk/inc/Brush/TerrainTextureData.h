#pragma once

namespace sqr
{
	class ITexture;
}

namespace sqr_tools
{
	class CEditContext;
	class CTerrainTextureData
	{
	public:

		CTerrainTextureData(void);
		virtual ~CTerrainTextureData(void);

		typedef map<int, string> TerrainTexIndexPool;
		typedef map<string, int> TerrainTexNamePool;

	private:
		TerrainTexIndexPool m_mapTerrainTexIndex;
		TerrainTexNamePool  m_mapTerrainTexName;
		ITexture*			m_pTerrainTexture;

		CEditContext*		m_pContext;

	public:
		void SetEditContext( CEditContext* pOwnerContext );
		void InitTerrainTexture();
		void LoadTerrainTexture();
		void InsertTerrainTexture(const vector<string>& texnames);
		void DeleteTerrainTexture(const vector<string> &texnames);
		bool CreateTerrainTexture(const vector<string> &texnames);

		void GetTextureNameByIndex(const int& index, string& texname);
		int  GetIndexByTextureName(const string& texname);
	};
}