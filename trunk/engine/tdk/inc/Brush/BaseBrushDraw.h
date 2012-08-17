#pragma once
#include "CVector2.h"
#include "CVector3.h"
#include "BaseBrushEnum.h"
#include "CBezierSampler.h"

namespace sqr
{
	class ITexture;
	class CEditCurveRender;
}

namespace sqr_tools
{
	class CBrushGridHepler;
	class CEditContext;
	class CBaseBrushDraw
	{
	public:
		typedef vector<CVector2I> brushGridPool;
		typedef vector<CVector3f> brushVertexsPool;

		CBaseBrushDraw(void);
		virtual ~CBaseBrushDraw(void);

	protected:
		CVector3f					m_brushCenter;
		int							m_nOuterWidth;
		int							m_nOuterHeight;
		int							m_nInnerWidth;
		int							m_nInnerHeight;
		int							m_nBrushUnit;
		DWORD						m_dwBrushColor;
		bool						m_bShowGrid;
		ITexture*					m_pTexture;
		brushGridPool				m_vecBrushGridIndices;
		brushVertexsPool			m_lastBrushVertexs;
		brushVertexsPool			m_vecBrushVertexs;
		CBrushGridHepler*			m_pBrushGridHepler;
		CEditCurveRender*			m_pEditCurveRender;
		CEditContext*				m_pContext;
		CBezierSampler<CVector3f>	m_curveSampler;

	public:
		void	  SetBrushCenter(const CVector3f& center)
		{
			m_brushCenter = center;
		}
		CVector3f GetBrushCenter() const
		{
			return m_brushCenter;
		}

		int		GetBrushOuterWidth() const
		{
			return m_nOuterWidth;
		}
		void	SetBrushOuterWidth(int width)
		{
			m_nOuterWidth = width;
		}
		void	IncBrushOuterWidth()
		{
			++m_nOuterWidth;
		}
		void	DecBrushOuterWidth()
		{
			if( m_nOuterWidth > 1 )
				--m_nOuterWidth;
		}

		int		GetBrushOuterHeight() const
		{
			return m_nOuterHeight;
		}
		void	SetBrushOuterHeight(int height)
		{
			m_nOuterHeight = height;
		}
		void	IncBrushOuterHeight()
		{
			++m_nOuterHeight;
		}
		void	DecBrushOuterHeight()
		{
			if( m_nOuterHeight > 1 )
				--m_nOuterHeight;
		}

		int		GetBrushInnerWidth() const
		{
			return m_nInnerWidth;
		}
		void	SetBrushInnerWidth(int width)
		{
			m_nInnerWidth = width;
		}
		void	IncBrushInnerWidth()
		{
			++m_nInnerWidth;
		}
		void	DecBrushInnerWidth()
		{
			if( m_nInnerWidth > 1 )
				--m_nInnerWidth;
		}

		int		GetBrushInnerHeight() const
		{
			return m_nInnerHeight;
		}
		void	SetBrushInnerHeight(int height)
		{
			m_nInnerHeight = height;
		}
		void	IncBrushInnerHeight()
		{
			++m_nInnerHeight;
		}
		void	DecBrushInnerHeight()
		{
			if( m_nInnerHeight > 1 )
				--m_nInnerHeight;
		}

		void	SetBrushUnit(const int unit)
		{
			m_nBrushUnit = unit;
		}

		void	SetIsShowGrid(const bool bShow)
		{
			m_bShowGrid = bShow;
		}

		void    GetBrushGridIndexs(brushGridPool& indexs)
		{
			indexs = m_vecBrushGridIndices;
		}

		void    GetBrushVertexIndexs(brushVertexsPool& indexs)
		{			
			indexs = m_vecBrushVertexs;
		}

		virtual	void		SetBrushColor(const DWORD color);
		virtual void		SetBrushSmooth(const float smooth);
		virtual	void		SetEditContext(CEditContext* pContext);
		virtual void		Init();
		virtual void		SetBrushTexture(const string& name);
		virtual void		GetBrushGridIndexs(brushGridPool& indexs, brushVertexsPool& vertexs);
		virtual void		UpdateControlPoints();
		virtual	void		UpdatePointsHeight();
		virtual void		Draw();
		virtual void		DrawGrid();
		virtual BRUSH_TYPE	GetBrushType() = 0;
	};
}
