#include "StdAfx.h"
#include "CAudioSystem.h"
#include "XACTAudioEngine.h"
#include "CAudioPlayer.h"
#include "CAudioListener.h"
#include "CAudioModule.h"
#include "StringHelper.h"
#include "PkgStream.h"
#include "CXmlConfig.inl"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"
#include "TimeHelper.h"
#include "CPkgDirectory.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_AUD;

const uint CAudioSystem::UPDATE_INTERVAL = 100;
const float fSpeedOfSoundInit = 343.5f;
const float fSpeedOfSoundStep = fSpeedOfSoundInit * 0.25f;

//------------------------------------------------------------------------------
CAudioSystem::CAudioSystem()
: isOpen(false)
, lastUpdateTime(0)
{
	CAudioPlayer::Create();
	new CAudioListener();
	CXACTAudioEngine::Create();

	nLoadXmlCount = 1;
}

//------------------------------------------------------------------------------
CAudioSystem::~CAudioSystem()
{
	Ast(!this->IsOpen());
	CAudioListener::Destroy();
	CAudioPlayer::Destroy();
	CXACTAudioEngine::Destroy();

	GraphicErr::SetErrAudioState("CAudioSystem析构完成");
}

//------------------------------------------------------------------------------
bool CAudioSystem::Open()
{
	SQR_TRY
	{
		if( this->isOpen )
			return this->isOpen;
		
		if( !this->adapterList.empty() )
			return this->isOpen;

		this->CollectSoundDirFileInfo();
		if( this->listSoundDirFileName.empty() )
		{
			MessageBox(NULL, "请检查res/sound文件夹是否是空的，如果为空，请重新下版本", "", MB_OK|MB_ICONERROR);
			return false;
		}

		if (GetAudioEngine()->Open())
		{
			// get adapters' info
			index_t adaptersize = GetAudioEngine()->GetNumAdapters();
			for (index_t i = 0; i < adaptersize; ++i)
			{
				CAudioAdapterInfo adapterInfo = GetAudioEngine()->GetAdapterInfo(i);
				this->adapterList.push_back(adapterInfo);
			}

			this->isOpen = true;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("Open AudioSystem");
		GfkAudioLogExp(exp);
		return false;
	}
	SQR_TRY_END;

	return this->isOpen;
}

//------------------------------------------------------------------------------
void CAudioSystem::Close()
{
	SQR_TRY
	{
		if(this->IsOpen())
		{
			this->adapterList.clear();
			GetAudioEngine()->Close();
			this->isOpen = false;
		}
		CAudioSystem::Destroy();
		CAudioModule::Destroy();
	}
	SQR_CATCH(exp)
	{
		GfkAudioLogExp(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void CAudioSystem::Update()
{
	UINT Flag = 0;
	SQR_TRY
	{
		if(!this->IsOpen())
			return;

		Flag = 1;
		uint64 time = GetProcessTime();//uint64
		Flag = 2;
		if (time - this->lastUpdateTime > UPDATE_INTERVAL)
		{
			Flag = 3;
			CXACTAudioEngine::GetInst()->Update();
			Flag = 4;
			this->lastUpdateTime = time;
		}
	}
	SQR_CATCH(exp)
	{
		switch(Flag)
		{
		case 0:
			exp.AppendType("_AT:IsOpen()");
			break;
		case 1:
			exp.AppendType("_AT:timeGetTime");
			break;
		case 2:
			exp.AppendType("_AT:if");
			break;
		case 3:		
			exp.AppendType("_AT:Update");
			break;
		case 4:		
			exp.AppendType("_AT:lastUpdateTime = time");
			break;
		default:
			exp.AppendType("_AT:other");
		}
		GfkLogAudioExpOnce(exp);
	}
	SQR_TRY_END;
}

//------------------------------------------------------------------------------
void CAudioSystem::SetAdapterIndex( index_t adapterIndex )
{
	GetAudioEngine()->SetDefaultAdapter(adapterIndex);
}

//------------------------------------------------------------------------------
index_t CAudioSystem::GetAdapterIndex() const
{
	return GetAudioEngine()->GetDefaultAdapter();
}

//------------------------------------------------------------------------------
bool CAudioSystem::LoadSoundBank( const char* path, bool streaming )
{
	Ast(this->IsOpen());
	return GetAudioEngine()->LoadSoundBank(path, streaming);
}

//------------------------------------------------------------------------------
void CAudioSystem::UnloadSoundBank( const char* path )
{
	Ast(this->IsOpen());
	GetAudioEngine()->UnloadSoundBank(path);
}

//------------------------------------------------------------------------------
void CAudioSystem::SetGlobalSettingsPath( const char* path )
{
	GetAudioEngine()->SetGlobalSettingsPath(path);
}

//------------------------------------------------------------------------------
const char* CAudioSystem::GetGlobalSettingsPath() const
{
	return GetAudioEngine()->GetGlobalSettingsPath().c_str();
}

//------------------------------------------------------------------------------
size_t CAudioSystem::GetNumSoundBanks() const
{
	return GetAudioEngine()->GetNumSoundBanks();
}

//------------------------------------------------------------------------------
const char* CAudioSystem::GetSoundBankName( index_t index ) const
{
	const CSoundBankPtr& soundBank = GetAudioEngine()->GetSoundBankByIndex(index);
	return soundBank->GetFileName().c_str();
}

//------------------------------------------------------------------------------
size_t CAudioSystem::GetNumCues( index_t soundBankIndex ) const
{
	const CSoundBankPtr& soundBank = GetAudioEngine()->GetSoundBankByIndex(soundBankIndex);
	if (soundBank.isvalid())
	{
		return soundBank->GetNumCues();
	}
	return 0;
}

//------------------------------------------------------------------------------
const char* CAudioSystem::GetCueName( index_t soundBankIndex, index_t cueIndex ) const
{
	const CSoundBankPtr& soundBank = GetAudioEngine()->GetSoundBankByIndex(soundBankIndex);
	return soundBank->GetCueProperty(cueIndex).GetCueName().c_str();
}

index_t CAudioSystem::GetSoundBankIndexByName( const char* name ) const
{
	return GetAudioEngine()->GetSoundBankIndexByName(name);
}

//------------------------------------------------------------------------------
bool
CAudioSystem::LoadSoundBankFromXml( const char* xmlFile )
{
	SQR_TRY
	{
		Ast(this->IsOpen());
		
		// load xml file
		ipkgstream file_strm(PATH_ALIAS_AUD.c_str(), xmlFile, true, eRO_File);
		if (!file_strm)
		{
			CAudioSystem::GetInst()->SoundFileIsExist(xmlFile);		
			file_strm.close();
			return false;
		}
		
		AString path = xmlFile;
		path = path.substr(0, path.find_last_of("/")+1);

		// parse soundbanks
		CXmlConfig *pAudioCfg = new CXmlConfig( "Audio" , file_strm );
		TiXmlNode* pNode = pAudioCfg->Get<TiXmlElement*>("SoundBank");
		while(pNode)
		{
			TiXmlElement* pElement = pNode->ToElement();
			if (!pElement)
				continue;

			TiXmlAttribute *attribute = pElement->FirstAttribute();
			if (!attribute)
				continue;
			AString name = attribute->Value();

			attribute = attribute->Next();
			if (!attribute)
				continue;
			bool stream = tobool(attribute->Value());
			
			GetAudioEngine()->LoadSoundBank(path + name, stream);

			pNode = pNode->NextSibling("SoundBank");
		}

		file_strm.close();
		SafeDelete(pAudioCfg);

		string msg = format("LoadSoundFinish: %d", nLoadXmlCount);
		GraphicErr::SetErrAudioState(msg);
		++nLoadXmlCount;
	}
	SQR_CATCH(exp)
	{
		GetAudioEngine()->SetIsOpen(false);
		exp.AppendType("LoadSoundBankFromXml");
		GfkAudioLogExp(exp);

		MessageBox(NULL, "音效配置文件损坏，为了感受更好的游戏体验，请重现下载并安装客户端", "系统提示", MB_OK|MB_ICONERROR);

		return false;
	}
	SQR_TRY_END;

	return true;
}

//------------------------------------------------------------------------------
void
CAudioSystem::SetGlobalVariable( const char* name, float value )
{
	if (GetAudioEngine()->IsAudioDeviceValid())
		GetAudioEngine()->SetGlobalVariable(name, value);
}

//------------------------------------------------------------------------------
float
CAudioSystem::GetGlobalVariable( const char* name )
{
	if (GetAudioEngine()->IsAudioDeviceValid())
		return GetAudioEngine()->GetGlobalVariable(name);
	else
		return 0.0f;
}

//------------------------------------------------------------------------------
void
CAudioSystem::SetSpeedOfSoundVariable(float value)
{
	if (GetAudioEngine()->IsAudioDeviceValid())
	{
		float fValue = fSpeedOfSoundInit + value * fSpeedOfSoundStep;
		GetAudioEngine()->SetGlobalVariable("SpeedOfSound", fValue);
	}
}

//------------------------------------------------------------------------------
float
CAudioSystem::GetSpeedOfSoundVariable(const char* name)
{
	if (GetAudioEngine()->IsAudioDeviceValid())
		return GetAudioEngine()->GetGlobalVariable("SpeedOfSound");
	else
		return 0.0f;
}

void CAudioSystem::CollectSoundDirFileInfo()
{
	CPkgDir pkgDir;
	int nFileNum = pkgDir.Open(PATH_ALIAS_AUD.c_str(), "sound");
	vector<string> files;
	string fileName;
	for ( int m = 0; m < nFileNum; ++m )
	{
		pkgDir.GetFileName(m, fileName);
		this->listSoundDirFileName.insert(fileName.c_str());
	}
}

bool CAudioSystem::SoundFileIsExist( const char* name )
{
	ASet<AString>::iterator iter = this->listSoundDirFileName.find(name);
	if( iter == this->listSoundDirFileName.end() )
	{
		string msg = format("%s 文件损坏，请重新下载并安装游戏", name);

		MessageBox(NULL, msg.c_str(), "系统提示", MB_OK|MB_ICONERROR);
		return false;
	}

	return true;
}
}// namespace sqr

