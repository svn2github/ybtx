#pragma once
//------------------------------------------------------------------------------
/**
	@class CPostEffectLoader

	从XML文件加载PostEffect的类
	特殊RTT Name: default, none, previous
*/
#include "CRefObject.h"
#include "CGraphicMallocObject.h"
class TiXmlNode;
namespace sqr
{
	class CXmlConfig;

	SmartPointer(CPostEffect);
	SmartPointer(CPostEffectLoader);
	SmartPointer(CRenderTexture);

	class CPostEffectLoader
		: public CRefObject
		, public CGraphicMallocObject
	{
	public:
		static const GString DefaultTarget;
		static const GString NoneTarget;
		static const GString PreviousTarget;

	public:
		CPostEffectLoader(const string& fileName);
		virtual ~CPostEffectLoader();

		/// 预加载Effect
		void PreLoadEffects(GVector<CPostEffectPtr>& outEffects);
		/// 加载PostEffect
		void LoadPostEffect(CPostEffectPtr& outEffect);
		/// 取得指定RTT
		CRenderTexture* GetRenderTexture(const string& name) const;
		/// 取得默认RTT
		CRenderTexture* CreateDefaultRTT();
		/// 释放不使用的RTT
		void ReleaseUnusedRTT();

	private:
		/// 解析Pass结点
		void ParsePostEffectPass(TiXmlNode* xmlNode, const CPostEffectPtr& postEffect);
		/// 创建指定的RTT
		const CRenderTexturePtr&  ParseRenderTexture(const string& name);

	private:
		CXmlConfig *pPostEffectCfg;
		TiXmlNode* xmlConfig;
		GMap<GString, CRenderTexturePtr>	renderTargetMap;
	};

	//------------------------------------------------------------------------------
	inline CRenderTexture*
		CPostEffectLoader::CreateDefaultRTT()
	{
		return this->ParseRenderTexture(DefaultTarget.c_str());
	}

}// namespace sqr
