#pragma  once 
#include "CScriptMallocObject.h"

	struct lua_State;

namespace sqr{
	class CBaseType : public CScriptMallocObject
	{				//Compsite模式，基类
	public:
		enum EFlag
		{ 
			eFlagNull		= 0x0000,	eFlagValue		= 0x0001,	
			eFlagFloat		= 0x0002,	eFlagDouble		= 0x0004,	eFlagStr		= 0x0008,
			eFlagCppObj		= 0x0010,	eFlagNumber		= 0x0020,
			eFlagBool		= 0x0040,	eFlagMemory		= 0x0080,	
		};

		inline bool IsFloat()			{	return (m_Flag & eFlagFloat )				== 0 ? false : true ; }
		inline bool IsDouble()			{	return (m_Flag & eFlagDouble )				== 0 ? false : true ; }
		inline bool IsStr()				{	return (m_Flag & eFlagStr )					== 0 ? false : true ; }
		inline bool IsCppObj()			{	return (m_Flag & eFlagCppObj )				== 0 ? false : true ; }
		inline bool IsNumber()			{	return (m_Flag & eFlagNumber )				== 0 ? false : true ; }
		inline bool IsBool()			{	return (m_Flag & eFlagBool )				== 0 ? false : true ; }


		CBaseType()
			: m_uOffset(0)
			, m_Flag(eFlagNull)
		{}

		virtual ~CBaseType()					{}

		virtual void	GetFromLuaStack( lua_State* pState, char* pDataBuf, int32 LuaStkId )= 0;		//不 pop 出堆栈
		virtual void	Push2LuaStack( lua_State* pState, char* pDataBuf )= 0;
		virtual size_t  GetLen()= 0;
		virtual void	GetValue( char* pDataBuf, void* pDes )= 0;
		virtual void	SetValue( char* pDataBuf, void* pValue ) = 0;				//从连续内存把值拷贝到对象树里
		void			SetOffset( size_t offset )			{ m_uOffset = offset; }
		size_t          GetOffset() const { return m_uOffset; }
		virtual size_t  GetPackBufLen( char* pDataBuf )		{ return GetLen(); };
		virtual void	GetPacker( char* pDataBuf, char* pDes )	
		{ 
			GetValue( pDataBuf, pDes ); 
		}

		virtual int32 UnPack( char* pDataBuf, char* pValue, size_t size )	
		{ 
			size_t n = GetLen(); 
			if( size < n )
				return 0; 
			SetValue( pDataBuf, pValue ); 
			return (int32)n; 
		}

		virtual bool HasArgList() { return false; }
		virtual bool HasVariant() { return false; }

	protected:
		size_t m_uOffset;
		int	m_Flag;
	};
}

