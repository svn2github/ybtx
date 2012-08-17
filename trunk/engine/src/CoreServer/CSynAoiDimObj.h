#pragma once 
#include "CPos.h"
#include "CBaseAoiObj.h"
#include "CRectangle.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{
	class CSynAoiViewObj;
	class CSynAoiDimScene;
	class CTraitsAoiQuad;
	class CAoiQuadNode;

	template<typename ObjectType>
	class TQuadNode;
	
	template<typename ObjectType>
	class TQuadTree;

	class CSynAoiDimObj : public CBaseAoiObj, public CQuadMallocObject
	{
		friend class CSynAoiDimScene;

		friend class TQuadNode<CTraitsAoiQuad>;
		friend class TQuadTree<CTraitsAoiQuad>;
		friend class CAoiQuadTree;

		typedef vector<CSynAoiDimObj*, TQuadAllocator<CSynAoiDimObj*> >	VecDimObject_t;

	public:
		CSynAoiDimObj(CSynAoiDimScene* pDimScene, CSynAoiViewObj* pViewObj, const CFPos& pos, uint32 uDimension);
		~CSynAoiDimObj();

		void Release();

		CSynAoiDimScene* GetScene() const
		{
			return m_pDimScene;
		}


		void SetSize( float fSize );
		void SetEyeSight( float fEyeSight );
		void SetStealth( float fStealth );
		void SetKeenness(float Keenness);
		void SetViewGroup(uint32 uGroupID);
		
		void SetPos(const CFPos& PixelPos);
		void ForceSetPos(const CFPos& PixelPos);	//移动结束后强制更新坐标点

		float GetSize() const; 
		float GetEyeSight() const; 
		float GetStealth() const; 
		float GetKeenness() const; 
		uint32 GetViewGroup() const; 
		
		void InsertToScene();
		void RemoveFrScene();

		//表明是否能看见对方，true就是能看见，false就不能 
		bool CanSee(CSynAoiDimObj *pOther); 

		void PostMsgToViewee(const void* pContext);
		void PostMsgToViewer(const void* pContext);

		void PostMsgToSyncee(const void* pContext);
		void PostMsgToSyncer(const void* pContext);

		const CFPos& GetPosition() const;

	private:
		
		void OnViewRelationChanged(CSynAoiDimObj* pViewObj,
			const CFPos& MyAoiPos,const CFPos& OtAoiPos,bool bSee);
		void OnViewRelationKept(CSynAoiDimObj* pOtObj,
			const CFPos& MyOldAoiPos,const CFPos& MyAoiPos,
			const CFPos& OtOldGridPos,const CFPos& OtAoiPos);

		void OnMsgFromViewer(const void* pContext);
		void OnMsgFromViewee(const void* pContext);
	
		void OnMsgToVieweeHandled(const void* pContext);
		void OnMsgToViewerHandled(const void* pContext);

		void OnMsgFromSyncer(const void* pContext);
		void OnMsgFromSyncee(const void* pContext);

		void OnMsgToSynceeHandled(const void* pContext);
		void OnMsgToSyncerHandled(const void* pContext);

		void SetViewRect(const CFPos& NewPos, const CFRect& rcMaxBox);
		const CFRect& GetViewRect() const;

		void SetSizeRect(const CFPos& NewPos, const CFRect& rcMaxBox);
		const CFRect& GetSizeRect() const;
		
		void IntSetPos(const CFPos& GridPos);

		void SetAoiPos(const CFPos& NewPos);
		
	private:
		void ModifyObservationProperty(float fNewEyeSight,float fNewKeenness);
		void ModifyVisibilityProperty(float fNewSize,float fNewStealth);
		void ModifyViewGroupProperty(uint32 uNewViewGroup);

		
		//查询在我的view rect里面的对象
		void QueryObserveeWithViewRect(VecDimObject_t& vecObject)const;
		void QueryObserveeWithViewRect(CSynAoiDimObj* pObj, VecDimObject_t& vecObject)const;
		//查询我在对方view rect里面的对象
		void QueryObserverWithSizeRect(VecDimObject_t& vecObject) const;
		void QueryObserverWithSizeRect(CSynAoiDimObj* pObj, VecDimObject_t& vecObject) const;

		//调用下列函数的时候，对象的位置已经在新的坐标上
		//由于是不对称的aoi了，所以我能看见的对象和能看见我的对象要分别计算
		void UpdateSeeRelationForMyEnterObject(const VecDimObject_t& vecObj, float NewEyeSight);
		void UpdateSeeRelationForMyLeaveObject(const VecDimObject_t& vecObj, const CFPos& pos, float OldEyeSight);
		void UpdateSeeRelationForMyStayObject(const CFPos& pos, float OldEyeSight, float NewEyeSight);
		void UpdateSeeRelationForOtEnterObject(const VecDimObject_t& vecObj, float NewSize);
		void UpdateSeeRelationForOtLeaveObject(const VecDimObject_t& vecObj, const CFPos& pos, float OldSize);
		void UpdateSeeRelationForOtStayObject(const CFPos& pos, float OldSize, float NewSize);
		
		void UpdateSeeRelationForMyEnterObject(const VecDimObject_t& vecObj);
		void UpdateSeeRelationForMyLeaveObject(const VecDimObject_t& vecObj,const CFPos& OldGridPos);
		void UpdateSeeRelationForMyStayObject(const CFPos& OldGridPos);

		void UpdateSeeRelationForOtEnterObject(const VecDimObject_t& vecObj);
		void UpdateSeeRelationForOtLeaveObject(const VecDimObject_t& vecObj,const CFPos& OldGridPos);
		void UpdateSeeRelationForOtStayObject(const CFPos& OldGridPos);

	private:
		CSynAoiViewObj*	m_pViewObj;
		CSynAoiDimScene*	m_pDimScene;

		CFRect m_rcViewRect;
		CFRect m_rcSizeRect;

#ifdef _WIN32
		typedef hash_set<CSynAoiDimObj*, hash_compare<CSynAoiDimObj*, less<CSynAoiDimObj*> >,
			TQuadAllocator<CSynAoiDimObj*> > ViewObjSet_t;
#else
		struct ViewObjHash
		{
			size_t operator() (const CSynAoiDimObj* pObj) const
			{
				size_t stPoint = (size_t)pObj;
				return stPoint;
			};
		};

		struct ViewObjEqual
		{
			bool operator() (const CSynAoiDimObj* pObj1, const CSynAoiDimObj* pObj2) const
			{
				return (size_t)pObj1 == (size_t)pObj2;
			}
		};

		typedef hash_set<CSynAoiDimObj*, ViewObjHash, ViewObjEqual, 
			TQuadAllocator<CSynAoiDimObj*> > ViewObjSet_t;
#endif	
		
		ViewObjSet_t		m_setObjInMyViewRect;		//在我的view rect里面的对象
		ViewObjSet_t		m_setObjInOtViewRect;		//我在对方view rect里面的对象

		CAoiQuadNode*	m_pObjectNode;
		CAoiQuadNode*	m_pViewRectNode;
		CAoiQuadNode*	m_pSizeRectNode;

		typedef list<CSynAoiDimObj*, TQuadAllocator<CSynAoiDimObj*> > ListQuadObject_t;
		ListQuadObject_t::iterator	m_ItObject;
		ListQuadObject_t::iterator	m_ItViewRect;
		ListQuadObject_t::iterator	m_ItSizeRect;
		
		static ListQuadObject_t	ms_DummyList;

		//由于每一个aoi object对应一个对象，所以对象的一些属性我们这里需要用到以作aoi 处理 
		float			m_fSize;		//体积
		float			m_fEyeSight;	//视力(视野范围)
		float			m_fStealth;		//隐形度
		float			m_fKeenness;	//敏锐度
		uint32			m_uViewGroup;	//观察组	观察组相同的两个Object忽略Stealth和Keenness属性

		CFPos			m_Pos;			
		uint32			m_uDimension;
	};

}
