#pragma once

namespace sqr
{
	uint16 Crc16WithInit( const void* pData, uint16 uLen, uint16 uInitCode );
	uint16 Crc16( const void* pData, uint16 uLen );
	uint32 Crc32( const void* pData, uint32 uLen );
	uint32 Crc32WithInit( const void* pData, uint32 uLen, uint32 uInitCode );
}
