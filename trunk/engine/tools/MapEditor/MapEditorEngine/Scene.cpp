#include "stdafx.h"
#include "Scene.h"
#include "TerrainMesh.h"
#include "Square.h"
#include "Operator.h"
#include "Operator.inl"
#include "WireBox.h"
#include "River.h"
#include "WireBox.h"
#include "CTileEditorModel.h"
#include "CMapEditObject.h"
#include "CMapEditModelObj.h"
#include "CMapEditEffectObj.h"
#include "CMapEditAudioObj.h"
#include "CMapEditLoginObj.h"
#include "CMapEditRgnAmbFxObj.h"
#include "CMapEditApp.h"
#include "EditStateOpertiaonMgr.h"
#include "CRegionAmbientEffectMgr.h"
#include "BaseHelper.h"

namespace sqr
{
	CSquare * CSceneManager::AddNodeSquare(float fSizeInWidth, float fSizeInDepth, DWORD dwColor, string texname, ISceneNode * pParent)
	{
		if ( !pParent )
			pParent = this;

		CSquare * pNode = new CSquare(pParent, texname, fSizeInWidth, fSizeInDepth, dwColor);
		return pNode;
	}

	CMapEditObject * CSceneManager::AddNodeModel(string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, float fLiftEx, float fShiftXEx, float fShiftZEx, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float xScale, float yScale, float zScale, bool bEffect, bool bAudio, bool bLogin, ISceneNode * pParent)
	{
		CMapEditObject * pNode = NULL;

		bool bRgnAmbinetFx = CMapEditApp::GetInst()->GetOperator()->GetDoesRgnAmbientFxBase();
		if( bRgnAmbinetFx )
			pNode = new CMapEditRgnAmbFxObj();
		else
		{
			if( bLogin )
				pNode = new CMapEditLoginObj();
			else
			{
				if( !bEffect && !bAudio )
					pNode = new CMapEditModelObj();
				else
				{
					if( bEffect && !bAudio )
						pNode = new CMapEditEffectObj();
					else
						pNode = new CMapEditAudioObj();
				}
			}
		}

		bool createSuc = false;
		createSuc = pNode->Create(strFileName, bSubUnit, fLift, fShiftX, fShiftZ, fLiftEx, fShiftXEx, fShiftZEx, fRotation, nRotationCnt, fPitch, nPitchCnt, fYaw, nYawCnt, xScale, yScale, zScale);

		if ( !createSuc )
			return NULL;
		else
			return pNode;
	}

	CTileEditorModel * CSceneManager::AddNodeTitleModel(string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, int &BlockInfoNum, int &LogicHeightInfoNum)
	{
		CTileEditorModel * pNode = new CTileEditorModel();
		pNode->Create(strFileName, bSubUnit, fLift, fShiftX, fShiftZ, BlockInfoNum, LogicHeightInfoNum);
		return pNode;
	}

	CWireBox * CSceneManager::AddNodeWireBox(float fHeight, float fWidth, float fDepth, DWORD dwColor,  bool bIsVisible, ISceneNode * pParent)
	{
		//if ( !pParent )
		//	pParent = this;

		CWireBox * pNode = new CWireBox(pParent,bIsVisible);
		pNode->Create(fHeight,fWidth,fDepth,dwColor);
		return pNode;
	}

	CRiver * CSceneManager::AddNodeRiver(/*vector<int> vecTextureHandles, */vector<string> vecTextureNames, bool bIsVisible, ISceneNode * pParent)
	{
		if ( !pParent )
			pParent = this;

		CRiver * pNode = new CRiver(pParent,bIsVisible);
		pNode->Create(vecTextureNames);
		return pNode;
	}

	CSceneManager::~CSceneManager()
	{
		SafeDelete(m_pGUIRoot);
	}

	CSceneManager::CSceneManager( ISceneNode * pParent /*= NULL*/ ) : ISceneNode(pParent)//,m_QuadTree(NULL)
	{
		m_pGUIRoot = new IGUISceneNode();
	}

