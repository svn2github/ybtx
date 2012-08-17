#include "stdafx.h"
#include "CMesh.h"
#include "CLODBuilder.h"

BOOL CLODBuilder::Init()
{
#if 0
	if( !m_Mesh || m_Mesh->GetLevel().size() == 0 || m_Mesh->GetVertexType() == VT_SOFTBODYANI )
		return FALSE;

	GVector<CMeshLevel>& MeshLevel = m_Mesh->GetMeshLevel();
	CModelLevel* OrgLevel = ( CModelLevel* )MeshLevel[0];

	if(	OrgLevel->m_VerAnimate.size() != 1|| 
		OrgLevel->m_VerAnimate[0].m_FramesVertex.size() != 1 )
		return FALSE;


	int i;
	m_Vers.resize( OrgLevel->m_VerAnimate[0].m_FramesVertex[0].size() );
	for( i = 0; i<(int)m_Vers.size(); i++ )
	{
		m_Vers[i].m_Pos = OrgLevel->m_VerAnimate[0].m_FramesVertex[0][i];
		m_Vers[i].m_Nor = OrgLevel->m_VerAnimate[0].m_FramesNormal[0][i];

		if( OrgLevel->m_TextCoord.size() )
			m_Vers[i].m_UV = OrgLevel->m_TextCoord[i];

		if( OrgLevel->m_Weight.size() )
			m_Vers[i].m_Wgh = OrgLevel->m_Weight[i];
		if( OrgLevel->m_SkeletalIndex.size() )
			m_Vers[i].m_SklInd = OrgLevel->m_SkeletalIndex[i];

		m_Vers[i].m_IsValid = TRUE;
		m_Vers[i].m_VerPos = i;
	}

	m_Tris.resize( OrgLevel->m_VertexIndex.size()/3 );
	for( i = 0; i<(int)m_Tris.size(); i++ )
	{
		m_Tris[i].m_VerInd[0] = OrgLevel->m_VertexIndex[ i*3 + 0 ];
		m_Tris[i].m_VerInd[1] = OrgLevel->m_VertexIndex[ i*3 + 1 ];
		m_Tris[i].m_VerInd[2] = OrgLevel->m_VertexIndex[ i*3 + 2 ];
		m_Tris[i].m_IsValid = TRUE;
	}
#endif
	return TRUE;
}

CMeshLevel* CLODBuilder::Build()
{
#if 0
	if( !Init() )
		return NULL;

	EdgeCollapseParam EgCllp;

	int TrisSize = (int)m_Tris.size();
	int DeleteTriNum = min( (int)( ( 1 - m_FacePercent )*TrisSize ), TrisSize - m_MinTris );
	for( int n = 0; n < DeleteTriNum; )
	{
		//找出最短边
		if( !FindShortestEdge( EgCllp ) )
			return NULL;
		n += EgCllp.m_TrisNum;

		m_Vers[ EgCllp.m_Verts[1] ].m_IsValid = FALSE;
		D3DXVECTOR3 Ver0 = m_Vers[ EgCllp.m_Verts[0] ].m_Pos;
		D3DXVECTOR3 Ver1 = m_Vers[ EgCllp.m_Verts[1] ].m_Pos;
		GenNewVerToFirstVer( EgCllp.m_Verts[0], EgCllp.m_Verts[1] );

		for( int ver = 0; ver < (int)m_Vers.size(); ver++ )
		{
			if( m_Vers[ver].m_Pos == Ver0 || m_Vers[ver].m_Pos == Ver1 )
				m_Vers[ver].m_Pos = m_Vers[ EgCllp.m_Verts[0] ].m_Pos;
		}

		for( UINT i = 0; i < EgCllp.m_TrisNum; i++ )
			m_Tris[EgCllp.m_Tris[i]].m_IsValid = FALSE;

		for( UINT curTri = 0; curTri < (int)m_Tris.size(); curTri++ )
		{
			if( !m_Tris[curTri].m_IsValid )
				continue;
			for( UINT curVert = 0; curVert < 3; curVert++ )
			{
				if( m_Tris[curTri].m_VerInd[curVert] == EgCllp.m_Verts[1] )
					m_Tris[curTri].m_VerInd[curVert] = EgCllp.m_Verts[0];
			}
		}
	}

	GVector<CMeshLevel*>& MeshLevel = m_Mesh->GetLevel();
	CModelLevel* OrgLevel = (CModelLevel*)MeshLevel[0];
	CModelLevel* NewLevel = new CModelLevel( m_Mesh );
	NewLevel->m_Dist = m_Dist;
	NewLevel->m_VerAnimate.resize( 1 );
	NewLevel->m_VerAnimate[0].m_FramesVertex.resize( 1 );
	NewLevel->m_VerAnimate[0].m_FramesNormal.resize( 1 );
	int curVert = 0;
	for( ;curVert<(int)m_Vers.size(); curVert++ )
	{
		if( !m_Vers[curVert].m_IsValid )
			continue;
		m_Vers[curVert].m_VerPos = (int)NewLevel->m_VerAnimate[0].m_FramesVertex[0].size();
		NewLevel->m_VerAnimate[0].m_FramesVertex[0].push_back( m_Vers[curVert].m_Pos );
		NewLevel->m_VerAnimate[0].m_FramesNormal[0].push_back( m_Vers[curVert].m_Nor );

		if( OrgLevel->m_TextCoord.size() )
			NewLevel->m_TextCoord.push_back( m_Vers[curVert].m_UV );

		if( OrgLevel->m_Weight.size() )
			NewLevel->m_Weight.push_back( m_Vers[curVert].m_Wgh );
		if( OrgLevel->m_SkeletalIndex.size() )
			NewLevel->m_SkeletalIndex.push_back( m_Vers[curVert].m_SklInd );
	}

	for( UINT curTri = 0; curTri < (int)m_Tris.size(); curTri++ )
	{
		if( !m_Tris[curTri].m_IsValid )
			continue;
		for( curVert = 0; curVert < 3; curVert++ )
			NewLevel->m_VertexIndex.push_back( m_Vers[ m_Tris[curTri].m_VerInd[curVert] ].m_VerPos );
	}

	return NewLevel;
#endif
	return NULL;
}


