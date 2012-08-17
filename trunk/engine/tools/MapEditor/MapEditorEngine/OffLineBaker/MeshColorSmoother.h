#pragma once
//------------------------------------------------------------------------------
/**
    @class CMeshColorSmoother

	用于Mesh顶点色计算的工具类.
	因为有些模型的邻接顶点的位置和法线是一样的, 这就需要它们的颜色也是一样的.
	把这样的顶点的颜色进行平均处理, 使模型的颜色过渡更为平滑.

    (C) 2009 ThreeOGCMan
*/
#include "CRefObject.h"
#include "CVector3.h"
#include "CColor4.h"

//------------------------------------------------------------------------------
namespace sqr
{

SmartPointer(CMeshColorSmoother);
class CMeshColorSmoother : public CRefObject
{
public:
	/// 添加一个顶点, 增加这个顶点的计数
	void AddVertex(const CVector3f& pos, const CVector3f& nml, const CColor4& clr);
	/// 取得平均后的顶点颜色
	D3DCOLOR GetSmoothedColor(const CVector3f& pos, const CVector3f& nml);

	/// 清空缓冲
	void Clear();

private:
	struct Vertex
	{
	public:
		Vertex() {};
		Vertex(const CVector3f& pos, const CVector3f& nml);;
		 
		bool operator==(const Vertex& rhs) const;
	public:
		CVector3f position;
		CVector3f normal;
	};
	struct AverageColor
	{
	public:
		AverageColor();
		AverageColor(const CColor4& clr);
	public:
		uint count;			//< 邻接顶点数目
		CColor4 colorSum;	//< 颜色和
	};

private:
	vector<Vertex> vertices;
	vector<AverageColor> colors;
};

//------------------------------------------------------------------------------
inline void
CMeshColorSmoother::Clear()
{
	this->vertices.clear();
	this->colors.clear();
}

//------------------------------------------------------------------------------
inline
CMeshColorSmoother::Vertex::Vertex( const CVector3f& pos, const CVector3f& nml )
: position(pos), normal(nml)
{
	// empty
}

//------------------------------------------------------------------------------
inline bool
CMeshColorSmoother::Vertex::operator==( const Vertex& rhs ) const
{
	return (this->position == rhs.position) && (this->normal == rhs.normal);
}

//------------------------------------------------------------------------------
inline
CMeshColorSmoother::AverageColor::AverageColor()
: count(0), colorSum(0, 0, 0, 0)
{
	// empty
}

//------------------------------------------------------------------------------
inline
CMeshColorSmoother::AverageColor::AverageColor( const CColor4& clr )
: count(1), colorSum(clr)
{
	// empty
}

}// namespace sqr

