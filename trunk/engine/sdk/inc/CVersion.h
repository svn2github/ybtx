#pragma once

namespace sqr
{
namespace Version
{
class COMMON_API CVersion
{
private:
	CVersion(void);
	~CVersion(void);

	uint8	m_uResMajor;
	uint8	m_uResMinor;
	uint16	m_uResPatch;
public:

	static uint32 GetBinVersionCode();

	const char* GetResVersionStr()const;
	uint32 GetResVersionCode()const;
	uint8 GetResMajor()const;
	uint8 GetResMinor()const;
	uint16 GetResPatch()const;

	void Init(const char* szFileName);

	static CVersion& Instance();
};
}
}

::Version;
