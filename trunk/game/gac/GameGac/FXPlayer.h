#pragma once
#include "IModelHandler.h"
#include "CTick.h"
#include "CEffectUnitHandler.h"
#include "ActorDef.h"
#include "ISpaceNodeHandler.h"
#include "PatternRef.h"
#include "TFXAllocator.h"

namespace sqr
{
	class CRenderObject;
	class CRenderScene;
}

class CEntityClient;
class CMagicClient;

class CBaseFX :public CTick,public CFXMallocObject
{
public:
	static vector<string>  Split(const string& src, const string& delimit);
	static void FixFXPath(string& strPath);
	void ReplayFX();
	void PlayFX(const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime);
protected:
	CBaseFX();
	virtual ~CBaseFX();
	virtual void PlayEffect(const string &strFXNameGroup, const string &strFXName)=0;
	uint32 m_uMask;
private:
	virtual void OnTick();
	string m_sName;
};

class CPlayerFX: public CBaseFX
{
public:
	CPlayerFX(CEntityClient* pEntity);
	static void PlayOnceFX(CEntityClient* pSrcEntity, const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime=0);
	void PlayLoopFX(const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime=0);
	void CancelFx();
	void StopFX();
protected:
	virtual void PlayEffect(const string &strFXNameGroup, const string &strFXName);
	~CPlayerFX();
	bool m_bLoop;
	uint32 m_uEntityID;
};

class CLeadingFX: private CPlayerFX ,public CEffectUnitHandler
{
public:
	CLeadingFX(CEntityClient* pEntity);
	static void PlayOnceFX(CEntityClient* pSrcEntity, const string &strFXNameGroup, const string &strFXName, CVector3f& vec3f, uint32 uDelayTime=0);
	void PlayLoopFX(const string &strFXNameGroup, const string &strFXName, CVector3f& vec3f, uint32 uDelayTime=0);
	void PlayLoopFX(const string &strFXNameGroup, const string &strFXName, CEntityClient* pTarget, uint32 uDelayTime=0);
	void AddLeadingPos(CVector3f& vec3f);
	void CancelFx();
	void StopFX();

	static void *operator new( size_t stAllocateBlock);
	static void* operator new( size_t , void* ptr);
	static void *operator new[]( size_t stAllocateBlock);
	static void operator delete(void* pMem);
	static void operator delete(void* , void* );
	static void operator delete[](void* pMem);

private:
	virtual void PlayEffect(const string &strFXNameGroup, const string &strFXName);
	bool RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS );
	EMap<float,CVector3f> m_mapDest;
	uint32	m_uTargetID;
};

class CPositionFX: public CBaseFX, public IModelHandler, public ISpaceNodeHandler
{
public:
	static void CppPlayPosFX(const TCHAR* strFXNameGroup, const TCHAR* strFXName, const CFPos& PixelPos);
	static void PlayOnceFX(const string &strFXNameGroup, const string &strFXName, const CFPos& PixelPos, uint32 uDelayTime=0);
private:
	CPositionFX(CRenderScene* pRenderScene,const CFPos& PixelPos);
	~CPositionFX();
	void OnDestroyed();
	void OnLinkAnimationEnd(const TCHAR * szName);
	virtual void PlayEffect(const string &strFXNameGroup, const string &strFXName);
	CRenderObject*	m_pPosRenderObj;
	CFPos				m_Pos;
};

class CMagicFX: public CBaseFX , public ISpaceNodeHandler
{
public:
	CMagicFX();
	void PlayLoopFX(const string &strFXNameGroup, const string &strFXName, uint32 uDelayTime=0);
	void CancelFx();
	virtual void PlayEffect(const string &strFXNameGroup, const string &strFXName);
	typedef TPtRefee<CMagicFX,CMagicClient> RefByMagic;
	RefByMagic& GetRefByMagic(){return m_RefByMagic;}
	CRenderObject* GetRenderObj()const {return m_pRenderObj;}
private:
	void OnDestroyed();
	~CMagicFX();
	RefByMagic m_RefByMagic;
	CRenderObject* m_pRenderObj;
};

class CTargetDirectionFX: public CEffectUnitHandler
{
public:
	static void CppPlayTargetDirFX(const TCHAR* strFXNameGroup, const TCHAR* strFXName,const CFPos& PixelPos);
	static void CppCancelTargetDirFX();
private:
	CTargetDirectionFX(const string &strFXNameGroup, const string &strFXName,const CFPos& PixelPos);
	~CTargetDirectionFX();
	bool RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS );
	uint32 m_uMask;
	CFPos				m_Pos;
	static CTargetDirectionFX* ms_pTargetDirectionFX;
};


class CLineDirectionFX :public CEffectUnitHandler
{
public:
	static void PlayOnceFX(CEntityClient* pSrcEntity, const string &strFXNameGroup, const string &strFXName, uint32 uTargetEntityID);
private:
	CLineDirectionFX(CEntityClient* pEntity, uint32 uTargetEntityID);
	void PlayEffect(const string &strFXNameGroup, const string &strFXName);
	bool RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS );
	uint32	m_uEntityID;
	uint32	m_uTargetEntityID;
};