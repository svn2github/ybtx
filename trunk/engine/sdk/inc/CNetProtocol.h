#pragma once
#include <string>
#include <vector>
#include "Script.h"
#include "ScriptDef.h"
#include "CDataFormat.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
#include "lua/luafile.h"

static const int32 PROTO_OK = 0;
static const int32 PROTO_ERROR = -1;



// 远程调用格式描述
class CProtoManager;

class SCRIPT_API CNetProtocol
{
	friend class CProtoManager;

private:
	std::vector<CCallBassArg*> m_vecArg;	// 参数列表
	std::string m_strFuncName;				// 打包接口名称，引擎内实现
	int32 m_uUnpackRef;						// 解包接口引用，脚本内实现
	uint16 m_uId;							// 协议id，有管理器自动分配
	bool  m_bIsMaker;						// 是否为远程函数定义者
	CProtoManager * m_pProMgr;
protected:

public:
	CNetProtocol(uint16 uId,CProtoManager *pProMgr);
	~CNetProtocol();

	int32 UnPack(lua_State*L,const char* pBuf,int32 nLeftLen);
	void Load(const char * pFileName,lua_State* L);
	void ProcessType(const char* path);
	void ConvertProtocol(lua_State*L);
	void FuncProc(lua_State* L);
	void ArglistProc(lua_State* L);
protected:
	static int32 Pack(lua_State* L); // 完成协议打包，被自动注册至lua虚拟机中供远程调用者使用

};


// 协议管理器
class SCRIPT_API CProtoManager
{
	friend class CNetProtocol;

private:
	std::vector<CNetProtocol*> m_Protocols;
	IPipe * m_IPipe;
	lua_State* m_L;
public:
	CProtoManager(IPipe * pIPipe,lua_State* L);
	~CProtoManager();

	// 对外的引擎接口
	int32 UnpackData();
	void SendError(std::string const & strError = "Write error");
	void ReadError(std::string const & strError = "Read error");
	// 对外的脚本接口
//		static int add_arg_type(lua_State*L); // 栈顶为数据格式描述文件路径
	void AddProtocol(const char * pFileName);
};


