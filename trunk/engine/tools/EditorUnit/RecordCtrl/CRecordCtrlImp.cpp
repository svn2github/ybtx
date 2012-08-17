#include "StdAfx.h"
#include "CameraCtrl\CRecordCamDelegate.h"
#include "RecordCtrl\CRecordCtrlImp.h"
#include "CEditContext.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"
#include "CCamera.h"

DEF_UI_IMP(CRecordCtrlImp);

CRecordCtrlImp::CRecordCtrlImp(void* param)
:pCamera(NULL)
{
	m_RecordCamCtrl=NULL;
	m_LastCamCtrl=NULL;
	m_CurActiveAnimateLeab=-1;
	m_CurActiveAnimate=0;				//当前激活的动画
	m_CurActiveKey=0;					//当前激活动画帧
	m_LastState=RECORD_STOP;
	m_CurState=RECORD_STOP;
	m_AnimateList.clear();
}

CRecordCtrlImp::~CRecordCtrlImp(void)
{
}

void CRecordCtrlImp::SetContext(CEditContext* pContext)
{
	if(m_pOwnContext == pContext)
		return;
	m_pOwnContext = pContext;
	if(m_pOwnContext)
	{
		CWindowTarget* dir=m_pOwnContext->GetWindowTarget();
		if(dir)
		{
			CRenderGroup* group=dir->GetGroup();
			if(group)
				pCamera=group->GetCamera();
			if(m_RecordCamCtrl)
				delete m_RecordCamCtrl;
			m_RecordCamCtrl=new CRecordCamDelegate();
		}
	}
}

//m_pOwnContext
//基本操作

void CRecordCtrlImp::Active()
{
	//面板激活操作 切换相机
	if(m_pOwnContext && m_RecordCamCtrl)
	{
		m_LastCamCtrl=m_pOwnContext->GetCurCamCtrller();
		m_pOwnContext->RegisterDelegate(m_RecordCamCtrl);
		//初始化第一帧的位置
	}
}

void CRecordCtrlImp::Play()
{
	//只有在播放的时候切换到录像相机 其他的时候使用当前相机
	Active();
	m_LastState=m_CurState;
	m_CurState=RECORD_PLAY;
	if(m_LastState==RECORD_STOP)
	{
		if(m_CurActiveAnimateLeab>=0)
			m_RecordCamCtrl->SetAnimate( m_AnimateList[m_CurActiveAnimateLeab]);
	}
	m_RecordCamCtrl->SetPlay(true);
}

void CRecordCtrlImp::Stop()
{
	m_LastState=m_CurState;
	m_CurState=RECORD_STOP;
	m_RecordCamCtrl->SetPlay(false);
	//恢复到默认相机
	m_pOwnContext->UnRegisterDelegate(m_RecordCamCtrl);
}

void CRecordCtrlImp::Pause()
{
	m_RecordCamCtrl->SetPlay(false);
	m_LastState=m_CurState;
	m_CurState=RECORD_PAUSE;
	if(m_pOwnContext && m_LastCamCtrl)
	{
		//m_pOwnContext->RegisterDelegate();
	}
}

RecordState CRecordCtrlImp::GetLastState()
{
	return m_LastState;
}

RecordState CRecordCtrlImp::GetCurState()
{
	return m_CurState;
}

void CRecordCtrlImp::PreView() //预览 所谓预览 即在面板操作中 场景中实时变换相机的位置
{
}

bool CRecordCtrlImp::SetActiveAniamte(int id)
{
	for(unsigned int i=0;i<m_AnimateList.size();i++)
	{
		if(m_AnimateList[i].id==id)
		{
			m_CurActiveAnimate=id;
			m_CurActiveAnimateLeab=i;
			return true;
		}
	}
	return false;
} 

bool CRecordCtrlImp::SetActiveKey(int id)
{
	if(m_CurActiveAnimateLeab==-1)
		return false;
	if((int)m_AnimateList[m_CurActiveAnimateLeab].keylist.size()<id)
		return false;
	m_CurActiveKey=id;
	return true;
}

void CRecordCtrlImp::GetActiveAniamte(int& id)
{
	id=m_CurActiveAnimate;
} 

void CRecordCtrlImp::GetActiveKey(int& id)
{
	id=m_CurActiveKey;
}

