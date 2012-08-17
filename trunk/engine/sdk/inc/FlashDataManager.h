#pragma once
//------------------------------------------------------------------------------
/**
@class FlashDataManager.h

*/
#include "TSingleton.h"
#include "SQRGuiMallocObject.h"
#include "CTexture.h"
#include "CRectangle.h"
#include "GUIBase.h"
#include "SQRWndDef.h"
#include "TSqrAllocator.inl"
#include <hash_map>

namespace sqr
{
	class FlashDataManager
		: public virtual CDynamicObject
		, public Singleton<FlashDataManager>
		, public SQRGuiMallocObject
	{
	public:
		FlashDataManager();
		~FlashDataManager();

		static FlashDataManager*	Inst();
		static const UIString		Flash_Root;				// 根
		static const UIString		Flash_Info;				// 节点
		static const UIString		Flash_Tex;				// 材质
		static const UIString		Flash_Name;				// 名字
		static const UIString		Flash_Ato;				// alpha
		static const UIString		Flash_Afrom;			// alpha
		static const UIString		Flash_Pos;				// 位置
		static const UIString		Flash_V;				// 单程时间
		static const UIString		Flash_Count;			// 闪烁次数
		static const UIString		Flash_RePos;
		static const int32			Flash_Scale;
		typedef UIMap<UIString, FlashWndInfo*>	FlashInfoMap;
		
		bool			Initialize(const string& Path, IGraphic* pIGraphic);
		void			Release(void);
		void			EraserFlashData(FlashInfoMap::iterator it);
		FlashWndInfo*	GetFlashInfo(UIString name);

		inline FlashInfoMap::iterator FlashBegin(void)		{ return m_FlashData.begin();	};
		inline FlashInfoMap::iterator FlashEnd(void)		{ return m_FlashData.end();		};
		inline FlashInfoMap&		  Flashmap(void)		{ return m_FlashData;			};
	private:
		static FlashDataManager*& IntInst();
		CFRect StringToCFRect(std::string& str);

		FlashInfoMap			m_FlashData;
		bool					m_isLoad;
		IGraphic*				m_pGraphic;
	};
}