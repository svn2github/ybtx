#pragma once
#include "CRenderBatch.h"
#include "CGeometryInstance.h"
#include "TSingleton.h"

namespace sqr
{
	class CGeometryBatch;

	enum EGeoBufType
	{
		EGBT_FILTER = 0,
		EGBT_ANI	= 1,
		EGBT_RS		= 2,
		EGBT_MODIFY	= 3,
		EGBT_MESH	= 4,
		EGBT_RENDER	= 5, //渲染最后节点
		EGBT_COUNT,
		FORCE_DWORD = 0xFFFFFFFF,
	};

	struct CreateFlag
	{
		static EGeoBufType	GBQueue[EGBT_COUNT];
		static UINT			GBIndex[EGBT_COUNT];

		CreateFlag();

		typedef CGeometryBatch* (*GCreateFun)(const CreateFlag& fFlag, BATCH_HANDLE fHandle, CGeometryParams* pParams, CGeometryBatch* pParent);
		
		GCreateFun			CreateRenderBatch;
		EGeoBufType			CreateBufType[EGBT_COUNT];
		UINT				CreateIndex;
		CGeometryBatch**	CreateBatch;
	};

	CGeometryBatch*	CreateGeometryBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle,
		CGeometryParams* pParams, CGeometryBatch* pParent = NULL);
	BATCH_HANDLE		GetBufferHandle(CGeometryInst* pGInst,EGeoBufType fGBT);

	class CGeometryBatch : public CRenderBatch
	{
	protected:
		typedef	GMap< BATCH_HANDLE, CGeometryBatch* > GeometryBatchMap;
	public:
		CGeometryBatch(const CreateFlag& fFlag, BATCH_HANDLE fHandle,
			CGeometryParams* pParams, CGeometryBatch* pParent);
		virtual ~CGeometryBatch();

		virtual void	ClearInstances(void);
		virtual void	AddGeometryInst( CGeometryInst* pGInst );
		virtual void	Render(void);

		virtual void	_UpdateBatch(void){};

		virtual void	ModifyRS(STATIC_RS&	RS){};
	protected:
		typedef	GVector<CGeometryBatch*>	RenderBatchVec;
		virtual void	_UpdateInst( CGeometryInst* pGInst ){};
		void			_DestroyBatch(BATCH_HANDLE fHandle);
		void			_Destroy(void);
		bool				m_bNeedUpdate;
		GeometryBatchMap	m_GeometryBuffer;
		BATCH_HANDLE		m_CurHandle;
		CGeometryParams*	m_pGeometryParams;	
		CGeometryBatch*		m_pParentBatch;
		CGeometryBatch*		m_vecRenderBatch[EGBT_COUNT];
		CreateFlag			m_CurCreateFlag;
	};

	inline void CGeometryBatch::_Destroy(void)
	{
		if(m_pParentBatch)
			m_pParentBatch->_DestroyBatch(m_CurHandle);
	}
}