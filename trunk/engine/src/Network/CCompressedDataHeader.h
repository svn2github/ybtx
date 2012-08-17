#pragma once

namespace sqr
{

#pragma pack( push, 1 )

	struct CDataHeader
	{
		uint16	m_uCompressed:1;
		uint16	m_uDataLen:15;
	};

	struct CCompressedDataHeader
		:public CDataHeader
	{
		uint16	m_uCompressedDataLen;
	};

#pragma pack( pop )

}
