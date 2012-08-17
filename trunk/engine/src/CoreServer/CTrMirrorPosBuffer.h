#pragma once
#include "CTrMirrorBuffer.h"

namespace sqr
{
	class CTrMirrorPosBuffer : public CTrMirrorBuffer
	{
	public:
		static	CTrMirrorBuffer* CreateBufferMgr(uint32 uDataNum);
		void	Release();

	private:
		CTrMirrorPosBuffer(uint32 uDataSize, uint32 uDataNum);
		~CTrMirrorPosBuffer();
	};
}
