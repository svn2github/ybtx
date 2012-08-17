#include "stdafx.h"
#include "CDynamicVB.h"
#include "CVertexHardwareBuffer.h"
#include "CIndexHardwareBuffer.h"
#include "CRenderPipeline.h"
#include "CLightSystem.h"
#include "BaseHelper.h"
#include "StringHelper.h"
#include "CVertexDeclaration.h"
#include "TSqrAllocator.inl"

CDynamicVB::CDynamicVB(void) : CDynamicPool(DefaultSize,HBL_DISCARD)
{
	m_CurStack			= 0;
	m_CurVBOffset		= 0;
	m_CurIBOffset		= 0;
	m_LightIndex		= 0;
}

CDynamicVB::~CDynamicVB(void)
{
	Destroy();
}

void	CDynamicVB::Destroy( void )
{
	ResetStack();
	m_RenderOP.Destroy();
	CDynamicPool::Destroy();
}

HRESULT CDynamicVB::Create( uint32 nVBSize, uint32 nIBSize, uint32 StackSize )
{
	m_RenderOP.Create();
	SetVertexBlockSize(nVBSize);
	SetIndexBlockSize(nIBSize);
	m_RenderStatck.resize(StackSize);

	m_CurVBOffset		= m_VerBlockSize;
	m_CurIBOffset		= m_IdxBlockSize;
	m_pActiveVB			= NULL;
	m_pActiveIB			= NULL;
	m_CurStack			= 0;

	ResetPool();
	return 0;
}

void CDynamicVB::Render()
{
	if (IsEmpty())
		return;

	bool IsCrush	= false;
	uint32 CurStack =	m_CurStack;
	m_CurStack		= 0;

	SQR_TRY
	{
		if(m_pActiveVB)
		{
			m_pActiveVB		= NULL;
			UnLockVertex( m_CurVBOffset );
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CDynamicVB::UnLockVertex");
		GfkLogExpOnce(exp);
		IsCrush = true;
	}
	SQR_TRY_END;

	SQR_TRY
	{
		if(m_pActiveIB)
		{
			m_pActiveIB		= NULL;
			UnLockIndex( m_CurIBOffset );
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CDynamicVB::UnLockIndex");
		string szErrMsg = format("render stack size:%u", m_RenderStatck.size());
		exp.AppendMsg(szErrMsg);
		GfkLogExpOnce(exp);
		IsCrush = true;
	}
	SQR_TRY_END;

	m_VerCurNum		= 0;
	m_IdxCurNum		= 0;
	m_VerOffset		= m_VerBlockSize;
	m_IdxOffset		= m_IdxBlockSize;
	m_CurVBOffset	= m_VerBlockSize;
	m_CurIBOffset	= m_IdxBlockSize;	

	if(IsCrush)
		return;

	CRenderPipeline::GetInst()->SetTextureMatrixImmediate(0,NULL);
// 	CLightSystem* pLight;
// 	m_pTarget->GetGroup()->GetLightSystem(*pLight);
// 	pLight->TurnOnGlobalLight(m_LightIndex);
	for ( uint32 i = 0; i < CurStack; i++ )
	{
		CRenderStack& RenderStack=m_RenderStatck[i];
		m_RenderOP.indexData->indexBuffer = RenderStack.m_pIB;
		m_RenderOP.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(RenderStack.m_VerFormat);
		m_RenderOP.vertexData->vertexBufferBinding->SetBinding(0,RenderStack.m_pVB, RenderStack.m_VBOffset, 0,RenderStack.m_Stride);
		m_RenderOP.vertexData->vertexStart = 0;
		m_RenderOP.vertexData->vertexCount = RenderStack.m_VertexNum;
		m_RenderOP.indexData->indexStart = RenderStack.m_IBOffset>>1;
		m_RenderOP.indexData->indexCount = RenderStack.m_IndexNum;
		m_RenderOP.operationType = RenderStack.m_PriType;
		m_RenderOP.useIndexes = (RenderStack.m_IndexNum != 0);
		CRenderPipeline::GetInst()->Render(&m_RenderOP,&(RenderStack.m_RP));
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S0);
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S1);
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S2);
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S3);
	}
	CRenderPipeline::GetInst()->_setVertexBufferBinding(NULL);
	ResetPool();
}

//HRESULT CDynamicVB::DrawAll()
//{
//	Render();
//	return 0;
//}

void CDynamicVB::ResetStack()
{
	if(m_pActiveVB)
	{
		m_pActiveVB			= NULL;
		UnLockVertex( 0 );
	}

	if(m_pActiveIB)	
	{
		m_pActiveIB			= NULL;
		UnLockIndex( 0 );
	}
	
	m_CurVBOffset		= m_VerBlockSize;
	m_CurIBOffset		= m_IdxBlockSize;
	size_t CurStack = m_CurStack;
	m_CurStack = 0;
	
	for ( uint32 i = 0; i < CurStack; ++i )
	{
		CRenderStack& RenderStack=m_RenderStatck[i];
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S0);
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S1);
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S2);
		SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S3);
	}
	
	ResetPool();
}