//动画操作
int	CRecordCtrlImp::InsertAnimate(int id)
{
	for(unsigned int i=0;i<m_AnimateList.size();i++)
	{
		if(m_AnimateList[i].id==id)
		{
			return id;
		}
	}
	RecordAnimate tmpAniamte;
	tmpAniamte.id=id;
	tmpAniamte.timelong=0;
	tmpAniamte.keylist.clear();
	m_AnimateList.push_back(tmpAniamte);
	m_CurActiveAnimateLeab=m_AnimateList.size()-1;
	InsertKeyInfo(0);
	return id;
}

void CRecordCtrlImp::DelAnimate(int id)
{
	vector<RecordAnimate>::iterator it=m_AnimateList.begin();
	while(it!=m_AnimateList.end())
	{
		if(it->id==id)
		{
			it->keylist.clear();
			it->tip.clear();
			m_AnimateList.erase(it);
			return ;
		}
		it++;
	}
	if(m_AnimateList.size()==0)
	{
		m_CurActiveAnimateLeab=-1;
		m_CurActiveAnimate=0;
	}
}

int	CRecordCtrlImp::GetAnimateID(int index)
{
	if((unsigned int)index>=m_AnimateList.size())
		return -1;
	return m_AnimateList[index].id;
}

int CRecordCtrlImp::GetAnimateCount()
{
	return m_AnimateList.size();
}

int	CRecordCtrlImp::GetAnimateKeyNum(int id)
{
	for(unsigned int i=0;i<m_AnimateList.size();i++)
	{
		if(m_AnimateList[i].id==id)
			return m_AnimateList[i].keylist.size();
	}
	return 0;
}

string	CRecordCtrlImp::GetAnimateTipMsg(int id)
{
	for(unsigned int i=0;i<m_AnimateList.size();i++)
	{
		if(m_AnimateList[i].id==id)
			return m_AnimateList[i].tip;
	}
	return 0;
}

void CRecordCtrlImp::SetAnimateTipMsg(int id,string msg)
{
	for(unsigned int i=0;i<m_AnimateList.size();i++)
	{
		if(m_AnimateList[i].id==id)
			m_AnimateList[i].tip=msg;
	}
}

int	CRecordCtrlImp::GetAnimateTimlong(int id)
{
	for(unsigned int i=0;i<m_AnimateList.size();i++)
	{
		if(m_AnimateList[i].id==id)
			return m_AnimateList[i].timelong;
	}
	return -1;
}

void CRecordCtrlImp::SetAnimateTimlong(int id,int tlong)
{
	for(unsigned int i=0;i<m_AnimateList.size();i++)
	{
		if(m_AnimateList[i].id==id)
			m_AnimateList[i].timelong=tlong;
	}
}

//动画帧帧操作
bool CRecordCtrlImp::InsertKeyInfo(int tim)
{
	//捕获
	if(!pCamera || tim<0)
		return false;
	
	bool flag=false;
	KeyInfo m_key;
	m_key.time=tim;	
	m_key.pos=pCamera->GetPosition();
	m_key.dir=pCamera->getDIRECTION();
	m_key.fov=pCamera->getFOVy();
	if(tim==0)
	{
		//第0帧 
		m_key.id=m_AnimateList[m_CurActiveAnimateLeab].keylist.size();
		m_AnimateList[m_CurActiveAnimateLeab].keylist.push_back(m_key);
		m_AnimateList[m_CurActiveAnimateLeab].timelong=0;
		return true;
	}

	vector<KeyInfo>::iterator it=m_AnimateList[m_CurActiveAnimateLeab].keylist.begin();
	while(it!=m_AnimateList[m_CurActiveAnimateLeab].keylist.end())
	{
		if(it->time >tim)
		{
			m_AnimateList[m_CurActiveAnimateLeab].keylist.insert(it,m_key);
			ResetKeyID();
			return true;
		}
		it++;
	}
	m_key.id=m_AnimateList[m_CurActiveAnimateLeab].keylist.size();
	m_AnimateList[m_CurActiveAnimateLeab].keylist.push_back(m_key);
	m_AnimateList[m_CurActiveAnimateLeab].timelong=m_key.time;
	return true;
}

bool CRecordCtrlImp::DelKeyInfo(int id)
{
	bool flag=false;
	vector<KeyInfo>::iterator it=m_AnimateList[m_CurActiveAnimateLeab].keylist.begin();
	while(it!=m_AnimateList[m_CurActiveAnimateLeab].keylist.end())
	{
		if(it->id==id)
		{
			KeyInfo key=(*it);
			m_AnimateList[m_CurActiveAnimateLeab].keylist.erase(it);
			if(key.time==m_AnimateList[m_CurActiveAnimateLeab].timelong)
				ResetKeyList();
			flag=true;
			break;
		}
		it++;
	}
	if(flag)
		ResetKeyID();
	return flag;
}

