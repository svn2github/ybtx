#pragma once
#include "GraphicBase.h"
namespace sqr
{
class CIndexHardwareBuffer;
class CVertexBufferBinding;
class CVertexDeclaration;

class VertexData : public CGraphicMallocObject
{
public:
	VertexData();
	~VertexData(void);

	CVertexDeclaration* vertexDeclaration;
	CVertexBufferBinding* vertexBufferBinding;

	size_t vertexStart;
	size_t vertexCount;

	VertexData* Clone(bool copyData = true) const;
	void Destroy(void);
};

class IndexData : public CGraphicMallocObject
{
public:
	IndexData();
	~IndexData();

	CIndexHardwareBuffer *indexBuffer;

	size_t indexStart;
	size_t indexCount;

	IndexData* Clone(bool copyData = true) const;
	void Destroy(void);
	//void optimiseVertexTriList(void);
};

// 这个类难道就是传说中的Mesh? 
// 解答：不能说这个是Mesh，渲染运算符表示了一次渲染的最小单位。Mesh是针对模型的更高级封装。二者意义不同
// 简单的说，3D场景中的物体是Mesh（有Load/Save 等行为方式，依赖于数据源）。
// 但是文字/GUI有意没有封装成Mesh，但是这些也要渲染。每次文字，GUI渲染都要用渲染运算符

class CRenderOperation : public CGraphicMallocObject
{
public:
	CRenderOperation();
	~CRenderOperation();

	VertexData* vertexData;
	IndexData* indexData;

	EPrimitiveType operationType;
	bool useIndexes;

	bool Create(void);
	void Destroy(void);

	CRenderOperation& operator = (const CRenderOperation& op);
	void  Clone(CRenderOperation& op) const;
};

}