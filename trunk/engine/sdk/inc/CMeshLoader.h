#ifndef _CMesh_Loader_H_
#define _CMesh_Loader_H_


template<class VertexClass>
bool LoadVertex( VertexClass& OutVertex, SaveInfo& SI, CBufFile& GraphicFile )
{
	if ( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVertex<10,11> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
	}
	else if ( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVertex<12,12> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
	}
	else if ( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVertex<12,16> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
	}
	else if ( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVertex<14,16> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
	}
	else if ( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVertex<16,16> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
	}
	else
		return false;

	return true;
}

template<class VertexClass>
bool LoadVertex1( VertexClass& OutVertex, SaveInfo& SI, CBufFile& GraphicFile )
{
	if ( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVertex<10,11,2> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
	}
	else if ( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVertex<12,12,2> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
	}
	else if ( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVertex<12,16,2> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
	}
	else if ( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVertex<14,16,2> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
	}
	else if ( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVertex<16,16,2> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
	}
	else
		return false;

	return true;
}

template<class VertexClass>
bool LoadVertex2( VertexClass& OutVertex, SaveInfo& SI, CBufFile& GraphicFile )
{
	if ( SI.sPos < 1024 && SI.sTex <= 1.02f )
	{
		CTinyVertex<10,11,3> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
		OutVertex.u2 = Vertex.GetU2();
		OutVertex.v2 = Vertex.GetV2();
	}
	else if ( SI.sPos < 4096 && SI.sTex <= 2.04f )
	{
		CTinyVertex<12,12,3> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
		OutVertex.u2 = Vertex.GetU2();
		OutVertex.v2 = Vertex.GetV2();
	}
	else if ( SI.sPos < 4096 && SI.sTex < 32.7f )
	{
		CTinyVertex<12,16,3> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
		OutVertex.u2 = Vertex.GetU2();
		OutVertex.v2 = Vertex.GetV2();
	}
	else if ( SI.sPos < 16384 && SI.sTex < 32.7f )
	{
		CTinyVertex<14,16,3> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
		OutVertex.u2 = Vertex.GetU2();
		OutVertex.v2 = Vertex.GetV2();
	}
	else if ( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		CTinyVertex<16,16,3> Vertex;
		GraphicFile.read( Vertex._Buf, sizeof( Vertex._Buf ) );
		OutVertex.x = (int16)( Vertex.GetX() + SI.xStr );
		OutVertex.y = (int16)( Vertex.GetY() + SI.yStr );
		OutVertex.z = (int16)( Vertex.GetZ() + SI.zStr );
		OutVertex.n = Vertex.GetN();
		OutVertex.u = Vertex.GetU();
		OutVertex.v = Vertex.GetV();
		OutVertex.u1 = Vertex.GetU1();
		OutVertex.v1 = Vertex.GetV1();
		OutVertex.u2 = Vertex.GetU2();
		OutVertex.v2 = Vertex.GetV2();
	}
	else
		return false;

	return true;
}

template<class VertexClass>
bool LoadVertexFull( VertexClass& OutVertex, SaveInfo& SI, CBufFile& GraphicFile )
{
	CVector3f v,n;
	CVector2f t;
	if ( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		GraphicFile.read( (char*)&v, sizeof( v ) );
		GraphicFile.read( (char*)&n, sizeof( n ) );
		GraphicFile.read( (char*)&t, sizeof( t ) );
		OutVertex.x = v.x;
		OutVertex.y = v.y;
		OutVertex.z = v.z;
		OutVertex.n = n.x;
		OutVertex.ny = n.y;
		OutVertex.nz = n.z;
		OutVertex.u = t.x;
		OutVertex.v = t.y;
		OutVertex.u1 = 0.0f;
		OutVertex.v1 = 0.0f;
		OutVertex.u2 = 0.0f;
		OutVertex.v2 = 0.0f;
		return true;
	}
	else
		return false;
}

template<class VertexClass>
bool LoadVertexFull1( VertexClass& OutVertex, SaveInfo& SI, CBufFile& GraphicFile )
{
	CVector3f v,n;
	CVector2f t,t1;
	if ( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		GraphicFile.read( (char*)&v, sizeof( v ) );
		GraphicFile.read( (char*)&n, sizeof( n ) );
		GraphicFile.read( (char*)&t, sizeof( t ) );
		GraphicFile.read( (char*)&t1,sizeof( t1) );
		OutVertex.x = v.x;
		OutVertex.y = v.y;
		OutVertex.z = v.z;
		OutVertex.n = n.x;
		OutVertex.ny = n.y;
		OutVertex.nz = n.z;
		OutVertex.u = t.x;
		OutVertex.v = t.y;
		OutVertex.u1 = t1.x;
		OutVertex.v1 = t1.y;
		OutVertex.u2 = 0.0f;
		OutVertex.v2 = 0.0f;
		return true;
	}
	else
		return false;
}

template<class VertexClass>
bool LoadVertexFull2( VertexClass& OutVertex, SaveInfo& SI, CBufFile& GraphicFile )
{
	CVector3f v,n;
	CVector2f t,t1,t2;
	if ( SI.sPos < 65536 && SI.sTex < 32.7f )
	{
		GraphicFile.read( (char*)&v, sizeof( v ) );
		GraphicFile.read( (char*)&n, sizeof( n ) );
		GraphicFile.read( (char*)&t, sizeof( t ) );
		GraphicFile.read( (char*)&t1, sizeof( t1 ) );
		GraphicFile.read( (char*)&t2, sizeof( t2 ) );
		OutVertex.x = v.x;
		OutVertex.y = v.y;
		OutVertex.z = v.z;
		OutVertex.n = n.x;
		OutVertex.ny = n.y;
		OutVertex.nz = n.z;
		OutVertex.u = t.x;
		OutVertex.v = t.y;
		OutVertex.u1 = t1.x;
		OutVertex.v1 = t1.y;
		OutVertex.u2 = t2.x;
		OutVertex.v2 = t2.y;
		return true;
	}
	else
		return false;
}
#endif