BOOL CLODBuilder::FindShortestEdge( EdgeCollapseParam& edgeCollapse )
{
#if 0
	float shortestEdge = FLT_MAX;
	bool foundEdge = false;

	UINT curTri,curVert;
	GMap< LODEdg, pair<WORD,WORD> > Edgs;

	for( curTri = 0; curTri < (int)m_Tris.size(); curTri++ )
	{
		//忽略已经被删除的面
		if( !m_Tris[curTri].m_IsValid )
			continue;

		//遍历边
		for( curVert = 0; curVert < 3; curVert++ )
		{
			UINT triVert = m_Tris[curTri].m_VerInd[curVert];
			UINT triNextVert = m_Tris[curTri].m_VerInd[( curVert + 1 )%3 ];
			float testLen = D3DXVec3Length( &( m_Vers[triVert].m_Pos - m_Vers[triNextVert].m_Pos ) );

			LODEdg edg( testLen, triVert, triNextVert );
			GMap< LODEdg, pair<WORD,WORD> >::iterator it = Edgs.find( edg );
			if( it == Edgs.end() )
				Edgs[ edg ] = pair<WORD,WORD>( curTri, 0xffff );
			else
			{
				pair<WORD,WORD>& c = Edgs[edg];
				if( c.first != 0xffff || c.second != 0xffff )
				{
					if( c.second != 0xffff || curTri == c.first )
						c = pair<WORD,WORD>( 0xffff, 0xffff );
					else
						c = pair<WORD,WORD>( c.first, curTri );
				}
			}
		}
	}

	for( GMap< LODEdg, pair<WORD,WORD> >::iterator it = Edgs.begin(); it != Edgs.end(); it++ )
	{
		const LODEdg& edg = it->first;
		if( it->second.first != 0xffff || it->second.second != 0xffff )
		{
			shortestEdge = edg.m_Length;
			edgeCollapse.m_Verts[0] = edg.m_Ver[0];
			edgeCollapse.m_Verts[1] = edg.m_Ver[1];
			edgeCollapse.m_Tris[0] = it->second.first;
			edgeCollapse.m_Tris[1] = it->second.second;
			edgeCollapse.m_TrisNum = it->second.second == 0xffff ? 1 : 2;
			return 1;
		}
	}
#endif
	return 0;
	/*
	//遍历所有面
	for( curTri = 0; curTri < (int)m_Tris.size(); curTri++ )
	{
	//忽略已经被删除的面
	if( !m_Tris[curTri].m_IsValid )
	continue;

	//遍历边
	for( curVert = 0; curVert < 3; curVert++ )
	{
	//得到边长
	UINT triVert = m_Tris[curTri].m_VerInd[curVert];
	UINT triNextVert = m_Tris[curTri].m_VerInd[( curVert + 1 )%3 ];
	float testLen = D3DXVec3Length( &( m_Vers[triVert].m_Pos - m_Vers[triNextVert].m_Pos ) );

	//得到共享边的面数
	UINT testTris[2];
	int EdgeShareNum = GetEdgeShareNum( triVert, triNextVert, testTris );
	if( ( testLen < shortestEdge ) && ( testLen > 0.0f ) && EdgeShareNum )
	{
	// 如果超过删除最大边长，忽略
	if( testLen >= m_DeleteMaxLen )
	continue;

	// 填充可能要删除的边的相关参数
	shortestEdge = testLen;
	edgeCollapse.m_Verts[0] = triVert;
	edgeCollapse.m_Verts[1] = triNextVert;
	edgeCollapse.m_Tris[0] = testTris[0];
	edgeCollapse.m_Tris[1] = testTris[1];
	edgeCollapse.m_TrisNum = EdgeShareNum;
	foundEdge = true;
	}
	}
	}

	return foundEdge;*/
}

