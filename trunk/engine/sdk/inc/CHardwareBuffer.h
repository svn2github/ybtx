#pragma once
#include "CGraphicMallocObject.h"
#include "GraphicClassDefine.h"
#include "TSafeBuffer.h"

namespace sqr
{
	/// 锁定属性选项
	enum LockOptions
	{
		/** 正常锁定，即：允许读写操作并且数据受到保护*/
		HBL_NORMAL,
		/** 当锁定的时候放弃所有原来的缓存空间;只有由 HBU_DYNAMIC
		属性创建的缓存区才能够使用.
		*/
		HBL_DISCARD,
		/** 以只读模式锁定一个。不允许使用在用HBU_WRITE_ONLY属性创建的缓存区中。
		将其静态托管, 即：HBU_DYNAMIC属性在此不起作用。.
		*/
		HBL_READ_ONLY,
		/** 基本上和HBL_NORMAL选项相同, 除了这个应用可以保证不会向已经写过的数
		据区再写入数据,可以用API对其直接进行优化. */
		HBL_NO_OVERWRITE

	};

	class CHardwareBuffer : public CGraphicMallocObject
	{
	protected:
		size_t	m_Size;
		Usage	m_Usage;
		bool	m_IsLocked;
		size_t	m_sLockStart;
		size_t	m_sLockSize;
		bool	m_SystemMemory;
		bool	m_UseShadowBuffer;
		bool	m_ShadowUpdated;
		LockOptions			m_lockOp;
		CHardwareBuffer*	m_pShadowBuffer;

	public:
		CHardwareBuffer(Usage usage, bool systemMemory, bool useShadowBuffer = true);
		virtual ~CHardwareBuffer();

		//锁定某一区域准备读写
		void* lock(size_t offset, size_t length, LockOptions options);
		//锁定缓存的全部区域
		void* lock( LockOptions options );

		//解锁全部缓存区域
		void unlock(void);


		virtual void read(size_t offset, size_t length, void* pDest) = 0;
		virtual void write(size_t offset, size_t length, const void* pSource,
			bool discardBuffer = false) = 0;
		virtual	void copy(CHardwareBuffer& srcBuffer, size_t srcOffset,
			size_t dstOffset, size_t length, bool discardBuffer = false);

		size_t			getSize(void);
		Usage			getUsage(void);

		bool			isSystemMemory(void);
		bool			isLocked(void);
		bool			hasShadowBuffer(void);
		virtual bool	IsValid(void);
		virtual void	updateRes(void);
		virtual void	freeRes(void);
	protected:
		virtual void*	_lock(size_t offset, size_t length, LockOptions options) = 0;
		virtual void	_unlock(void) = 0;
		virtual void	_updateFromShadow(void);
	};

	template <class T>
	class THardwareSafeBuffer :public TSafeBuffer<T>
	{
	public:
		THardwareSafeBuffer(CHardwareBuffer * pHBuffer, LockOptions options)
			: TSafeBuffer(pHBuffer->lock(options),pHBuffer->getSize())
			,m_pHBuffer(pHBuffer)
		{
		}

		~THardwareSafeBuffer()
		{
			m_pHBuffer->unlock();
		}
	protected:
		CHardwareBuffer* m_pHBuffer;
	};
}
