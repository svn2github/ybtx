#ifndef _CVertex_Hardware_Buffer_H_
#define _CVertex_Hardware_Buffer_H_

#include "CVertexHardwareBuffer.h"
#include "Dx9Base.h"

namespace sqr
{
/*
这是一个使用D3D来管理现存的类型，通常情况下应该尽可能的
使用显存而<不>是内存(即useSystemMem = false)。如果使用显存
则应当使用ShadowBuffer，以保证数据的安全。但如果使用内存作为
顶点描述的话，则不应当使用ShadowBuffer。
*/
class DX9VertexHardwareBuffer : public CVertexHardwareBuffer
{
public:
	DX9VertexHardwareBuffer(size_t vertexSize, size_t numVertices,
	                        Usage usage, void* pInitData = NULL,
							bool useSystemMem = false, bool useShadowBuffer = true);
	~DX9VertexHardwareBuffer();

	LPDIRECT3DVERTEXBUFFER9 getD3D9VertexBuffer(void) const
	{
		return m_pD3DBuffer;
	}

	void updateRes();
	void freeRes();
	bool IsValid(void);
protected:
	void*	_lock(size_t offset, size_t length, LockOptions options);
	void	_unlock(void);
	bool	_imp_create(void);
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pD3DBuffer;
	D3DPOOL					m_D3DPool;
};
}

#endif //_CVertex_Hardware_Buffer_H_