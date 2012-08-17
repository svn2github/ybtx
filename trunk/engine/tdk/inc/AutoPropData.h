#pragma once
#include "CVector3.h"
namespace sqr_tools
{
	enum ECtrlDataType
	{
		E_INTEGER		= 0,	// int
		E_FLOAT			= 1,	// float
		E_BYTE			= 2,	// byte
		E_BOOLValue		= 3,	// bool

		E_DBWORD		= 4,
		E_VECTOR		= 5,	// vector3f
		E_VECTOR2F		= 6,	// vecto2f

		E_STRING		= 7,	// string
		E_STRINGBUTTON	= 8,	// string with button
		E_ESTRING		= 9,	// EString
		E_ESTRINGBUTTON	= 10,	// EString with button
		E_ESETESTRING	= 11,	// ESet<EString>
		E_EVECESTRING	= 12,	// ESet<EString>
		E_COLORSTRING	= 13,	// COLOR
		E_CHARPTR		= 14,	// char[16]
		E_CUEESTRING	= 15,	// cue string

		E_COMBO			= 16,	// enum
		E_FloatMap		= 17,	// map<float, float>
		E_EFloatMap		= 18,	// EMap<float, float>
		E_BYTEMap		= 19,	// map<float, BYTE>
		E_EBYTEMap		= 20,	// EMap<float, BYTE>

		E_VectorMap		= 21,	// map<float, vector3f>
		E_EVectorMap	= 22,	// EMap<float, vector3f>

		E_EEStrMap		= 23,	// EMap<float, EString>

		E_COLOR4Map		= 24,
		E_COLORMap		= 25,
		E_UINT32CLMap	= 26,
		E_FLOATCLMap	= 27,
		E_ECOLORMap		= 28,


		E_TYPECOUNT		= 29,
	};

	class	AutoDataHelper;
	typedef void (*PFN_UPDATECALLBACK)( AutoDataHelper* );
	struct ECtrlData
	{
		ECtrlDataType			m_eDataType;
		PFN_UPDATECALLBACK		m_pFunc;
		AutoDataHelper*			m_pObj;
		void*			m_pData;
		void*			m_pHelpInfo;
		void*			m_pAssistInfo;

		ECtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj,  void* data);
		ECtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj,  const void* helpinfo, void* data);
		ECtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj,  const void* helpinfo, void* assitInfo, void* data);
		static ECtrlData*		CreateCtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj
								, const void* helpinfo, void* data);
		~ECtrlData();
	};
	
	struct EAutoCtrlData
	{
		string			m_Title;
		ECtrlData*		m_pCtrlData;
		EAutoCtrlData(const char* title, ECtrlData* data);
		~EAutoCtrlData();
	};

	class CCombDataHelper
	{
	public:
		virtual void		PushItem( string name, int value );
		virtual int			GetItemNum() const;
		virtual const char* GetItemName( int idx ) const;
		virtual int			GetValueByIdx( int selIdx );
		virtual int			GetIdxByValue( int value );
		virtual void		Reset();

	private:
		struct CombData
		{
			CombData( const string& name_in, int value_in );
			string	name;
			int		value;
		};
		vector<CombData> m_data;
	};

	class CComboCtrlData 
	{
	public:
		CComboCtrlData( void* val );
		~CComboCtrlData();
		
		void	ResetHelper();

	public:
		void*				m_pVal;
		CCombDataHelper*	m_pHelper;
	};

	class CFloatMapData
	{
	public:
		CFloatMapData(ECtrlDataType tp, void* pMap);
		
		void		SetMapData(float idx, float val);
		void		EraseMapData(float idx);
		int			GetMapCount();
		float		GetMapIndex(int n);
		float		GetMapValue(int n);
		bool		GetMapValue(float idx,float& value);
	public:
		ECtrlDataType	m_eMapType;
		void*			m_pMap;
	};

	class CVector3fMapData
	{
	public:
		CVector3fMapData(ECtrlDataType tp, void* pMap);

		int				GetVector3MapCount();
		float			GetVector3MapIndex(int n);
		float			GetVector3MapXVal(float idx);
		float			GetVector3MapYVal(float idx);
		float			GetVector3MapZVal(float idx);

		void			InsertMapNode(float idx, CVector3f& vec3f);
		void			EraseMapNode(float idx);
		CVector3f*		GetVector3Node(float idx);

	public:
		ECtrlDataType	m_eMapType;
		void*			m_pMap;
	};

	class CEStrMapData
	{
	public:
		CEStrMapData(ECtrlDataType tp, void* pMap);

		int				GetEStrMapCount();
		float			GetEStrMapIndex(int n);

		void			InsertMapNode(float idx, const char* estr);
		void			SetNodeEStr(float idx, const char* tex);
		void			EraseMapNode(float idx);
		const char*		GetEStrNode(float idx);

	public:
		ECtrlDataType	m_eMapType;
		void*			m_pMap;
	};
	
}