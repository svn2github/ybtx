#include "stdafx.h"
#include "CGpuProgramMgr.h"
#include "pkgstream.h"
#include "BaseHelper.h"
#include "CGraphicExp.h"
#include "CXmlConfig.inl"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"

const GString CGpuProgramMgr::st_ShaderVerPath[] = 
{
	"",		//Shader_V_0 
	"v10/",	//Shader_V_1
	"v20/",	//Shader_V_2
	"v20/",	//Shader_V_3
	"v20/",	//Shader_V_4
};

CGpuProgramMgr::CGpuProgramMgr()
: m_ActivePro(NULL)
, m_SysPro(NULL)
, m_isLock(false)
, m_isLoaded(false)
, m_eShaderVer(Shader_V_0)
{
}

CGpuProgramMgr::~CGpuProgramMgr()
{
	for (GpuProList::iterator it = m_ProgramList.begin();it!=m_ProgramList.end();it++)
		GfkSafeDelete(it->second);
	m_ProgramList.clear();
}

CGpuProgram* CGpuProgramMgr::create(const string& str,DWORD handle,GpuProgramType gpt,const string& fun)
{
	return create(str.c_str(),str.length(),handle,gpt,fun);
}

CGpuProgram* CGpuProgramMgr::create(const char* buf,size_t fsize,DWORD handle,GpuProgramType gpt,const string& fun)
{
	destroy(handle);
	CGpuProgram *pPro;// = _create( handle, gpt, fun );

	switch (gpt)
	{
	case GPT_VERTEX_PROGRAM:
		pPro = _create_vertex_pro(handle,fun);
		break;
	case GPT_FRAGMENT_PROGRAM:
		pPro = _create_fragment_pro(handle,fun);
		break;
	case GPT_EFFECTS_PROGRAM:
		pPro = _create_effects_pro(handle,fun);
		break;
	default:
		pPro = NULL;
		break;
	}

	if (NULL!=pPro && pPro->load(buf,fsize))
	{
		m_ProgramList.insert(GpuProList::value_type(handle,pPro));
		return pPro;
	}
	GfkSafeDelete(pPro);
	return NULL;
}

void CGpuProgramMgr::destroy( DWORD handle)
{
	GpuProList::iterator it = m_ProgramList.find(handle);
	if (it!=m_ProgramList.end())
	{
		if (m_ActivePro == it->second)
		{
			m_ActivePro->unbind();
			m_ActivePro = NULL;
		}
		CGpuProgram* tmp = it->second;
		m_ProgramList.erase(it);
		GfkSafeDelete(tmp);
	}
	return;
}

CGpuProgram* CGpuProgramMgr::bind( const SHADER_HANDLE& handle )
{
	if (m_isLock)
		return m_SysPro;

	if (0==handle.GetPSShaderID())
	{
		unbind();
		_bind(handle.GetVSShaderID());
		return NULL;
	}
	_bind(handle.GetVSShaderID());
	GpuProList::iterator it = m_ProgramList.find(handle.GetPSShaderID());
	if (it != m_ProgramList.end())
	{
		if (it->second!=m_ActivePro && it->second != NULL)
		{
			unbind();
			m_ActivePro = it->second;
		}
		m_ActivePro->bind();
		return m_ActivePro;
	}
	unbind();
	return NULL;
}

void CGpuProgramMgr::_bind(DWORD SysID)
{
	if (0==SysID || 0xffffffff == SysID)
	{
		if (m_SysPro)
		{
			m_SysPro->unbind();
			m_SysPro = NULL;
		}
		return;
	}

	GpuProList::iterator it = m_ProgramList.find(SysID);
	if (it != m_ProgramList.end())
	{
		if (it->second!=m_SysPro && it->second != NULL)
		{
			if (m_SysPro)
			{
				m_SysPro->unbind();
				m_SysPro = NULL;
			}

			if (m_ActivePro)
			{
				m_ActivePro->unbind();
				m_ActivePro = NULL;
			}

			it->second->bind();
			m_SysPro = it->second;
		}
		return;
	}

	if (m_SysPro)
	{
		m_SysPro->unbind();
		m_SysPro = NULL;
	}
	return;
}

void CGpuProgramMgr::unbind(void)
{
	if (m_ActivePro)
		m_ActivePro->unbind();
	m_ActivePro = NULL;
}

void CGpuProgramMgr::lockGpuProgram(bool isLock)
{
	m_isLock = isLock;
}

CGpuProgram* CGpuProgramMgr::getGpuProgram(DWORD handle)
{
	GpuProList::iterator it = m_ProgramList.find(handle);
	if (it != m_ProgramList.end())
		return it->second;
	return NULL;
}

