/*!
ServerObj defines all the necessary things for writing an object.
*/

#ifndef __SERVEROBJ_H__
#define __SERVEROBJ_H__

#include "Property.h"

/*!
Class flags.
*/
enum
{
	//! Instances of the class can't be created in MapEditor, 比如CPlayer.
	CF_HIDDEN =       (1<<0),

	/*!
	This class doesn't get used at runtime (the engine
	won't instantiate these objects out of the world file).
	比如NPC的基类*/
	CF_NORUNTIME =    (1<<1),

	/*!
	This is a special class that the server creates an
	instance of at the same time that it creates the
	server shell.  The object is always around.  This
	should be used as much as possible instead of adding
	code to the server shell.
	*/
	CF_STATIC =       (1<<2),

	/*!
	Objects of this class do not have an associated HOBJECT
	*/
	CF_CLASSONLY =	  (1<<3),

	/*
	*	此类别与地图、区域、格子相关联
	*/
	CF_ATTACHTOMAP	= (1 << 4),

	/*
	*	此类别需要最大量的Cache
	*/
	CF_MAXCACHE		= (1 << 5)
};

/*!
Used to avoid crashes from version mismatches.
*/
#define _OBJ_VERSION 1

/*!
These are your (optional) construct/destruct functions.
In C++, you should always use the default ones, because they
will call your constructor/destructor.  In C, you can use
them to init and term your object.  In most cases, you should
add any aggregates you have in your construct function, since
ReadProp/PostPropRead get called right after and your aggregates
might want to look at them.
*/
typedef void (*ConstructObjectFn)(void *pObject);
typedef void (*DestructObjectFn)(void *pObject);

inline void* operator new(size_t size, void *ptr, int asdf, char a)
{
	return ptr;
}

inline void operator delete(void *pDataPtr, void *ptr, int asdf, char a)
{
}

/*!
Callbacks used for creating plugins.
*/
class IObjectPlugin;
typedef IObjectPlugin* (*CreateObjectPluginFn)();

// 开始 类的定义，申明了类属性的数组，将被记录到ClassDef中
#define BEGIN_CLASS(name) \
	static PropDef  _##name##_Props__[] = { \
	ADD_STRINGPROP("__NOPROP__!!", "")

