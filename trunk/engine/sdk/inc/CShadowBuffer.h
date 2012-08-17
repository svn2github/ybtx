#ifndef _CShadowBuffer_H_
#define _CShadowBuffer_H_

#include "CHardwareBuffer.h"

namespace sqr
{
/*
这个类型主要是用来管理内存，为显卡上的硬件缓存提供内存上的映射空间

<备注> 因为存在这样一个问题：当程序对显卡失去独占权的时候，
显卡会自动释放显卡上的资源。所以在独占权失而复得的时候首先要做的就是
将CShadowBuffer中的数据拷贝给显卡，使其可以继续正常工作。
<备注> 在实际应用中，如果需要经常对缓存中的数据进行修改，那修改显存中
数据的效率要低于修改内存中的数据，在这种情况下，CShadowBuffer可以作为
一个缓冲区来使用。加快数据修改和拷贝的速度。
*/
class ShadowBuffer : public CHardwareBuffer
{
public:
	ShadowBuffer(size_t fSize, Usage usage = HBU_DYNAMIC);
	~ShadowBuffer();
public:
	void read(size_t offset, size_t length, void* pDest);
	void write(size_t offset, size_t length, const void* pSource,
	           bool discardBuffer = false);
protected:
	void* _lock(size_t offset, size_t length, LockOptions options);
	void _unlock(void);
	TBYTE* m_pData;
};
}


#endif //_CShadowBuffer_H_