bool  CGpuProgramMgr::load(const string& path,const string& file, ShaderVer sVer)
{
	SQR_TRY	
	{
		GString temp;
		m_ShaderPath = path.c_str();
		m_eShaderVer = sVer;
		temp = m_ShaderPath + file.c_str();
		m_ShaderPath += st_ShaderVerPath[m_eShaderVer];
		bool AllComplate = true;

		ipkgstream in(PATH_ALIAS_SHD.c_str(), temp.c_str());
		if (!in)
			GenErr(string("Can't open ") + temp.c_str());

		CXmlConfig *pShaderCfg = new CXmlConfig( "shader_config" , in );

		if (m_SysPro)
			m_SysPro->unbind();

		m_SysPro = NULL;
		if (m_ActivePro)
			m_ActivePro->unbind();
		m_ActivePro = NULL;

		TiXmlNode *pIncludeNode = NULL;
		for(pIncludeNode = pShaderCfg->Get<TiXmlElement*>("include_config")->FirstChild(); pIncludeNode; 
			pIncludeNode = pIncludeNode->NextSibling("include"))
		{
			TiXmlElement *pElement = pIncludeNode->ToElement();
			if (!pElement)
				continue;			

			AllComplate &= loadIncludeByXML( pElement->FirstAttribute() );
		}

		TiXmlNode *pMacroNode = NULL;
		for(pMacroNode = pShaderCfg->Get<TiXmlElement*>("macro_config")->FirstChild(); pMacroNode; 
			pMacroNode = pMacroNode->NextSibling("macro"))
		{
			TiXmlElement *pElement = pMacroNode->ToElement();
			if (!pElement)
				continue;	

			AllComplate &= loadMacroByXML( pElement->FirstAttribute() );
		}

 		TiXmlNode *pGroupNode = NULL;
 		for(pGroupNode = pShaderCfg->Get<TiXmlElement*>("group_config")->FirstChild(); pGroupNode;
			pGroupNode = pGroupNode->NextSibling("Group"))
 		{
 			if (!pGroupNode->ToElement())
 				continue;
 
 			AllComplate &= LoadGroupByXml( pGroupNode );
 
 			;
 		}

		m_pShaderInc->Clear();
		in.close();
		GfkSafeDelete(pShaderCfg);

		this->m_isLoaded = true;

		return AllComplate;
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("Shader ³õÊ¼»¯Ê§°Ü");
		GfkLogExp(exp);	
	}
	SQR_TRY_END;
	return false;
}

bool CGpuProgramMgr::LoadGroupByXml(TiXmlNode *pDoc)
{
	bool AllComplate = true;
	TiXmlAttribute* XmlAtt = pDoc->ToElement()->FirstAttribute();
	DWORD GroupID = MakeGroupID(atoi(XmlAtt->Value()));
	XmlAtt=XmlAtt->Next();
	DWORD Mask = (atoi(XmlAtt->Value()));
	if (Mask!=0)Mask|=0xC0000000;
	for (TiXmlNode * pi = pDoc->FirstChild();pi;pi=pi->NextSibling()){
		if (!pi->ToElement())
			continue;
		AllComplate &= loadShaderByXML(pi->ToElement()->FirstAttribute(),GroupID,Mask);
	}
	return AllComplate;
}

