inline void CCharacter::SetName( const TCHAR* name )
{
	m_sName = name;
}


inline string CCharacter::GetName()
{
	return m_sName;
}

inline void CCharacter::SetScaling( float scaling )
{
	m_fScaling = scaling;
}

inline float CCharacter::GetScaling()
{
	return m_fScaling;
}

inline void CCharacter::SetAniFileName( const TCHAR* name )
{
	m_sBAniFileName = name;
}

inline string CCharacter::GetAniFileName()
{
	return m_sBAniFileName;
}

inline void CCharacter::SetModPiece( CModelPiece* pModelPiece )
{
	m_BodyModelPiece = pModelPiece;
}


inline void CModelPiece::SetIndexName( const TCHAR* name )
{
	m_sIndexName = name;
}

inline string CModelPiece::GetIndexName()
{
	return m_sIndexName;
}

inline void CModelPiece::SetModelFileName( const TCHAR* name )
{
	m_sModelFile = name;
}

inline string CModelPiece::GetModelFileName()
{
	return m_sModelFile;
}

inline void CModelPiece::SetPieceName( const TCHAR* pieceName )
{
	if(pieceName)
		m_sPieceName = pieceName;
}

inline string CModelPiece::GetPieceName()
{
	return m_sPieceName;
}

inline void CModelPiece::SetRenderStyle( const TCHAR* style )
{
	m_sRenderStyle = style;
}

inline string CModelPiece::GetRenderStyle()
{
	return m_sRenderStyle;
}

inline void CPlayerModelPart::SetName( const TCHAR* name )
{
	m_strName = name;
}

inline string CPlayerModelPart::GetName()
{
	return m_strName;
}

inline string CPlayerModelPart::GetModelFileName()
{
	return m_strModelFile;
}

inline void CPlayerModelPart::SetAniFileName( const TCHAR* name )
{
	m_strAniFile = name;
}

inline string CPlayerModelPart::GetAniFileName()
{
	return m_strAniFile;
}

inline void CPlayerModelPart::SetEffectFileName( const TCHAR* name )
{
	m_strEffectFile = name;
}

inline string CPlayerModelPart::GetEffectFileName()
{
	return m_strEffectFile;
}

inline void CPlayerModelPart::SetPieceName( const TCHAR* pieceName )
{
	if(pieceName)
		m_strPieceName = pieceName;
}

inline string CPlayerModelPart::GetPieceName()
{
	return m_strPieceName;
}

inline void CPlayerModelPart::SetRenderStyle( const TCHAR* style )
{
	m_strRenderStyle = style;
}

inline string CPlayerModelPart::GetRenderStyle()
{
	return m_strRenderStyle;
}

inline void CPlayerModelPart::SetHumanMaleHidePiece( const TCHAR* name )
{
	m_strHumanMaleHidePiece = name;
}

inline string CPlayerModelPart::GetHumanMaleHidePiece()
{
	return m_strHumanMaleHidePiece;
}

inline void CPlayerModelPart::SetHumanFeMaleHidePiece( const TCHAR* name )
{
	m_strHumanFeMaleHidePiece = name;
}

inline string CPlayerModelPart::GetHumanFeMaleHidePiece()
{
	return m_strHumanFeMaleHidePiece;
}

inline void CPlayerModelPart::SetDwarfHidePiece( const TCHAR* name )
{
	m_strDwarfHidePiece = name;
}

inline string CPlayerModelPart::GetDwarfHidePiece()
{
	return m_strDwarfHidePiece;
}

inline void CPlayerModelPart::SetElfHidePiece( const TCHAR* name )
{
	m_strElfHidePiece = name;
}

inline string CPlayerModelPart::GetElfHidePiece()
{
	return m_strElfHidePiece;
}

inline void CPlayerModelPart::SetOrcHidePiece( const TCHAR* name )
{
	m_strOrcHidePiece = name;
}

inline string CPlayerModelPart::GetOrcHidePiece()
{
	return m_strOrcHidePiece;
}

inline void CPlayerModelPart::SetDynamic( const TCHAR* name )
{
	m_strDynamic = name;
}

inline string CPlayerModelPart::GetDynamic()
{
	return m_strDynamic;
}

inline void CPlayerModelPart::SetTransform( const TCHAR* name )
{
	m_strTransform = name;
}

inline string CPlayerModelPart::GetTransform()
{
	return m_strTransform;
}