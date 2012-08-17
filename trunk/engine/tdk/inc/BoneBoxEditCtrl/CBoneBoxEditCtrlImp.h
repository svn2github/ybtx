#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr
{
	struct CBaseAABB;
}

namespace sqr_tools
{
	class CBoneBoxEditDelegate;
	class CBoneBoxEditCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CBoneBoxEditCtrlImp);
	public:
		CBoneBoxEditCtrlImp(void* param);
		~CBoneBoxEditCtrlImp(void);

	public:
		virtual void SetContext(CEditContext* pContext);
		virtual void UpdateData(std::string str);
		virtual int GetBoxNum();
		virtual std::string GetBoxNameByIndex(int index);
		virtual void GetBoxInfo(float& max_x, float& max_y, float& max_z, float& min_x, float& min_y, float& min_z);
		virtual void SetBoxInfo(const float max_x, const float max_y, const float max_z, const float min_x, const float min_y, const float min_z);
		virtual void DeleteBox(std::string name);
		virtual void NodeClick(std::string name);
		virtual void SetUpdateList(bool value);
		virtual bool GetUpdateList(void);
		virtual void Reset(void);
		virtual void SetShowBox(bool value);
		virtual void Render(void);

	private:
		std::vector<std::string> m_vtNames;
		std::string m_strSelBoneName;
		CBaseAABB* m_pAABB;
		bool m_bUpdateList;
		bool m_bShowBox;
		CBoneBoxEditDelegate* m_pDelegate;
	};
}
