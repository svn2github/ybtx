#pragma once
#include "TSingleton.h"

/**
	@class IoSystem

	IO 系统的核心管理类.
	封装常用的文件操作.
	提供目录名称的解析和映射, 默认已经映射的有:
	bin:	可执行文件路径
	temp:	Windows临时目录
	work:	当前工作目录
 */
namespace sqr
{

class IoSystem : public TSingleton<IoSystem>
{
public:
	/// 创建路径中没有的目录
	bool CreateDirectory(const string& dir) const;
	/// 删除一个空目录
	bool DeleteDirectory(const string& dir) const;
	/// 目录是否存在
	bool DirectoryExists(const string& dir) const;

	/// 复制文件
	bool CopyFile(const string& from, const string& to) const;
	/// 删除文件
	bool DeleteFile(const string& path) const;
	/// 文件是否存在
	bool FileExists(const string& path) const;
	/// 设置文件的只读属性
	void SetReadOnly(const string& path, bool readOnly) const;
	/// 文件是否只读
	bool IsReadOnly(const string& path) const;

	/// 列出目录下的文件
	vector<string> ListFiles(const string& dir, const string& pattern) const;
	/// 列出目录下的子目录
	vector<string> ListDirectories(const string& dir, const string& pattern) const;

	/// 设置目录映射
	void RegisterDirMapping(const string& shortName, const string& dir, const bool bFull = true);
	/// 解析指定路径
	string GetFullPath(const string& shortPath) const;
	string GetRegisterDir(const string& shortName) const;
	/// 设置工作目录
	void SetWorkDirectory(const string& dir);
	void SetWorkSrcDirectory(const string& dir);
	void SetWorkTgtDirectory(const string& dir);

private:
	friend class TSingleton<IoSystem>;
	IoSystem();
	~IoSystem();
private:
	map<string, string> dirMappings;
};
}// namespace sqr
