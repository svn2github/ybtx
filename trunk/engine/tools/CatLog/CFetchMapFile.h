#pragma once

//在指定地方获取map file文件，供log分析处理

class CFetchMapFile
{
public:
	static CFetchMapFile& Inst();

	//根据版本down下来map file的list
	void Fetch(uint32 uVersion, bool bGac = true);
	
	void SetPath(const char* szPath);

private:
	CFetchMapFile();
	~CFetchMapFile();

private:
	string	m_strMapFilePath;
};

