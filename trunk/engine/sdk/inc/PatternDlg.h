#pragma once

/*-----------------------------------------------------------------------------
*  Example:
*  typedef TPtManagedDlg1<MyClass> Event;
*  class Callback : public Event::CallbackType
*  {
*      bool exec(MyClass);
*  };
*  Callback *cb = new Callback;
*  Event ev;
*  ev += cb;
*  ev();
*-----------------------------------------------------------------------------*/

#include <list>
#include "PtDlgCallback.h"
#include "CommonDefs.h"

namespace sqr
{
	enum EDlgReturnCode
	{
		EDlgReturn_None 	= 0,
		EDlgReturn_Delete 	= 1 << 0,
		EDlgReturn_Break 	= 1 << 1,
	};
	/*
	* =====================================================================================
	*        Class:  DlgWrapper
	*  Description:  静态函数指针包装器，强烈呼吁变长模板参数....
	* =====================================================================================
	*/
	template <class RetType>
	class COMMON_API DlgWrapper0
	{
	public:
		typedef RetType (*FuncPtrType) ();
		typedef TPtDlgCallback0<RetType> CallbackType;
		typedef DlgWrapper0<RetType> ThisType;

		/* ====================  LIFECYCLE     ======================================= */
		DlgWrapper0 (FuncPtrType pFunc) : m_pFunc(pFunc),m_pCallback(NULL)        /* constructor */
		{
		}

		DlgWrapper0 (CallbackType* pCallback) : m_pFunc(NULL),m_pCallback(pCallback)        /* constructor */
		{
			m_pCallback->AddRef();
		}

		DlgWrapper0(const DlgWrapper0& Other) : m_pFunc(Other.m_pFunc),m_pCallback(Other.m_pCallback)
		{
			if(m_pCallback)m_pCallback->AddRef();
		}

		~DlgWrapper0()
		{
			if(m_pCallback)m_pCallback->Release();
		}
		/* ====================  OPERATORS     ======================================= */
		inline RetType operator()()
		{
			return m_pCallback != NULL ? (*m_pCallback)() : (*m_pFunc)();
		}

		inline bool operator == (ThisType Other)
		{
			return this->m_pFunc == Other.m_pFunc && this->m_pCallback == Other.m_pCallback;
		}
	private:
		FuncPtrType m_pFunc;
		CallbackType* m_pCallback;

	}; /* -----  end of class  FuncWrapper  ----- */

	template <class Param1, class RetType>
	class COMMON_API DlgWrapper1
	{
	public:
		typedef RetType (*FuncPtrType) (Param1);
		typedef TPtDlgCallback1<Param1, RetType> CallbackType;
		typedef DlgWrapper1<Param1, RetType> ThisType;

		/* ====================  LIFECYCLE     ======================================= */
		DlgWrapper1 (FuncPtrType pFunc) : m_pFunc(pFunc),m_pCallback(NULL)        /* constructor */
		{
		}

		DlgWrapper1 (CallbackType* pCallback) : m_pFunc(NULL),m_pCallback(pCallback)        /* constructor */
		{
			m_pCallback->AddRef();
		}

		DlgWrapper1(const DlgWrapper1& Other) : m_pFunc(Other.m_pFunc),m_pCallback(Other.m_pCallback)
		{
			if(m_pCallback)m_pCallback->AddRef();
		}

		~DlgWrapper1()
		{
			if(m_pCallback)m_pCallback->Release();
		}
		/* ====================  OPERATORS     ======================================= */
		inline RetType operator()(Param1 p1)
		{
			return m_pCallback != NULL ? (*m_pCallback)(p1) : (*m_pFunc)(p1);
		}
		inline bool operator == (ThisType Other)
		{
			return this->m_pFunc == Other.m_pFunc && this->m_pCallback == Other.m_pCallback;
		}
	private:
		FuncPtrType m_pFunc;
		CallbackType* m_pCallback;

	}; /* -----  end of class  FuncWrapper  ----- */

	template <class Param1, class Param2, class RetType>
	class COMMON_API DlgWrapper2
	{
	public:
		typedef RetType (*FuncPtrType) (Param1, Param2);
		typedef TPtDlgCallback2<Param1, Param2, RetType> CallbackType;
		typedef DlgWrapper2<Param1, Param2, RetType> ThisType;

