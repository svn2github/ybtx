/*!
ClassDefine.h defines all the necessary things for writing an Class.
*/

#ifndef __CLASSDEFINE_H__
#define __CLASSDEFINE_H__

#include "ClassProperty.h"

// DirectObject 标志
#define		_DIRECTOBJ_EXT		"DO"
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

	/*
	*	此类别与地图、区域、格子相关联
	*/
	CF_ATTACHTOMAP	= (1<<2),

	/*
	*	此类别需要最大量的Cache
	*/
	CF_MAXCACHE		= (1<<3),
};

/*
 *	类的类别
 */

#ifdef GAS
/*
 *	不对应客户端对象
 */
#define CC_NOCLIENTCLASS			0
/*
*	对应一个客户端的主动对象类类，这个标志只对服务端的仲裁对象类起作用
*/
#define	CC_BINDDIRECTORCLASS	(1<<0)

/*
*   对应一个客户端的被动对象类类，这个标志只对服务端的仲裁对象类起作用
*/
#define	CC_BINDFOLLOWERCLASS	(1<<1)

#else
#ifdef GAC
/*
*	客户端的主动对象
*/
#define	CC_DIRECTORCLASS		1
/*
*	客户端的被动对象
*/
#define	CC_FOLLOWERCLASS		2
/*
*	客户端的
*/
#define	CC_PERFORMERCLASS		3
#endif
#endif

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

inline void* operator new(size_t /*size*/, void* ptr, int /*asdf*/, char /*a*/)
{
	return ptr;
}

inline void operator delete(void* /*pDataPtr*/, void* /*ptr*/, int /*asdf*/, char /*a*/)
{
}

// 开始 类的定义，申明了类属性的数组，将被记录到ClassDef中,第一个属性将不会被使用
#define BEGIN_CLASS(name) \
	static PropDef  _##name##_Props__[] = { \
	ADD_STRINGPROP("__NOPROP__!!", "")

