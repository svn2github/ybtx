#pragma once
#include "DataChunk.h"

namespace IO
{

/// chunk版本号的映射关系.
class VersionMapChunk : public DataChunk
{
public:
	/// 单件实例.
	static VersionMapChunk* Instance();

	VersionMapChunk();
	virtual ~VersionMapChunk();
	/// 注册版本号
	void RegisterVersion(const Util::FourCC& id, uint ver);
	/// 查找版本号
	uint FindVersion(const Util::FourCC& id);

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 拷贝赋值
	VersionMapChunk& operator=(const VersionMapChunk& rhs);

private:
	static VersionMapChunk* instance;

	std::map<uint, uint> versionMap;
};

#define DeclareChunk(type) \
public: \
	static Util::FourCC FOURCC; \
	static uint VERSION; \
	static bool RegisterToVersionMap(); \
private:

#define ImplementChunk(type, fourcc, version) \
	Util::FourCC type::FOURCC = fourcc; \
	uint type::VERSION = version; \
	bool type##_registered = type::RegisterToVersionMap(); \
	bool type::RegisterToVersionMap() \
	{ \
		IO::VersionMapChunk::Instance()->RegisterVersion(type::FOURCC, type::VERSION); \
		return true; \
	}

}// namespace IO

