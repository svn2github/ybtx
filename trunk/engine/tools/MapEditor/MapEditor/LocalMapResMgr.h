#pragma once
#include "TSingleton.h"
#include "CVector3.h"

namespace sqr
{
	class CLoadMapResInfo: public Singleton<CLoadMapResInfo>
	{
	public:
		struct CREATE_ITEM
		{
			uint32					dwResType;		//资源类型:MOD,SKE,EFX		(必填)
			std::string				szFileName;		//资源文件名						(必填)
			std::string				szClassName;	//资源类名							(必填)
			std::string				szParam;		//类型参数
		};

		struct EditorModelInfo
		{
			///给editor用的变量
			CVector3f vUserOffsetEx; //操作移动
			bool	  bBakingColor;
			int		  nMirrorType;
			int		  nAffectRatio;
			///trans state 0 - auto 1 -  trans 2 - untrans
			BYTE	  bTransState;
		};

		struct EditorOffsetInfo
		{
			///给editor用的变量
			CVector3f vUserOffsetEx; //操作移动
		};

		struct OBJECT_CREATE_PARAM
		{
			std::vector<CREATE_ITEM> Items;
		};

		struct CTerrainSetUnit
		{
			std::vector<OBJECT_CREATE_PARAM> m_UnitPart;
		};

		void DealGridObjectsToTileRegion();
		void MakeTerrainTexture();

	public:
		CLoadMapResInfo(void);
		~CLoadMapResInfo(void);
		void  DestroyEditorModelInfo();
		void  DestroyAllContainerInfo();

		//区域物件
		vector<CTerrainSetUnit>		m_SetUnit;
		vector<string>			    m_CueNameUnit;

		///用于记录地图中用到的所有纹理的名字，保存各层纹理信息则保存index就行
		map<string, int16>			m_mapTerrainTextureNames;
		map<int16, string>			m_mapIndexTextureNames;///用于记录地图中用到的所有纹理的名字
		vector<string>				m_vecWaterTextureNames;///用于记录地图中用到的所有水纹理的名字
		set<string>					m_setTerrainTexIndexs;
		vector<EditorModelInfo *>   vecEditorSingleModelInfo;///读取一个TileRegion(4*4Region)物体属性列表，用于给gameobject用，并根据索引对应是哪个模型，有时加载模型会失败
		vector<EditorModelInfo *>   vecEditorLoginModelInfo;
		vector<EditorOffsetInfo *>  vecEditorPointLightOffsetInfo;///light offset info
		vector<EditorOffsetInfo *>  vecEditorSpotLightOffsetInfo;
		vector<EditorOffsetInfo *>  vecEditorAudioOffsetInfo;///light offset info
		size_t						editorSingleModelCnt;///not coclude login mode;
		size_t						editorLoginModelCnt;
	};
}