//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of Millennium 2000
//  
//////////////////////////////////////////////////////////////////////////

#ifndef __BLUELY_GRAPHICS__
#define __BLUELY_GRAPHICS__
//暂时屏蔽d3d9types.h C4819 
#pragma   warning(disable:4819)   

#define WIN32_LEAN_AND_MEAN

#define INVALID -1

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete[] (p);   (p)=NULL; } }
#endif

#if !defined(SAFE_DESTROY)
#define SAFE_DESTROY(p)      { if((p) != NULL) { (p)->Destroy(); delete (p); (p)=NULL;}}
#endif

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

#define MINIMAP_SCREEN_SIZE 192
#define DIRTY_TOLORENCE 10
#define MINIMAP_BLOCK_SIZE 32
#define MINIMAP_BLOCK_GRID_SIZE (2*16)
#define GRID_BATCH_SIZE 1000
#define TRIANGLE_BATCH_SIZE (GRID_BATCH_SIZE*2)
#define VERTEX_BATCH_SIZE (TRIANGLE_BATCH_SIZE*3)
#define STDFOV 30.0f
#define MixObjHeight 200 //物体消隐最小高度

// 新视角
#define DT1_STDYAW 45
#define DT1_STDPITCH 30
#define DT1_STDDISTANCE 1200.0f

#define DT2_STDYAW 45
#define DT2_STDPITCH 35
#define DT2_STDDISTANCE 1530.0f

#define YBTX_STDYAW		45
#define YBTX_STDPITCH	45
#define YBTX_STDFOV		35
#define YBTX_MIN_DISTANCE	(9 * 64.0f)
#define YBTX_MAX_DISTANCE	(26 * 64.0f)

extern float STDYAW;		
extern float STDPITCH;		
extern float STDDISTANCE;	

#define STDLOGICDISTANCE 1400.0f
#define STDMINIDISTANCE 30000.0f

#define STDNEARPLANE 50.0f
#define STDFARPLANE 300000.0f

#define STDGFARPLANE 300000.0f
#define STDGNEARPLANE 50.0f

#define STDMINIFARPLANE 300000.0f
#define STDMININEARPLANE 50.0f

//////////////////////////////////////////////////////////////////////////
#define LOCAL_SCENEREGIONVERSION 0x54
//////////////////////////////////////////////////////////////////////////

#define CLIENT_VERSION_WITH_AVATAR_FX_ADDED 0x0b
#define CLIENT_VERSION_WITH_SELF_ZTEST_CHECKED 0x0a
#define CLIENT_VERSION_WITH_TEXTURE_CHECKED 0x09
#define CLIENT_VERSION_WITH_RENDERSTYLE_ADDED 0x08
#define CLIENT_VERSION_WITH_INFLUENCE_AND_SHADOW_Z_TEST_ADDED 0x07
#define SERVER_VERSION 0x05 //
#define FOGDENSITY 0.0002f
#define FOGSTART 1020.0f
#define FOGEND 8000.0f
#define DOTRATIO .005f
#define DOTRANGE 400.0f
#define SPACE_GRANULARITY (4.0f)
#define HEIGHT_LIMIT (16000.0f)
#define HEIGHT_LEVEL (32)
#define ROTATION_STEP (3.141592654f*2.0f/256.0f)
#define SCALE_STEP 0.2f
#define ORTHOGONAL_DISTANCE 2048.0f

#endif
