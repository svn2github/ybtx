#ifndef ___EFFECTCLASS_H___
#define ___EFFECTCLASS_H___

#include "EffectBase.h"
#include <map>
#include <string>
#include "IEffectUnit.h"
#include "IEffectClassDesc.h"

typedef   IEffectUnit*		(*LPCREATE_EFFECT_FUN)( IEffectUnitProp* pProp );
typedef   IEffectUnitProp*	(*LPCREATE_EFFECTPROP_FUN)( IEffectClassDesc* pClassDesc );



class EFFECT_API CEffectClass : public CEffectMallocObject
{
	EString				m_ClassName;
	CEffectClass*		m_pParent;
	LPCREATE_EFFECT_FUN m_pCreateFun;
public:

	static IEffectUnit* CreateEffectUnit( IEffectUnitProp* pProp );
	CEffectClass( const char* szClassName, LPCREATE_EFFECT_FUN pFun, CEffectClass* pParentClass );
	~CEffectClass(void);
	bool FindParentClass( const char* ) const;
};

class EFFECT_API CEffectClassEx : public CEffectClass
{
	WORD					m_EffectUnitNameResID;
	LPCREATE_EFFECTPROP_FUN m_pEffectUnitPropCreateFun;
public:

	static IEffectUnitProp* CreateEffectUnitProp( EBaseEffectUnit eBaseType, IEffectClassDesc* pClassDesc );
	static const char*		GetEffectName( EBaseEffectUnit eBaseType );
	static CEffectClass*	GetEffectClass( EBaseEffectUnit eBaseType );
	CEffectClassEx( EBaseEffectUnit eBaseType, WORD uResID, const char* szClassName, 
		LPCREATE_EFFECT_FUN pEffectFun, LPCREATE_EFFECTPROP_FUN pEffectPropFun );
};


#define DYNAMIC_DECLARE_EFFECTUNITBASE( class_name )\
public: \
	static IEffectUnit*		CreateEffectUnit( IEffectUnitProp* pProp ); \
	static IEffectUnitProp* CreateEffectUnitProp( IEffectClassDesc* pClassDesc ); \
public: \
	static CEffectClassEx*   class##class_name; \
	EBaseEffectUnit	GetEffectUniType(void);\


#define DYNAMIC_CREATE_EFFECTUNITBASE(class_type, class_resID, class_name, class_prop_name) \
	CEffectClassEx* class_name::class##class_name;\
	IEffectUnit* class_name::CreateEffectUnit( IEffectUnitProp* pProp ){ return new class_name( pProp ); };\
	IEffectUnitProp* class_name::CreateEffectUnitProp( IEffectClassDesc* pClassDesc ){ return new class_prop_name( class_type, pClassDesc ); }; \
	EBaseEffectUnit class_name::GetEffectUniType(void ) { return class_type; }\

#define DYNAMIC_REGISTER_EFFECTUNITBASE(class_type, class_resID, class_name, class_prop_name) \
	class_name::class##class_name =new CEffectClassEx(class_type, class_resID, #class_name, class_name::CreateEffectUnit, class_name::CreateEffectUnitProp );\


#define DYNAMIC_UNREGISTER_EFFECTUNITBASE(class_name) \
	GfkSafeDelete(class_name::class##class_name);\

#define DYNAMIC_DECLARE_EFFECTUNIT(class_name)\
private: \
	static IEffectUnit* CreateEffectUnit( IEffectUnitProp* pProp ); \
	virtual bool FindParentClass( const char* ) const;\
public: \
	static CEffectClass class##class_name; \


#define DYNAMIC_CREATE_EFFECTUNIT(class_name, parent_class_name) \
	CEffectClass class_name::class##class_name = CEffectClass(#class_name, CreateEffectUnit, &parent_class_name::class##parent_class_name );\
	IEffectUnit* class_name::CreateEffectUnit( IEffectUnitProp* pProp ){ return new class_name( pProp ); };\
	bool class_name::FindParentClass( const char* szClassName ) const { return class##class_name.FindParentClass( szClassName ); };\


#endif