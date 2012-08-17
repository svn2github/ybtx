#include "stdafx.h"
#include "Square.h"
#include "Render.h"
#include "CTexture.h"

namespace sqr
{
	CSquare::CSquare(ISceneNode * pParent, string texname, float fSizeInWidth, float fSizeInDepth, DWORD dwColor) : 
		ISceneNode(pParent), m_strLockTextureName(texname), m_fSizeInWidth(fSizeInWidth), m_fSizeInDepth(fSizeInDepth), m_dwColor(dwColor)
	{
		m_vPos[0] = CVector3f(-m_fSizeInWidth*0.5f, 0.0f, -m_fSizeInDepth*0.5f);
		m_vPos[1] = CVector3f(-m_fSizeInWidth*0.5f, 0.0f, m_fSizeInDepth*0.5f);
		m_vPos[2] = CVector3f(m_fSizeInWidth*0.5f, 0.0f, -m_fSizeInDepth*0.5f);
		m_vPos[3] = CVector3f(m_fSizeInWidth*0.5f, 0.0f, m_fSizeInDepth*0.5f);

		m_dwClr[0] = dwColor;
		m_dwClr[1] = dwColor;
		m_dwClr[2] = dwColor;
		m_dwClr[3] = dwColor;
		
		m_vUV[0] = CVector2f(0.0f,1.0f);
		m_vUV[1] = CVector2f(0.0f,0.0f);
		m_vUV[2] = CVector2f(1.0f,1.0f);
		m_vUV[3] = CVector2f(1.0f,0.0f);

		////添加自己的SceneNode名字
		//m_strSceneNodeName = "SquareSceneNode";
		m_pRS = NULL;

		m_pLockTexture = CRenderDevice::GetInst()->GetTexture(m_strLockTextureName.c_str());
	}

	void CSquare::Render()
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();
		CMatrix amatrix = this->GetAbsolute();
		CMatrix matrix;
		matrix._11 = amatrix._11; matrix._12 = amatrix._12; matrix._13 = amatrix._13; matrix._14 = amatrix._14;
		matrix._21 = amatrix._21; matrix._22 = amatrix._22; matrix._23 = amatrix._23; matrix._24 = amatrix._24;
		matrix._31 = amatrix._31; matrix._32 = amatrix._32; matrix._33 = amatrix._33; matrix._34 = amatrix._34;
		matrix._41 = amatrix._41; matrix._42 = amatrix._42; matrix._43 = amatrix._43; matrix._44 = amatrix._44;
		pRender->DrawLayer3DSolidQuad(&m_pRS, m_vPos, m_vUV, m_dwClr, m_pLockTexture, matrix);
		m_pRS->m_TexOP_S0 = TOP_MODULATE;
		m_pRS->m_TexArg1_S0 = TA_TEXTURE;
		m_pRS->m_TexArg2_S0 = TA_DIFFUSE;

		m_pRS->m_AlpOP_S0 = TOP_SELECTARG1;
		m_pRS->m_AlpArg1_S0 = TA_TEXTURE;

		m_pRS->m_AlphaTestEnable = true;
		m_pRS->m_Alpharef = 0x40;
		m_pRS->m_AlphaTestFun = CMP_GREATER; 
		m_pRS->m_AlphaBlendEnable = false;
	}

	void CSquare::SetWidth(float fWidth)
	{
		m_fSizeInWidth = fWidth;
		m_vPos[0] = CVector3f(-m_fSizeInWidth*0.5f, 0.0f, -m_fSizeInDepth*0.5f);
		m_vPos[1] = CVector3f(-m_fSizeInWidth*0.5f, 0.0f, m_fSizeInDepth*0.5f);
		m_vPos[2] = CVector3f(m_fSizeInWidth*0.5f, 0.0f, -m_fSizeInDepth*0.5f);
		m_vPos[3] = CVector3f(m_fSizeInWidth*0.5f, 0.0f, m_fSizeInDepth*0.5f);
	}

	void CSquare::SetDepth(float fDepth)
	{
		m_fSizeInDepth = fDepth;
		m_vPos[0] = CVector3f(-m_fSizeInWidth*0.5f, 0.0f, -m_fSizeInDepth*0.5f);
		m_vPos[1] = CVector3f(-m_fSizeInWidth*0.5f, 0.0f, m_fSizeInDepth*0.5f);
		m_vPos[2] = CVector3f(m_fSizeInWidth*0.5f, 0.0f, -m_fSizeInDepth*0.5f);
		m_vPos[3] = CVector3f(m_fSizeInWidth*0.5f, 0.0f, m_fSizeInDepth*0.5f);
	}
}