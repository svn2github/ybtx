#include "stdafx.h"
#include "CurvePatch\CEditBezierPatch.h"
#include "CurvePatch\BezierControlPoint.h"
#include "CGraphic.h"
#include "CWindowTarget.h"
#include "TraceHelper.h"
const float STEP = 0.1f;
const int   POINTNUM = int(1.0f / 0.1f) + 1;
const int   divs = 7;
namespace sqr
{
	CEditBezierPatch::CEditBezierPatch()
	{
		m_nPatchWidth  = 4;
		m_nPatchHeight = 4;
		m_matRelative.Identity();
	}

	CEditBezierPatch::~CEditBezierPatch()
	{
		this->Clear();
	}

	void CEditBezierPatch::Clear()
	{
	}

	void CEditBezierPatch::AddPoint(const CVector3f& point)
	{
// 		VerColorTex vertex;
// 		vertex.p = point;
// 
// 		m_PatchControlPoints.push_back(vertex);
	}

	void CEditBezierPatch::Rectify(const CVector3f& point, const int controlPointIndex)
	{
	}

	void CEditBezierPatch::DeletePoint(const CVector3f& point, const int controlPointIndex)
	{
	}

	int CEditBezierPatch::ControlPointIsSelect(const CVector3f& point, CVector3f& selectpoint)
	{
		size_t size = m_BeizerControlPoints.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CBezierControlPoint *temp = m_BeizerControlPoints[i];
			temp->SetIsSelect(false);
		}

		for ( size_t i = 0; i < size; ++i )
		{
			CBezierControlPoint *temp = m_BeizerControlPoints[i];
			if(temp->IsSelect(point))
			{
				selectpoint = temp->GetControlPoint();
				return i;
			}
			else
				continue;
		}

		return -1;
	}

	void CEditBezierPatch::SetTranslate(const CVector3f& trans)
	{
		m_matRelative.SetTranslate( trans.x, trans.y, trans.z );
	}

	void CEditBezierPatch::DrawBezierPointsLine()
	{
// 		vector<CVector3f> ControlPoints = m_BeizerCurve.GetAllControllerPoints();
// 		
// 		size_t PointNum = ControlPoints.size();
// 		if( PointNum > 1 )
// 		{
// 			VerColor3D* pVB;
// 			RenderState* pRS;
// 			CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 
// 				PointNum, PointNum-1, VerColor3D::Format, sizeof(VerColor3D), 
// 				(void**)&pVB, NULL, (void**)&pRS );
// 			pRS->m_LightEnable = FALSE;
// 			pMatrix->Identity();
// 
// 			for(size_t i = 0; i < PointNum; ++i)
// 			{
// 				pVB[i].p = ControlPoints[i];
// 				pVB[i].diffuse = 0xffffffff;
// 			}
// 
// 			*pMatrix = m_matRelative; 
// 		}
	}

	void CEditBezierPatch::DrawBezierCurve()
	{
// 		vector<CVector3f> ControlPoints = m_BeizerCurve.GetAllControllerPoints();
// 		if( ControlPoints.empty() )
// 			return;
// 
// 		size_t PointNum = ControlPoints.size();
// 		if( PointNum == 1 )
// 			return;
// 
// 		this->DrawBezierPointsLine();
// 
// 		if( PointNum > 2 )
// 		{
// 			VerColor3D* pVB;
// 			RenderState* pRS;
// 
// 			CMatrix* pMatrix = CGraphic::GetInst()->GetRenderStack( RS_DEFAULT, NULL, NULL, PT_LINESTRIP, 
// 				POINTNUM, POINTNUM-1, VerColor3D::Format, sizeof(VerColor3D), 
// 				(void**)&pVB, NULL, (void**)&pRS );
// 
// 			m_BeizerCurve.BinomialCoffs();
// 
// 			int pointNum = 0;
// 			CVector3f point;
// 
// 			point = m_BeizerCurve.Get(0.5f);
// 			point = m_BeizerCurve.Get(1.0f);
// 
// 			for (float t = 0.0f; t < 1.01f; t+=0.1f )
// 			{
// 				point = m_BeizerCurve.Get(t);
// 				//DebugOut("pointt = %f, %f, %f, %f\n", t, point.x, point.y, point.z);
// 				pVB[pointNum].p = point;
// 				pVB[pointNum].diffuse = 0xffff0000;
// 				++pointNum;
// 			}
// 		}
	}

	CVector3f CEditBezierPatch::Bernstein( float u, const vector<CVector3f> vecPoints )
	{
		CVector3f	a, b, c, d, r;

		a = pow((1-u),3) * vecPoints[0];
		b = u*pow((1-u),2) * 3 * vecPoints[1];
		c = pow(u,2)*(1-u) * 3 * vecPoints[2];
		d = pow(u,3) * vecPoints[3];

		r = a + b + c + d;

		return r;
	}

	void CEditBezierPatch::GenBezierPatch()
	{
		float py = 0.0f, px = 0.0f, pyold = 0.0f; 
		map<int, vector<CVector3f>> mapXControlPoints;
		vector<CVector3f> temp, last, anchors0, anchors1, anchors2, anchors3;
		temp.resize(4);
		anchors0.resize(4);
		anchors1.resize(4);
		anchors2.resize(4);
		anchors3.resize(4);
		last.resize(divs+1);

		int index = 0;
		for ( int i = 0; i < 4; ++i )
		{
			index = i * m_nPatchWidth;
			temp[i] = m_BeizerControlPoints[index]->GetControlPoint();
		}

		for ( int i = 0; i < 4; ++i )
		{
			int j = 0;

			index = j++ * m_nPatchWidth;
			anchors0[i] = m_BeizerControlPoints[index+i]->GetControlPoint();

			index = j++ * m_nPatchWidth;
			anchors1[i] = m_BeizerControlPoints[index+i]->GetControlPoint();

			index = j++ * m_nPatchWidth;
			anchors2[i] = m_BeizerControlPoints[index+i]->GetControlPoint();

			index = j++ * m_nPatchWidth;
			anchors3[i] = m_BeizerControlPoints[index+i]->GetControlPoint();
		}

 		for ( int v = 0; v <= divs; ++v ) 
 		{						
 			// create the first line of points
 			px = ((float)v)/((float)divs);			// percent along y axis
 			// use the 4 points from the derives curve to calculate the points along that curve
 			last[v] = Bernstein(px, temp);
 		}

  		for ( int u = 1; u <= divs; ++u ) 
  		{
  			py	  = ((float)u)/((float)divs);			// Percent along Y axis
  			pyold = ((float)u-1.0f)/((float)divs);		// Percent along old Y axis
  
  			temp[0] = Bernstein(py, anchors0);	// Calculate new bezier points
  			temp[1] = Bernstein(py, anchors1);
  			temp[2] = Bernstein(py, anchors2);
  			temp[3] = Bernstein(py, anchors3);
  
  			for ( int v = 0; v <= divs; ++v ) 
			{
  				px = ((float)v)/((float)divs);			// Percent along the X axis
  
  				//glTexCoord2f(pyold, px);				// Apply the old texture coords
  				//glVertex3d(last[v].x, last[v].y, last[v].z);	// Old Point
 
  				last[v] = Bernstein(px, temp);			// Generate new point
  				//glTexCoord2f(py, px);					// Apply the new texture coords
  				//glVertex3d(last[v].x, last[v].y, last[v].z);	// New Point
  			}
  		}
	}
}