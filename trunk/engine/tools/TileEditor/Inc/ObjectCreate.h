/*
 *	Object creation structure declaration and related constants and macros
 */

#ifndef __ARKOBJECTCREATE_H__
#define __ARKOBJECTCREATE_H__

#include "property.h"

//! Object is being created at runtime.
#define PRECREATE_NORMAL        0.0f

//! Object is being loaded from a world file.  Read props in.
#define PRECREATE_WORLDFILE     1.0f

//! Object is created from CreateObjectProps.  Use GetPropGeneric to read props.
#define PRECREATE_STRINGPROP    2.0f

//! Object comes from a savegame.
#define PRECREATE_SAVEGAME      3.0f

/*!
Different object types.  Some can only be created on the client.
*/
enum
{
	//! Invisible object.  Note, client's aren't told about
	OT_NORMAL =           0,

	//! Model object.
	OT_MODEL =            1,

	//! WorldModel.
	OT_WORLDMODEL =       2,

	//! Sprite.
	OT_SPRITE =           3,

	//! Dynamic light.
	OT_LIGHT =            4,

	//! Camera.
	OT_CAMERA =           5,

	//! Particle system.
	OT_PARTICLESYSTEM =   6,

	//! Poly grid.
	OT_POLYGRID =         7,

	//! Line system.
	OT_LINESYSTEM =       8,

	//! Container.
	OT_CONTAINER =        9,

	//! Canvas (game code renders it).
	OT_CANVAS =           10,

	//! Volume effect.
	OT_VOLUMEEFFECT =     11,

	/*!
	NOTE: the high bit of the object type is reserved
	for the engine's networking.
	ALSO NOTE: The networking assumes there are < 16 object types!
	*/
	NUM_OBJECTTYPES =     12,
};

/*!
aximum number of skins per model.
*/
#define MAX_MODEL_TEXTURES      32
#define MAX_MODEL_RENDERSTYLES  8
#define MAX_CHILD_MODELS		32
#define MAX_CS_FILENAME_LEN		127

/*!
This structure is used when creating objects.  When you want to
create an object, you call ILTServer::CreateObject with one of these.
The structure you pass in is passed to the object's PostPropRead function,
where it can override whatever it wants to.
对象创建时，通过OnPreCreate传给游戏，让游戏来填写一些信息，此时对象的游戏属性
已经读入了
*/
struct ObjectCreateStruct 
{
    ObjectCreateStruct() 
	{
        Clear();
    }

    void Clear() 
	{

        m_ObjectType		= 0;
        m_Flags				= 0;
        m_Pos.Init();
        m_Scale.Init(1.0f, 1.0f, 1.0f);
        m_Rotation.Init();
        m_UserData			= 0;
        m_Filename[0]		= 0;

        uint32 i;
        for (i=0; i < MAX_MODEL_TEXTURES; i++) {
            m_SkinNames[i][0] = 0;
        }
        for (i=0; i < MAX_MODEL_RENDERSTYLES; i++) {
            m_RenderStyleNames[i][0] = 0;
        }
		
		for (i=1; i < MAX_CHILD_MODELS ; i++ )
		{
			m_Filenames[i][0]= 0;
		}

        m_Name[0]			= 0;

		m_cProperties.Reset();
    }

/*!
ain info.
*/
    uint16 m_ObjectType;
    uint32 m_Flags;

    LTVector m_Pos;
    LTVector m_Scale;
    LTRotation m_Rotation;


//! User data.
    uint32 m_UserData;

/*!
This is the model, sound, or sprite filename.
It also can be the WorldModel name.
This can be zero-length when it's not needed.
*/
	union {

    char m_Filename[MAX_CS_FILENAME_LEN+1];  //!
	char m_Filenames[MAX_CHILD_MODELS][MAX_CS_FILENAME_LEN +1];

	};

    union {
/*!
This can be zero-length. If you set it for an
OT_MODEL, it's the skin filename.
*/
        char m_SkinName[MAX_CS_FILENAME_LEN+1];  //!
        char m_SkinNames[MAX_MODEL_TEXTURES][MAX_CS_FILENAME_LEN+1];
    };

    union {
/*!
This can be zero-length. If you set it for an
OT_MODEL, it's the skin filename.
*/
        char m_RenderStyleName[MAX_CS_FILENAME_LEN+1];   //!
        char m_RenderStyleNames[MAX_MODEL_RENDERSTYLES][MAX_CS_FILENAME_LEN+1];
    };

/*!
Server only info.
*/

//! This object's name.
    char m_Name[MAX_CS_FILENAME_LEN+1];

//! List of properties
	GenericPropList m_cProperties;
};


/*!
Initialize an ObjectCreateStruct.
*/
#define INIT_OBJECTCREATESTRUCT(theStruct) \
{ \
    (theStruct).Clear();\
}

#endif //__LTOBJECTCREATE_H__