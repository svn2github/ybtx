inline void CSceneEnvMgr::SetSkyTextureName(std::string name)
{
	m_envs.m_strTextureSkyName = name;
}

inline std::string CSceneEnvMgr::GetSkyTextureName() const
{
	return m_envs.m_strTextureSkyName;
}

inline void  CSceneEnvMgr::SetIsBackPicMove( const bool& bset )
{
	m_bSetBackPicMove = bset;
}

inline bool  CSceneEnvMgr::GetISBackPicMove() const
{
	return m_bSetBackPicMove;
}

inline void  CSceneEnvMgr::SetBackPicLeftMoveOffset( const float& offset )
{
	m_fBackLeftMove = offset;
}

inline float CSceneEnvMgr::GetBackPicLeftMoveOffset() const
{
	return m_fBackLeftMove;
}

inline void  CSceneEnvMgr::SetBackPicRightMoveOffset( const float& offset )
{
	m_fBackRightMove = offset;
}

inline float  CSceneEnvMgr::GetBackPicRightMoveOffset() const
{
	return m_fBackRightMove;
}

inline void CSceneEnvMgr::SetBackTextureUVCenter( const CVector2f& center )
{
	m_UVCenter = center;
}

inline void CSceneEnvMgr::SetBackTextureUCenter( const float u )
{
	m_UVCenter.x = u;
}

inline void CSceneEnvMgr::SetBackTextureVCenter( const float v )
{
	m_UVCenter.y = v;
}

inline CVector2f CSceneEnvMgr::GetBackTextureUVCenter() const
{
	return m_UVCenter;
}

inline void CSceneEnvMgr::AdjustBackTextureUVCenter()
{
	m_UVCenter = CVector2f(0.5f, 0.5f);
}

inline void CSceneEnvMgr::SetBackTextureScale( const int scale )
{
	m_fSkyBoxScale = scale * 0.01f;
}

inline float CSceneEnvMgr::GetBackTextureScale() const
{
	return m_fSkyBoxScale;
}

inline void  CSceneEnvMgr::SetBackPicTopMoveOffset( const float& offset )
{
	m_fBackTopMove = offset;
}

inline float CSceneEnvMgr::GetBackPicTopMoveOffset() const
{
	return m_fBackTopMove;
}

inline void  CSceneEnvMgr::SetBackPicBottonMoveOffset( const float& offset )
{
	m_fBackBottonMove = offset;
}

inline float CSceneEnvMgr::GetBackPicBottonMoveOffset() const
{
	return m_fBackBottonMove;
}

inline string CSceneEnvMgr::GetSkyFarBackTextureName()
{
	return m_strSkyboxTexture;
}

inline DWORD CSceneEnvMgr::GetSceneAmbientColor() const
{
	return m_envs.m_dwSceneAmbient;
}

inline DWORD CSceneEnvMgr::GetPlayerAmbientColor() const
{
	return m_envs.m_dwPlayerAmbientColor;
}

inline DWORD CSceneEnvMgr::GetSceneDirColor() const
{
	return m_envs.m_dwSceneDirColor;
}

inline DWORD CSceneEnvMgr::GetPlayerDirColor() const
{
	return m_envs.m_dwPlayerDirColor;
}

inline CVector3f CSceneEnvMgr::GetDirDir() const
{
	return  m_envs.m_vDirDir;
}

inline void CSceneEnvMgr::SetDirDir(const CVector3f v)
{
	m_envs.m_vDirDir = v;
}

inline DWORD CSceneEnvMgr::GetShadowColor() const
{
	return m_envs.m_dwShadowColor;
}

inline DWORD CSceneEnvMgr::GetSceneSpecurlarColor() const
{
	return m_envs.m_dwSceneSpecularColor;
}

inline DWORD CSceneEnvMgr::GetPlayerSpecurlarColor() const
{
	return m_envs.m_dwPlayerSpecularColor;
}

inline float CSceneEnvMgr::GetSpecularReflectionPower(const int index) const
{
	return m_fSReflectionPower[index];
}

inline bool	CSceneEnvMgr::GetDoesEnableFog() const
{
	return m_bEnableFog;
}

inline void CSceneEnvMgr::SetVertexFog(FogModeType mode)
{
	m_fogMode = mode;
}