// 结束 类的定义
/*!
End class macros with flags.
*/
#ifdef GAC
#define END_DIRECTOR_CLASS(name, parent, serverclass, flags, cacheobjcount)\
		}; \
		extern ClassDef _##parent##_Class__; \
		DO_DEFAULT_FUNCTIONS(name)\
		END_CLASS_SYMBOL(name, &_##parent##_Class__, serverclass, flags|CF_HIDDEN, CC_DIRECTORCLASS,cacheobjcount, Default##name##Constructor, Default##name##Destructor, 0, NULL)


/*!
Only used internally.
*/
#define END_DIRECTOR_CLASS_NOPARENT(name) \
		}; \
		DO_DEFAULT_FUNCTIONS(name)\
		END_CLASS_SYMBOL(name, (ClassDef*)0, 0, CF_HIDDEN|CF_NORUNTIME, CC_DIRECTORCLASS, 0, Default##name##Constructor, Default##name##Destructor, 0, NULL)


#define END_FOLLOWER_CLASS(name, parent, serverclass, flags, cacheobjcount)\
		}; \
		extern ClassDef _##parent##_Class__; \
		DO_DEFAULT_FUNCTIONS(name)\
		END_CLASS_SYMBOL(name, &_##parent##_Class__, serverclass, flags|CF_HIDDEN, CC_FOLLOWERCLASS, cacheobjcount, Default##name##Constructor, Default##name##Destructor, 0, NULL)


/*!
Only used internally.
*/
#define END_FOLLOWER_CLASS_NOPARENT(name) \
		}; \
		DO_DEFAULT_FUNCTIONS(name)\
		END_CLASS_SYMBOL(name, (ClassDef*)0, 0, CF_HIDDEN|CF_NORUNTIME, CC_FOLLOWERCLASS, 0, Default##name##Constructor, Default##name##Destructor, 0, NULL)


#define END_PERFORMER_CLASS(name, parent, flags, cacheobjcount)\
		}; \
		extern ClassDef _##parent##_Class__; \
		DO_DEFAULT_FUNCTIONS(name)\
		END_CLASS_SYMBOL(name, &_##parent##_Class__, 0, flags, CC_PERFORMERCLASS, cacheobjcount, Default##name##Constructor, Default##name##Destructor, 0, NULL)


/*!
Only used internally.
*/
#define END_PERFORMER_CLASS_NOPARENT(name) \
		}; \
		DO_DEFAULT_FUNCTIONS(name)\
		END_CLASS_SYMBOL(name, (ClassDef*)0, 0, CF_HIDDEN|CF_NORUNTIME, CC_PERFORMERCLASS, 0, Default##name##Constructor, Default##name##Destructor, 0, NULL)

#else
#ifdef GAS
#define END_MEDIATOR_CLASS(name, parent, flags, type, cacheobjcount)\
			}; \
			extern ClassDef _##parent##_Class__; \
			DO_DEFAULT_FUNCTIONS(name)\
			END_CLASS_SYMBOL(name, &_##parent##_Class__, flags, type, cacheobjcount, Default##name##Constructor, Default##name##Destructor, 0, NULL)


/*!
Only used internally.
*/
#define END_MEDIATOR_CLASS_NOPARENT(name) \
			}; \
			DO_DEFAULT_FUNCTIONS(name)\
			END_CLASS_SYMBOL(name, (ClassDef*)0, CF_HIDDEN|CF_NORUNTIME, 0, 0, Default##name##Constructor, Default##name##Destructor, 0, NULL)

// huh...
#define END_PROXY_CLASS(name, flags, type, cacheobjcount)\
			}; \
			DO_DEFAULT_FUNCTIONS(name)\
			END_CLASS_SYMBOL(name, (ClassDef*)0, flags, type, cacheobjcount, Default##name##Constructor, Default##name##Destructor, 0, NULL)

#endif
#endif

/*!
Define the default constructor/destructor functions.
*/

#define DO_AUTO_CLASSLIST(name) \
	static __ClassDefiner __##name##_definer(&_##name##_Class__);

#define DO_DEFAULT_FUNCTIONS(_className)\
	void Default##_className##Constructor(void *ptr)\
	{\
		::new(ptr, (int)0, (char)0) _className();\
	}\
	void Default##_className##Destructor(void *ptr)\
	{\
		_className *thePtr = (_className*)ptr;\
		thePtr->~_className();\
	}

#ifdef GAC
#define END_CLASS_SYMBOL(name, parentSymbol, serverclass, flags, type, cacheobjcount, construct_fn, destruct_fn, classFlags, lightFn) \
		ClassDef _##name##_Class__ = {													\
		#name, parentSymbol,															\
		#serverclass,																	\
		flags,																			\
		type,																			\
		construct_fn, destruct_fn,														\
		(sizeof(_##name##_Props__) / sizeof(PropDef)) - 1,								\
		&_##name##_Props__[1],															\
		sizeof(name),																	\
		cacheobjcount};																	\
		DO_AUTO_CLASSLIST(name)															\
		/* Declare a global instance of our implementation class. */                    \
		/* It is not static so that we will get link errors if two instances */         \
		/* are created with the same instance name. */                                  \
		int32 __impl_instance_##name##__;
#else
#ifdef GAS
#define END_CLASS_SYMBOL(name, parentSymbol, flags, type, cacheobjcount, construct_fn, destruct_fn, classFlags, lightFn) \
			ClassDef _##name##_Class__ = {													\
			#name, parentSymbol,															\
			NULL,																			\
			flags,																			\
			type,																			\
			construct_fn, destruct_fn,														\
			(sizeof(_##name##_Props__) / sizeof(PropDef)) - 1,								\
			&_##name##_Props__[1],															\
			sizeof(name),																	\
			cacheobjcount};																	\
			DO_AUTO_CLASSLIST(name)															\
			/* Declare a global instance of our implementation class. */                    \
			/* It is not static so that we will get link errors if two instances */         \
			/* are created with the same instance name. */                                  \
			int32 __impl_instance_##name##__;
#endif
#endif

#define link_to_object(name)														\
	extern int32 __impl_instance_##name##__;                                        \
	int32* __impl_instance_##name##_Ptr__ = &__impl_instance_##name##__;



struct PropDef
{
	// 构造函数
	PropDef(char *pName, short type, CColor3 valColor, CPos valPos,
	        float valFloat, char *valString, unsigned long propFlags)
	{
		m_PropName				= pName;
		m_PropType				= type;
		m_DefaultValueColor		= valColor;
		m_DefaultValuePos		= valPos;
		m_DefaultValueFloat		= valFloat;
		m_DefaultValueString	= valString;
		m_PropFlags				= propFlags;
	}

	//  属性的名字
	char*		m_PropName;
	// One of the PT_ defines above.
	short		m_PropType;
	// Default color value.
	CColor3		m_DefaultValueColor;
	// Default pos value
	CPos		m_DefaultValuePos;
	// 缺省的浮点值
	float		m_DefaultValueFloat;
	// 属性的字符串值
	char*		m_DefaultValueString;
	// PF_的组合
	uint32		m_PropFlags;
};

// 类的描述
struct ClassDef
{
public:
	// 类的名字
	char*		m_ClassName;
	// 父类的描述
	ClassDef*	m_ParentClass;
	// 只有客户端用到，服务端为空，为了兼容编辑器
	char*		m_ServerClassName;
	// A combination of the CF_ flags above.
	uint32		m_ClassFlags;
	// 类的类型 CC_
	uint32		m_ClassType;
	// 类的构造函数
	ConstructObjectFn m_ConstructFn;
	// 类的析构函数
	DestructObjectFn m_DestructFn;
	// 类中有多少个属性
	short		m_nProps;
	// 类中的属性列表
	PropDef*	m_Props;
	// How big an object of this class is (set automatically).
	uint32		m_ClassObjectSize;
	// 此类的对象最多有多少个. 预先分配内存
	uint32		m_nCacheObjtCount;
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

		if (0 == __g_ClassDefinerHead)
		{
			m_pNext		= __g_ClassDefinerHead;
			__g_ClassDefinerHead = this;
			__g_ClassDefinerEnd = __g_ClassDefinerHead;
		}
		else
		{
			// 将需要绑定到地图的类，放在列表的最前部
			if (m_pClass->m_ClassFlags & CF_ATTACHTOMAP)
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


#ifndef _WIN32
#define DEFINE_CLASSES() \
	ClassDef **__g_cpp_classlist=0;\
	__cpp_classlist_auto_free __free_the_g_cpp_classlist;\
  	extern "C" ClassDef** ObjectDLLSetup(int *nDefs, int *version); \
	__ClassDefiner *__g_ClassDefinerHead=0;\
	__ClassDefiner *__g_ClassDefinerEnd=0;	\
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
#else
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
#endif // _WIN32


/*!

Property flags,给编辑器用的编辑器需要修改，适合自己的需要.
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
#define NOCOLOR CColor3(0.0f, 0.0f, 0.0f)
#define NOPOS	CPos(0, 0)

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

#define ADD_SOLID_FLAG(defVal, flags) \
    ADD_PROP_FLAG(Solid, PT_BOOL, defVal, 0, flags)

#define ADD_TOUCHNOTIFY_FLAG(defVal, flags) \
    ADD_PROP_FLAG(TouchNotify, PT_BOOL, defVal, 0, flags)

#define ADD_RAYHIT_FLAG(defVal, flags) \
    ADD_PROP_FLAG(Rayhit, PT_BOOL, defVal, 0, flags)

/*!
Full property definitions.
*/
#define ADD_PROP_FLAG(name, type, valFloat, valString, flags) \
    PropDef(#name, type, NOCOLOR, NOPOS, valFloat, valString, flags),

#define ADD_REALPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_REAL, NOCOLOR, NOPOS, val, "", flags),

#define ADD_STRINGPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_STRING, NOCOLOR, NOPOS, 0.0f, val, flags),

#define ADD_COLORPROP_FLAG(name, defX, defY, defZ, flags) \
    PropDef(#name, PT_COLOR, CColor3(defX, defY, defZ), NOPOS, 0.0f, (char*)0, flags),

#define ADD_POSPROP_FLAG(name, defX, defY, flags) \
	PropDef(#name, PT_POS, NOCOLOR, CPos(defX, defY), 0.0f, (char*)0, flags),

#define ADD_LONGINTPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_LONGINT, NOCOLOR, NOPOS, (float)val, (char*)0, flags),

#define ADD_BYTEPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_BYTE, NOCOLOR, NOPOS, (float)val, (char*)0, flags),

#define ADD_BOOLPROP_FLAG(name, val, flags) \
    PropDef(#name, PT_BOOL, NOCOLOR, NOPOS, (float)val, (char*)0, flags),

/*!
Add properties without flags (only here for backward compatibility).
*/
#define ADD_PROP(name, type, valFloat, valString) \
    ADD_PROP_FLAG(name, type, valFloat, valString, 0)

#define ADD_REALPROP(name, val) \
    ADD_REALPROP_FLAG(name, val, 0)

#define ADD_STRINGPROP(name, val) \
    ADD_STRINGPROP_FLAG(name, val, 0)

#define ADD_POSPROP(name, defX, defY) \
	ADD_POSPROP_FLAG(name, defX, defY, 0)

#define ADD_COLORPROP(name, valR, valG, valB) \
	ADD_COLORPROP_FLAG(name, valR, valG, valB, 0)

#define ADD_LONGINTPROP(name, val) \
    ADD_LONGINTPROP_FLAG(name, val, 0)

#define ADD_BYTEPROP(name, val) \
	ADD_BYTEPROP_FLAG(name, val, 0)

#define ADD_BOOLPROP(name, val) \
    ADD_BOOLPROP_FLAG(name, val, 0)

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






