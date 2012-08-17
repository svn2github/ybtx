#pragma once

namespace sqr
{
#if !defined LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

#ifdef _WIN32
	typedef unsigned int		uint;
#endif

#if defined _WIN32

	typedef	__int8			int8;
	typedef	__int16			int16;
	typedef	__int32			int32;
	typedef	__int64			int64;

	typedef	unsigned __int8		uint8;
	typedef	unsigned __int16	uint16;
	typedef	unsigned __int32	uint32;
	typedef	unsigned __int64	uint64;


#elif defined __linux__
#define TCHAR				char
	typedef	char			int8;
	typedef	short			int16;
	typedef	int			int32;
	typedef	long long		int64;

	typedef	unsigned char		uint8;
	typedef	unsigned short		uint16;
	typedef	unsigned int		uint32;
	typedef	unsigned long long	uint64;


#elif defined __FreeBSD__
#define TCHAR				char
	typedef	char			int8;
	typedef	short			int16;
	typedef	int			int32;
	typedef	long long		int64;

	typedef	unsigned char			uint8;
	typedef	unsigned short		uint16;
	typedef	unsigned int		uint32;
	typedef	unsigned long long	uint64;

#else
#error "Unsupported platform."
#endif

	typedef unsigned int		index_t;
#define	InvalidIndex		0xffffffff

}