bool CGpuProgramMgr::loadShaderByXML(TiXmlAttribute* XmlAtt,DWORD GroupId,DWORD Mask)
{
	string szTemp;
	DWORD id = 0, type = 0;
	id = atoi(XmlAtt->Value()) | GroupId;
	XmlAtt=XmlAtt->Next();
	szTemp = m_ShaderPath.c_str();
	szTemp += XmlAtt->Value();
	XmlAtt=XmlAtt->Next();
	type = atoi(XmlAtt->Value());
	id |= (type<<30);
	CPkgFile PkgFile;
	if ( PkgFile.Open( PATH_ALIAS_SHD.c_str(), szTemp.c_str() ) != eFE_SUCCESS )
		return false;

	int32 fileSize = PkgFile.Size();
	char* pBuf = new char[fileSize + 1];

	PkgFile.read( pBuf, fileSize );

	XmlAtt = XmlAtt->Next();
	string temp =  XmlAtt->ValueStr();
	XmlAtt = XmlAtt->Next();


	CGpuProgram *pPro;

	if (0 == Mask)
	{
		if ( NULL==(pPro = getGpuProgram(id)) )
		{
			switch (type)
			{
			case GPT_VERTEX_PROGRAM:
				pPro = _create_vertex_pro(id,temp.c_str());
				break;
			case GPT_FRAGMENT_PROGRAM:
				pPro = _create_fragment_pro(id,temp.c_str());
				break;
			case GPT_EFFECTS_PROGRAM:
				pPro = _create_effects_pro(id,temp.c_str(),XmlAtt->Value());
				break;
			default:
				pPro = NULL;
				break;
			}
			if (NULL!=pPro && pPro->load(pBuf,fileSize))
			{
				m_ProgramList.insert(GpuProList::value_type(id,(CGpuProgram*)pPro));
			}
			else
				GfkSafeDelete(pPro);
		}
		else
		{
			if (!pPro->load(pBuf,fileSize))
				destroy(id);
		}
	}
	else
	{
		for (uint i = 0;i<ShaderMacroMgr::Num_System_Macro; ++i)
		{
			if ((ShaderMacroMgr::System_Macro_ID[i]&Mask) == ShaderMacroMgr::System_Macro_ID[i])
			{
				DWORD tempid = id | ShaderMacroMgr::System_Macro_ID[i];
				if ( NULL==(pPro = getGpuProgram(tempid)) )
				{
					switch (type)
					{
					case GPT_VERTEX_PROGRAM:
						pPro = _create_vertex_pro(tempid,temp.c_str());
						break;
					case GPT_FRAGMENT_PROGRAM:
						pPro = _create_fragment_pro(tempid,temp.c_str());
						break;
					case GPT_EFFECTS_PROGRAM:
						pPro = _create_effects_pro(tempid,temp.c_str(),XmlAtt->Value());
						break;
					default:
						pPro = NULL;
						break;
					}
					if (NULL!=pPro && pPro->load(pBuf,fileSize))
					{
						m_ProgramList.insert(GpuProList::value_type(tempid,(CGpuProgram*)pPro));
					}
					else
						GfkSafeDelete(pPro);
				}
				else
				{
					if (!pPro->load(pBuf,fileSize))
						destroy(tempid);
				}
			}
		}
	}

	delete pBuf;
	PkgFile.Close();
	return pPro!=NULL;
}

bool CGpuProgramMgr::loadIncludeByXML(TiXmlAttribute* XmlAtt)
{
	string temp;
	string name = XmlAtt->Value();
	temp =	m_ShaderPath.c_str();
	temp += "include/" + name;
	CPkgFile PkgFile;
	if ( PkgFile.Open( PATH_ALIAS_SHD.c_str(), temp.c_str() ) != eFE_SUCCESS )
		return false;
	int32 fileSize = PkgFile.Size();
	char* pBuf = new char[fileSize+1];
	PkgFile.read( pBuf, fileSize );
	pBuf[fileSize] = NULL;
	m_pShaderInc->InsertInclude( name, pBuf );
	delete pBuf;
	PkgFile.Close();
	return true;
}

bool CGpuProgramMgr::loadMacroByXML(TiXmlAttribute* XmlAtt)
{
	GString temp;
	DWORD id = atoi(XmlAtt->Value());
	//XmlAtt=XmlAtt->GetNextSibling();
	XmlAtt=XmlAtt->Next();
	temp = m_ShaderPath + "macro/" + XmlAtt->Value();
	CPkgFile PkgFile;
	if ( PkgFile.Open( PATH_ALIAS_SHD.c_str(), temp.c_str() ) == eFE_SUCCESS )
	{
		int32 fileSize = PkgFile.Size();
		char* pBuf = new char[fileSize+1];
		PkgFile.read( pBuf, fileSize );
		pBuf[fileSize] = NULL;
		m_pMacroMgr->LoadMacro( MakeSysVS_ID(id) , pBuf );
		delete pBuf;
		PkgFile.Close();
	}

	XmlAtt=XmlAtt->Next();
	temp = m_ShaderPath + "macro/" + XmlAtt->Value();
	if ( PkgFile.Open( PATH_ALIAS_SHD.c_str(), temp.c_str() ) == eFE_SUCCESS )
	{
		int32 fileSize = PkgFile.Size();
		char* pBuf = new char[fileSize+1];
		PkgFile.read( pBuf, fileSize );
		pBuf[fileSize] = NULL;
		m_pMacroMgr->LoadMacro( MakeSysPS_ID(id) , pBuf );
		delete pBuf;
		PkgFile.Close();
	}

	XmlAtt=XmlAtt->Next();
	temp = m_ShaderPath + "macro/" + XmlAtt->Value();
	if ( PkgFile.Open( PATH_ALIAS_SHD.c_str(), temp.c_str() ) == eFE_SUCCESS )
	{
		int32 fileSize = PkgFile.Size();
		char* pBuf = new char[fileSize+1];
		PkgFile.read( pBuf, fileSize );
		pBuf[fileSize] = NULL;
		m_pMacroMgr->LoadMacro( Input_Macro_ID(id) , pBuf );
		delete pBuf;
		PkgFile.Close();
	}
	return true;
}

