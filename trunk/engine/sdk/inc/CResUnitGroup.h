#pragma once
#include "CRefObject.h"
#include "GraphicBase.h"
#include "CGraphicMallocObject.h"
#include "CResourceContainer.h"
#include "GraphicRes.h"
#define HAS_FILE_FORMAT() \
	friend class CDataSources; \
	protected: static CFileFormatMgr* st_pFormat; \
	public: inline static CFileFormatMgr* FORMAT(void) { return st_pFormat; };

#define DEFINE_FILE_FORMAT(class) \
	CFileFormatMgr* class::st_pFormat = NULL;
		
namespace sqr
{
	class CResUnitGroup 	
		: public GraphicRes
		, public CResUnit
	{
		friend class CResUintNode;
	private:	
		uint32		m_NodeRef;

	protected:
		CResUnitGroup( EResType fType, const string& szName = "" , const wstring& szAlias = L"")
			: CResUnit( szName,szAlias, fType ),m_NodeRef(0) 
		{ 
			AddRef(); 
		}

		virtual ~CResUnitGroup() 
		{
		}

	private:
				uint32	AddNodeRef();
		virtual void	NodeRelease();
				void	Free();

	public:
				int32	AddRef(void);// { return GraphicRes::AddRef(); }
	};

	class CResUintNode 
		: public CGraphicMallocObject
		, public CRefObject
	{
	public:
		CResUintNode(CResUnitGroup* m_pGroup)
			: m_pGroup(m_pGroup){};

		virtual ~CResUintNode() { m_pGroup = NULL; }

		int32	AddRef();
		void	Release();
		virtual void	Free(){};
	protected:
		CResUnitGroup* m_pGroup;
	};

	inline int32  CResUintNode::AddRef()
	{
		m_pGroup->AddNodeRef();
		return CRefObject::AddRef();
	}

	inline void  CResUintNode::Release()
	{
		CRefObject::Release();
		m_pGroup->NodeRelease();
	}
}