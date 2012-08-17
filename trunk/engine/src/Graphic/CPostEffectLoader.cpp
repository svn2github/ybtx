#include "StdAfx.h"
#include "CPostEffectLoader.h"
#include "CPostEffect.h"
#include "CPostEffectPass.h"
#include "CRenderTexture.h"
#include "CGpuProgramMgr.h"
#include "PkgStream.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CXmlConfig.inl"
#include "StringHelper.h"
#include "TSqrAllocator.inl"
#include "CodeCvs.h"

namespace sqr
{
	const GString CPostEffectLoader::DefaultTarget = "default";
	const GString CPostEffectLoader::NoneTarget = "none";
	const GString CPostEffectLoader::PreviousTarget = "previous";
	const DWORD POST_VS_ID = 10;
}

//------------------------------------------------------------------------------
CPostEffectLoader::CPostEffectLoader( const string& fileName )
{
	// load xml file
	wstring path = CGraphic::GetInst()->GetShaderConfigPath();
	path.append(utf8_to_utf16(fileName));
	ipkgstream stream;
	if ( stream.open(PATH_ALIAS_SHD.c_str(), utf16_to_utf8(path).c_str()) != 0 )
		GenErr( GraphicErr::GetErrTypeStr(ShaderInit_Err), "can't open " + utf16_to_utf8(path) );

	pPostEffectCfg = new CXmlConfig( "PostProcess" , stream );
	xmlConfig = pPostEffectCfg->GetRootNode();
}

//------------------------------------------------------------------------------
CPostEffectLoader::~CPostEffectLoader()
{
	GfkSafeDelete(this->pPostEffectCfg);
	xmlConfig = NULL;
}
//------------------------------------------------------------------------------
void
CPostEffectLoader::PreLoadEffects( GVector<CPostEffectPtr>& outEffects )
{
	Ast(outEffects.empty());

	for (TiXmlNode* node = xmlConfig->FirstChild(); node != NULL;
		node = node->NextSibling())
	{
		TiXmlElement* pElement = node->ToElement();
		if (pElement && pElement->ValueStr() == "PostEffect")
		{
			CPostEffectPtr effect = new CPostEffect();
			string name = pElement->Attribute("name");
			string reset = pElement->Attribute("reset");
			effect->SetName(name);
			effect->SetReset("true" == reset);
			outEffects.push_back(effect);
		}
	}
}

//------------------------------------------------------------------------------
void
CPostEffectLoader::LoadPostEffect( CPostEffectPtr& outEffect )
{
	Ast(outEffect.isvalid());
	Ast(outEffect->IsEmpty());

	// parse effects
	for (TiXmlNode* pNode = xmlConfig->FirstChild(); pNode != NULL;
		pNode = pNode->NextSibling())
	{
		TiXmlElement* pElement = pNode->ToElement();
		if (pElement && pElement->ValueStr() == "PostEffect" &&
			pElement->Attribute("name") == string(outEffect->GetName()))
		{
			// parse children
			for (TiXmlNode* pSubNode = pNode->FirstChild(); pSubNode != NULL;
				pSubNode = pSubNode->NextSibling())
			{
				if (!pSubNode->ToElement())
					continue;
				ParsePostEffectPass(pSubNode, outEffect);
			}
		}
	}
}

