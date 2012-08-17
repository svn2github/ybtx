#include "stdafx.h"
#include "CMapEffect.h"
#include "CEffectGroup.h"
#include "ToolSetModel.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"
#include "UserMsg.h"

sqr::CMapEffect::CMapEffect()
{
	m_IEffectGroupForSceneLight = NULL;
}

sqr::CMapEffect::~CMapEffect()
{
	if ( m_IEffectGroupForSceneLight )
		m_IEffectGroupForSceneLight = NULL;

	map<string, void *>::iterator beg = EffectGroupList.begin();
	map<string, void *>::iterator end = EffectGroupList.end();
	for ( beg; beg != end; ++beg )
	{
		if( beg->second )
			beg->second = NULL;
	}
	EffectGroupList.clear();

	map<string, stAmbientEffectInfo *>::iterator iter;
	for( iter = mapAmientUseEffect.begin(); iter != mapAmientUseEffect.end(); ++iter )
	{
		delete iter->second;
	}
	mapAmientUseEffect.clear();

	beg = RgnAmbientFxGroupList.begin();
	end = RgnAmbientFxGroupList.end();
	for ( beg; beg != end; ++beg )
	{
		if( beg->second )
			beg->second = NULL;
	}
	RgnAmbientFxGroupList.clear();
}

void sqr::CMapEffect::CreateEffectGroup( const string& strFXFileName, const string& effectname, void ** p )
{
	IEffectManager::GetInst()->CreateEffectGroup(strFXFileName.c_str(),(IEffectGroup **)p);

	if( !*p )
	{
		string log = strFXFileName;
		log = log + "无法载入特效文件";

		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return;
	}

	EffectGroupList[effectname] = *p;
}

void sqr::CMapEffect::GetFXNames( string & strFXFileName, map< string, vector< string > > & mapFX, void ** p )
{
	IEffectManager::GetInst()->CreateEffectGroup(strFXFileName.c_str(),(IEffectGroup **)p);

	if( !*p )
	{
		string log = strFXFileName;
		log = log + "无法载入特效文件";

		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return;
	}

	for ( size_t i = 0; i < ((IEffectGroup*)(*p))->GetEffectNum(); ++i )
	{
		string strFXFullPathName = ((IEffectGroup*)(*p))->GetEffectName(i);
		string strFXGroup = strFXFullPathName.substr(0,strFXFullPathName.find_first_of('\\'));
		string strFXInstance = strFXFullPathName.substr(strFXFullPathName.find_first_of('\\')+1);
		mapFX[strFXGroup].push_back(strFXInstance);
	}
}

void* sqr::CMapEffect::GetLocalFXGroup( const string& effectname )
{
	size_t npos = effectname.find("场景光源\\Standar");

	if( npos != -1 )
		return m_IEffectGroupForSceneLight;
	else
	{
		npos = effectname.find("环境特效集1");

		if( npos != -1 )
			return m_IEffectGroupForSceneLight;
		else
		{
			if( EffectGroupList.find(effectname) == EffectGroupList.end() )
				return NULL;
			else
				return EffectGroupList[effectname];
		}
	}
}

