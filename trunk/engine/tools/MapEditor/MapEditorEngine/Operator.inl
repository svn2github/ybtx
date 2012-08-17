#include <Windows.h>
#include <atlconv.h>	// º”»Îatlsd.lib

inline void COperator::MoveHitPoint( CVector3f v )
{
	m_vHitPoint.x += v.x;
	m_vHitPoint.y += v.y;
	m_vHitPoint.z += v.z;
}

inline void COperator::MoveHitPoint(float x, float y, float z)
{
	m_vHitPoint.x += x;
	m_vHitPoint.y += y;
	m_vHitPoint.z += z;
}

inline void COperator::SetHitPoint(CVector3f v)
{
	m_vHitPoint = v;
}

inline CVector3f COperator::GetHitPoint()
{
	return m_vHitPoint;
}

inline void COperator::SetIsPreViewModel(bool b)
{
	m_bPreViewModel = b;
}

inline bool COperator::IsPreViewModel()
{
	return m_bPreViewModel;
}

inline bool COperator::GetDoesShowEffectBase() const 
{
	return m_bShowEffectBase;
}

inline void COperator::SetDoesShowEffectBase(bool b)
{
	m_bShowEffectBase = b;
}

inline bool COperator::GetDoesRgnAmbientFxBase() const
{
	return m_bRgnAmbientFxBase;
}

inline void COperator::SetDoesRgnAmbientFxBase(bool b)
{
	m_bRgnAmbientFxBase = b;
}

inline int COperator::GetGridIndexHot()
{
	return m_processParams.dwGridIndexHot;
}

inline void COperator::SetGridIndexHot( DWORD dw )
{
	m_processParams.dwGridIndexHot = dw;
}

inline DWORD COperator::GetVertexIndexHot()
{
	return m_processParams.dwVertexIndexHot;
}

inline void COperator::SetVertexIndexHot( DWORD dw )
{
	m_processParams.dwVertexIndexHot = dw;
}

inline void COperator::AddIndividualEffectObject( CTObject * p )
{
	m_vecIndividualEffectObjects.push_back(p);
}

inline void COperator::RemoveIndividualEffectObject(CTObject * p)
{
	m_vecIndividualEffectObjects.erase(
		remove(m_vecIndividualEffectObjects.begin(), m_vecIndividualEffectObjects.end(), p), m_vecIndividualEffectObjects.end() );
}

inline void COperator::AddAmbientObject( CTObject * p )
{
	m_vecAmbientObjectsPairs.push_back(make_pair(p,true));
}

inline void COperator::SetTitleActiveObject( CTObject *p )
{
	m_pTitleObject = p;
}

inline CTObject * COperator::GetTitleActiveObject()
{
	return m_pTitleObject;
}

inline CTObject * COperator::GetObjectHot()
{
	return m_processParams.pObjectHot.get_unsafe();
}

inline void COperator::SetRebirthResult( bool bRebirthResult )
{
	m_bRebirthResult = bRebirthResult;
}

inline bool COperator::GetRebirthResult()
{
	return m_bRebirthResult;
}

inline void COperator::SetCreateDyanmicLight( bool b )
{
	m_bCreateDynamicLight = b;
}

inline bool COperator::GetCreateDynamicLight()
{
	return m_bCreateDynamicLight;
}

inline void COperator::SetRebirthEffectIsSuc( bool b )
{
	m_bRebirthEffectSuc = b;
}

inline bool COperator::GetRebirthEffectIsSuc()
{
	return m_bRebirthEffectSuc;
}


inline bool COperator::IsReceiveShadow() const
{
	return m_bReceiveShadow;
}

inline void COperator::SetReceiveShadow( const bool bReceiveShadow )
{
	m_bReceiveShadow = bReceiveShadow;
}

inline void COperator::SetCurIsSetSpotLight( const bool bSetSpotLight )
{
	m_bCurSetSpotLight = bSetSpotLight;
}

inline bool COperator::GetCurIsSetSpotLight() const
{
	return m_bCurSetSpotLight;
}

inline void COperator::SetIsMoveSpotLightTargetPos( const bool bMove )
{
	m_bMoveSpotLightTargetPos = bMove;
}

inline bool COperator::GetIsMoveSpotLightTargetPos() const
{
	return m_bMoveSpotLightTargetPos;
}

inline void COperator::SetShowSpecialGridCueMat(const bool b)
{
	m_bShowSpecialGridCueMat = b;
}

inline bool COperator::GetShowSpecialGridCueMat() const
{
	return m_bShowSpecialGridCueMat;
}

inline void COperator::SetLightEffectTerrain( const bool b )
{
	m_bSceneLightEffectTerrain = b;
}

inline bool COperator::GetLightEffectTerrain() const
{
	return m_bSceneLightEffectTerrain;
}

//------------------------------------------------------------------------------
inline void COperator::SetLastAutoSaveTime( DWORD time)
{
	m_dwLastAutoSaveTime = time;
}

//------------------------------------------------------------------------------
inline DWORD COperator::GetLastAutoSaveTime() const
{
	return m_dwLastAutoSaveTime;
}

// -----------------------------------------------------------------------------
inline void 
COperator::SetIsResizeMap( bool b )
{
	m_bReSizeMap = b;
}

// -----------------------------------------------------------------------------
inline bool 
COperator::GetIsResizeMap() const
{
	return m_bReSizeMap;
}

// -----------------------------------------------------------------------------
inline bool 
COperator::GetIsUseSpecular() const
{
	return m_bUseSpecular;
}

// -----------------------------------------------------------------------------
inline void 
COperator::SetIsSetVertexDiffuseColor(bool b)
{
	m_bSetVertexDiffuseColor = b;
}

// -----------------------------------------------------------------------------
inline bool 
COperator::GetIsSetVertexDiffuseColor() const
{
	return m_bSetVertexDiffuseColor;
}

// -----------------------------------------------------------------------------
inline void 
COperator::SetIsPreviewTerrainBaking(bool b)
{
	m_bPreviewTerrainBaking = b;
}

// -----------------------------------------------------------------------------
inline bool 
COperator::GetIsPreviewTerrainBaking() const
{
	return m_bPreviewTerrainBaking;
}


inline void	COperator::SetShowWireBox(bool b)
{
	m_bShowWireBox = b;
}

inline bool	COperator::GetShowWireBox(void)
{
	return m_bShowWireBox;
}

