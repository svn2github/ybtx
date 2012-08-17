#include "stdafx.h"
#include "CCtrlBase.h"
#include "CEditContext.h"
#include "AnimationGroupEditCtrl/CAnimationGroupEditImp.h"
//#include "AnimationGroupEditCtrl/CAnimationGroupDelegate.h"
#include "CEditPieceGroup.h"
#include "CAnimationGroup.h"
#include "CEditAnimationsRes.h"
#include "CEditSkeletalsMgr.h"
#include "ContextMsg.h"
#include "CAnimation.h"
#include "PathManager/CPathManager.h"
#include "CEditorConfig.h"

DEF_UI_IMP(CAnimationGroupEditImp);

#define EDITMODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 
#define MODEL ((CModel*)m_pOwnContext->GetActiveObj())

CAnimationGroupEditImp::CAnimationGroupEditImp(void* param)
{
	m_pAnimateGroup = new CEditAnimationGroup("",L"");
	m_pAnimateGroup->AddRef();
	m_pSkeletalFrame = NULL;
	m_strCurAgpName = "";
	m_pModel = NULL;
}

CAnimationGroupEditImp::~CAnimationGroupEditImp(void)
{

}

void CAnimationGroupEditImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);
}

void CAnimationGroupEditImp::ActiveNode( CRenderNode* pNode )
{
	if (IsNodesChanged())
	{
		UpdateNodes();
		CCtrlBase::Update();
	}
	else if (m_pModel != EDITMODEL)
	{
		m_pModel = EDITMODEL;
		if (!m_strCurAgpName.empty())
		{
			m_CurAni = NULL;
			m_pSkeletalFrame = NULL;
			m_pAnimateGroup = NULL;
			AddAgp(m_strCurAgpName);
		}
	}
}

std::string CAnimationGroupEditImp::CalName( sqr::IDNAME name,float fSpeed )
{
	std::string strName;
	strName = name;
	char speed[256];
	sprintf_s(speed, ",%.2f", fSpeed);
	strName += speed;
	return strName;
}

std::string CAnimationGroupEditImp::AddAgp( std::string str )
{
	if (MODEL)
	{
		m_strCurAgpName = str;

		MODEL->StopAni(-1);

		std::string sAgpFile = str.substr(0, str.length()-4) + ".agp";
		CEditAnimationGroup* tpAnimationGroup = new CEditAnimationGroup(sAgpFile);
		string strErr;

		tpAnimationGroup->AddRef();
		if(tpAnimationGroup->BinLoad()<0)
		{
			char msg[256];
			sprintf_s(msg, "动画列表%s没载入成功", sAgpFile.c_str());
			return msg;
		}

		CEditSkeletalFrame* tSkeletalFrame;
		CSkeletalsMgr::GetInst()->GetSkeInPool(tpAnimationGroup->GetSkeletalFrameName(),(CSkeletalFrame**)&tSkeletalFrame);
		if(!tSkeletalFrame)
		{
			std::string msg = "没有找到骨架文件";
			msg += tpAnimationGroup->GetSkeletalFrameName();
			tpAnimationGroup->Release();
			return msg;
		}

		if( !m_pSkeletalFrame)
		{
			EDITMODEL->ClearAnimateGroup();
			EDITMODEL->SetSkeletalFrame(  tSkeletalFrame );
			EDITMODEL->SetEditAnimateGroup( tpAnimationGroup);	

			m_pAnimateGroup = tpAnimationGroup;
			m_pAnimateGroup->AddRef();
			m_pSkeletalFrame = tSkeletalFrame;

			SKELETAL_MASK mask;
			m_pSkeletalFrame->FillSkeletalMask(mask);
			EDITMODEL->SetSkesMask(mask);

			EDITMODEL->CalSkeletalSize(0);

			m_vtAnimName.clear();
			// to mfc animation name list
			CAnimationGroup* pAniGrp = tpAnimationGroup;
			std::string strName;
			uint32 animateNum = pAniGrp->GetAnimateNum();
			for( uint32 i = 0; i < animateNum; i++ )
			{
				sqr::IDNAME Name = (*pAniGrp)[i]->GetName();
				float sp = (*pAniGrp)[i]->GetGroundSpeed();
				strName = CalName(Name,sp);
				m_vtAnimName.push_back(strName);
			}

			tpAnimationGroup->Release();

			ChangeAnimation(string((*pAniGrp)[0]->GetName()));

			return "添加成功";
		}

		string ErrSkeletal;
		if( m_pSkeletalFrame->IsFitForMe( *tSkeletalFrame, ErrSkeletal ) )
		{
			CAnimationGroup::CAnimateList& NewList = tpAnimationGroup->GetAnimateList();
			CAnimationGroup::CAnimateList& OrgList = m_pAnimateGroup->GetAnimateList();
			for( int i = 0; i < (int)NewList.size(); i++ )
			{
				sqr::IDNAME NewName = NewList[i]->GetName();
				int j = 0;
				for( ; j < (int)OrgList.size(); j++ )
				{
					sqr::IDNAME OrgName = OrgList[j]->GetName();
					if( OrgName == NewName )
						break;
				}
				if( j == (int)OrgList.size() )
				{
					std::string name = NewName;
					OrgList.push_back( NewList[i] );
					m_vtAnimName.push_back(name);
				}
			}
			NewList.clear();
		}
		else
		{
			str += "文件骨骼结构和已经加入的文件骨骼结构不符, 请检查骨骼:";
			str += ErrSkeletal.c_str();
			return str;
		}

		ChangeAnimation(string((*m_pAnimateGroup)[0]->GetName()));

		return "添加成功";
	}
	else
		return "添加失败";
}


