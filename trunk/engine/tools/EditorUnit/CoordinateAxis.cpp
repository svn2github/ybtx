#include "stdafx.h"
#include "CoordinateAxis.h"
#include "CGraphic.h"
#include "CCamera.h"
#include "CWindowTarget.h"
#include <float.h>

const float limitValue		= 5.0f;
const DWORD RED_COLOR		= 0xffff0000;
const DWORD GREEN_COLOR		= 0xff00ff00;
const DWORD BLUE_COLOR		= 0xff0000ff;
const DWORD YELLOW_COLOR	= 0xffffff00;
bool  YAXIS					= false;

namespace sqr
{
	CCoordinateAxis::CCoordinateAxis()
	{
		m_AxisPoint.resize(PointNum);
		m_matRelative.Identity();
		m_fEndValue = 100.0f;
		m_nSelectAxisIndex = -1;
	}

	void CCoordinateAxis::Create()
	{
		m_AxisPoint.resize(PointNum);

		///create axis x
		m_AxisPoint[0].p		= CVector3f(0.0f,   0.0f, 0.0f);
		m_AxisPoint[0].diffuse  = RED_COLOR;
		m_AxisPoint[1].p		= CVector3f(m_fEndValue, 0.0f, 0.0f);
		m_AxisPoint[1].diffuse  = RED_COLOR;
		
 		m_AxisPoint[2].p        = CVector3f(m_fEndValue, 0.0f, 0.0f);
 		m_AxisPoint[2].diffuse	= RED_COLOR;
 		m_AxisPoint[3].p		= CVector3f(90.0f, 10.0f, 0.0f);
 		m_AxisPoint[3].diffuse	= RED_COLOR;
 
 		m_AxisPoint[4].p		 = CVector3f(m_fEndValue, 0.0f, 0.0f);
 		m_AxisPoint[4].diffuse   = RED_COLOR;
 		m_AxisPoint[5].p		 = CVector3f(90.0f, -10.0f, 0.0f);
 		m_AxisPoint[5].diffuse   = RED_COLOR;

		///create axis y
		m_AxisPoint[6].p		 = CVector3f(0.0f, 0.0f, 0.0f);
		m_AxisPoint[6].diffuse   = GREEN_COLOR;
		m_AxisPoint[7].p		 = CVector3f(0.0f, m_fEndValue, 0.0f);
		m_AxisPoint[7].diffuse   = GREEN_COLOR;

 		m_AxisPoint[8].p         = CVector3f(0.0f, m_fEndValue, 0.0f);
 		m_AxisPoint[8].diffuse	 = GREEN_COLOR;
 		m_AxisPoint[9].p		 = CVector3f(-10.0f, 90.0f, 0.0f);
 		m_AxisPoint[9].diffuse	 = GREEN_COLOR;
 
 		m_AxisPoint[10].p		 = CVector3f(0.0f, m_fEndValue, 0.0f);
 		m_AxisPoint[10].diffuse  = GREEN_COLOR;
 		m_AxisPoint[11].p		 = CVector3f(10.0f, 90.0f, 0.0f);
 		m_AxisPoint[11].diffuse  = GREEN_COLOR;

		///create axis z
		m_AxisPoint[12].p		 = CVector3f(0.0f, 0.0f, 0.0f);
		m_AxisPoint[12].diffuse  = BLUE_COLOR;
		m_AxisPoint[13].p		 = CVector3f(0.0f, 0.0f, m_fEndValue);
		m_AxisPoint[13].diffuse  = BLUE_COLOR;

 		m_AxisPoint[14].p        = CVector3f(0.0f, 0.0f, m_fEndValue);
 		m_AxisPoint[14].diffuse	 = BLUE_COLOR;
 		m_AxisPoint[15].p		 = CVector3f(-10.0f, 0.0f, 90.0f);
 		m_AxisPoint[15].diffuse	 = BLUE_COLOR;
 
 		m_AxisPoint[16].p		 = CVector3f(0.0f, 0.0f, m_fEndValue);
 		m_AxisPoint[16].diffuse  = BLUE_COLOR;
 		m_AxisPoint[17].p		 = CVector3f(10.0f, 0.0f, 90.0f);
 		m_AxisPoint[17].diffuse  = BLUE_COLOR;

		///letter x
		m_AxisPoint[18].p		= CVector3f(m_fEndValue+5.0f, 4.0f, 0.0f);
		m_AxisPoint[18].diffuse  = RED_COLOR;
		m_AxisPoint[19].p		= CVector3f(m_fEndValue+10.0f, -4.0f, 0.0f);
		m_AxisPoint[19].diffuse  = RED_COLOR;

		m_AxisPoint[20].p        = CVector3f(m_fEndValue+5.0f, -4.0f, 0.0f);
		m_AxisPoint[20].diffuse	= RED_COLOR;
		m_AxisPoint[21].p		= CVector3f(m_fEndValue+10.0f, 4.0f, 0.0f);
		m_AxisPoint[21].diffuse	= RED_COLOR;

		m_AxisPoint[22].p        = CVector3f(m_fEndValue+5.0f, -4.0f, 0.0f);
		m_AxisPoint[22].diffuse	= RED_COLOR;
		m_AxisPoint[23].p		= CVector3f(m_fEndValue+10.0f, 4.0f, 0.0f);
		m_AxisPoint[23].diffuse	= RED_COLOR;

 		///letter y
 		m_AxisPoint[24].p		 = CVector3f(0.0f, m_fEndValue+5.0f, 0.0f);
 		m_AxisPoint[24].diffuse   = GREEN_COLOR;
 		m_AxisPoint[25].p		 = CVector3f(0.0f, m_fEndValue+10.0f, 0.0f);
 		m_AxisPoint[25].diffuse   = GREEN_COLOR;
 
 		m_AxisPoint[26].p         = CVector3f(0.0f, m_fEndValue+10.0f, 0.0f);
 		m_AxisPoint[26].diffuse	 = GREEN_COLOR;
 		m_AxisPoint[27].p		 = CVector3f(-4.0f, m_fEndValue+15.0f, 0.0f);
 		m_AxisPoint[27].diffuse	 = GREEN_COLOR;
 
 		m_AxisPoint[28].p         = CVector3f(0.0f, m_fEndValue+10.0f, 0.0f);
 		m_AxisPoint[28].diffuse	 = GREEN_COLOR;
 		m_AxisPoint[29].p		 = CVector3f(4.0f, m_fEndValue+15.0f, 0.0f);
 		m_AxisPoint[29].diffuse	 = GREEN_COLOR;
 
 		///letter z
 		m_AxisPoint[30].p		 = CVector3f(-4.0f, 5.0f, m_fEndValue+5.0f);
 		m_AxisPoint[30].diffuse  = BLUE_COLOR;
 		m_AxisPoint[31].p		 = CVector3f(4.0f, 5.0f,  m_fEndValue+5.0f);
 		m_AxisPoint[31].diffuse  = BLUE_COLOR;
 
 		m_AxisPoint[32].p        = CVector3f(4.0f, 5.0f,  m_fEndValue+5.0f);
 		m_AxisPoint[32].diffuse	 = BLUE_COLOR;
 		m_AxisPoint[33].p		 = CVector3f(-4.0f, 0.0f, m_fEndValue+5.0f);
 		m_AxisPoint[33].diffuse	 = BLUE_COLOR;
 
 		m_AxisPoint[34].p		 = CVector3f(-4.0f, 0.0f, m_fEndValue+5.0f);
 		m_AxisPoint[34].diffuse  = BLUE_COLOR;
 		m_AxisPoint[35].p		 = CVector3f(4.0f, 0.0f,  m_fEndValue+5.0f);
 		m_AxisPoint[35].diffuse  = BLUE_COLOR;
	}