void sqr::CMapEffect::ToolSetModel_LinkEffect( const string& effectname, CTObject * pModel )
{
	SQR_TRY
	{
		void *p = NULL;
		p = EffectGroupList[effectname];
		if( p == NULL )
		{
			string log = effectname + " 特效没有找到，请检查";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return;
		}

		IEffect * pEffect		= NULL;
		EffectCreateInfo Ninfo	= {NULL,NULL,NULL} ;
		IEffectManager::GetInst()->CreateEffect((IEffectGroup*)p, effectname.c_str(), &pEffect);
		if ( pEffect && pModel)
		{ 
			pModel->GetModelGroup()->LinkEffect(pEffect,(IEffectGroup*)p);
		}
		else
		{
			string log = effectname + " 特效没有找到，有可能是该特效版本号不对";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	SQR_CATCH(exp)
	{
		MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);

		return;
	}
	SQR_TRY_END;
}

void sqr::CMapEffect::ToolSetModel_LinkEffect( const string& effectname, void * pAmbient )
{
	SQR_TRY
	{
		void *p = NULL;
		p = EffectGroupList[effectname];
		if( p == NULL )
		{
			string log = effectname + " 特效没有找到，请检查";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return;
		}

		IEffect * pEffect		= NULL;
		EffectCreateInfo Ninfo	= {NULL,NULL,NULL} ;
		IEffectManager::GetInst()->CreateEffect((IEffectGroup*)p, effectname.c_str(), &pEffect);

		if ( pEffect )
		{
			CTObject * pModel = NULL;

			if ( pAmbient == NULL )
				pModel =  CToolSetModel::GetInst()->GetLastActiveObject();
			else
				pModel = (CTObject *)pAmbient;

			if ( pModel ) 
				pModel->GetModelGroup()->LinkEffect(pEffect,(IEffectGroup*)p);
		}
		else
		{
			string log = effectname + " 特效没有找到，请检查";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	SQR_CATCH(exp)
	{
		MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);

		return;
	}
	SQR_TRY_END;
}

void sqr::CMapEffect::ToolSetModel_LinkEffect( string str, void * p, void * pAmbient )
{
	if( p == NULL )
	{
		string log = str + " 特效没有找到，请检查, 如果进行保存特效信息将会丢失";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return;
	}

	IEffect * pEffect		= NULL;
	EffectCreateInfo Ninfo	= {NULL,NULL,NULL} ;

	SQR_TRY
	{
		IEffectManager::GetInst()->CreateEffect((IEffectGroup*)p,str.c_str(),&pEffect);

		if ( pEffect )
		{
			CTObject * pModel = NULL;

			if ( pAmbient == NULL )
				pModel =  CToolSetModel::GetInst()->GetLastActiveObject();
			else
				pModel = (CTObject *)pAmbient;

			if ( pModel && pModel->GetModelGroup() ) 
			{
				pModel->GetModelGroup()->LinkEffect(pEffect,(IEffectGroup*)p);
			}
		}
		else
		{
			string log = str + " 特效没有找到，请检查";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	SQR_CATCH(exp)
	{
		MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);

		return;
	}
	SQR_TRY_END;
}

void sqr::CMapEffect::AddUseAmbientEffect( const string& effectname, const int nProbability )
{
	stAmbientEffectInfo *pAmbientUseEffect = new stAmbientEffectInfo;
	pAmbientUseEffect->strEffect    = effectname;
	pAmbientUseEffect->nProbability = nProbability;
	mapAmientUseEffect[effectname]  = pAmbientUseEffect;
}

void sqr::CMapEffect::DeleteUseAmbientEffect( const string& effectname )
{
	map<string, stAmbientEffectInfo *>::iterator iter = mapAmientUseEffect.find(effectname);
	if( iter != mapAmientUseEffect.end() )
	{
		delete iter->second;
		mapAmientUseEffect.erase(iter);
	}
}

void sqr::CMapEffect::CreateRgnAmbientFxGroup( const string& strFXFileName, const string& effectname, void ** p )
{
	IEffectManager::GetInst()->CreateEffectGroup(strFXFileName.c_str(),(IEffectGroup **)p);

	if( !*p )
	{
		string log = strFXFileName;
		log = log + "无法载入特效文件";

		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return;
	}

	RgnAmbientFxGroupList[effectname] = *p;
}

void* sqr::CMapEffect::GetRgnAmbientFXGroup( const string& effectname )
{
	if( RgnAmbientFxGroupList.find(effectname) == RgnAmbientFxGroupList.end() )
		return NULL;
	else
		return RgnAmbientFxGroupList[effectname];
}

bool sqr::CMapEffect::ToolSetModel_LinkRgnFxLink( const string& effectname, void * pAmbient )
{
	SQR_TRY
	{
		void *p = NULL;
		p = RgnAmbientFxGroupList[effectname];
		if( p == NULL )
		{
			string log = effectname + " 特效没有找到，请检查";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return false;
		}

		IEffect * pEffect		= NULL;
		EffectCreateInfo Ninfo	= {NULL,NULL,NULL} ;
		IEffectManager::GetInst()->CreateEffect((IEffectGroup*)p, effectname.c_str(), &pEffect);

		if ( pEffect )
		{
			CTObject * pModel = NULL;
			pModel =  pModel = (CTObject *)pAmbient;

			if ( pModel ) 
			{
				pModel->GetModelGroup()->LinkEffect(pEffect,(IEffectGroup*)p);
				return true;
			}
		}
		else
		{
			string log = effectname + " 特效没有找到，请检查";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	SQR_CATCH(exp)
	{
		MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);

		return false;
	}
	SQR_TRY_END;

	return false;
}

void sqr::CMapEffect::SaveAmbientFXInfo( FILE * fp )
{
	DWORD dwAmbientCnt = 0;
	fwrite(&dwAmbientCnt, sizeof(dwAmbientCnt), 1, fp);
	
	return;

	::SendMessage(m_hFXSetView, WM_FXSETVIEW_SAVEAMBIENTFXINFO,(WPARAM)fp,0);
}

void sqr::CMapEffect::SaveAmbientFXInfoAsClient( FILE * fp )
{
	::SendMessage(m_hFXSetView, WM_FXSETVIEW_SAVEAMBIENTFXINFOASCLIENT,(WPARAM)fp,0);
}

void sqr::CMapEffect::ReadAmbientFXInfo( FILE * fp )
{
	//::SendMessage(m_hFXSetView, WM_FXSETVIEW_READAMBIENTFXINFO,(WPARAM)fp,0);
	DWORD dwCnt = 0;
	fread(&dwCnt,sizeof(DWORD),1,fp);
	//ambientFxList.resize(dwCnt);
	for ( size_t i = 0; i < dwCnt; ++i )
	{
		//AmbientFxPro *pTemp = new AmbientFxPro;
		
		DWORD dwItemNameLen;
		fread(&dwItemNameLen,sizeof(DWORD),1,fp);
		string strItemName;
		strItemName.resize(dwItemNameLen);
		fread((TCHAR*)strItemName.data(),dwItemNameLen,1,fp);

		int nProbability;
		fread(&nProbability,sizeof(int),1,fp);

		fread(&dwItemNameLen,sizeof(DWORD),1,fp);
		string strSkyTextureFileName;
		strSkyTextureFileName.resize(dwItemNameLen);
		fread((TCHAR*)strSkyTextureFileName.data(),dwItemNameLen,1,fp);

		//pTemp->strItemName				= strItemName;
		//pTemp->strSkyTextureFileName	= strSkyTextureFileName;
		//pTemp->nProbability				= nProbability;
		//ambientFxList[i]				= pTemp;
	}
}

void sqr::CMapEffect::LoadAmbientFXModel()
{
	::SendMessage(m_hFXSetView, WM_FXSETVIEW_LOADAMBIENTFXINFO,0,0);
}

void CMapEffect::SetLocalEffectGroupNames( const map< string, vector< string > >& list )
{
	this->mapLocalEffectNameList.clear();
	this->mapLocalEffectNameList = list;
}