#pragma once

#include "CVector3.h"
#include "CDynamicObject.h"
#include "FindPathDefs.h"
#include "TWhole.h"
#include "TPart.h"
#include "CPkgFile.h"
#include "CSceneCommon.h"
#include <deque>
#include "CMetaSceneMallocObject.h"

namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;
	class CPixelPath;
	class IDynamicBarrierInfo;

	template<typename Traits>
	class TCoreScene;

	template<typename Traits>
	class TMetaSceneMgr;

	template<typename Traits>
	class TMetaScene							//多个场景副本只有一份
		:public virtual CDynamicObject
		,public TWhole<typename Traits::MetaScene_t,typename Traits::CoreScene_t>
		,public TPart<typename Traits::MetaScene_t,typename Traits::MetaSceneMgr_t>
		,public CMetaSceneMallocObject
	{
	protected:
		typedef TMetaSceneMgr<Traits>						MetaSceneMgr_t;
		typedef typename Traits::MetaSceneMgr_t				ImpMetaSceneMgr_t;
		typedef typename Traits::MetaScene_t				ImpMetaScene_t;
		typedef typename Traits::CoreScene_t				ImpCoreScene_t;
		typedef TCoreScene<Traits>							Scene_t;
		typedef TPart<ImpMetaScene_t,ImpMetaSceneMgr_t>		ParentPart_t;

		friend class TCoreScene<Traits>;

		TMetaScene(const char* szSceneName,const uint32 uSceneVer,MetaSceneMgr_t* pMgr);
		~TMetaScene();

		virtual uint32 GetXInRegion( uint32 uRegionId );
		virtual uint32 GetYInRegion( uint32 uRegionId );
		virtual void LoadSceneInfo();
		virtual void LoadRegionInfo();
		virtual void LoadLinkRegionInfo();

		virtual void LoadOneRegion( CPkgFile& File, uint32 nChunk, uint32 uRegionId );
		virtual void LoadAll();

		virtual void LoadRegionBlock( CPkgFile& File, uint32 uRegionId );

		///Exact blocl
		virtual void LoadRegionExactBlock( CPkgFile& File, uint32 uRegionId );

		virtual const char* GetRegionFileName( string& str ) { return str.c_str(); };

		virtual void LoadOneChunk( CPkgFile& File, uint32 uRegionId, uint32 uMagicID );
		//For init before loading arg(region) file to create Mgr Obj like CFindPathMgr, CTrapMgr etc.
		virtual void PreLoadRegionInfo();

		string				m_sSceneName;

		int32				m_nMagicID;			// "AMAP"
		int32				m_nVersion;			// 版本
		int32				m_nVersionID;
		uint8				m_uGridWidth;		// 每个格子的的宽度有多少个象素，假定Grid的宽，高一致  *****去掉

		bool				m_bCave;			// shadow
		IMap*				m_pMapPath;			// 寻路信息



		// 场景中子区域的个数
		int					m_nSubRegionNum;

		// 场景中区域的个数
		uint32				m_uRegionNum;

		// 场景的宽度，以子区域为单位
		uint32				m_uSubWidthInRegion;

		// 场景的宽度，以区域为单位
		uint32				m_uWidthInRegion;

		// 场景的高度，以区域为单位
		uint32				m_uHeightInRegion;

		// 场景的宽度，以格子为单位
		uint32				m_uWidthInGrid;

		// 场景的高度，以格子为单位
		uint32				m_uHeightInGrid;

		// 场景的宽度，以象素为单位
		uint32				m_uWidthInPixel;

		// 场景的高度，以象素为单位
		uint32				m_uHeightInPixel;

	public:
		uint32 GetWidthInRegion() const;
		uint32 GetHeightInRegion() const;
		uint32 GetWidthInGrid()const;
		uint32 GetHeightInGrid()const;
		uint32 GetWidthInPixel();
		uint32 GetHeightInPixel();
		uint32 GetGridX( uint32 uGridId );
		uint32 GetGridY( uint32 uGridId );
		uint32 GetRegionLeftX( uint32 uRegionId );
		uint32 GetRegionDownY( uint32 uRegionId );

		CPixelPath* CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType,
			EBarrierType EBarrier, const IDynamicBarrierInfo* pBarrierInfo = NULL);
		CPixelPath* CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType,
			EBarrierType eBarrierType, int32 nMaxStep, const IDynamicBarrierInfo* pBarrierInfo = NULL);

		CPixelPath* CreatePath(const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, const IDynamicBarrierInfo* pBarrierInfo = NULL);

		const char* GetSceneName()const;
		uint32 GetRegionVersionID() const { return m_nVersionID; };

		MetaSceneMgr_t* GetMetaSceneMgr()const;

		EBarrierType GetBarrier( const CPos& GridPos )const;
		EBarrierType GetBarrier( const CPos& GridPos, const IDynamicBarrierInfo* pBarrierInfo )const;

	private:
		static void CalcCrossPoint(CFPos& PixelPosCross, const CFPos& PixelPosBegin1, const CFPos& PixelPosEnd1, const CFPos& PixelPosBegin2, const CFPos& PixelPosEnd2);
		void RevisePixelPosEnd(const CFPos& PixelPosBegin, CFPos& PixelPosEndRevised)const;
		CPixelPath* AssemblePixelPath(IPath*, const CFPos&, const CFPos&, const IDynamicBarrierInfo* pBarrierInfo);

	};
}
