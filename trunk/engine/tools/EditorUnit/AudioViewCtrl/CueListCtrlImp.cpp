#include "StdAfx.h"
#include "AudioViewCtrl/CueListCtrlImp.h"
#include "IAudioModule.h"

DEF_UI_IMP(CCueListCtrlImp);
CCueListCtrlImp::CCueListCtrlImp(void* param)
{
	
}

CCueListCtrlImp::~CCueListCtrlImp(void)
{
}

void CCueListCtrlImp::Update()
{
	CCtrlBase::Update();
}


bool CCueListCtrlImp::_ProcMsg( const CMsg& msg )
{
	return false;
}

void CCueListCtrlImp::CreateAudioSystem()
{
	//如果音频系统已经启动 就直接返回
	if(GetAudioModule()->GetAudioSystem()->IsOpen())
		return;

	if (!GetAudioModule()->GetAudioSystem()->Open())
	{
		MessageBox(NULL,"AudioSystem打开失败","提示",MB_OK);
		return;
	}

	SQR_TRY
	{
		GetAudioModule()->GetAudioSystem()->LoadSoundBankFromXml("sound/config.xml");
	}
	SQR_CATCH(exp)
	{
		string str = exp.ErrorMsg();
		MessageBox(NULL, "sound/config.xml 载入失败", "提示", MB_OK);
	}
	SQR_TRY_END;
}

void CCueListCtrlImp::LoadMusicResource()
{
	IAudioSystem* audioSystem = GetAudioModule()->GetAudioSystem();
	if ( NULL != audioSystem )
	{
		for (index_t i = 0; i < audioSystem->GetNumSoundBanks(); i++)
		{
			const string& name = audioSystem->GetSoundBankName(i);
			
			for (index_t j = 0; j < audioSystem->GetNumCues(i); j++)
			{
				const string& cueName = audioSystem->GetCueName(i, j);
				m_cueList[name].push_back(cueName);
			}
		}
	}

}

size_t CCueListCtrlImp::GetMusicTypeCount()
{
	return m_cueList.size();
}

void CCueListCtrlImp::GetMusicTypeName( const size_t index, string& name )
{
	cuePool::iterator iter = m_cueList.begin();
	for ( size_t m = 0; m < index; ++m )
		++iter;

	if( iter != m_cueList.end() )
		name = iter->first;
}

size_t CCueListCtrlImp::GetEachMusicTypeCount(const size_t index)
{
	cuePool::iterator iter = m_cueList.begin();
	for ( size_t m = 0; m < index; ++m )
		++iter;

	if( iter != m_cueList.end() )
		return iter->second.size();

	return 0;
}

void CCueListCtrlImp::GetCueName( const string& musictypename, const size_t index, string& name )
{
	cuePool::iterator iter = m_cueList.find(musictypename);
	if( iter != m_cueList.end() )
	{
		vector<string> cuelist = iter->second;
		if( index < cuelist.size() )
			name = cuelist[index];
	}
}

void CCueListCtrlImp::SetCurSelectCueName( const string& name )
{
	PlayCurSelectCue(false);

	m_strCurCueName = name;
	
	PlayCurSelectCue(true);
}

void CCueListCtrlImp::GetCurSelectCueName( string& name )
{
	name = m_strCurCueName;
}

const char* CCueListCtrlImp::GetCurSelectCueName()
{
	return m_strCurCueName.c_str();
}

void CCueListCtrlImp::PlayCurSelectCue( const bool bPlay )
{
	if( m_strCurCueName.empty() )
		return;

	if( bPlay )
		PlayCue(m_strCurCueName.c_str());
	else
		StopCue(m_strCurCueName.c_str());
}