// 结束 类的定义
/*!
End class macros with a plugin interface.
Just specify the plugin class name.  The class must derive from IObjectPlugin.
*/
#define END_CLASS_PLUGIN(name, parent, flags, cacheobjcount, pluginClassName)\
	}; \
	extern ClassDef _##parent##_Class__; \
	IObjectPlugin* __Create##name##__Plugin() {return new pluginClassName;}\
	DO_DEFAULT_FUNCTIONS(name)\
	END_CLASS_SYMBOL(name, &_##parent##_Class__, flags, cacheobjcount, Default##name##Constructor, Default##name##Destructor, 0, __Create##name##__Plugin)

/*!
End class macros with flags.
*/
#define END_CLASS(name, parent, flags, cacheobjcount)\
	}; \
	extern ClassDef _##parent##_Class__; \
	DO_DEFAULT_FUNCTIONS(name)\
	END_CLASS_SYMBOL(name, &_##parent##_Class__, flags, cacheobjcount, Default##name##Constructor, Default##name##Destructor, 0, NULL)


/*!
Only used internally.
*/
#define END_CLASS_NOPARENT(name) \
	}; \
	DO_DEFAULT_FUNCTIONS(name)\
	END_CLASS_SYMBOL(name, (ClassDef*)0, 0, 0, Default##name##Constructor, Default##name##Destructor, 0, NULL)


/*!
Define the default constructor/destructor functions.
*/

#define DO_AUTO_CLASSLIST(name) \
	static __ClassDefiner __##name##_definer(&_##name##_Class__);

#define DO_DEFAULT_FUNCTIONS(_className)\
	void Default##_className##Constructor(void *ptr)\
	{\
		::new(ptr, (int)0, (char)0) _className;\
	}\
	void Default##_className##Destructor(void *ptr)\
	{\
		_className *thePtr = (_className*)ptr;\
		thePtr->~_className();\
	}


#define END_CLASS_SYMBOL(name, parentSymbol, flags, cacheobjcount, construct_fn, destruct_fn, classFlags, lightFn) \
	ClassDef _##name##_Class__ = {													\
	#name, parentSymbol,															\
	flags,																			\
	construct_fn, destruct_fn,														\
	lightFn,																		\
	(sizeof(_##name##_Props__) / sizeof(PropDef)) - 1,								\
	&_##name##_Props__[1],															\
	sizeof(name),																	\
	cacheobjcount,\
	(void*)0, (void*)0 };															\
	DO_AUTO_CLASSLIST(name)															\
	/* Declare a global instance of our implementation class. */                    \
	/* It is not static so that we will get link errors if two instances */         \
	/* are created with the same instance name. */                                  \
	int32 __impl_instance_##name##__;                  

#define link_to_object(name)														\
	extern int32 __impl_instance_##name##__;                                        \
	int32* __impl_instance_##name##_Ptr__ = &__impl_instance_##name##__;



/*!
Forward declaration of class DEdit internally augments \b PropDef data with
*/
class DEditInternal;

struct PropDef 
{
	// 构造函数
	PropDef(char *pName, short type, LTVector valVec,
		float valFloat, char *valString, unsigned long propFlags)
	{
		m_PropName				= pName;
		m_PropType				= type;
		m_DefaultValueVector	= valVec;
		m_DefaultValueFloat		= valFloat;
		m_DefaultValueString	= valString;
		m_PropFlags				= propFlags;
		m_pInternal				= 0;
		m_pDEditInternal		= 0;
	}
	
	//  属性的名字
	char *m_PropName;
	// One of the PT_ defines above.
	short m_PropType;
	// Default vector/color value.
	LTVector m_DefaultValueVector;
	// 缺省的浮点值
	float m_DefaultValueFloat;
	// 属性的字符串值
	char* m_DefaultValueString;
	// PF_的组合
	unsigned long m_PropFlags;

	// 这两个属性是给编辑器用的， 根据编辑器得需求做一定得修改
	// Don't touch!
	DEditInternal* m_pDEditInternal;
	// Don't touch!
	void* m_pInternal;
};

// 编辑器中定义的类，需要修改
class CProjectClass;

// 类的描述
struct ClassDef 
{
public:
	// 这两个函数是编辑器中使用的， 根据编辑器得需求做一定得修改
	CProjectClass *DEditGetClassHook() 
	{
		return (CProjectClass*)m_pInternal[0];
	}

	void DEditSetClassHook(CProjectClass *pClass) 
	{
		m_pInternal[0] = pClass;
	}

public:
	// 类的名字
	char*		m_ClassName;
	// 父类的描述
	ClassDef*	m_ParentClass;
	// A combination of the CF_ flags above.
	uint32		m_ClassFlags;
	// 类的构造函数
	ConstructObjectFn m_ConstructFn;
	// 类的析构函数
	DestructObjectFn m_DestructFn;
	// 相关的插件函数
	CreateObjectPluginFn m_PluginFn;
	// 类中有多少个属性
	short		m_nProps;
	// 类中的属性列表
	PropDef*	m_Props;
	// How big an object of this class is (set automatically).
	long		m_ClassObjectSize;
	// 此类的对象最多有多少个. 预先分配内存
	uint32		m_nCacheObjtCount;

	// 引擎和编辑器中都要用到，根据不同得需求做修改
	void* m_pInternal[2];
};


/*!
 使用这个方法将所有的ClassDef自动串接起来
*/

class __ClassDefiner;
extern __ClassDefiner *__g_ClassDefinerHead;
extern __ClassDefiner *__g_ClassDefinerEnd;

class __ClassDefiner
{
public:

	__ClassDefiner(ClassDef *pDef)
	{
		m_pClass	= pDef;
		m_pNext		= NULL;

		if(0 == __g_ClassDefinerHead)
		{
			m_pNext		= __g_ClassDefinerHead;
			__g_ClassDefinerHead = this;
			__g_ClassDefinerEnd = __g_ClassDefinerHead;
		}
		else
		{
			// 将需要绑定到地图的类，放在列表的最前部
			if(m_pClass->m_ClassFlags & CF_ATTACHTOMAP)
			{
				m_pNext		= __g_ClassDefinerHead;
				__g_ClassDefinerHead = this;

			}
			else
			{
				__g_ClassDefinerEnd->m_pNext = this;
				__g_ClassDefinerEnd = this;
			}
		}	
	}

	ClassDef*		m_pClass;
	__ClassDefiner*	m_pNext;
};

extern ClassDef**	__g_cpp_classlist;
// 利用全局变量得析构，自动释放分配得内存
class __cpp_classlist_auto_free 
{
public:
	~__cpp_classlist_auto_free() 
	{
		if (__g_cpp_classlist) 
		{
			free(__g_cpp_classlist);
			__g_cpp_classlist = 0;
		}
	}
};


#ifdef _WIN32
	#define DEFINE_CLASSES()									\
	ClassDef **__g_cpp_classlist=0;								\
	__cpp_classlist_auto_free __free_the_g_cpp_classlist;		\
	extern "C" __declspec(dllexport) ClassDef** ObjectDLLSetup(int *nDefs,int *version); \
	__ClassDefiner *__g_ClassDefinerHead=0;						\
	__ClassDefiner *__g_ClassDefinerEnd=0;						\
	ClassDef** ObjectDLLSetup(int *nDefs, int *version)			\
	{															\
		int nClasses;											\
		__ClassDefiner *pCurDefiner;							\
		*version = _OBJ_VERSION;								\
		nClasses=0;												\
		pCurDefiner = __g_ClassDefinerHead;						\
		while (pCurDefiner)										\
		{														\
			pCurDefiner = pCurDefiner->m_pNext;					\
			++nClasses;											\
		}														\
		__g_cpp_classlist = (ClassDef**)malloc(sizeof(ClassDef*) * nClasses);\
		nClasses=0;\
		pCurDefiner = __g_ClassDefinerHead;\
		while (pCurDefiner)\
		{\
			__g_cpp_classlist[nClasses] = pCurDefiner->m_pClass;\
			pCurDefiner = pCurDefiner->m_pNext;\
			++nClasses;\
		}\
		*nDefs = nClasses; \
		return __g_cpp_classlist; \
	}
#else
#define DEFINE_CLASSES() \
	ClassDef **__g_cpp_classlist=0;\
	__cpp_classlist_auto_free __free_the_g_cpp_classlist;\
  	ClassDef** ObjectDLLSetup(int *nDefs, int *version); \
	__ClassDefiner *__g_ClassDefinerHead=0;\
	ClassDef** ObjectDLLSetup(int *nDefs, int *version) \
	{\
		int nClasses;\
		__ClassDefiner *pCurDefiner;\
		*version = _OBJ_VERSION;\
		
		nClasses=0;\
		pCurDefiner = __g_ClassDefinerHead;\
		while (pCurDefiner)\
		{\
			pCurDefiner = pCurDefiner->m_pNext;\
			++nClasses;\
		}\
		__g_cpp_classlist = (ClassDef**)malloc(sizeof(ClassDef*) * nClasses);\
		nClasses=0;\
		pCurDefiner = __g_ClassDefinerHead;\
		while (pCurDefiner)\
		{\
			__g_cpp_classlist[nClasses] = pCurDefiner->m_pClass;\
			pCurDefiner = pCurDefiner->m_pNext;\
			++nClasses;\
		}\
		*nDefs = nClasses; \
		return __g_cpp_classlist; \
	}
#endif // _WIN32


/*!
 
Property flags.
*/
enum
{
//! Property doesn't show up in DEdit.
    PF_HIDDEN =           (1<<0),

//! Property is a number to use as radius for drawing circle.  There can be more than one.
    PF_RADIUS =           (1<<1),

//! Property is a vector to use as dimensions for drawing box. There can be only one.
    PF_DIMS =             (1<<2),

//! Property is a field of view.
    PF_FIELDOFVIEW =      (1<<3),

//! Used with \b PF_DIMS.  Causes DEdit to show dimensions rotated with the object.
    PF_LOCALDIMS =        (1<<4),

//! This property owns the group it's in.
    PF_GROUPOWNER =       (1<<5),

	// NOTE: Bits (1<<6) through (1<<11) are reserved for the group number as described below...

//! If \b PF_FIELDOFVIEW is set, this defines the radius for it.
    PF_FOVRADIUS =        (1<<12),

/*!
If the object is selected, DEdit draws a line to any objects
referenced (by name) in \b PF_OBJECTLINK properties.  It won't
draw any more than \b MAX_OBJECTLINK_OBJECTS.
*/
    PF_OBJECTLINK =       (1<<13),

//! This indicates to DEdit that a string property is a filename in the resource.
    PF_FILENAME =         (1<<14),

/*!
If this property is a vector and its object is on a path,
the path is drawn as a bezier curve.  The curve segment from
this object to the next is defined as
(THIS.Pos, THIS.Pos + THIS.NextTangent, NEXT.Pos + NEXT.PrevTangent, NEXT.Pos).
*/
    PF_BEZIERPREVTANGENT =        (1<<15) ,
    PF_BEZIERNEXTTANGENT =        (1<<16),

//! This string property has a populatable combobox with dropdown-list style (ie listbox, no edit control)
    PF_STATICLIST =       (1<<17),

//! This string property has a populatable combobox with dropdown style (ie listbox+edit control)
    PF_DYNAMICLIST =      (1<<18),

/*!
This is a composite property with a custom dedit control that
incorporates all group members.  This flag notifies DEdit that
it should look for a custom control that matches the type name
of this property.
*/
    PF_COMPOSITETYPE =    (1<<19),

/*!
This property defines a measurement or other value that is in,
or relative to, world coordinates.  If this flag is specified,
any scaling done to the world as a whole for unit conversion
will also be applied to this property.
*/
    PF_DISTANCE =         (1<<20),

/*!
This property defines the filename of a model to be displayed
for the object. This is usually used in conjunction with PF_FILENAME.
If the path is not absolute, it will append the filename to the
project directory. If no extension, or LTB is provided, it will look
first for an LTA file, then an LTC.
*/
    PF_MODEL =			(1<<21),

/*!
This property defines that the associated vector should be used
to render an orthographic frustum from the object. This frustum will
have the width and height specified in the X and Y values of the vector
and the far clip plane specified in the Z value.
*/
    PF_ORTHOFRUSTUM =	(1<<22),

/*!
If this property changes, PreHook_PropChanged() will be called to give it's object
a chance to debug the new value.
*/

	PF_NOTIFYCHANGE	=	(1<<23),

/*!
This value specifies that the group should be treated as an event, and use the appropriate
event editor dialog to edit the fields.
*/

	PF_EVENT		=	(1<<24),

/*!
This value specifies that the field is the name of a texture script group and should allow the user
to select from their premade texture scripts or create new ones.
*/

	PF_TEXTUREEFFECT	=	(1<<25),

};


//the number of bits we have allocated for the groups (0 is not a valid group)
#define NUM_GROUP_BITS		6

//starting bit to store the groups in
#define FIRST_GROUP_BIT		6

//helper macro to specify the appropriate group
#define PF_GROUP(GroupNum)	(((GroupNum) & ((1 << NUM_GROUP_BITS) - 1)) << FIRST_GROUP_BIT)

//mask for pulling out all the group bits
#define PF_GROUPMASK		(((1 << NUM_GROUP_BITS) - 1) << FIRST_GROUP_BIT)

//helper macro to get the group out of a flag
#define GET_PF_GROUP(Flags)	((Flags & PF_GROUPMASK) >> FIRST_GROUP_BIT)
    



/*!
It adds a dummy property here so C syntax doesn't break when you have no properties.
*/
#define NOCOLOR LTVector(0.0f, 0.0f, 0.0f)

/*!
If you want level designers to be able to set object flags, you should
use these macros so the property names are standardized.
*/

/*!
Use the \b ADD_X_FLAG macros in your property list. 
Note that the name of the Prop Flag added cannot have spaces, or ButeMgr breaks.
*/
#define ADD_VISIBLE_FLAG(defVal, flags) \
    ADD_PROP_FLAG(Visible, PT_BOOL, defVal, 0, flags)

#define ADD_SHADOW_FLAG(defVal, flags) \
    ADD_PROP_FLAG(Shadow, PT_BOOL, defVal, 0, flags)

#define ADD_ROTATABLESPRITE_FLAG(defVal, flags) \
    ADD_PROP_FLAG(RotatableSprite, PT_BOOL, defVal, 0, flags)

#define ADD_SOLID_FLAG(defVal, flags) \
    ADD_PROP_FLAG(Solid, PT_BOOL, defVal, 0, flags)

#define ADD_GRAVITY_FLAG(defVal, flags) \
   ADD_PROP_FLAG(Gravity, PT_BOOL, defVal, 0, flags)

#define ADD_TOUCHNOTIFY_FLAG(defVal, flags) \
    ADD_PROP_FLAG(TouchNotify, PT_BOOL, defVal, 0, flags)

#define ADD_RAYHIT_FLAG(defVal, flags) \
    ADD_PROP_FLAG(Rayhit, PT_BOOL, defVal, 0, flags)

/*!
Full property definitions.
*/
#define ADD_PROP_FLAG(name, type, valFloat, valString, flags) \
    PropDef(#name, type, NOCOLOR, valFloat, valString, flags),

#define ADD_REALPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_REAL, NOCOLOR, val, "", flags),

#define ADD_STRINGPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_STRING, NOCOLOR, 0.0f, val, flags),

#define ADD_VECTORPROP_FLAG(name, flags) \
    PropDef(#name, PT_VECTOR, NOCOLOR, 0.0f, (char*)0, flags),

#define ADD_VECTORPROP_VAL_FLAG(name, defX, defY, defZ, flags) \
    PropDef(#name, PT_VECTOR, LTVector(defX, defY, defZ), 0.0f, (char*)0, flags),

#define ADD_LONGINTPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_LONGINT, NOCOLOR, (float)val, (char*)0, flags),

#define ADD_ROTATIONPROP_FLAG(name, flags) \
    PropDef(#name, PT_ROTATION, NOCOLOR, 0.0f, (char*)0, flags),

#define ADD_BOOLPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_BOOL, NOCOLOR, (float)val, (char*)0, flags),

#define ADD_COLORPROP_FLAG(name, valR, valG, valB, flags) \
    PropDef(#name, PT_COLOR, LTVector(valR, valG, valB), 0.0f, (char*)0, flags),

#define ADD_OBJECTPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_STRING, NOCOLOR, 0.0f, val, flags | PF_OBJECTLINK),

/*!
Add properties without flags (only here for backward compatibility).
*/
#define ADD_PROP(name, type, valFloat, valString) \
    ADD_PROP_FLAG(name, type, valFloat, valString, 0)

#define ADD_REALPROP(name, val) \
    ADD_REALPROP_FLAG(name, val, 0)

#define ADD_STRINGPROP(name, val) \
    ADD_STRINGPROP_FLAG(name, val, 0)

#define ADD_VECTORPROP(name) \
    ADD_VECTORPROP_FLAG(name, 0)

#define ADD_VECTORPROP_VAL(name, defX, defY, defZ) \
    ADD_VECTORPROP_VAL_FLAG(name, defX, defY, defZ, 0)

#define ADD_LONGINTPROP(name, val) \
    ADD_LONGINTPROP_FLAG(name, val, 0)

#define ADD_ROTATIONPROP(name) \
    ADD_ROTATIONPROP_FLAG(name, 0)

#define ADD_BOOLPROP(name, val) \
    ADD_BOOLPROP_FLAG(name, val, 0)

#define ADD_COLORPROP(name, valR, valG, valB) \
    ADD_COLORPROP_FLAG(name, valR, valG, valB, 0)

#define ADD_OBJECTPROP(name, val) \
    ADD_OBJECTPROP_FLAG(name, val, 0)

/*!
Define a group with this.
*/
#define PROP_DEFINEGROUP(groupName, groupFlag) \
    PropDef(#groupName, PT_LONGINT, NOCOLOR, (float)0.0f, (char*)0, groupFlag|PF_GROUPOWNER),

/*!
Define a composite type with this.
*/
#define PROP_DEFINECOMPOSITETYPE(typeName, groupFlag) \
    PropDef(#typeName, PT_LONGINT, NOCOLOR, (float)0.0f, (char*)0, groupFlag|PF_COMPOSITETYPE|PF_GROUPOWNER),


#endif  //! __SERVEROBJ_H__
 

 



