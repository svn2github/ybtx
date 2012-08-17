#pragma once

class NpcCheckInfo
{
public:
	static bool CheckNpcFightInfo(const char* szName);
	static bool CheckNpcAITypeInfo(const char* szAIType);
	static bool CheckNpcAIDataInfo(const char* szAIData);
	static bool CheckNpcResInfo(const char* szNpcName);
	static bool CheckNpcTypeInfo(const char* szNpcType);
	static bool CheckNpcAoiTypeInfo(const char* szNpcAI);
};