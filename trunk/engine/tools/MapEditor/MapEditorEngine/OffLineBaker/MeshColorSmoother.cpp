//------------------------------------------------------------------------------
//  MeshColorSmoother.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "MeshColorSmoother.h"
#include "CColor.h"
#include "CColor4.inl"

namespace sqr
{

//------------------------------------------------------------------------------
void
CMeshColorSmoother::AddVertex( const CVector3f& pos, const CVector3f& nml, const CColor4& clr )
{
	Vertex vertex(pos, nml);
	for (index_t i = 0; i < this->vertices.size(); ++i)
	{
		if (this->vertices[i] == vertex)
		{
			this->colors[i].colorSum += clr;
			++this->colors[i].count;
		}
	}
	// not found, add a new one
	this->vertices.push_back(vertex);
	this->colors.push_back(clr);
}

//------------------------------------------------------------------------------
D3DCOLOR
CMeshColorSmoother::GetSmoothedColor( const CVector3f& pos, const CVector3f& nml )
{
	Vertex vertex(pos, nml);
	for (index_t i = 0; i < this->vertices.size(); ++i)
	{
		if (this->vertices[i] == vertex)
		{
			CColor4 color = this->colors[i].colorSum;
			color /= float(this->colors[i].count);
			color.a = 1.0f;
			return color.MakeDWordSafe();
		}
	}
	// not exist
	GenErr("This vertex has not added in.");
	return CColor::Black;
}

}// namespace sqr