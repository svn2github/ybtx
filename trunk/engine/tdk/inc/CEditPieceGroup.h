#pragma once
#include "CEditorClassPre.h"
#include "CPieceGroup.h"
#include "CEditSubject.h"
#include "CEditMesh.h"
#include "CEditInfo.h"
#include "CEditModFormat.h"

namespace sqr_tools
{
	class CEditPieceRenderStyle 
		: public CPieceRenderStyle
		, public CEditSubject 
	{
	public:
		string m_Path;
		CEditPieceRenderStyle() : m_pPiece(0) {};
		CEditPieceRenderStyle( CEditPiece* pPiece ) : m_pPiece(pPiece) {};
		CEditPieceRenderStyle( CEditPiece* pPiece, CEditRenderStyle* pRenderStyle );
		void	SetRSIndexName( const string& rsIndex ) { m_RSIndexName = rsIndex.c_str(); }
		void	SetRSName(const string& rsName) {m_RenderStyleName = rsName.c_str();}
		void	SetRenderStyle( CEditRenderStyle* pRenderStyle );
		void	SetPath(const string& Name){m_Path = Name.c_str();}
		void	ReLoad(const string& filename);
		void	PushTexture( ITexture* pText );
		int		BinSave( ofstream& File );
		CEditRenderStyle*	GetEditRenderStyle(void);
		void				SetParentPiece( CEditPiece* pPiece );
		CEditPiece*			GetParentPiece();
		void	FreeTextureNames();
	
	protected:
		HRESULT _CreateRenderStyle( const TCHAR* szName, CRenderStyle** pRS );
		HRESULT	_CreateStyleTexture( const TCHAR* szTexName );
		
	private:
		CEditPiece* m_pPiece;
	};

	class CEditPiece 
		: public sqr::CPiece
		, public CEditSubject 
	{
	public:
		CEditPiece(CPieceGroup* pGroup)
			: CPiece( pGroup ),m_pForceStyle(NULL),m_bMeshDirty(FALSE)
		{
		}

		string		m_Path;
		BOOL		m_bMeshDirty;

		void InsertRenderStyle(int S,int D);
		void DeleteRS( int i );
		void SetPieceName( const string& Name );
		void SetPath(const string& Name);
		void SetForceRenderStyle(CEditRenderStyle* pStyle);
		void BinSave( ofstream& File );
		bool DecideIsSet(void);
		void ChangeRsByName(const string& rsName,CEditPieceRenderStyle* pRs);
		void AddRS( CEditPieceRenderStyle* rs );
		void SetAABoxExtends( const CVector3f& min, const CVector3f& max );

		CEditMeshWriter* GetMeshWriter();

	protected:		
		CEditPieceRenderStyle*	m_pForceStyle;
		CPieceRenderStyle*  _GetRenderStyle(uint32 Num);
		CPieceRenderStyle*	_NewRenderStyle(void);
		//CMesh*				_NewVertexAniMesh(void);
		//CMesh*				_NewStaticMesh(void);
		//CMesh*				_NewSkeletalMesh(void);
		void				_SetPieceName( const string& strPieceName );
		
	};

	class CEditPieceGroup 
		: public sqr::CPieceGroup
		, public CEditSubject 
		, public CEditInfo
	{
		friend CEditModel;
	public:
		static CEditModFormat Format;
		//static CModFormat Format;

		string			m_TexPath;
		CEditPieceGroup(const string& szName = "", const wstring& Alias = L"");

		int				ReLoad( const string& filename );
		int				BinSave( const string& filename );
		CPieceClass*	GetPieceClass( int index );
		GVector< CPieceClass >& GetPieceClass();
		void			DelPieceClass( int index );
		DWORD			GetVersion(void);
		void			SetPath(const string& Name);
		bool			Check();
		void			SetPortraitInfo( PortraitInfo* pi );
		void			Clear();

	protected:
		DWORD			m_dwVersion;
		CPiece*			_NewPiece(void);

	private:
		string			m_str_sample;
		int				m_int_sample;

		friend class CEditModCoder;
	};
}
