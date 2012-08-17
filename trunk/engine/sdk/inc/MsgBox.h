#pragma once
#include "GUIBase.h"

namespace sqr
{
	class SQRWnd;
	class CEmptyCallback;
	class SQRMsgBoxHandlerHolder;
	class SQRHolderHandler : public SQRGuiMallocObject
	{
	public:
		virtual bool CallBack( int index, void* pParam )
		{
			return true;
		};
		virtual ~SQRHolderHandler() {};
		virtual void Release() {};
	};

	class SQRMsgBoxHandler
		: public SQRGuiMallocObject 
		, public virtual CDynamicObject
	{
	public:
		virtual bool CallBack( int nIndex, void* pParam ) { CALL_CLASS_CALLBACK_2_RET(bool, nIndex, pParam) return false;};
		virtual ~SQRMsgBoxHandler();
		void SetHandler(SQRMsgBoxHandlerHolder* pHolder);
	private:
		SQRMsgBoxHandlerHolder* m_pHolder;
	};

	class SQRMsgBoxHandlerHolder: public SQRHolderHandler
	{
		SQRMsgBoxHandler* m_pHandler;
	public:
		SQRMsgBoxHandlerHolder( SQRMsgBoxHandler* pHandler );
		~SQRMsgBoxHandlerHolder();
		bool CallBack( int nIndex, void* pParam );
		void Release();
		void SetHandler(SQRMsgBoxHandler* pHandler);
	};

	template <typename Fun, typename Class >
	class SQRHolder:public SQRHolderHandler
	{
	public:
		SQRHolder( Fun& fun, Class* pClass ):_fun(fun), _pClass( pClass ) {}

		bool CallBack( int index, void* pParam )
		{
			if ( _pClass )
				return ( _pClass->*_fun )( index, pParam );
			return true;
		};

		void Release()
		{
			delete this;
		}

	private:
		Fun _fun;
		Class* _pClass;
	};

	template <typename Fun>
	class SQRHolder< Fun, int >:public SQRHolderHandler
	{
	public:
		SQRHolder( Fun& fun):_fun(fun) {}

		bool CallBack( int index, void* pParam )
		{
			return _fun(index, pParam);
		};

		void Release()
		{
			delete this;
		};
	private:
		Fun _fun;
	};

	void GUI_API
		RegistMsgBox(
		uint32			nStyle,
		SQRWnd*			pRoot,
		const char*		szResName,
		const char*		szInfo	 = NULL,
		const char*		szBtnOK  = NULL,
		const char*		szBtnCnl = NULL
		);

	GUI_API SQRWnd*
		MsgBox(
		uint32			nStyle,
		SQRHolderHandler*	pHolder,
		SQRWnd*			pParent,
		const char*		sText,
		uint32			dwBtnStyle,
		void*			pParam,
		const char*		sImageStr,
		bool			autoresize
		);

	void GUI_API
		CloseAllMsgBox(   SQRWnd*			pWnd );

	template <class _FunTy, class _ClassTy >
	SQRWnd* MessageBox(
		uint32			nStyle,
		_FunTy 			value,
		SQRWnd*			pParent,
		const char*		sText,
		_ClassTy*		pClass,
		uint32			dwBtnStyle = MB_BtnNone,
		void*			pParam = NULL,
		bool			autoresize = false
		)
	{
		return ::MsgBox( nStyle, new SQRHolder<_FunTy, _ClassTy>( value, pClass ), pParent, sText, dwBtnStyle, pParam, "",autoresize);
	};


	template <class _FunTy>
	SQRWnd* MessageBox(
		uint32			nStyle,
		_FunTy 			value,
		SQRWnd*			pParent,
		const char*		sText,
		int				pClass = NULL,
		DWORD			dwBtnStyle = MB_BtnNone,
		void*			pParam = NULL,
		bool			autoresize = false
		)
	{
		return ::MsgBox( nStyle, new SQRHolder< _FunTy, int>( value ), pParent, sText, dwBtnStyle, pParam,"",autoresize);
	};

#define MSG_CALL_NULL CEmptyCallback()

	class CEmptyCallback
	{
	public:
		bool  operator() ( int index, void* pParam )
		{
			return true;
		};
	};
}