UINT CLODBuilder::GetEdgeShareNum( UINT ver0, UINT ver1, UINT* tris )
{
#if 0
	UINT numTrisFound = 0;

	for( UINT curTri = 0; curTri < (int)m_Tris.size(); curTri++ )
	{
		//忽略已经被删除的面
		if( !m_Tris[curTri].m_IsValid )
			continue;

		for( UINT curVert = 0; curVert < 3; curVert++ )
		{
			UINT test0 = m_Tris[curTri].m_VerInd[curVert];
			UINT test1 = m_Tris[curTri].m_VerInd[ ( curVert + 1 )%3 ];

			if( ( ( test0 == ver0 ) && ( test1 == ver1 ) ) ||
				( ( test1 == ver0 ) && ( test0 == ver1 ) ) )
			{
				//边被两个以上的多边形共享，不可删除
				if( numTrisFound >= 2 )
					return 0;
				tris[ numTrisFound++ ] = curTri;
			}
		}
	}

	if( numTrisFound != 2  || tris[0] == tris[1] )
		return 0;

	return numTrisFound;
#endif
	return 0;
}	

void CLODBuilder::GenNewVerToFirstVer( UINT ver0, UINT ver1 )
{
#if 0
	m_Vers[ver0].m_Pos = m_Vers[ver0].m_Pos*0.5f + m_Vers[ver1].m_Pos*0.5f;
	m_Vers[ver0].m_Nor = m_Vers[ver0].m_Nor*0.5f + m_Vers[ver1].m_Nor*0.5f;
	m_Vers[ver0].m_Dif = ColorBlend( m_Vers[ver0].m_Dif, m_Vers[ver1].m_Dif, 0.5f );
	m_Vers[ver0].m_UV.x = m_Vers[ver0].m_UV.x*0.5f + m_Vers[ver1].m_UV.x*0.5f;
	m_Vers[ver0].m_UV.y = m_Vers[ver0].m_UV.y*0.5f + m_Vers[ver1].m_UV.y*0.5f;
	D3DXVec3Normalize( &m_Vers[ver0].m_Nor, &m_Vers[ver0].m_Nor );

	GMap< BYTE, float > Skl;
	int i = 0;
	for( ; i<4; i++ )
	{
		if( m_Vers[ver0].m_SklInd.b[i] )
		{
			if( Skl.find( m_Vers[ver0].m_SklInd.b[i] ) == Skl.end() )
				Skl[ m_Vers[ver0].m_SklInd.b[i] ] =  m_Vers[ver0].m_Wgh.GetWeight(i);
			else
				Skl[ m_Vers[ver0].m_SklInd.b[i] ] += m_Vers[ver0].m_Wgh.GetWeight(i);
		}

		if( m_Vers[ver1].m_SklInd.b[i] )
		{
			if( Skl.find( m_Vers[ver1].m_SklInd.b[i] ) == Skl.end() )
				Skl[ m_Vers[ver1].m_SklInd.b[i] ] =  m_Vers[ver1].m_Wgh.GetWeight(i);
			else
				Skl[ m_Vers[ver1].m_SklInd.b[i] ] += m_Vers[ver1].m_Wgh.GetWeight(i);
		}
	}

	float W[4] = { 0, 0, 0, 0 };
	float Total = 0;
	for( GMap<BYTE, float>::reverse_iterator it = Skl.rbegin(); it != Skl.rend() && i < 4; it++, i++ )
	{			
		m_Vers[ver0].m_SklInd.b[i] = (*it).first;
		Total += W[i] = (*it).second;
	}

	if( Total > 0.0001f )
	{
		m_Vers[ver0].m_Wgh = CTinyWeight( W[0]/Total, W[1]/Total, W[2]/Total, W[3]/Total );
	}
	m_Vers[ver1] = m_Vers[ver0];
#endif
}