//------------------------------------------------------------------------------
void
CPostEffectLoader::ParsePostEffectPass( TiXmlNode* xmlNode, const CPostEffectPtr& postEffect )
{
	CPostEffectPassPtr pass = new CPostEffectPass();
	string target = xmlNode->ToElement()->Attribute("target");
	bool clear = tobool(xmlNode->ToElement()->Attribute("clear"));

	DWORD vsID = POST_VS_ID | MakeGroupID(4) | SSI_BASE_VS_SYSTEM_ID;
	DWORD psID = atoi(xmlNode->ToElement()->Attribute("effectId"))|MakeGroupID(4)|SSI_BASE_PS_SYSTEM_ID;
	
	if ( NULL != CGpuProgramMgr::GetInst()->getGpuProgram(vsID) &&
		 NULL != CGpuProgramMgr::GetInst()->getGpuProgram(psID) )
	{
		SHADER_HANDLE shader_handle;
		shader_handle  = vsID;
		shader_handle |= psID;

		pass->SetEffect(shader_handle);
		pass->SetTargetNeedClear(clear);
		pass->SetTarget(this->ParseRenderTexture(target));

		// sources
		for (TiXmlNode* node = xmlNode->FirstChild(); node != NULL;
			node = node->NextSibling())
		{
			if (!node->ToElement())
				continue;
			string source = node->ToElement()->Attribute("name");
			pass->AddSource(this->ParseRenderTexture(source));
		}
		postEffect->AddPass(pass);
	}
}

// -----------------------------------------------------------------------------
const CRenderTexturePtr&
CPostEffectLoader::ParseRenderTexture( const string& name )
{
	if ( NoneTarget == name.c_str() || PreviousTarget == name.c_str() )
	{
		static const CRenderTexturePtr nullPtr;
		return nullPtr;
	}
	// search existed
	GMap<GString, CRenderTexturePtr>::iterator it = renderTargetMap.find(name.c_str());
	if (it != renderTargetMap.end())
		return it->second;

	// parse RTT's parameters
	TiXmlNode* xmlNode = NULL;
	TiXmlElement* pElement = NULL;
	GString rttName;
	for (xmlNode = xmlConfig->FirstChild(); xmlNode != NULL; xmlNode = xmlNode->NextSibling())
	{
		pElement = xmlNode->ToElement();
		if (!pElement || pElement->ValueStr() != "RenderTexture")
			continue;
		rttName = pElement->Attribute("name");
		if (rttName == name.c_str())
			break;
	}

	uint width, height;
	if ( (pElement->Attribute("width")) 
		&& (pElement->Attribute("height") ))
	{
		width = atoi(pElement->Attribute("width"));
		height = atoi(pElement->Attribute("height"));
	}
	else
	{
		// relative width & height
		CIRect rect;
		CMainWindowTarget::GetInst()->GetRect(rect);
		width  = uint(rect.Width() * atof(pElement->Attribute("relWidth")));
		height = uint(rect.Height() * atof(pElement->Attribute("relHeight")));
	}
	_texFORMAT format = RTTFormat_Kit::FromString(pElement->Attribute("format"));

	bool isUseDepth = tobool(pElement->Attribute("depth"));
	bool msaa		= false;
	if (pElement->Attribute("msaa"))
	{
		msaa = tobool(pElement->Attribute("msaa"));
	}
	// create RTT
	CRenderTexturePtr renderTarget = CGraphic::GetInst()->CreateRenderTarget();
	renderTarget->SetWidth(width);
	renderTarget->SetHeight(height);
	renderTarget->SetFormat(format);
	renderTarget->SetUseDepthBuffer(isUseDepth);
	renderTarget->SetAntiAlias(msaa);
	renderTarget->Create();
	renderTarget->Release();// set reference count to 1

	return renderTargetMap.insert(make_pair(name.c_str(), renderTarget)).first->second;
}

//------------------------------------------------------------------------------
CRenderTexture*
CPostEffectLoader::GetRenderTexture( const string& name ) const
{
	GMap<GString, CRenderTexturePtr>::const_iterator it = renderTargetMap.find(name.c_str());
	if (it != renderTargetMap.end())
		return it->second.get();

	return NULL;
}

//------------------------------------------------------------------------------
void
CPostEffectLoader::ReleaseUnusedRTT()
{
	for (GMap<GString, CRenderTexturePtr>::iterator it = this->renderTargetMap.begin();
		it != this->renderTargetMap.end(); )
	{
		const CRenderTexturePtr& rtt = it->second;
		if (rtt->GetRefCount() == 1)
			this->renderTargetMap.erase(it++);
		else
			++it;
	}
}
