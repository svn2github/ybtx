#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr
{
	class CSkeletal;
}

namespace sqr_tools
{
	class CBoneListCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CBoneListCtrlImp);
	public:
		CBoneListCtrlImp(void* param);
		~CBoneListCtrlImp(void);

	public:
		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
		virtual std::string GetRootSkeletalName(void);
		virtual void FindSkeletal( const CSkeletal* ske, const CSkeletal** resultSke, std::string strName );
		virtual int GetChildSkeletalNum( std::string strName );
		virtual std::string GetChildSkeletalNameByIndex( std::string strName, int index );
		virtual void Reset(void);
		virtual void AddNode(std::string name);
		//virtual void AddNodes(std::vector<std::string> str_vector);
		virtual void SendMsg(void);

	private:
		std::string m_strLastSkeName;
		std::vector<std::string> m_vtBoneNames;
	};

}
