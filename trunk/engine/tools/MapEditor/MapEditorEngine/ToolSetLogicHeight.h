#pragma once
#include "CEventOwner.h"
#include "TSingleton.h"

namespace sqr
{
	struct RenderState;

	class CToolSetLogicHeight: public Singleton<CToolSetLogicHeight>
	{
	public:
		struct SProcessParams
		{
			SProcessParams()
			{
				dwVertexBrushSizeInX = 1;
				dwVertexBrushSizeInZ = 1;
			}

			DWORD dwVertexBrushSizeInX;
			DWORD dwVertexBrushSizeInZ;
		};

	private:

		SProcessParams	m_processParams;
		vector<DWORD>	m_vecVertexIndicesLock;

		RenderState *	m_pRS;

		float			m_fCopyLogicHeight;
		float			m_fModelLogicHeight;
		float			m_fLogicCheckDiffNum;///逻辑高检测的对比值
		bool			m_bCheckMapLogicHeight;
		vector<DWORD>	m_vecErrorLogicHeightVertexs;
	
	public:

		CToolSetLogicHeight();
		~CToolSetLogicHeight();
		void			OnEvent(const CMsg& msg);
		void			RenderUnit(const vector<DWORD> & vertexIndexs);
		void			Render();

		DWORD			GetVertexBrushSizeInX();
		void			SetVertexBrushSizeInX(DWORD dw);
		void			IncVertexBrushSizeInX();
		void			DecVertexBrushSizeInX();

		DWORD			GetVertexBrushSizeInZ();
		void			SetVertexBrushSizeInZ(DWORD dw);
		void			IncVertexBrushSizeInZ();
		void			DecVertexBrushSizeInZ();

		vector<DWORD> & GetBrushVertexIndices();

		void			CopyLogicHeight();
		void			PastateLogicHeight();
		void			EraseLogicHeight();
		void			QuickAddLogicHeight();
		void			QucikDeduceLogicHeight();
		void			AddLogicHeight();
		void			DeduceLogicHeight();
		void			CheckUpMapLogic(map<DWORD, DWORD>& errorVertexIndexs);
		void			SelecErrorLogicVertexIndex(map<DWORD, DWORD> errorVertexIndexs);
		void			ClearErrorLogicVertexIndex();
		void			SaveMapLogicHeight();
		void			LoadMapLogicHeight();

		inline void		SetMeshLogicHeight( const float fLogicHeight )
		{
			if( fLogicHeight < 0.0f )
				return;

			m_fModelLogicHeight = fLogicHeight;
		}

		//------------------------------------------------------------------------------
		inline float	GetMeshLogicHeight() const
		{
			return m_fModelLogicHeight;
		}

		inline void		SetLogicCheckDiffNum(const float fValue)
		{
			m_fLogicCheckDiffNum = fValue;
		}

		inline float	GetLogicCheckDiffNum() const
		{
			return m_fLogicCheckDiffNum;
		}

		inline void		SetIsCheckMapLogicHeight(const bool b)
		{
			m_bCheckMapLogicHeight = b;
		}

		inline vector<DWORD> GetIsCheckMapLogicHeight() const 
		{
			return m_vecErrorLogicHeightVertexs;
		}
		
	};
}