		/* ====================  LIFECYCLE     ======================================= */
		DlgWrapper2 (FuncPtrType pFunc) : m_pFunc(pFunc),m_pCallback(NULL)        /* constructor */
		{
		}

		DlgWrapper2 (CallbackType* pCallback) : m_pFunc(NULL),m_pCallback(pCallback)        /* constructor */
		{
			m_pCallback->AddRef();
		}

		DlgWrapper2(const DlgWrapper2& Other) : m_pFunc(Other.m_pFunc),m_pCallback(Other.m_pCallback)
		{
			if(m_pCallback)m_pCallback->AddRef();
		}

		~DlgWrapper2()
		{
			if(m_pCallback)m_pCallback->Release();
		}
		/* ====================  OPERATORS     ======================================= */
		inline RetType operator()(Param1 p1, Param2 p2)
		{
			return m_pCallback != NULL ? (*m_pCallback)(p1, p2) : (*m_pFunc)(p1, p2);
		}
		inline bool operator == (ThisType Other)
		{
			return this->m_pFunc == Other.m_pFunc && this->m_pCallback == Other.m_pCallback;
		}
	private:
		FuncPtrType m_pFunc;
		CallbackType* m_pCallback;

	}; /* -----  end of class  FuncWrapper  ----- */

	template <class Param1, class Param2, class Param3, class RetType>
	class COMMON_API DlgWrapper3
	{
	public:
		typedef RetType (*FuncPtrType) (Param1, Param2, Param3);
		typedef TPtDlgCallback3<Param1, Param2, Param3, RetType> CallbackType;
		typedef DlgWrapper3<Param1, Param2, Param3, RetType> ThisType;

		/* ====================  LIFECYCLE     ======================================= */
		DlgWrapper3 (FuncPtrType pFunc) : m_pFunc(pFunc),m_pCallback(NULL)        /* constructor */
		{
		}

		DlgWrapper3 (CallbackType* pCallback) : m_pFunc(NULL),m_pCallback(pCallback)        /* constructor */
		{
			m_pCallback->AddRef();
		}

		DlgWrapper3(const DlgWrapper3& Other) : m_pFunc(Other.m_pFunc),m_pCallback(Other.m_pCallback)
		{
			if(m_pCallback)m_pCallback->AddRef();
		}

		~DlgWrapper3()
		{
			if(m_pCallback)m_pCallback->Release();
		}
		/* ====================  OPERATORS     ======================================= */
		inline RetType operator()(Param1 p1, Param2 p2, Param3 p3)
		{
			return m_pCallback != NULL ? (*m_pCallback)(p1, p2, p3) : (*m_pFunc)(p1, p2, p3);
		}

		inline bool operator == (ThisType Other)
		{
			return this->m_pFunc == Other.m_pFunc && this->m_pCallback == Other.m_pCallback;
		}
	private:
		FuncPtrType m_pFunc;
		CallbackType* m_pCallback;

	}; /* -----  end of class  FuncWrapper  ----- */

	template <class Param1, class Param2, class Param3, class Param4, class RetType>
	class COMMON_API DlgWrapper4
	{
	public:
		typedef RetType (*FuncPtrType) (Param1, Param2, Param3, Param4);
		typedef TPtDlgCallback4<Param1, Param2, Param3, Param4, RetType> CallbackType;
		typedef DlgWrapper4<Param1, Param2, Param3, Param4, RetType> ThisType;

		/* ====================  LIFECYCLE     ======================================= */
		DlgWrapper4 (FuncPtrType pFunc) : m_pFunc(pFunc),m_pCallback(NULL)        /* constructor */
		{
		}

		DlgWrapper4 (CallbackType* pCallback) : m_pFunc(NULL),m_pCallback(pCallback)        /* constructor */
		{
			m_pCallback->AddRef();
		}

		DlgWrapper4(const DlgWrapper4& Other) : m_pFunc(Other.m_pFunc),m_pCallback(Other.m_pCallback)
		{
			if(m_pCallback)m_pCallback->AddRef();
		}

		~DlgWrapper4()
		{
			if(m_pCallback)m_pCallback->Release();
		}
		/* ====================  OPERATORS     ======================================= */
		inline RetType operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4)
		{
			return m_pCallback != NULL ? (*m_pCallback)(p1, p2, p3, p4) : (*m_pFunc)(p1, p2, p3, p4);
		}

