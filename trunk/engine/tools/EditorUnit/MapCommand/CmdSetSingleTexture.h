#pragma once
#include "Cmd.h"
#include "CVector2.h"

namespace sqr_tools
{
	class CEditContext;
	class CCmdSetSingleTexture : public CCmd
	{
		struct SDiff
		{
			CVector2I		gridCoord;
			vector<string>  gridTextures;

			SDiff()
			{
				gridTextures.resize(2);
			};
		};

		vector<SDiff>	m_vecDiffs;
		int				m_nHotLayer;
		string			m_strHotTextureName;
		CEditContext*	m_pEditContext;

		virtual void _Execute();
		virtual void _UnExecute();
	public:
		CCmdSetSingleTexture(CEditContext* pContext);
		~CCmdSetSingleTexture() { }


		void AddGrid(const CVector2I& gridcoord);
		void SetHotLayer(const int nHotLayer)
		{
			m_nHotLayer = nHotLayer;
		}
		void SetHotLayerTextureName(const string& texname)
		{
			m_strHotTextureName = texname;
		}
	};

}