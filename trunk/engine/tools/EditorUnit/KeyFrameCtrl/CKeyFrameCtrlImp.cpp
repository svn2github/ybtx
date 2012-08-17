#include "StdAfx.h"
#include "KeyFrameCtrl/CKeyFrameCtrlImp.h"
#include "CEditContext.h"
#include "PathManager/CPathManager.h"
#include "ContextMsg.h"

DEF_UI_IMP(CKeyFrameCtrlImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CKeyFrameCtrlImp::CKeyFrameCtrlImp(void* param)
{
}

CKeyFrameCtrlImp::~CKeyFrameCtrlImp(void)
{
}

void CKeyFrameCtrlImp::Update()
{
	CCtrlBase::Update();
}

void sqr_tools::CKeyFrameCtrlImp::SetKeyFrame( const string& key )
{
	if (MODEL)
	{
		m_strKeyFrame = key;
		MODEL->InsertFrameString( m_strKeyFrame.c_str(), (int)MODEL->GetCurFrameNum() );

		MSG_SYS_SELCHANGE msg(NULL);
		m_pOwnContext->EventCollect(msg);
	}
}

std::string sqr_tools::CKeyFrameCtrlImp::GetKeyFrame()
{
	if (MODEL)
	{
		m_strKeyFrame = MODEL->GetAnimResName();
		return m_strKeyFrame;
	}
	else
		return "";
}

std::string sqr_tools::CKeyFrameCtrlImp::SaveKeyframeInfo( void )
{
	if (MODEL)
	{
		string WorkDir;
		string FileName = MODEL->GetAnimationGroupName();

		CPathSetImp::GetInst()->GetResFileName(WorkDir, FileName);
		string PathName = WorkDir + "character/keyframeinfo";

		size_t npos = FileName.rfind("/");
		string strKeyInfoFile = "";

		if( npos != -1 )
			strKeyInfoFile = FileName.substr(npos+1, FileName.length());

		npos = strKeyInfoFile.find(".");
		if( npos != -1 )
			strKeyInfoFile = strKeyInfoFile.substr(0, npos);


		string tempRootPath;
		tempRootPath = CPathSetImp::GetInst()->GetPath(0);

		WIN32_FIND_DATA FindData;
		HANDLE hFind = FindFirstFile((PathName.c_str()),&FindData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			string log;
			BOOL bCreat = FALSE;

			bCreat = ::CreateDirectory(PathName.c_str(), NULL);
			if( bCreat == FALSE )
			{
				log = PathName + "创建文件夹失败";
				return log;
			}
		}

		strKeyInfoFile = PathName + "/" + strKeyInfoFile + ".fky";

		string log;
		if( MODEL->ExportFrameKey(strKeyInfoFile) )
		{
			log = strKeyInfoFile + "关键帧信息保存完毕";
			return log;
		}
		else
		{
			log = strKeyInfoFile + "关键帧信息保存失败";
			return log;
		}
	}
	else
		return "保存失败";
}

void sqr_tools::CKeyFrameCtrlImp::ActiveNode( CRenderNode* pNode )
{
	Update();
}