		inline bool operator == (ThisType Other)
		{
			return this->m_pFunc == Other.m_pFunc && this->m_pCallback == Other.m_pCallback;
		}
	private:
		FuncPtrType m_pFunc;
		CallbackType* m_pCallback;

	}; /* -----  end of class  FuncWrapper  ----- */
	/*
	* =====================================================================================
	*        Class:  TPtManagedDlgBase
	*  Description:  TPtManagedDlg基类
	* =====================================================================================
	*/
	template < class DlgWrapperType>
	class COMMON_API TPtManagedDlgBase
	{
	public:
		/* ====================  LIFECYCLE     ==================== */
		TPtManagedDlgBase()                             /* constructor */
		{
		}

		~TPtManagedDlgBase()
		{
		}

		/* ====================  OPERATORS     ===================== */

		void operator -= (DlgWrapperType dlg)
		{
			m_WrapperListStruct.m_WrapperList.remove(dlg);
		}

		void operator += (DlgWrapperType dlg)
		{
			m_WrapperListStruct.m_WrapperList.push_back(dlg);
		}

		/* ====================  OPERATIONS    ===================== */
	protected:
		//恶心的VS，不这么写编译不过
		typedef typename std::list<DlgWrapperType> WrapperListType;
		struct WrapperListStruct
		{
			WrapperListType m_WrapperList;
		};
		WrapperListStruct m_WrapperListStruct;
	}; /* ----------  end of template class  TPtManagedDlgBase  ---------- */

	/*
	* =====================================================================================
	*        Class:  TPtManagedDlg
	*  Description:  资源托管的事件回调机制，在析构或回调返回false时会释放回调的资源，故同一个回调对象的实例不可挂载在多个事件对象上
	* =====================================================================================
	*/
	class COMMON_API TPtManagedDlg0 : public TPtManagedDlgBase<DlgWrapper0<int> >
	{
	public:
		typedef TPtDlgCallback0<int> CallbackType;
		typedef DlgWrapper0<int> DlgWrapperType;
		typedef TPtManagedDlgBase<DlgWrapperType> BaseType;


		/* ====================  LIFECYCLE     =========================== */
		TPtManagedDlg0() { }                    /* constructor */

		~TPtManagedDlg0() { }                   /* destructor */
		/* ====================  OPERATORS     =========================== */
		int operator() ()
		{
			int iRet = EDlgReturn_None;
			for (BaseType::WrapperListType::iterator it = this->m_WrapperListStruct.m_WrapperList.begin(); it != this->m_WrapperListStruct.m_WrapperList.end();)
			{
				int iLocalRet = (*it)();
				iRet |= iLocalRet;
				if (iLocalRet & EDlgReturn_Delete)
					this->m_WrapperListStruct.m_WrapperList.erase(it++);
				else
					++ it;
				if (iLocalRet & EDlgReturn_Break)
				{
					break;
				}
			}
			return iRet;
		}
	}; /* ----------  end of template class  TPtManagedDlg0  ---------- */

	template <class Param1>
	class COMMON_API TPtManagedDlg1 : public TPtManagedDlgBase<DlgWrapper1<Param1, int> >
	{
	public:
		typedef TPtDlgCallback1<Param1, int> CallbackType;
		typedef DlgWrapper1<Param1, int> DlgWrapperType;
		typedef TPtManagedDlgBase<DlgWrapperType> BaseType;


		/* ====================  LIFECYCLE     =========================== */
		TPtManagedDlg1() { }                    /* constructor */

		~TPtManagedDlg1() { }                   /* destructor */
		/* ====================  OPERATORS     =========================== */
		int operator() (Param1 p1)
		{
			int iRet = EDlgReturn_None;
			for (typename BaseType::WrapperListType::iterator it = this->m_WrapperListStruct.m_WrapperList.begin(); it != this->m_WrapperListStruct.m_WrapperList.end();)
			{
				int iLocalRet = (*it)(p1);
				iRet |= iLocalRet;
				if (iLocalRet & EDlgReturn_Delete)
					this->m_WrapperListStruct.m_WrapperList.erase(it++);
				else
					++ it;
				if (iLocalRet & EDlgReturn_Break)
				{
					break;
				}
			}
			return iRet;
		}
	}; /* ----------  end of template class  TPtManagedDlg1  ---------- */

	template <class Param1, class Param2>
	class COMMON_API TPtManagedDlg2 : public TPtManagedDlgBase<DlgWrapper2<Param1, Param2, int> >
	{
	public:
		typedef TPtDlgCallback2<Param1, Param2, int> CallbackType;
		typedef DlgWrapper2<Param1, Param2, int> DlgWrapperType;
		typedef TPtManagedDlgBase<DlgWrapperType> BaseType;

		/* ====================  LIFECYCLE     =========================== */
		TPtManagedDlg2() { }                    /* constructor */

		~TPtManagedDlg2() { }                   /* destructor */
		/* ====================  OPERATORS     =========================== */
		int operator() (Param1 p1, Param2 p2)
		{
			int iRet = EDlgReturn_None;
			for (typename BaseType::WrapperListType::iterator it = this->m_WrapperListStruct.m_WrapperList.begin(); it != this->m_WrapperListStruct.m_WrapperList.end();)
			{
				int iLocalRet = (*it)(p1, p2);
				iRet |= iLocalRet;
				if (iLocalRet & EDlgReturn_Delete)
					this->m_WrapperListStruct.m_WrapperList.erase(it++);
				else
					++ it;
				if (iLocalRet & EDlgReturn_Break)
				{
					break;
				}
			}
			return iRet;
		}
	}; /* ----------  end of template class  TPtManagedDlg2  ---------- */

	template <class Param1, class Param2, class Param3>
	class COMMON_API TPtManagedDlg3 : public TPtManagedDlgBase<DlgWrapper3<Param1, Param2, Param3, int> >
	{
	public:
		typedef TPtDlgCallback3<Param1, Param2, Param3, int> CallbackType;
		typedef DlgWrapper3<Param1, Param2, Param3, int> DlgWrapperType;
		typedef TPtManagedDlgBase<DlgWrapperType> BaseType;

		/* ====================  LIFECYCLE     =========================== */
		TPtManagedDlg3() { }                    /* constructor */

		~TPtManagedDlg3() { }                   /* destructor */
		/* ====================  OPERATORS     =========================== */
		int operator() (Param1 p1, Param2 p2, Param3 p3)
		{
			int iRet = EDlgReturn_None;
			for (typename BaseType::WrapperListType::iterator it = this->m_WrapperListStruct.m_WrapperList.begin(); it != this->m_WrapperListStruct.m_WrapperList.end();)
			{
				int iLocalRet = (*it)(p1, p2, p3);
				iRet |= iLocalRet;
				if (iLocalRet & EDlgReturn_Delete)
					this->m_WrapperListStruct.m_WrapperList.erase(it++);
				else
					++ it;
				if (iLocalRet & EDlgReturn_Break)
				{
					break;
				}
			}
			return iRet;
		}
	}; /* ----------  end of template class  TPtManagedDlg3  ---------- */

	template <class Param1, class Param2, class Param3, class Param4>
	class COMMON_API TPtManagedDlg4 : public TPtManagedDlgBase<DlgWrapper4<Param1, Param2, Param3, Param4, int> >
	{
	public:
		typedef TPtDlgCallback4<Param1, Param2, Param3, Param4, int> CallbackType;
		typedef DlgWrapper4<Param1, Param2, Param3, Param4, int> DlgWrapperType;
		typedef TPtManagedDlgBase<DlgWrapperType> BaseType;

		/* ====================  LIFECYCLE     =========================== */
		TPtManagedDlg4() { }                    /* constructor */

		~TPtManagedDlg4() { }                   /* destructor */
		/* ====================  OPERATORS     =========================== */
		int operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4)
		{
			int iRet = EDlgReturn_None;
			for (typename BaseType::WrapperListType::iterator it = this->m_WrapperListStruct.m_WrapperList.begin(); it != this->m_WrapperListStruct.m_WrapperList.end();)
			{
				int iLocalRet = (*it)(p1, p2, p3, p4);
				iRet |= iLocalRet;
				if (iLocalRet & EDlgReturn_Delete)
					this->m_WrapperListStruct.m_WrapperList.erase(it++);
				else
					++ it;
				if (iLocalRet & EDlgReturn_Break)
				{
					break;
				}
			}
			return iRet;
		}
	}; /* ----------  end of template class  TPtManagedDlg4  ---------- */
}