void CRecordCtrlImp::ResetKeyList()
{
	//当前动画 帧列表 冒泡排序
	if(m_CurActiveAnimateLeab<0)
		return ;
	int keycount=m_AnimateList[m_CurActiveAnimateLeab].keylist.size();
	for(int i=0;i<keycount;i++)
	{
		for(int j=0;j<keycount-i;j++)
		{
			if(m_AnimateList[m_CurActiveAnimateLeab].keylist[j].time>m_AnimateList[m_CurActiveAnimateLeab].keylist[j+1].time)
			{
				int tmpTime=m_AnimateList[m_CurActiveAnimateLeab].keylist[j].time;
				m_AnimateList[m_CurActiveAnimateLeab].keylist[j].time=m_AnimateList[m_CurActiveAnimateLeab].keylist[j+1].time;
				m_AnimateList[m_CurActiveAnimateLeab].keylist[j+1].time=tmpTime;
			}
		}
	}
	if(keycount==0)
		m_AnimateList[m_CurActiveAnimateLeab].timelong=0;
	else
	{
		m_AnimateList[m_CurActiveAnimateLeab].timelong=m_AnimateList[m_CurActiveAnimateLeab].keylist[keycount-1].time;
		ResetKeyID();
	}
}

void CRecordCtrlImp::ResetKeyID()
{
	int newID=0;
	vector<KeyInfo>::iterator it=m_AnimateList[m_CurActiveAnimateLeab].keylist.begin();
	while(it!=m_AnimateList[m_CurActiveAnimateLeab].keylist.end())
	{
		it->id=newID;
		newID++;
		it++;
	}
}

void CRecordCtrlImp::GetKeyInfoPos(int keyid,float& x,float& y,float& z)
{
	x=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].pos.x;
	y=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].pos.y;
	z=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].pos.z;
}

void CRecordCtrlImp::GetKeyInfoDir(int keyid,float& x,float& y,float& z)
{
	x=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].dir.x;
	y=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].dir.y;
	z=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].dir.z;
}

void CRecordCtrlImp::GetKeyInfoFov(int keyid,float& fov)
{
	fov=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].fov;
}

void CRecordCtrlImp::GetKeyInfoSpeed(int keyid,float& speed)
{
	speed=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].speed;
}

void CRecordCtrlImp::GetKeyInfoTime(int keyid,int& time)
{
	time=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].time;
}

void CRecordCtrlImp::GetKeyInfoID(int keyid,int& id)
{
	id=m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].id;
}

void CRecordCtrlImp::SetKeyInfoId(int keyid)
{
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].id=keyid;
}

void CRecordCtrlImp::SetKeyInfoPos(int keyid,float x,float y,float z)
{
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].pos.x=x;
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].pos.y=y;
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].pos.z=z;
}

void CRecordCtrlImp::SetKeyInfoDir(int keyid,float x,float y,float z)
{
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].dir.x=x;
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].dir.y=y;
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].dir.z=z;
}

void CRecordCtrlImp::SetKeyInfoFov(int keyid,float fov)
{
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].fov=fov;
}

void CRecordCtrlImp::SetKeyInfoSpeed(int keyid,float speed)
{
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].speed=speed;
}

void CRecordCtrlImp::SetKeyInfoTime(int keyid,int time)
{
	m_AnimateList[m_CurActiveAnimateLeab].keylist[keyid].time=time;
}

//功能 插值生成路径信息
void CRecordCtrlImp::GenerateLineLerp()
{}
void CRecordCtrlImp::GenerateBLineLerp()
{}
void CRecordCtrlImp::GenerateLerp()
{}

//序列化存储路径信息
void CRecordCtrlImp::LoadPath(char* filename)
{}
void CRecordCtrlImp::SavePath()
{}

bool CRecordCtrlImp::_ProcMsg(const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_RECORD_ACTIVE:
				{
					Active();
					break;
				}
			case MGT_RECORD_PLAY:
				{
					Play();
					break;
				}
			case MGT_RECORD_PAUSE:
				{
					Pause();
					break;
				}
			case MGT_RECORD_STOP:
				{
					Stop();
					break;
				}
			case MGT_RECORD_REPLAY:
				{
					Stop();
					Play();
					break;
				}
			}
		}
	}
	return false;
}