#pragma once
#include "SQRWnd.h"

class TiXmlAttribute;
class CBigMap: public sqr::SQRWnd
{
	DYNAMIC_DECLARE_WND( CBigMap )
public:
	CBigMap();
	virtual ~CBigMap(void);
	void DrawWndBackground();
	int32 DestroyWnd();
	void SetBigMapWnd(SQRWnd * pWnd);
	void UpdateVisibleRegion();
	bool AddAreaFrog(const TCHAR* AreaName);
	bool RemoveAreaFrog(const TCHAR* AreaName);
	virtual void InitMapData( const TCHAR* szMapFile);
	
private:
	void ClearUp();
	void DrawBigMap();
	void AlphaTexsRenderToTexture();
	bool LoadConfig(string& SceneName);
	bool LoadNameByXml(TiXmlAttribute *XmlAtt);
	bool LoadMapConfigByXml(TiXmlAttribute *XmlAtt);
protected:
	UIMap<UIString,UIString>	m_GameRegionToFileName;
	UIList<UIString>			m_AlphaTexNameList;
	string		m_SceneName;
	float		m_fRTWidth;
	float		m_fRTHeight;
	CVector2f	m_BackTexUV[4];
	ITexture*	m_MapTextureOpen;
	ITexture*   m_MapTextureClose;
	SQRWnd		*m_pWnd;
	DWORD		m_bOpen;
	DWORD		m_bClose;
	UIMap<UIString,ITexture*> m_MapRegionAlphaTex;
	CRenderTexture*		m_pAlphaTexture;
};