void CAnimationGroupEditImp::ClearAgp( void )
{
	if(MODEL)
	{
		m_CurAni = NULL;
		EDITMODEL->DelFramework();
		m_pSkeletalFrame=NULL;
		if (m_pAnimateGroup)
		{
			m_pAnimateGroup->Destroy();
			SafeRelease(m_pAnimateGroup);
		}
		m_vtAnimName.clear();

		MSG_SYS_SELCHANGE msg(NULL);
		m_pOwnContext->EventCollect(msg);
	}
}


std::string CAnimationGroupEditImp::SaveAgp( std::string str )
{
	if (m_pSkeletalFrame && m_pAnimateGroup)
	{
		if( m_pSkeletalFrame->GetSkeletalNum() && m_pAnimateGroup->GetAnimateNum() )
		{
			m_pAnimateGroup->BinSave(str);
			return "保存成功";
		}
		else
			return "没有动画信息，无法保存";
	}
	else
		return "不存在骨架，保存失败";
}

void CAnimationGroupEditImp::ChangeAnimation( std::string str )
{
	if (EDITMODEL)
	{
		CEditAnimate* anim = (CEditAnimate*)m_pAnimateGroup->GetAniByName(IDNAME(str.c_str()));
		EDITMODEL->SetNextAnimation( IDNAME(str.c_str()),FramePair(0,-1),true,0,1.0f );

		MSG_SYS_SELCHANGE msg(NULL);
		m_pOwnContext->EventCollect(msg);
	}
}

int CAnimationGroupEditImp::GetCount( void )
{
	return m_vtAnimName.size();
}

std::string CAnimationGroupEditImp::GetAnimNameByIndex( int index )
{
	return m_vtAnimName[index];
}

void CAnimationGroupEditImp::ChangeCurSelSkeBoneName( std::string str )
{
	if(EDITMODEL && EDITMODEL->GetSketetalFrame())
	{
		if(!str.empty())
			(*EDITMODEL->GetSketetalFrame())[EDITMODEL->GetCurSelSkelatal()]->m_pSkeletal->SetName(str.c_str());
	}
}

bool CAnimationGroupEditImp::IsNodesChanged( void )
{
	if(m_pAnimateGroup)
	{
		uint32 animateNum = m_pAnimateGroup->GetAnimateNum();
		for( uint32 i = 0; i < animateNum; i++ )
		{
			sqr::IDNAME Name = (*m_pAnimateGroup)[i]->GetName();
			float sp = (*m_pAnimateGroup)[i]->GetGroundSpeed();
			std::string strName = CalName(Name,sp);
			if(0 != m_vtAnimName[i].compare(strName))
				return true;
		}	
	}
	else if (m_vtAnimName.size())
		return true;
	return false;
}

void CAnimationGroupEditImp::UpdateNodes( void )
{
	std::string strName;
	m_vtAnimName.clear();
	if (m_pAnimateGroup)
	{
		uint32 animateNum = m_pAnimateGroup->GetAnimateNum();
		for( uint32 i = 0; i < animateNum; i++ )
		{
			sqr::IDNAME Name = (*m_pAnimateGroup)[i]->GetName();
			float sp = (*m_pAnimateGroup)[i]->GetGroundSpeed();
			strName = CalName(Name,sp);
			m_vtAnimName.push_back(strName);
		}
	}
}

std::string sqr_tools::CAnimationGroupEditImp::GetLastAGPFile( void )
{
	std::string keyName = "AddAGPBt";
	std::string pathFileValue;
	TiXmlNode* pConfigXml = CEditorConfig::GetInst()->GetEditorConfig("FileDialogSetting");
	TiXmlElement* pElement = pConfigXml->ToElement();
	TiXmlNode* pNode = NULL;

	pNode = pConfigXml->FirstChild(keyName);
	if(pNode!=NULL)
	{ 
		//结点存在 获取属性值
		pElement=pNode->ToElement();
		pathFileValue = pElement->Attribute("Path");
	}
	else
	{
		std::string temp = CPathSetImp::GetInst()->GetPath(0);
		TiXmlElement newNode(keyName);
		if (!temp.empty())
			newNode.SetAttribute("Path", temp);
		else
			newNode.SetAttribute("Path", "c:");
		pConfigXml->InsertEndChild(newNode);
		CEditorConfig::GetInst()->SaveEditorConfig();
	}
	return pathFileValue;
}

void sqr_tools::CAnimationGroupEditImp::SaveNewAGPFile( std::string name )
{
	std::string keyName = "AddAGPBt";
	string pathFileValue;
	TiXmlNode* pConfigXml = CEditorConfig::GetInst()->GetEditorConfig("FileDialogSetting");
	TiXmlElement* pElement = pConfigXml->ToElement();
	TiXmlNode* pNode = NULL;
	pNode = pConfigXml->FirstChild(keyName);
	if(pNode)
	{
		pElement=pNode->ToElement();
		pElement->SetAttribute("Path",name);
		CEditorConfig::GetInst()->SaveEditorConfig();
	}
}