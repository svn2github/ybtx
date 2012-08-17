#pragma once
#include "ExsertStructDB.h"

namespace sqr_tools
{
	class CEditModel;
}

namespace sqr
{
	class IEffect;
	class CPieceGroup;
	class IEffectGroup;

	class CTileEditorModel
	{
	private:

		CEditModel *m_pModel;
		IEffect *m_pEffect;
		IEffectGroup *m_pGroup;

		ECenter_Type m_eCenterType;

		string m_strFileName;

		bool m_bSubUnit;

	public:

		CTileEditorModel();
		~CTileEditorModel();

		void Create(string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, int &BlockInfoNum, int &LogicHeightInfoNum);
		void Render();
		void UnRender();


		void SetCenterTypeOrigin(ECenter_Type eCenterType);
		bool SetAnimationByAgpFileName(string strAgpFileName);

		void LinkEffect(IEffect * pEffect, IEffectGroup * pGroup);
		void UnlinkEffect();
	};
}