inline CSceneEnvMgr::FogModeType CSceneEnvMgr::GetFogMode() const
{
	return m_fogMode;
}

inline DWORD CSceneEnvMgr::GetFogColor() const
{
	return m_envs.m_dwFogColor;
}

inline float CSceneEnvMgr::GetFogStart() const
{
	return m_envs.m_fFogStart;
}

inline float CSceneEnvMgr::GetFogEnd() const
{
	return m_envs.m_fFogEnd;
}

inline float CSceneEnvMgr::GetDensity() const
{
	return m_envs.m_fFogDensity;
}

inline void CSceneEnvMgr::RgnReSetColorInfo( DWORD ambient, DWORD diffuse, DWORD fog )
{
	m_envs.m_dwSceneAmbient  = ambient;
	m_envs.m_dwSceneDirColor = diffuse;
	m_envs.m_dwFogColor		 = fog;
}

inline void CSceneEnvMgr::RgnReSetFogParam( const int nFogMode, const float start, const float end, const float density )
{
	m_fogMode = (FogModeType)nFogMode;

	m_envs.m_fFogStart   = start;
	m_envs.m_fFogEnd	  = end;
	m_envs.m_fFogDensity = density;
}

inline void CSceneEnvMgr::RgnReSetSpecularColorInfo( DWORD sceneSpecular, DWORD playerSpecular, float fPower )
{
	m_envs.m_dwSceneSpecularColor  = sceneSpecular;
	m_envs.m_dwPlayerSpecularColor = playerSpecular;
	m_fSReflectionPower[0]         = fPower;
}

inline void CSceneEnvMgr::RgnReSetPlayerColor( DWORD ambient, DWORD diffuse )
{
	m_envs.m_dwPlayerAmbientColor = ambient;
	m_envs.m_dwPlayerDirColor = diffuse;
}

inline string & CSceneEnvMgr::GetBackGroundMusic() 
{
	return m_strBackgroundMusic;
}

inline void CSceneEnvMgr::SetIsPlaySceneCues( const bool b )
{
	m_bIsPlaySceneCues = b;
}

inline bool CSceneEnvMgr::GetIsPlaySceneCues() const
{
	return m_bIsPlaySceneCues;
}

inline void CSceneEnvMgr::SetIsGenerateMinMap( const bool b )
{
	m_bGenerateMinMap = b;
}

inline bool CSceneEnvMgr::GetIsGenerateMinMap() const
{
	return m_bGenerateMinMap;
}
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
inline void CSceneEnvMgr::SetLightMapZOffset(const float zOffset)
{
	m_fLightMapZOffset = zOffset;
}

// -----------------------------------------------------------------------------
inline float CSceneEnvMgr::GetLightMapZOffset() const
{
	return m_fLightMapZOffset;
}

inline void CSceneEnvMgr::SetIsLightMapShadow(bool b)
{
	m_bLightMapShadow = b;
}

inline bool CSceneEnvMgr::GetIsLightMapShadow() const 
{
	return m_bLightMapShadow;
}

inline void CSceneEnvMgr::SetIsShowScreenRangeGrid(bool b)
{
	m_bSetLightByScreenRange = b;
}

inline bool CSceneEnvMgr::GetIsShowScreenRangeGrid() const
{
	return m_bSetLightByScreenRange;
}

inline bool CSceneEnvMgr::HadBloomTarget() 
{
	return m_pTarget != NULL;
}


inline float CSceneEnvMgr::GetGaussBrightPass() const
{
	return m_fGaussBrightPass;
}


inline float CSceneEnvMgr::GetGaussMultiplier() const
{
	return m_fGaussMultiplier;
}

inline float CSceneEnvMgr::GetGaussMean() const
{
	return m_fGaussMean;
}

inline float CSceneEnvMgr::GetGaussStdDev() const
{
	return m_fGaussStdDev;
}

inline float CSceneEnvMgr::GetGaussExposure() const
{
	return m_fGaussExposure;
}

inline float CSceneEnvMgr::GetGaussSample() const
{
	return m_fGaussSam;
}

inline bool CSceneEnvMgr::GetIsTurnOnGaussTarget() const
{
	return m_bTurnOnGassTarget;
}


