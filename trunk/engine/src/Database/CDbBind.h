#pragma once
#include "CDbMallocObject.h"

namespace sqr
{
	class CDbBind : private MYSQL_BIND
				  , public CDbMallocObject
	{
	public:
		CDbBind(void);
		~CDbBind(void);

		void SetBuffer(const void* pBuffer);
		void SetIsNull(bool* pIsNull);
		void SetLength(size_t* pLength);
		void SetBuffType(enum_field_types eType);
		void SetIsUnsigned(bool bIsUnsigned );
		void SetBuffLength(size_t uSize);
	};
}
