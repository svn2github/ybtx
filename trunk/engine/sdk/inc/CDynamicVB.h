#pragma once
#include "CRenderStyle.h"
#include "CRenderOperation.h"
#include "CRenderBatch.h"
#include "CDynamicPool.h"

namespace sqr
{
	class CVertexHardwareBuffer;
	class CIndexHardwareBuffer;

	//动态顶点缓冲区
	class CDynamicVB 
		: public CRenderBatch
		, protected CDynamicPool
	{
		//渲染堆栈
		struct CRenderStack
		{
			CRenderStack()
				: m_PriType(PT_FORCE_DWORD)
				, m_IndexNum(0)
				, m_VertexNum(0)
				, m_VerFormat(0)
				, m_Stride(0)
				, m_IBOffset(0)
				, m_VBOffset(0)
				, m_pVB(0)
				, m_pIB(0)
			{
			}

			RenderParam				m_RP;				//渲染参数
			EPrimitiveType			m_PriType;			//图元类型
			uint32					m_PrimitiveCount;	//图元数量
			uint32					m_IndexNum;
			uint32					m_VertexNum;
			uint32					m_VerFormat;
			uint32					m_Stride;
			uint32					m_IBOffset;
			uint32					m_VBOffset;
			CVertexHardwareBuffer*	m_pVB;
			CIndexHardwareBuffer*	m_pIB;
		};

		CVertexHardwareBuffer*	m_pActiveVB;
		CIndexHardwareBuffer*	m_pActiveIB;
		char*					m_pVerBuf;
		char*					m_pIdxBuf;

		size_t					m_CurVBOffset;
		size_t					m_CurIBOffset;
		size_t					m_CurStack;
		uint					m_LightIndex;
		typedef  GVector<CRenderStack> RenderVector;
		RenderVector			m_RenderStatck;
		CRenderOperation		m_RenderOP;

	public:
		CDynamicVB(void);
		~CDynamicVB(void);

		HRESULT Create( uint32 nVBSize, uint32 nIBSize, uint32 StackSize = 32 );
		void	Destroy( void );
		RenderParam& GetRenderStack( EPrimitiveType PrimitiveType,
			uint32 VertexNum,
			uint32 PrimitiveCount,
			uint32 VerFormat,
			uint32 VerStride,
			void** ppVB,
			void** ppIB );

		//HRESULT	DrawAll();

		void	ResetStack();

		void	Render();

		void			SetLightIndex(uint index)
		{
			m_LightIndex = index;
		}
		void			Release() { delete this; };
		size_t			GetSize() { return m_RenderStatck.size(); }
		//void OptimizeDP(void);
		bool IsEmpty() const;
	};

	//------------------------------------------------------------------------------
	inline bool
	CDynamicVB::IsEmpty() const
	{
		return (0 == m_CurStack);
	}

};