RenderParam& CDynamicVB::GetRenderStack( EPrimitiveType PrimitiveType,
        uint32 VertexNum,
        uint32 PrimitiveCount,
        uint32 VerFormat,
        uint32 VerStride,
        void** ppVB,
        void** ppIB )
{
	if ( PrimitiveCount == 0 )
		GenErr( GraphicErr::GetErrTypeStr(RenderRuntime_Err),"非法的三角形数量" );

	uint32 VBSize = VertexNum*VerStride;
	uint32 IBSize = 0;
	
	switch (PrimitiveType)
	{
		case PT_LINELIST:
			if (ppIB)
				IBSize = PrimitiveCount*4;
			else
				VertexNum = PrimitiveCount*2;
			break;
		case PT_TRIANGLELIST:
			if (ppIB)
				IBSize = PrimitiveCount*6;
			else
				VertexNum = PrimitiveCount*3;
			break;
	}

	if (VBSize + m_CurVBOffset> m_VerBlockSize)
	{
		if(m_pActiveVB)
		{
			m_pActiveVB	= NULL;
			UnLockVertex( m_CurVBOffset );
		}
		
		m_pVerBuf	  = 0;
		m_CurVBOffset = 0;
		size_t dummy;
		m_pActiveVB = LockVertexBuffer(max(m_VerBlockSize,VBSize),(void**)&m_pVerBuf, dummy);
	}
		
	if(IBSize>0 && IBSize + m_CurIBOffset > m_IdxBlockSize)
	{	
		if(m_pActiveIB)
		{
			m_pActiveIB = NULL;
			UnLockIndex( m_CurIBOffset );
		}
	
		m_pIdxBuf	  = 0;	
		m_CurIBOffset = 0;
		size_t dummy;
		m_pActiveIB = LockIndexBuffer(max(m_IdxBlockSize,IBSize),(void**)&m_pIdxBuf, dummy);
	}

	SQR_TRY
	{
		if ( m_CurStack >= (uint32)m_RenderStatck.size() )
			m_RenderStatck.push_back( CRenderStack() );
	}
	SQR_CATCH(exp)
	{
		ResetPool();
		string szErrMsg = format("render stack size:%u", m_RenderStatck.size());
		exp.AppendType("CDynamicVB渲染堆栈空间不足");
		exp.AppendMsg(szErrMsg);
		SQR_THROW (exp);
	}
	SQR_TRY_END
	
	CRenderStack& TempStack  = m_RenderStatck[m_CurStack];

	TempStack.m_pVB				= m_pActiveVB;
	TempStack.m_pIB				= m_pActiveIB;
	TempStack.m_PriType			= PrimitiveType;
	TempStack.m_PrimitiveCount	= PrimitiveCount;
	TempStack.m_IndexNum		= IBSize/2;
	TempStack.m_VertexNum		= VertexNum;
	TempStack.m_VerFormat		= VerFormat;
	TempStack.m_Stride			= VerStride;
	TempStack.m_IBOffset		= m_CurIBOffset;
	TempStack.m_VBOffset		= m_CurVBOffset;
	TempStack.m_RP.m_RS.m_ShaderType.Clear();

	*ppVB =  m_pVerBuf + m_CurVBOffset;
	m_CurVBOffset += VBSize;

	if ( ppIB )
	{
		*ppIB =  m_pIdxBuf + m_CurIBOffset;
		m_CurIBOffset += IBSize;
	}

	CRenderStack& RenderStack =  m_RenderStatck[ m_CurStack ];
	m_CurStack++;
	SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S0);
	SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S1);
	SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S2);
	SafeRelease(RenderStack.m_RP.m_RS.m_Texture_S3);
	return RenderStack.m_RP;
}

//void CDynamicVB::OptimizeDP(void)
//{
//	if(m_CurStack>1)
//	{
//		CRenderStack& LastStack =  m_RenderStatck[ m_CurStack ];
//		CRenderStack& PreStack  =  m_RenderStatck[ m_CurStack - 1 ];
//		if(LastStack.m_pVB == PreStack.m_pVB &&
//			LastStack.m_pIB == PreStack.m_pIB &&
//			LastStack.m_VerFormat == PreStack.m_VerFormat &&
//			LastStack.m_RP.m_RS.m_Texture_S0 == LastStack.m_RP.m_RS.m_Texture_S1 &&
//			LastStack.m_RP.m_RS.m_TexOP_S0 == LastStack.m_RP.m_RS.m_TexOP_S0 &&
//			LastStack.m_RP.m_Mat == LastStack.m_RP.m_Mat )
//		{
//			PreStack.m_VertexNum+= LastStack.m_VertexNum;
//			PreStack.m_IndexNum += LastStack.m_IndexNum;
//			PreStack.m_PrimitiveCount += LastStack.m_PrimitiveCount;
//			--m_CurStack;
//		}
//	}
//}
