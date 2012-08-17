#pragma once

#include "GUIBase.h"

namespace sqr
{

	typedef   SQRWnd* (*LPREGISTER_FUN)();

	class GUI_API SQRWndClass
	{
		static UIMap< UIString, LPREGISTER_FUN >* ms_pWndCreateMap;
	public:
		UIString		m_ClassName;
		SQRWndClass*	m_pParent;

		static SQRWnd* CreateWndObject( const char* pszWndClass );
		SQRWndClass( const char* pszWndClass, LPREGISTER_FUN lpCreatefun, SQRWndClass* pParentClass );
		~SQRWndClass();
		bool FindParentClass( const char* ) const;
	};

#define DYNAMIC_DECLARE_WND(class_name)\
private: \
	static SQRWnd* CreateClassObject(); \
public: \
	static SQRWndClass	class##class_name; \
	static StyleStruct* Style##class_name; \
	virtual const char* GetClassName() const;\
	virtual StyleStruct* GetStyleStruct() const; \
	virtual bool FindParentClass( const char* ) const;\
private:


#define DYNAMIC_CREATE_WNDBASE(class_name) \
	SQRWndClass class_name::class##class_name = SQRWndClass(#class_name, CreateClassObject, NULL );\
	StyleStruct* class_name::Style##class_name = NULL;\
	SQRWnd* class_name::CreateClassObject(){ return new class_name; };\
	const char* class_name::GetClassName() const { return class##class_name.m_ClassName.c_str(); };\
	StyleStruct* class_name::GetStyleStruct() const { return Style##class_name; };\
	bool class_name::FindParentClass( const char* szClassName ) const { return class##class_name.FindParentClass( szClassName ); };


#define DYNAMIC_CREATE_WND(class_name, parent_class_name) \
	SQRWndClass class_name::class##class_name = SQRWndClass(#class_name, CreateClassObject, &parent_class_name::class##parent_class_name );\
	StyleStruct* class_name::Style##class_name = NULL;\
	SQRWnd* class_name::CreateClassObject(){ return new class_name; };\
	const char* class_name::GetClassName() const { return class##class_name.m_ClassName.c_str(); };\
	StyleStruct* class_name::GetStyleStruct() const { return Style##class_name; };\
	bool class_name::FindParentClass( const char* szClassName ) const { return class##class_name.FindParentClass( szClassName ); };

}

;
