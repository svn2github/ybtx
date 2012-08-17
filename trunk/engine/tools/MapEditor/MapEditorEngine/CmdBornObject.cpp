#include "stdafx.h"
#include "CmdBornObject.h"
#include "CMapEditApp.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "Operator.h"
#include "Operator.inl"
#include "OperationLog.h"
#include "ToolSetModel.h"
#include "SceneAudioMgr.h"
#include "CMapEditApp.h"
#include "CEditContext.h"

void CCmdBornObject::_Execute()
{
	if( m_diff.veCMapEditModelFileNames.empty() )
		return;

	

	if ( m_diff.pObject == NULL )
	{
		string modelname = m_diff.veCMapEditModelFileNames[0];
		size_t npos = modelname.rfind("\\");
		modelname = modelname.substr(npos+1, modelname.length());
		char szObjectBuf[256];
		sprintf(szObjectBuf, "DO born object，num = %d, name = %s", m_diff.veCMapEditModelFileNames.size(), modelname.c_str());
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);

		m_diff.pObject = new CTObject;
		CEditModelGroup * pModelGroup = new CEditModelGroup;

		vector<CMapEditObject*> vec;
		vector<string>::iterator iter = m_diff.veCMapEditModelFileNames.begin();
		for ( int i = 0 ; iter != m_diff.veCMapEditModelFileNames.end(); ++iter, ++i )
		{
			CMapEditObject * p = NULL;

			p = CSceneManager::GetInst()->AddNodeModel(*iter, m_diff.bSubUnit, m_diff.vUserOffset.y, m_diff.vUserOffset.x, m_diff.vUserOffset.z, m_diff.vUserOffsetEx.y, m_diff.vUserOffsetEx.x, m_diff.vUserOffsetEx.z, m_diff.fRotation, m_diff.nRotationCnt,
				m_diff.fPitch, m_diff.nPitchCnt, m_diff.fYaw, m_diff.nYawCnt, m_diff.xScale, m_diff.yScale, m_diff.zScale, m_diff.bEffectBase, m_diff.bAudio, m_diff.bLoginModel);

			if ( NULL == p )
			{
// 				string log = *iter;
// 				log += "没有找到";
// 				MessageBox(0, log.c_str(), "路径错误", 0);

				//delete 掉 new的东西，并将指针情况
				delete pModelGroup;
				pModelGroup = NULL;

				m_diff.pObject = NULL;

				return;
			}
			vec.push_back(p);
			//CMapEditApp::GetInst()->GetRenderScene()->AddToRenderScene(p);
			p->SetVisible(true);
			p->SetGroupOffset(m_diff.vGroupOffset);
		}

		pModelGroup->AddContainer(vec,m_diff.bSubUnit,m_diff.bCenterType,0,0);
		pModelGroup->SetItemName(m_diff.strItemName);
		pModelGroup->SetModelFileNames(m_diff.veCMapEditModelFileNames);
		pModelGroup->SetIsCameraOriented(m_diff.bCameraOriented);
		pModelGroup->SetIsEffectBased(m_diff.bEffectBase);
		pModelGroup->SetResGroupMark(m_diff.bResGroupMark);
		pModelGroup->SetIsVisibleByConfigure(m_diff.bVisibleByConfigure);
		pModelGroup->SetIsTranslucent(m_diff.bTranslucent);

		pModelGroup->SetCastShadow(m_diff.bShadow);

		m_diff.pObject->SetModelGroup(pModelGroup);
		m_diff.pObject->SetCastShadow(m_diff.bShadow);
	}
	else
	{
		if ( m_diff.bCameraOriented ) 
			return;
	}

	m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_MOVING);

	///effect
	if ( m_diff.bEffectBase )
	{
		///也即是环境特效
		if ( m_diff.bCameraOriented )
		{
			CMapEditApp::GetInst()->GetOperator()->AddAmbientObject(m_diff.pObject);
			m_diff.pObject->GetModelGroup()->HideSelf();
		}
		else
		{
			CMapEditApp::GetInst()->GetOperator()->AddIndividualEffectObject(m_diff.pObject);
			CToolSetModel::GetInst()->AddActiveObject(m_diff.pObject);
		}
	}
	else
	{
		CToolSetModel::GetInst()->AddActiveObject(m_diff.pObject);
	}

	//		///sound
	// 		if( m_diff.pObject->GetModelGroup()->m_pSceneAudio )
	// 		{
	// 			CMapEditApp::GetInst()->GetOperator()->AddIndividualSoundObject(m_diff.pObject);
	// 		}
}