	void CCoordinateAxis::CalSelectAxisIndex(const CVector3f& pos)
	{
		m_nSelectAxisIndex = -1;
		float tg = abs(pos.z / pos.x);

		float x = abs(pos.x);
		float y = abs(pos.y);
		float z = abs(pos.z);

		if( tg < 0.5f && x > 0.0f )
			m_nSelectAxisIndex = 0;
		else
		{
			if( tg < 80.0f && x < limitValue && z > 0.0f )
				m_nSelectAxisIndex = 2;
			else
			{
				if( x < limitValue && z < limitValue && y >= 0.0f )
					m_nSelectAxisIndex = 1;
			}
		}

		this->Update();
	}

	void CCoordinateAxis::CalSelectAxisIndex(const CVector2f& pt, const DWORD dwWndWidth, const DWORD dwWndHeight)
	{
		CCamera *pCamera = CMainWindowTarget::GetInst()->GetCamera();

		CMatrix matProj = pCamera->getProjectionMatrix();
		CMatrix matView = pCamera->getViewMatrix();
		CMatrix matViewProj = matView * matProj;
		CMatrix matWorldProj;
		matWorldProj.Identity();

		CVector3f startX = CVector3f(0.0f, 0.0f, 0.0f) * m_matRelative;
		CVector3f endX = CVector3f(m_fEndValue, 0.0f, 0.0f) * m_matRelative;
		CVector3f endY = CVector3f(0.0, m_fEndValue, 0.0f) * m_matRelative;
		CVector3f endZ = CVector3f(0.0f, 0.0f, m_fEndValue) * m_matRelative;

		CVector3f startx = startX * matViewProj;
		CVector3f starty = startX * matViewProj;
		CVector3f startz = startX * matViewProj;
		CVector3f endx = endX * matViewProj;
		CVector3f endy = endY * matViewProj;
		CVector3f endz = endZ * matViewProj;

		startx.x = startx.x * 0.5f + 0.5f;startx.y = -startx.y * 0.5f + 0.5f;
		starty = startx;
		startz = startx;

		endx.x = endx.x * 0.5f + 0.5f;endx.y = -endx.y * 0.5f + 0.5f;///将y轴方向转换成与client坐标系一样，即从下往上
		endy.x = endy.x * 0.5f + 0.5f;endy.y = -endy.y * 0.5f + 0.5f;
		endz.x = endz.x * 0.5f + 0.5f;endz.y = -endz.y * 0.5f + 0.5f;

		CVector2f startx2D = CVector2f(startx.x * dwWndWidth, startx.y * dwWndHeight);
		CVector2f endx2D = CVector2f(endx.x * dwWndWidth, endx.y * dwWndHeight);
		CVector2f starty2D = CVector2f(starty.x * dwWndWidth, starty.y * dwWndHeight);
		CVector2f endy2D = CVector2f(endy.x * dwWndWidth, endy.y * dwWndHeight);
		CVector2f startz2D = CVector2f(startx.x * dwWndWidth, startx.y * dwWndHeight);
		CVector2f endz2D = CVector2f(endz.x * dwWndWidth, endz.y * dwWndHeight);

		m_nSelectAxisIndex = -1;

		bool bXIntersect = this->PointIntersects2D(pt, startx2D, endx2D);
		if(bXIntersect)
			m_nSelectAxisIndex = 0;

 		bool bYIntersect = this->PointIntersects2D(pt, starty2D, endy2D);
 		if(bYIntersect)
 			m_nSelectAxisIndex = 1;
 
 		bool bZIntersect = this->PointIntersects2D(pt, startz2D, endz2D);
 		if(bZIntersect)
 			m_nSelectAxisIndex = 2;
		
		Update();
	}

