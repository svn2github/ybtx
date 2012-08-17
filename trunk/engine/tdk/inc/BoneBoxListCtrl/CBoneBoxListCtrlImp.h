#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr_tools
{
	class CBoneBoxListDelegate;
	class CBoneBoxListCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CBoneBoxListCtrlImp);
	public:
		CBoneBoxListCtrlImp(void* param);
		~CBoneBoxListCtrlImp(void);

	public:
		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
		virtual void UpdateList(void);
		virtual void UpdateData(std::vector<std::string> str_vector);
		virtual int  GetBoneNum(void);
		virtual std::string GetBoneNameByIndex(int index);
		virtual void SetSelectedBoneName(std::string name);
		virtual int  HasBone(std::string name);
		virtual int	 AddSkeletalBox( const string& SkName );
		virtual bool DelSkeletalBox( const string& SkName );
		virtual std::string AddPickBoneBox(void);
		virtual	bool FindItem( const string& SkName );
		virtual void LoadDefaultBone(void);
		virtual void AddHead(void);
		virtual bool IsPieceGroupChanged(void);

	private:
		std::vector<std::string> m_vtBoneNameList;
		std::string m_strLastSkeName;
		std::string m_strSelBoneName;
		int m_iItemNum;
		CBoneBoxListDelegate* m_pDelegate;
	};
}