void CCmdBornObject::_UnExecute()
{
	if( m_diff.veCMapEditModelFileNames.empty() )
		return;

	

	if ( m_diff.pObject != NULL )
	{
		string modelname = m_diff.veCMapEditModelFileNames[0];
		size_t npos = modelname.rfind("\\");
		modelname = modelname.substr(npos+1, modelname.length());

		char szObjectBuf[256];
		sprintf(szObjectBuf, "UNDO born object，num = %d, name = %s", m_diff.veCMapEditModelFileNames.size(), modelname.c_str());
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);

		vector<CMapEditObject *>::iterator it = m_diff.pObject->m_pModelGroup->m_vecSelfPresentation.begin();
		vector<CMapEditObject *>::iterator ite = m_diff.pObject->m_pModelGroup->m_vecSelfPresentation.end();
		for (;it!=ite;++it)
			(*it)->SetVisible(false);

		if ( m_diff.bCameraOriented == false )
			m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_DANGLING);

		///effect
		if ( m_diff.bEffectBase )
		{
			if ( m_diff.bCameraOriented == false )
			{
				CMapEditApp::GetInst()->GetOperator()->RemoveIndividualEffectObject(m_diff.pObject);
				CToolSetModel::GetInst()->RemoveActiveObject(m_diff.pObject);
			}
		}
		else
		{
			CToolSetModel::GetInst()->RemoveActiveObject(m_diff.pObject);
		}

		///sound
		if( m_diff.pObject->GetModelGroup()->m_pSceneAudio )
		{
			CSceneAudioMgr::GetInst()->RemoveIndividualSoundObject(m_diff.pObject);
		}
	}
}

void CCmdBornObject::SetModelFileNames( vector<string> const & vec )
{
	m_diff.veCMapEditModelFileNames = vec;
}

void CCmdBornObject::SetSubUnit( bool b )
{
	m_diff.bSubUnit = b;
}

void CCmdBornObject::SetTranslucent( bool b )
{
	m_diff.bTranslucent = b;
}

void CCmdBornObject::SetCastShadow( bool b )
{
	m_diff.bShadow = b;
}

void CCmdBornObject::SetCenterType( sqr::ECenter_Type eCenterType )
{
	m_diff.bCenterType = eCenterType;
}

void CCmdBornObject::SetBlockInfo( vector< vector<sqr::SBlockInfo> > & vvBlock )
{
	m_diff.vvBlock = vvBlock;
}

void CCmdBornObject::SetHeightInfo( vector< vector<sqr::SLogicHeightInfo> > & vvHeight )
{
	m_diff.vvHeight = vvHeight;
}

void CCmdBornObject::SetUserOffset( float fLift, float fShiftX, float fShiftZ )
{
	m_diff.vUserOffset.x = fShiftX;
	m_diff.vUserOffset.y = fLift;
	m_diff.vUserOffset.z = fShiftZ;
}

void CCmdBornObject::SetUserOffsetEx( float fLiftEx, float fShiftXEx, float fShiftZEx )
{
	m_diff.vUserOffsetEx.x = fShiftXEx;
	m_diff.vUserOffsetEx.y = fLiftEx;
	m_diff.vUserOffsetEx.z = fShiftZEx;
}

void CCmdBornObject::SetIsEffectBase( bool b )
{
	m_diff.bEffectBase = b;
}

void CCmdBornObject::SetResGroupMark( BYTE bResGroupMark )
{
	m_diff.bResGroupMark = bResGroupMark;
}

void CCmdBornObject::SetIsVisibleByConfigure(bool b)
{
	m_diff.bVisibleByConfigure = b;
}

void CCmdBornObject::SetCameraOriented( bool b )
{
	m_diff.bCameraOriented = b;
}

void CCmdBornObject::SetScaleZ( float z )
{
	m_diff.zScale = z;
}

void CCmdBornObject::SetItemName( string & str )
{
	m_diff.strItemName = str;
}

void CCmdBornObject::SetGroupOffset( CVector3f vGroupOffset )
{
	m_diff.vGroupOffset = vGroupOffset;
}

void CCmdBornObject::SetRotationCount( int nRotationCnt )
{
	m_diff.nRotationCnt = nRotationCnt;
}

void CCmdBornObject::SetRotation( float fRotation )
{
	m_diff.fRotation = fRotation;
}

void CCmdBornObject::SetYawCount( int nRotationCnt )
{
	m_diff.nYawCnt = nRotationCnt;
}

void CCmdBornObject::SetYaw( float fR )
{
	m_diff.fYaw = fR;
}

void CCmdBornObject::SetPitchCount( int nRotationCnt )
{
	m_diff.nPitchCnt = nRotationCnt;
}

void CCmdBornObject::SetPitch( float fR )
{
	m_diff.fPitch = fR;
}

void CCmdBornObject::SetScaleX( float x )
{
	m_diff.xScale = x;
}

void CCmdBornObject::SetScaleY( float y )
{
	m_diff.yScale = y;
}

void CCmdBornObject::SetIsAudio( bool b )
{
	m_diff.bAudio = b;
}

void CCmdBornObject::SetIsLoginModel( bool b )
{
	m_diff.bLoginModel = b;
}