	bool CCoordinateAxis::PointIntersects2D(const CVector2f& pt, const CVector2f& start, const CVector2f& end)
	{
		CLine2D line;
		line.CreateLine(start, end);

		return line.IsIntersects(pt);
	}

	void CCoordinateAxis::Update()
	{
		return;
		switch(m_nSelectAxisIndex)
		{
		case 0://x axis
			{
				int startIndex = 0;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = YELLOW_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = GREEN_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = BLUE_COLOR;

				int startLetterIndex = startIndex;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = YELLOW_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = GREEN_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = BLUE_COLOR;

				break;
			}

		case 1:// y axis
			{
				int startIndex = 0;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = RED_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = YELLOW_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = BLUE_COLOR;

				int startLetterIndex = startIndex;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = RED_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = YELLOW_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = BLUE_COLOR;

				break;
			}

		case 2:// z axis
			{
				int startIndex = 0;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = RED_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = GREEN_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = YELLOW_COLOR;

				int startLetterIndex = startIndex;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = RED_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = GREEN_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = YELLOW_COLOR;

				break;
			}

		case -1://no select axis
			{
				int startIndex = 0;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = RED_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = GREEN_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startIndex++].diffuse  = BLUE_COLOR;

				int startLetterIndex = startIndex;
				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = RED_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = GREEN_COLOR;

				for (int i = 0; i < eachAxisPointNum; ++i )
					m_AxisPoint[startLetterIndex++].diffuse  = BLUE_COLOR;

				break;
			}

		default:
			break;
		}
	}

	void CCoordinateAxis::SetTranslate(const CVector3f& trans)
	{
		m_matRelative.SetTranslate( trans.x, trans.y, trans.z );
	}

	void CCoordinateAxis::Render()
	{
		VerColor3D* pVB;
		RenderState* pRS;
		CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINELIST, 
			PointNum, PointNum/2, VerColor3D::Format, sizeof(VerColor3D), 
			(void**)&pVB, NULL, (void**)&pRS );
		pRS->m_LightEnable = FALSE;
		pMatrix->Identity();

		for(int i = 0; i < PointNum; ++i)
			pVB[i] = m_AxisPoint[i];

		*pMatrix = m_matRelative; 
	}


	void CLine2D::CreateLine( const CVector2f& start, const CVector2f& end )
	{
		m_LineStart = start;
		m_LineEnd   = end;
		YAXIS		= false;

		float xDif = fabs(end.x - start.x);
		if(xDif < limitValue)
		{
			YAXIS = true;
			return;
		}

		m_fSlope = float(end.y - start.y) / float(end.x - start.x);
		m_fFixValuel = start.y - start.x * m_fSlope;
	}

	bool CLine2D::IsIntersects( const CVector2f& point )
	{
		if( YAXIS )
		{
			float diff0 = fabs(m_LineStart.x - point.x);
			float diff1 = fabs(m_LineEnd.x - point.x);
			if( diff0 < limitValue && diff1 < limitValue )
				return true;
			else
				return false;
		}

		float ff0 = float(point.y - m_LineStart.y) / float(point.x - m_LineStart.x);
		float ff1 = float(m_LineEnd.y - m_LineStart.y) / float(m_LineEnd.x - m_LineStart.x);

		float final = m_fSlope * point.x + m_fFixValuel;
		if(_isnan(final))
			return false;
		else
		{
			float diff = fabs(final - point.y);
			if(diff < limitValue)
				return true;
			else
				return false;
		}

		return false;
	}
}