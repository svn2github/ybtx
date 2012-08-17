#include "stdafx.h"
#include "CRenderOperation.h"
#include "CHardwareBufferMgr.h"
#include "CVertexHardwareBuffer.h"
#include "CIndexHardwareBuffer.h"
#include "CVertexDeclaration.h"
#include "BaseHelper.h"


VertexData::VertexData()
		:vertexStart(0),vertexCount(0),vertexDeclaration(NULL),vertexBufferBinding(NULL)
{
}

VertexData::~VertexData(void)
{}



VertexData* VertexData::Clone(bool copyData) const
{
	VertexData* dest = new VertexData();
	dest->vertexBufferBinding = new CVertexBufferBinding;


	const CVertexBufferBinding::VertexBufferBindingMap& bindings =
	    this->vertexBufferBinding->GetBindings();
	CVertexBufferBinding::VertexBufferBindingMap::const_iterator vbi, vbend;
	vbend = bindings.end();
	for (vbi = bindings.begin(); vbi != vbend; ++vbi)
	{
		CVertexHardwareBuffer* srcbuf = vbi->second.m_pBuffer;
		CVertexHardwareBuffer* dstBuf;
		if (copyData)
		{
			void *srcData = srcbuf->lock(vbi->second.m_sOffset, vbi->second.m_sLenght, HBL_READ_ONLY);
			size_t VerNum = vbi->second.m_sLenght / srcbuf->getVertexSize();
			dstBuf =
			    CHardwareBufferMgr::GetInst()->createVertexBuffer(
			        srcbuf->getVertexSize(), 
					VerNum, srcbuf->getUsage(),
					srcData,
					srcbuf->isSystemMemory(),
			        srcbuf->hasShadowBuffer());
			srcbuf->unlock();
			//dstBuf->copy(*srcbuf, , 0, vbi->second.m_sLenght, true);
		}
		else
		{
			dstBuf = srcbuf;
		}
		dest->vertexBufferBinding->SetBinding(vbi->first, dstBuf);
	}

	dest->vertexStart = this->vertexStart;
	dest->vertexCount = this->vertexCount;

	//------------------------------------------------
	/*if(copyData)
	{
		const VertexDeclaration::VertexElementList elems =
			this->vertexDeclaration->getElements();
		VertexDeclaration::VertexElementList::const_iterator ei, eiend;
		eiend = elems.end();
		for (ei = elems.begin(); ei != eiend; ++ei)
		{
			dest->vertexDeclaration->AddElement(
				ei->getSource(),
				ei->getOffset(),
				ei->getType(),
				ei->getSemantic(),
				ei->getIndex() );
		}
	}
	else
	{
		dest->vertexDeclaration = vertexDeclaration;
	}*/
	dest->vertexDeclaration = vertexDeclaration;
	return dest;
}

void VertexData::Destroy(void)
{
	vertexBufferBinding->DestroyAllBuffer();
	//GfkSafeDelete(vertexDeclaration);
}


IndexData::IndexData()
		:indexBuffer(NULL),indexStart(0),indexCount(0)
{}

IndexData::~IndexData()
{}

IndexData* IndexData::Clone(bool copyData) const
{
	IndexData* dest = new IndexData();
	if (indexBuffer)
	{
		if (copyData)
		{
			void *srcData = indexBuffer->lock(0, indexBuffer->getSize(), HBL_READ_ONLY);
			dest->indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(indexBuffer->getNumIndexes(),
			                    indexBuffer->getUsage(),srcData,indexBuffer->isSystemMemory(),indexBuffer->hasShadowBuffer());
			indexBuffer->unlock();
			//dest->indexBuffer->copy(*indexBuffer, 0, 0, indexBuffer->getSize(), true);
		}
		else
		{
			dest->indexBuffer = indexBuffer;
		}
	}
	dest->indexCount = indexCount;
	dest->indexStart = indexStart;
	return dest;
}

void IndexData::Destroy(void)
{
	if (NULL!=indexBuffer)
		CHardwareBufferMgr::GetInst()->DestroyBuffer(indexBuffer);
}

CRenderOperation::CRenderOperation()
		:vertexData(NULL), operationType(PT_TRIANGLELIST),
		useIndexes(true),indexData(NULL)
{}

CRenderOperation::~CRenderOperation()
{
}

CRenderOperation& CRenderOperation::operator = (const CRenderOperation& op)
{
	if (&op==this)
		return *this;
	if (NULL==vertexData)
		vertexData = new VertexData();
	if (NULL==indexData)
		indexData = new IndexData();
	//vertexData = op.vertexData->Clone(false);
	vertexData->vertexBufferBinding = op.vertexData->vertexBufferBinding;
	vertexData->vertexDeclaration = op.vertexData->vertexDeclaration;
	vertexData->vertexStart = op.vertexData->vertexStart;
	vertexData->vertexCount = op.vertexData->vertexCount;
	//indexData = op.indexData->Clone(false);
	indexData->indexBuffer = op.indexData->indexBuffer;
	indexData->indexStart = op.indexData->indexStart;
	indexData->indexCount = op.indexData->indexCount;
	operationType = op.operationType;
	useIndexes = op.useIndexes;
	return *this;
}

void CRenderOperation::Clone(CRenderOperation& op) const
{
	if (&op==this)
		return;
	if (NULL!=op.vertexData)GfkSafeDelete(op.vertexData->vertexBufferBinding);
	GfkSafeDelete(op.vertexData);
	GfkSafeDelete(op.indexData);
	op.vertexData = vertexData->Clone(false);
	op.indexData = indexData->Clone(false);
	op.operationType = operationType;
	op.useIndexes = useIndexes;
}

bool CRenderOperation::Create(void)
{
	if (vertexData!=NULL||indexData!=NULL)
		return false;
	//if(NULL == (vertexData = new VertexData()))
	//	return false;
	//if(NULL == (indexData = new IndexData()))
	//{
	//	delete vertexData;
	//	return false;
	//}
	vertexData = new VertexData();
	indexData = new IndexData();
	vertexData->vertexBufferBinding = new CVertexBufferBinding();

	return true;
}

void CRenderOperation::Destroy()
{
	if ( NULL!=vertexData )
	{
		GfkSafeDelete(vertexData->vertexBufferBinding);
		delete vertexData;
		vertexData = NULL;
	}
	GfkSafeDelete(indexData);
}

