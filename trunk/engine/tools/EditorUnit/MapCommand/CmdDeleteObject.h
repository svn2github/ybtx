#pragma once
#include "Cmd.h"

namespace sqr_tools
{
	class CTerrainModelBrush;
	class CEditContext;
	class CEditModel;
	class CCmdDeleteObject : public CCmd
	{
		struct SDiffBornInfo
		{
			CEditModel*		pParentModel;
			vector<string>	vecEditModelFileNames;
			string			strAgpFileName;
			string			strEffectFileName;
			string			strEffectName;
			
			SDiffBornInfo() : pParentModel(NULL) { }
			~SDiffBornInfo() {}
			
		};

		SDiffBornInfo		m_diff;

		CEditContext*		m_pEditContext;
		CTerrainModelBrush* m_pModelBrush;

	public:
		CCmdDeleteObject(CEditContext* pContext);
		~CCmdDeleteObject() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void SetEditModelFileNames(const vector<string>& names);
		void SetEditModelAgpName(const string& agpname);
		void SetEditModelEffectInfo(const string& effectFile, const string& effectname);
		void SetBornParentModel(CEditModel* pParent);
		void SetTerrainBrush(CTerrainModelBrush* pBrush);
	};

}