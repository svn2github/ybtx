#pragma once

#include "CCtrlBase.h"
#include "CRecordStruct.h"

#define MGT_RECORD_PLAY		0x01
#define MGT_RECORD_PAUSE	0x02
#define	MGT_RECORD_STOP		0x03
#define MGT_RECORD_REPLAY	0x04
#define MGT_RECORD_ACTIVE	0x05

namespace sqr
{
	class CCamera;
	class CCameraController;
}

namespace sqr_tools
{

class CRecordCamDelegate;

class CRecordCtrlImp :public CCtrlBase
{
	REG_UI_IMP(CRecordCtrlImp);
public:
	CRecordCtrlImp(void* param);
	~CRecordCtrlImp(void);

protected:
	CCamera* pCamera;
	CRecordCamDelegate* m_RecordCamCtrl;
	CCameraController* m_LastCamCtrl;
	vector<RecordAnimate>	m_AnimateList;
	int		m_CurActiveAnimate;				//当前激活的动画ID
	int		m_CurActiveAnimateLeab;			//当前激活的动画 下标
	int		m_CurActiveKey;					//当前激活动画帧
	RecordState m_LastState;
	RecordState m_CurState;
public:
	//基本操作
	virtual void Active();
	virtual void Play();
	virtual void Stop();
	virtual void Pause();
	virtual RecordState GetCurState();
	virtual RecordState GetLastState();

	virtual bool SetActiveAniamte(int id); 
	virtual bool SetActiveKey(int id);	  
	virtual void GetActiveAniamte(int& id); 
	virtual void GetActiveKey(int& id);
	virtual void PreView();

	//动画操作
	virtual int		InsertAnimate(int id);
	virtual void	DelAnimate(int id);
	virtual int		GetAnimateCount();
	virtual int		GetAnimateID(int index);
	virtual int		GetAnimateKeyNum(int id);
	virtual string	GetAnimateTipMsg(int id);
	virtual void	SetAnimateTipMsg(int id,string msg);
	virtual int		GetAnimateTimlong(int id);
	virtual void	SetAnimateTimlong(int id,int tlong);

	//动画帧帧操作
	virtual bool InsertKeyInfo(int time);	//在该动画序列中,在当前时间出入一关键帧
	virtual bool DelKeyInfo(int id);		//删除指定ID帧
	virtual void ResetKeyID();              //重新排列帧ID
	virtual void ResetKeyList();			//重新排列帧序
	virtual void GetKeyInfoPos(int keyid,float& x,float& y,float& z); //获取和设置帧信息 都是针对于当前激活的动画而言
	virtual void GetKeyInfoDir(int keyid,float& x,float& y,float& z);
	virtual void GetKeyInfoFov(int keyid,float& fov);
	virtual void GetKeyInfoSpeed(int keyid,float& speed);
	virtual void GetKeyInfoTime(int keyid,int& tim);
	virtual void GetKeyInfoID(int keyid,int& id);

	virtual void SetKeyInfoId(int keyid);
	virtual void SetKeyInfoPos(int keyid,float x,float y,float z);
	virtual void SetKeyInfoDir(int keyid,float x,float y,float z);
	virtual void SetKeyInfoFov(int keyid,float fov);
	virtual void SetKeyInfoSpeed(int keyid,float speed);
	virtual void SetKeyInfoTime(int keyid,int time);

	//功能 插值生成路径信息
	virtual void GenerateLineLerp();
	virtual void GenerateBLineLerp();
	virtual void GenerateLerp();

	//序列化存储路径信息
	virtual void LoadPath(char* filename);
	virtual void SavePath();
	
public:
	//重载
	virtual void SetContext(CEditContext* pContext);
	virtual bool _ProcMsg(const CMsg& msg );
};

//!namespace
}