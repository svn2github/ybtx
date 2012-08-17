#pragma once
#include "CEditorClassPre.h"

namespace sqr_tools
{
	struct EdgeCollapseParam
	{
		UINT m_TrisNum;
		UINT m_Tris[2];		
		UINT m_Verts[2];	
	};

	struct LODVer
	{
		CVector3f	m_Pos;
		CVector3f	m_Nor;

		DWORD		m_Dif;			
		CVector2f	m_UV;		
		CTinyWeight	m_Wgh;
		SkltIndex	m_SklInd;

		BOOL		m_IsValid;
		int			m_VerPos;
	};

	struct LODEdg 
	{
		LODEdg():m_Length(0){ m_Ver[0] = 0; m_Ver[1] = 0; }
		LODEdg( float Len, WORD Tri1, WORD Tri2 ){ m_Length = Len; m_Ver[0] = Tri1; m_Ver[1] = Tri2; };
		float m_Length;
		WORD  m_Ver[2];
	};

	inline bool operator<( const LODEdg& arg1, const LODEdg& arg2 )
	{
		return ( arg1.m_Ver[0] != arg2.m_Ver[0] || arg1.m_Ver[1] != arg2.m_Ver[1] ) && 
			( arg1.m_Ver[0] != arg2.m_Ver[1] || arg1.m_Ver[1] != arg2.m_Ver[0] ) && 
			arg1.m_Length < arg2.m_Length ;
	}

	inline bool operator>( const LODEdg& arg1, const LODEdg& arg2 )
	{
		return ( arg1.m_Ver[0] != arg2.m_Ver[0] || arg1.m_Ver[1] != arg2.m_Ver[1] ) && 
			( arg1.m_Ver[0] != arg2.m_Ver[1] || arg1.m_Ver[1] != arg2.m_Ver[0] ) && 
			arg1.m_Length > arg2.m_Length ;
	}

	struct LODTri
	{
		WORD		m_VerInd[3];
		WORD		m_EdgInd[3];
		BOOL		m_IsValid;
	};

	class CLODBuilder
	{
		CMesh*	m_Mesh;
		float	m_Dist;
		float	m_FacePercent;
		float	m_DeleteMaxLen;
		int		m_MinTris;

		vector<LODTri> m_Tris;
		vector<LODVer> m_Vers;

		BOOL Init();
		BOOL FindShortestEdge( EdgeCollapseParam& EgCllParam );
		UINT GetEdgeShareNum( UINT ver0, UINT ver1, UINT* tris );
		void GenNewVerToFirstVer(  UINT ver0, UINT ver1 );

	public:
		CLODBuilder( CMesh* Mesh, float Dist, float FacePercent, float DeleteMaxLen, int MinTris )
		{
			m_Mesh = Mesh;
			m_Dist = Dist;
			m_FacePercent = FacePercent;
			m_DeleteMaxLen = DeleteMaxLen;
			m_MinTris = MinTris;
		}

		CMeshLevel* Build();
	};
}