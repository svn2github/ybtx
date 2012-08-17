#pragma once
#include "Cmd.h"
#include "CVector3.h"

namespace sqr_tools
{
	class CTerrainModelBrush;
	class CEditContext;
	class CEditModel;
	class CCmdPlaceObject : public CCmd
	{
		struct SDiffBornInfo
		{
			CEditModel*		pParentModel;
			CVector3f		vPosition;
			
			SDiffBornInfo() : pParentModel(NULL) { }
			~SDiffBornInfo() {}
			
		};

		SDiffBornInfo		m_diff;

		CEditContext*		m_pEditContext;
		CTerrainModelBrush* m_pModelBrush;

	public:
		CCmdPlaceObject(CEditContext* pContext);
		~CCmdPlaceObject() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void SetTerrainBrush(CTerrainModelBrush* pBrush);
		void SetActiveModel(CEditModel* pModel);
		void SetActiveModelPosition(const CVector3f& pos);
	};

}