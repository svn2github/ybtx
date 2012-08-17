#pragma once 

#include "CBaseType.h"
#include "ExpHelper.h"
#include "ScriptHelper.h"
#include "ScriptCommon.h"

extern "C"
{
#include "lua/lua.h"
}

namespace sqr
{
	inline double u2d( uint32 uValue )
	{
		double d;
		if( ((int32)uValue) >= 0 )
			d = (int)uValue;
		else
		{
			uint32* e = (uint32*)&d;
			e[1] = 0x41e00000 + ((uValue&0x7fffffff)>>11);
			e[0] = uValue<<21;
		}
		return d; 
	}

	inline double LL2d(int64 uValue)
	{
		int   flag=0;   
		double   d;   
		if(uValue < 0)   
		{
			//负数处理   
			flag   =   1;   
			uValue = -uValue;   
		}
		d = (unsigned long)(uValue >> 32);   
		//   直接运算1<<32会有数值溢出   
		d *=  (1<<16);   
		d *=  (1<<16); 
		d += (unsigned long)(uValue & 0xFFFFFFFF);   
		if(flag)   
			d = -d;   
		return d;

	}

	inline int32 absLuaStackIdx( lua_State* pState, int32 LuaStkId )
	{
		int r = LuaStkId > 0 ? LuaStkId : ( LuaGetTop( pState ) + LuaStkId + 1 );;
		Ast( r >=0 );
		return r;
	}


	class CClass;

	template<typename T>	//int32,int16,uint16,int8,uint8,uint32,int64,float,double
	inline void __push2Lua(lua_State* pState, char* pDataBuf)				{ lua_pushnumber( pState, *(T*)pDataBuf ); }
	template<>
	inline void __push2Lua<uint32>(lua_State* pState, char* pDataBuf)		{ lua_pushnumber( pState, u2d( *(uint32*)pDataBuf ) ); }
	template<>
	inline void __push2Lua<int64>(lua_State* pState, char* pDataBuf)		{ lua_pushnumber( pState, LL2d( *(int64*)pDataBuf ) ); }
	template<>
	inline void __push2Lua<uint64>(lua_State* pState, char* pDataBuf)		{ lua_pushnumber( pState, LL2d( *(int64*)pDataBuf ) ); }
	template<>
	inline void __push2Lua<const char*>(lua_State* pState, char* pDataBuf)	
	{ 
		////很有可能传入NULL，这时候lua就会爆掉了，如果为空，传入""
		const char* buf = *(const char**)pDataBuf; 
	/*	if(buf == NULL)
		{
			lua_pushstring(pState, "");
			return;
		}*/
		lua_pushstring( pState, buf); 
	}

	template<>
	inline void __push2Lua<bool>(lua_State* pState, char* pDataBuf)			{ lua_pushboolean( pState, *(bool*)pDataBuf ); }

	template<typename T>	//int32,int16,uint16,int8,uint8,uint32,int64,float,double
	inline T __getFromLua(lua_State* pState, int32 LuaStkId)
	{	return (T)lua_tonumber( pState, LuaStkId );	}

	template<>
	inline const char* __getFromLua<const char*>(lua_State* pState, int32 LuaStkId)
	{	return (const char*)lua_tostring( pState, LuaStkId );	}

	template<>
	inline bool __getFromLua<bool>(lua_State* pState, int32 LuaStkId)
	{	return lua_toboolean( pState, LuaStkId ) != 0; }

	template<typename T>
	class TTypeValue : public CBaseType
	{
	public:
		TTypeValue()
		{
			m_Flag = m_Flag | eFlagValue | eFlagNumber;
		}

		virtual void GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId )
		{
			int32 absIdx = absLuaStackIdx( pState, LuaStkId );
			*(T*)( pDataBuf + m_uOffset ) = __getFromLua<T>( pState, absIdx );
		}

		virtual void Push2LuaStack( lua_State* pState, char* pDataBuf )
		{
			__push2Lua<T>( pState, (pDataBuf + m_uOffset) );
		}

		virtual size_t	GetLen()		{	return sizeof(T);	}

		virtual void SetValue( char* pDataBuf, void* pValue )	
		{
			*(T*)( pDataBuf + m_uOffset ) = *( (T*)pValue );
		}

