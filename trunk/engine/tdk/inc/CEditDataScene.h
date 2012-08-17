#pragma once
#include "GraphicClassDefine.h"
#include "CDataScene.h"

namespace sqr_tools
{
	class CEditTerrainMgr;
	class CEditSkyMgr;
	class CEditWaterMgr;
	class CEditLogicDataMgr;
	class CYbtxGacCoder;
	class CEditDataScene 
		: public CDataScene
	{
	public:
		friend class CEditContext;
		friend class CYbtxGacCoder;
	public:
		CEditDataScene( const string& sName = "", uint32 Wdith = 0, uint32 Height = 0, uint32 RecycleSize = 0 );
		~CEditDataScene();

	public:
		void				InitEditDateScene(CWindowTarget* pTarget);
		CEditTerrainMgr*	GetTerrainMgr(void);
		CEditSkyMgr*		GetSkyMgr(void);
		CEditWaterMgr*		GetWaterMgr(void);
		CEditLogicDataMgr*	GetLogicDataMgr(void);
		void				SetWidth( uint16 width );
		void				SetHeight( uint16 height );
	};
}