	void CSceneManager::DrawWorld()
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		CEditStateOpertiaonMgr::GetInst()->RenderMainPlayer();//绘制主角
		if(CMapEditApp::GetInst()->GetWater())
			CMapEditApp::GetInst()->GetWater()->RenderRivers();	// 水绘制
		if(!CMapRegionAmbientEffectMgr::HasInst())
			CMapRegionAmbientEffectMgr::Create();
		CMapRegionAmbientEffectMgr::GetInst()->RenderAllReginAmbientFx();
	}

	void CSceneManager::DrawWin32GUI()
	{
		m_pGUIRoot->Render();
	}

	void CSceneManager::UpdateWorldFPSLimited()
	{
		this->UpdateFPSLimited();
	}

	IGUISceneNode::IGUISceneNode( ISceneNode * pParent /*= NULL*/ ) : ISceneNode(pParent)/*,m_pSwapChain(NULL)*/
	{

	}

	IGUISceneNode::~IGUISceneNode()
	{
		//SafeRelease(m_pSwapChain);
	}

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	ISceneNode::ISceneNode( ISceneNode * pParent,bool bIsVisible /*= true*/, CVector3f & vecTranslation /*= CVector3f(0.0f,0.0f,0.0f)*/, CVector3f & vecRotation /*= CVector3f(0.0f,0.0f,0.0f)*/, CVector3f & vecScale /*= CVector3f(1.0f,1.0f,1.0f)*/ )
			: m_bIsVisible(bIsVisible)
			, m_pParent(pParent)
			, m_vecRelativeTranslation(vecTranslation)
			, m_vecRelativeRotation(vecRotation)
			, m_vecRelativeScale(vecScale)
	{
		if ( m_pParent )
			m_pParent->AddChild(this);

		UpdateAbsolute();
	}

	ISceneNode::~ISceneNode() /* destructor will remove all children */
	{
		Clear();
	}

	void ISceneNode::SetVisible( bool b )
	{
		m_bIsVisible = b;
	}

	bool ISceneNode::IsVisible()
	{
		return m_bIsVisible;
	}

	void ISceneNode::Clear()
	{
		while ( !m_children.empty() )
		{
			m_children.pop_front();
		}
		m_children.clear();
	}

	void ISceneNode::AddChild( ISceneNode * p )
	{
		Ast(p);
		m_children.push_back(p);
	}

	void ISceneNode::RemoveChild( ISceneNode * p )
	{
		m_children.remove(p);
	}

	void ISceneNode::Remove()
	{
		if ( m_pParent )
			m_pParent->RemoveChild(this);
	}

	void ISceneNode::UpdateAbsolute()
	{
 		if ( m_pParent )
 			m_matAbsolute = m_pParent->GetAbsolute() * GetRelative();
 		else
			m_matAbsolute = GetRelative();

		for_each(m_children.begin(), m_children.end(), mem_fun(&ISceneNode::UpdateAbsolute));
	}

	void ISceneNode::SetAbsolute( CMatrix mat )
	{
		m_matAbsolute = mat;

		for_each(m_children.begin(), m_children.end(), mem_fun(&ISceneNode::UpdateAbsolute));
	}

	const CMatrix & ISceneNode::GetRelative()
	{
		CMatrix matTrans,matRot,matScale;
		matTrans.SetTranslate( m_vecRelativeTranslation.x, m_vecRelativeTranslation.y, m_vecRelativeTranslation.z );
		matRot.SetRotation( m_vecRelativeRotation.x, m_vecRelativeRotation.y, m_vecRelativeRotation.z );
		matScale.SetScale( m_vecRelativeScale.x, m_vecRelativeScale.y, m_vecRelativeScale.z );
		return m_matRelative = matRot*matTrans*matScale;
	}

	const CMatrix & ISceneNode::GetAbsolute()
	{
		return m_matAbsolute;
	}

	void ISceneNode::SetRelativePos( CVector3f & v )
	{
		m_vecRelativeTranslation = v;
	}

	void ISceneNode::SetRelativeRotation( CVector3f & v )
	{
		m_vecRelativeRotation = v;
	}

	void ISceneNode::Render()
	{
		list<ISceneNodePtr>::iterator it = m_children.begin();

		for (; it != m_children.end(); ++it)
		{
			if ( (*it)->IsVisible() )
			{
				(*it)->Render();
			}
		}
	}

	void ISceneNode::Update( const CMsg& msg )
	{
		OnEvent(msg);

		list<ISceneNodePtr>::iterator it = m_children.begin();

		for (; it != m_children.end(); ++it)
		{
			if ( (*it)->IsVisible() )
			{
				(*it)->Update(msg);
			}
		}
	}

	void ISceneNode::UpdateFPSLimited()
	{
		list<ISceneNodePtr>::iterator it = m_children.begin();

		for (; it != m_children.end(); ++it)
		{
			if ( (*it)->IsVisible() )
			{
				(*it)->UpdateFPSLimited();
			}
		}
	}

	void ISceneNode::OnEvent( const CMsg& msg )
	{

	}
}