		void GetValue( char* pDataBuf, void* pDes )
		{
			memcpy( pDes, pDataBuf + m_uOffset, sizeof(T) );
		}
	};

	template<>
	inline TTypeValue<float>::TTypeValue()
	{
		m_Flag = m_Flag | eFlagValue | eFlagFloat | eFlagNumber;
	}

	template<>
	inline TTypeValue<double>::TTypeValue()
	{
		m_Flag = m_Flag | eFlagValue | eFlagDouble | eFlagNumber;
	}

	template<>
	inline TTypeValue<bool>::TTypeValue()
	{
		m_Flag = m_Flag | eFlagValue | eFlagBool;
	}

	template<>
	inline TTypeValue<const char*>::TTypeValue()
	{
		m_Flag = m_Flag | eFlagValue | eFlagStr ;
	}

	typedef TTypeValue<int32> 			CTypeInt32;
	typedef TTypeValue<int16> 			CTypeInt16;
	typedef TTypeValue<int8> 			CTypeInt8;
	typedef TTypeValue<uint32> 			CTypeUint32;
	typedef TTypeValue<uint16> 			CTypeUint16;
	typedef TTypeValue<uint8> 			CTypeUint8;
	typedef TTypeValue<float> 			CTypeFloat;
	typedef TTypeValue<double> 			CTypeDouble;
	typedef TTypeValue<bool> 			CTypeBool;
	typedef TTypeValue<int64>			CTypeInt64;
	typedef TTypeValue<uint64>			CTypeUint64;

	class CTypeSize : public CBaseType
	{
	public:
		CTypeSize(){ m_Flag = m_Flag | eFlagValue | eFlagNumber; }

		virtual void	GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId )
		{ 
			int32 absIdx = absLuaStackIdx( pState, LuaStkId );
			*(size_t*)( pDataBuf + m_uOffset ) = (size_t)lua_tonumber( pState, absIdx );	
		}
		virtual void	Push2LuaStack( lua_State* pState, char* pDataBuf )
		{  
			__push2Lua<size_t>( pState, (pDataBuf + m_uOffset) );
		}
		virtual void	SetValue( char* pDataBuf, void* pValue )
		{ 
			*(size_t*)( pDataBuf + m_uOffset ) = *( (size_t*)pValue );
		}
		virtual void	GetValue( char* pDataBuf, void* pDes )
		{ 
			memcpy( pDes, pDataBuf + m_uOffset, sizeof(size_t) ); 
		}
		virtual size_t	GetLen()	{	return sizeof( size_t ); }
	};

	class CTypeString : public TTypeValue<const char*>
	{
		bool m_bLongStr;
	public:
		CTypeString( bool bLongStr );
		size_t	GetPackBufLen( char* pDataBuf );
		void	GetPacker( char* pDataBuf, char* pDes );
		int32	UnPack( char* pDataBuf, char* pValue, size_t size );

		void GetFromLuaStack( lua_State* pState, char* pDataBuf, size_t& stLen, int32 LuaStkId )
		{
			int32 absIdx = absLuaStackIdx( pState, LuaStkId );
			*(const char**)( pDataBuf + m_uOffset ) = (const char*)lua_tolstring( pState, absIdx, &stLen );
		}

		void Push2LuaStack( lua_State* pState, char* pDataBuf, size_t stLen )
		{
			const char* buf = *(const char**)(pDataBuf + m_uOffset); 
			if(buf == NULL)
			{
				lua_pushstring(pState, "");
				return;
			}
			lua_pushlstring( pState, buf, stLen); 
		}
	};

	class CBaseObject;
	//对象类型的buf为对象的地址
	class CTypeCppObj : public CTypeSize
	{
	protected:
		CClass*	m_pClassInfo;
	public:
		CTypeCppObj( CClass* pClassInfo, CBaseType::EFlag eFlag );

		virtual void	GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId );
		virtual void	Push2LuaStack( lua_State* pState, char* pDataBuf );
		virtual void	GetPacker( char* pDataBuf, char* pDes )				{ GenErr( "对象不支持打包" );}
		virtual int32	UnPack( char* pDataBuf, char* pValue, size_t size )	{ GenErr( "对象不支持打包" );return 0;}
		virtual size_t	GetLen()	{	return sizeof( void* ); }
		const char*	GetClassName() const;
	};

	class CTypeMemory : public CBaseType
	{
	public:
		CTypeMemory();
		virtual void	GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId );
		virtual void	Push2LuaStack( lua_State* pState, char* pDataBuf );
		virtual void	SetValue( char* pDataBuf, void* pValue );
		virtual void	GetValue( char* pDataBuf, void* pDes );
		virtual size_t	GetLen()	{	return sizeof( void* ); }	
	};

	class CVariant;
	//lua中数据为任意类型，c++中为CVariant
	class CTypeVariant : public CTypeSize
	{
	public:
		CTypeVariant();
		~CTypeVariant();
		virtual void GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId );
		virtual void Push2LuaStack( lua_State* pState, char* pDataBuf );
		virtual void	GetPacker( char* pDataBuf, char* pDes )				{ GenErr( "对象不支持打包" );}
		virtual int32	UnPack( char* pDataBuf, char* pValue, size_t size )	{ GenErr( "对象不支持打包" );return 0;}
		virtual void Release();
		virtual bool HasVariant() { return true; }
		
		virtual size_t	GetLen()	{	return sizeof( void* ); }

	private:
		CVariant *m_pCurVar;
	};

	//支持可变参数，参数为基本类型
	class CTypeArgList : public CTypeSize
	{
	public:
		CTypeArgList();
		~CTypeArgList();
		void GetArgListFromLuaStack(lua_State *pState, int32 LuaStkId);
		virtual void GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId );
		virtual void Push2LuaStack( lua_State* pState, char* pDataBuf );
		virtual void	GetPacker( char* pDataBuf, char* pDes )				{ GenErr( "对象不支持打包" );}
		virtual int32	UnPack( char* pDataBuf, char* pValue, size_t size )	{ GenErr( "对象不支持打包" );return 0;}
		virtual bool HasArgList() { return true; }
		int32 ParamNumber() const { return  m_iParamNumber; }

		//virtual void	SetValue( char* pDataBuf, void* pValue );
		//virtual void	GetValue( char* pDataBuf, void* pDes );
		virtual size_t	GetLen()	{	return sizeof( void* ); }

	private:
		void Release();
	private:
		vector<CVariant*> * m_pvecVarList;
		int32 m_iParamNumber;
	};
}

