#pragma once
#include "Cmd.h"
#include "CVector3.h"

namespace sqr_tools
{
	class CTerrainModelBrush;
	class CEditContext;
	class CEditModel;
	class CCmdBornObject : public CCmd
	{
		struct SDiffBornInfo
		{
			CEditModel*		pParentModel;
			vector<string>	vecEditModelFileNames;
			string			strAgpFileName;
			string			strEffectFileName;
			string			strEffectName;
			float			fScale;
			CVector3f		vRotate;
			bool			bVisibleByConfigure;
			
			SDiffBornInfo() : pParentModel(NULL), fScale(1.0f), vRotate(0.0f, 0.0f, 0.0f) { }
			~SDiffBornInfo() {}
			
		};

		SDiffBornInfo		m_diff;

		CEditContext*		m_pEditContext;
		CTerrainModelBrush* m_pModelBrush;

	public:
		CCmdBornObject(CEditContext* pContext);
		~CCmdBornObject() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void SetEditModelFileNames(const vector<string>& names);
		void SetEditModelAgpName(const string& agpname);
		void SetEditModelEffectInfo(const string& effectFile, const string& effectname);
		void SetTerrainBrush(CTerrainModelBrush* pBrush);
		void SetScale(const float fScale)
		{
			m_diff.fScale = fScale;
		}
		void SetRotation(const CVector3f& rotate)
		{
			m_diff.vRotate = rotate;
		}
		void SetIsVisibleByConfigure(bool b)
		{
			m_diff.bVisibleByConfigure = b;
		}
	};

}