#pragma once

//----------------------------------
// 这个头文件严格按照的D3D方式来定义
// 所以渲染状态可以直接赋值给 D3D API
// 如果改用其它API 请在实现中自己影射吧
// 嘎嘎~~~这种方法可以保证D3D下的性能最优
//----------------------------------
namespace sqr
{
	#define LOCK_READONLY           0x00000010L
	#define LOCK_DISCARD            0x00002000L
	#define LOCK_NOOVERWRITE        0x00001000L
	#define LOCK_NOSYSLOCK          0x00000800L
	#define LOCK_DONOTWAIT          0x00004000L                  
	#define LOCK_NO_DIRTY_UPDATE     0x00008000L


	#define TA_SELECTMASK        0x0000000f  // mask for arg selector
	#define TA_DIFFUSE           0x00000000  // select diffuse color (read only)
	#define TA_CURRENT           0x00000001  // select stage destination register (read/write)
	#define TA_TEXTURE           0x00000002  // select texture color (read only)
	#define TA_TFACTOR           0x00000003  // select RS_TEXTUREFACTOR (read only)
	#define TA_SPECULAR          0x00000004  // select specular color (read only)
	#define TA_TEMP              0x00000005  // select temporary register color (read/write)
	#define TA_CONSTANT          0x00000006  // select texture stage constant
	#define TA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
	#define TA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)

	#define CLIPPLANE0 (1 << 0)
	#define CLIPPLANE1 (1 << 1)
	#define CLIPPLANE2 (1 << 2)
	#define CLIPPLANE3 (1 << 3)
	#define CLIPPLANE4 (1 << 4)
	#define CLIPPLANE5 (1 << 5)

	/* Flags to construct D3DRS_COLORWRITEENABLE */
	#define COLORWRITEENABLE_RED     (1UL<<0)
	#define COLORWRITEENABLE_GREEN   (1UL<<1)
	#define COLORWRITEENABLE_BLUE    (1UL<<2)
	#define COLORWRITEENABLE_ALPHA   (1UL<<3)


	#define STREAMSOURCE_INDEXEDDATA  (1<<30)
	#define STREAMSOURCE_INSTANCEDATA (2<<30)

	/*
	* State enumerants for per-stage processing of fixed function pixel processing
	* Two of these affect fixed function vertex processing as well: TEXTURETRANSFORMFLAGS and TEXCOORDINDEX.
	*/
	typedef enum _TEXTURESTAGESTATETYPE
	{
		TSS_COLOROP        =  1, /* TEXTUREOP - per-stage blending controls for color channels */
		TSS_COLORARG1      =  2, /* TA_* (texture arg) */
		TSS_COLORARG2      =  3, /* TA_* (texture arg) */
		TSS_ALPHAOP        =  4, /* TEXTUREOP - per-stage blending controls for alpha channel */
		TSS_ALPHAARG1      =  5, /* TA_* (texture arg) */
		TSS_ALPHAARG2      =  6, /* TA_* (texture arg) */
		TSS_BUMPENVMAT00   =  7, /* float (bump mapping matrix) */
		TSS_BUMPENVMAT01   =  8, /* float (bump mapping matrix) */
		TSS_BUMPENVMAT10   =  9, /* float (bump mapping matrix) */
		TSS_BUMPENVMAT11   = 10, /* float (bump mapping matrix) */
		TSS_TEXCOORDINDEX  = 11, /* identifies which set of texture coordinates index this texture */
		TSS_BUMPENVLSCALE  = 22, /* float scale for bump map luminance */
		TSS_BUMPENVLOFFSET = 23, /* float offset for bump map luminance */
		TSS_TEXTURETRANSFORMFLAGS = 24, /* TEXTURETRANSFORMFLAGS controls texture transform */
		TSS_COLORARG0      = 26, /* TA_* third arg for triadic ops */
		TSS_ALPHAARG0      = 27, /* TA_* third arg for triadic ops */
		TSS_RESULTARG      = 28, /* TA_* arg for result (CURRENT or TEMP) */
		TSS_CONSTANT       = 32, /* Per-stage constant TA_CONSTANT */

		TSS_FORCE_DWORD   = 0x7fffffff, /* force 32-bit size enum */
	} TEXTURESTAGESTATETYPE;

	/*
	* State enumerants for per-sampler texture processing.
	*/
	typedef enum _SAMPLERSTATETYPE
	{
		SAMP_ADDRESSU       = 1,  /* TEXTUREADDRESS for U coordinate */
		SAMP_ADDRESSV       = 2,  /* TEXTUREADDRESS for V coordinate */
		SAMP_ADDRESSW       = 3,  /* TEXTUREADDRESS for W coordinate */
		SAMP_BORDERCOLOR    = 4,  /* COLOR */
		SAMP_MAGFILTER      = 5,  /* TEXTUREFILTER filter to use for magnification */
		SAMP_MINFILTER      = 6,  /* TEXTUREFILTER filter to use for minification */
		SAMP_MIPFILTER      = 7,  /* TEXTUREFILTER filter to use between mipmaps during minification */
		SAMP_MIPMAPLODBIAS  = 8,  /* float Mipmap LOD bias */
		SAMP_MAXMIPLEVEL    = 9,  /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
		SAMP_MAXANISOTROPY  = 10, /* DWORD maximum anisotropy */
		SAMP_SRGBTEXTURE    = 11, /* Default = 0 (which means Gamma 1.0,
									 no correction required.) else correct for
									 Gamma = 2.2 */
		SAMP_ELEMENTINDEX   = 12, /* When multi-element texture is assigned to sampler, this
									 indicates which element index to use.  Default = 0.  */
		SAMP_DMAPOFFSET     = 13, /* Offset in vertices in the pre-sampled displacement map.
									 Only valid for DMAPSAMPLER sampler  */
		SAMP_FORCE_DWORD   = 0x7fffffff, /* force 32-bit size enum */
	} SAMPLERSTATETYPE;

		/* Special sampler which is used in the tesselator */
	#define DMAPSAMPLER 256

		// Samplers used in vertex shaders
	#define VERTEXTEXTURESAMPLER0 (DMAPSAMPLER+1)
	#define VERTEXTEXTURESAMPLER1 (DMAPSAMPLER+2)
	#define VERTEXTEXTURESAMPLER2 (DMAPSAMPLER+3)
	#define VERTEXTEXTURESAMPLER3 (DMAPSAMPLER+4)

	#define TSS_TCI_PASSTHRU                             0x00000000
	#define TSS_TCI_CAMERASPACENORMAL                    0x00010000
	#define TSS_TCI_CAMERASPACEPOSITION                  0x00020000
	#define TSS_TCI_CAMERASPACEREFLECTIONVECTOR          0x00030000
	#define TSS_TCI_SPHEREMAP                            0x00040000

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

	typedef enum _RENDERSTATETYPE {
		RS_ZENABLE                   = 7,    /* ZBUFFERTYPE (or TRUE/FALSE for legacy) */
		RS_FILLMODE                  = 8,    /* FILLMODE */
		RS_SHADEMODE                 = 9,    /* SHADEMODE */
		RS_ZWRITEENABLE              = 14,   /* TRUE to enable z writes */
		RS_ALPHATESTENABLE           = 15,   /* TRUE to enable alpha tests */
		RS_LASTPIXEL                 = 16,   /* TRUE for last-pixel on lines */
		RS_SRCBLEND                  = 19,   /* BLEND */
		RS_DESTBLEND                 = 20,   /* BLEND */
		RS_CULLMODE                  = 22,   /* CULL */
		RS_ZFUNC                     = 23,   /* CMPFUNC */
		RS_ALPHAREF                  = 24,   /* FIXED */
		RS_ALPHAFUNC                 = 25,   /* CMPFUNC */
		RS_DITHERENABLE              = 26,   /* TRUE to enable dithering */
		RS_ALPHABLENDENABLE          = 27,   /* TRUE to enable alpha blending */
		RS_FOGENABLE                 = 28,   /* TRUE to enable fog blending */
		RS_SPECULARENABLE            = 29,   /* TRUE to enable specular */
		RS_FOGCOLOR                  = 34,   /* COLOR */
		RS_FOGTABLEMODE              = 35,   /* FOGMODE */
		RS_FOGSTART                  = 36,   /* Fog start (for both vertex and pixel fog) */
		RS_FOGEND                    = 37,   /* Fog end      */
		RS_FOGDENSITY                = 38,   /* Fog density  */
		RS_RANGEFOGENABLE            = 48,   /* Enables range-based fog */
		RS_STENCILENABLE             = 52,   /* BOOL enable/disable stenciling */
		RS_STENCILFAIL               = 53,   /* STENCILOP to do if stencil test fails */
		RS_STENCILZFAIL              = 54,   /* STENCILOP to do if stencil test passes and Z test fails */
		RS_STENCILPASS               = 55,   /* STENCILOP to do if both stencil and Z tests pass */
		RS_STENCILFUNC               = 56,   /* CMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
		RS_STENCILREF                = 57,   /* Reference value used in stencil test */
		RS_STENCILMASK               = 58,   /* Mask value used in stencil test */
		RS_STENCILWRITEMASK          = 59,   /* Write mask applied to values written to stencil buffer */
		RS_TEXTUREFACTOR             = 60,   /* COLOR used for multi-texture blend */
		RS_WRAP0                     = 128,  /* wrap for 1st texture coord. set */
		RS_WRAP1                     = 129,  /* wrap for 2nd texture coord. set */
		RS_WRAP2                     = 130,  /* wrap for 3rd texture coord. set */
		RS_WRAP3                     = 131,  /* wrap for 4th texture coord. set */
		RS_WRAP4                     = 132,  /* wrap for 5th texture coord. set */
		RS_WRAP5                     = 133,  /* wrap for 6th texture coord. set */
		RS_WRAP6                     = 134,  /* wrap for 7th texture coord. set */
		RS_WRAP7                     = 135,  /* wrap for 8th texture coord. set */
		RS_CLIPPING                  = 136,
		RS_LIGHTING                  = 137,
		RS_AMBIENT                   = 139,
		RS_FOGVERTEXMODE             = 140,
		RS_COLORVERTEX               = 141,
		RS_LOCALVIEWER               = 142,
		RS_NORMALIZENORMALS          = 143,
		RS_DIFFUSEMATERIALSOURCE     = 145,
		RS_SPECULARMATERIALSOURCE    = 146,
		RS_AMBIENTMATERIALSOURCE     = 147,
		RS_EMISSIVEMATERIALSOURCE    = 148,
		RS_VERTEXBLEND               = 151,
		RS_CLIPPLANEENABLE           = 152,
		RS_POINTSIZE                 = 154,   /* float point size */
		RS_POINTSIZE_MIN             = 155,   /* float point size min threshold */
		RS_POINTSPRITEENABLE         = 156,   /* BOOL point texture coord control */
		RS_POINTSCALEENABLE          = 157,   /* BOOL point size scale enable */
		RS_POINTSCALE_A              = 158,   /* float point attenuation A value */
		RS_POINTSCALE_B              = 159,   /* float point attenuation B value */
		RS_POINTSCALE_C              = 160,   /* float point attenuation C value */
		RS_MULTISAMPLEANTIALIAS      = 161,  // BOOL - set to do FSAA with multisample buffer
		RS_MULTISAMPLEMASK           = 162,  // DWORD - per-sample enable/disable
		RS_PATCHEDGESTYLE            = 163,  // Sets whether patch edges will use float style tessellation
		RS_DEBUGMONITORTOKEN         = 165,  // DEBUG ONLY - token to debug monitor
		RS_POINTSIZE_MAX             = 166,   /* float point size max threshold */
		RS_INDEXEDVERTEXBLENDENABLE  = 167,
		RS_COLORWRITEENABLE          = 168,  // per-channel write enable
		RS_TWEENFACTOR               = 170,   // float tween factor
		RS_BLEndBatch                   = 171,   // BLEndBatch setting
		RS_POSITIONDEGREE            = 172,   // NPatch position interpolation degree. DEGREE_LINEAR or DEGREE_CUBIC (default)
		RS_NORMALDEGREE              = 173,   // NPatch normal interpolation degree. DEGREE_LINEAR (default) or DEGREE_QUADRATIC
		RS_SCISSORTESTENABLE         = 174,
		RS_SLOPESCALEDEPTHBIAS       = 175,
		RS_ANTIALIASEDLINEENABLE     = 176,
		RS_MINTESSELLATIONLEVEL      = 178,
		RS_MAXTESSELLATIONLEVEL      = 179,
		RS_ADAPTIVETESS_X            = 180,
		RS_ADAPTIVETESS_Y            = 181,
		RS_ADAPTIVETESS_Z            = 182,
		RS_ADAPTIVETESS_W            = 183,
		RS_ENABLEADAPTIVETESSELLATION = 184,
		RS_TWOSIDEDSTENCILMODE       = 185,   /* BOOL enable/disable 2 sided stenciling */
		RS_CCW_STENCILFAIL           = 186,   /* STENCILOP to do if ccw stencil test fails */
		RS_CCW_STENCILZFAIL          = 187,   /* STENCILOP to do if ccw stencil test passes and Z test fails */
		RS_CCW_STENCILPASS           = 188,   /* STENCILOP to do if both ccw stencil and Z tests pass */
		RS_CCW_STENCILFUNC           = 189,   /* CMPFUNC fn.  ccw Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
		RS_COLORWRITEENABLE1         = 190,   /* Additional ColorWriteEnables for the devices that support PMISCCAPS_INDEPENDENTWRITEMASKS */
		RS_COLORWRITEENABLE2         = 191,   /* Additional ColorWriteEnables for the devices that support PMISCCAPS_INDEPENDENTWRITEMASKS */
		RS_COLORWRITEENABLE3         = 192,   /* Additional ColorWriteEnables for the devices that support PMISCCAPS_INDEPENDENTWRITEMASKS */
		RS_BLENDFACTOR               = 193,   /* COLOR used for a constant blend factor during alpha blending for devices that support PBLENDCAPS_BLENDFACTOR */
		RS_SRGBWRITEENABLE           = 194,   /* Enable rendertarget writes to be DE-linearized to SRGB (for formats that expose USAGE_QUERY_SRGBWRITE) */
		RS_DEPTHBIAS                 = 195,
		RS_WRAP8                     = 198,   /* Additional wrap states for vs_3_0+ attributes with DECLUSAGE_TEXCOORD */
		RS_WRAP9                     = 199,
		RS_WRAP10                    = 200,
		RS_WRAP11                    = 201,
		RS_WRAP12                    = 202,
		RS_WRAP13                    = 203,
		RS_WRAP14                    = 204,
		RS_WRAP15                    = 205,
		RS_SEPARATEALPHABLENDENABLE  = 206,  /* TRUE to enable a separate blending function for the alpha channel */
		RS_SRCBLENDALPHA             = 207,  /* SRC blend factor for the alpha channel when RS_SEPARATEDESTALPHAENABLE is TRUE */
		RS_DESTBLENDALPHA            = 208,  /* DST blend factor for the alpha channel when RS_SEPARATEDESTALPHAENABLE is TRUE */
		RS_BLEndBatchALPHA              = 209,  /* Blending operation for the alpha channel when RS_SEPARATEDESTALPHAENABLE is TRUE */
		RS_FORCE_DWORD               = 0x7fffffff, /* force 32-bit size enum */

	} RENDERSTATETYPE;


	typedef enum _TEXTUREFILTERTYPE
	{
		TEXF_NONE            = 0,			// filtering disabled (valid for mip filter only)
		TEXF_POINT           = 1,			// nearest
		TEXF_LINEAR          = 2,			// linear interpolation
		TEXF_ANISOTROPIC     = 3,			// anisotropic
		TEXF_PYRAMIDALQUAD   = 6,			// 4-sample tent
		TEXF_GAUSSIANQUAD    = 7,			// 4-sample gaussian
		TEXF_FORCE_DWORD     = 0x7fffffff,  // force 32-bit size enum
	} TEXTUREFILTERTYPE;


	typedef enum _TEXTUREOP
	{
		// Control
		TOP_DISABLE              = 1,      // disables stage
		TOP_SELECTARG1           = 2,      // the default
		TOP_SELECTARG2           = 3,

		// Modulate
		TOP_MODULATE             = 4,      // multiply args together
		TOP_MODULATE2X           = 5,      // multiply and  1 bit
		TOP_MODULATE4X           = 6,      // multiply and  2 bits

		// Add
		TOP_ADD                  =  7,   // add arguments together
		TOP_ADDSIGNED            =  8,   // add with -0.5 bias
		TOP_ADDSIGNED2X          =  9,   // as above but left  1 bit
		TOP_SUBTRACT             = 10,   // Arg1 - Arg2, with no saturation
		TOP_ADDSMOOTH            = 11,   // add 2 args, subtract product
		// Arg1 + Arg2 - Arg1*Arg2
		// = Arg1 + (1-Arg1)*Arg2

		// Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
		TOP_BLENDDIFFUSEALPHA    = 12, // iterated alpha
		TOP_BLENDTEXTUREALPHA    = 13, // texture alpha
		TOP_BLENDFACTORALPHA     = 14, // alpha from RS_TEXTUREFACTOR

		// Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
		TOP_BLENDTEXTUREALPHAPM  = 15, // texture alpha
		TOP_BLENDCURRENTALPHA    = 16, // by alpha of current color

		// Specular mapping
		TOP_PREMODULATE            = 17,     // modulate with next texture before use
		TOP_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
		// COLOROP only
		TOP_MODULATECOLOR_ADDALPHA = 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
		// COLOROP only
		TOP_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
		// COLOROP only
		TOP_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
		// COLOROP only

		// Bump mapping
		TOP_BUMPENVMAP           = 22, // per pixel env map perturbation
		TOP_BUMPENVMAPLUMINANCE  = 23, // with luminance channel

		// This can do either diffuse or specular bump mapping with correct input.
		// Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
		// where each component has been scaled and offset to make it signed.
		// The result is replicated into all four (including alpha) channels.
		// This is a valid COLOROP only.
		TOP_DOTPRODUCT3          = 24,

		// Triadic ops
		TOP_MULTIPLYADD          = 25, // Arg0 + Arg1*Arg2
		TOP_LERP                 = 26, // (Arg0)*Arg1 + (1-Arg0)*Arg2

		TOP_FORCE_DWORD = 0x7fffffff,
	} TEXTUREOP;


	typedef enum _SHADEMODE {
		SHADE_FLAT               = 1,
		SHADE_GOURAUD            = 2,
		SHADE_PHONG              = 3,
		SHADE_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
	} SHADEMODE;

	typedef enum _FILLMODE {
		FILL_POINT               = 1,
		FILL_WIREFRAME           = 2,
		FILL_SOLID               = 3,
		FILL_FORCE_DWORD         = 0x7fffffff, /* force 32-bit size enum */
	} FILLMODE;

	typedef enum _BLEND {
		BLEND_ZERO               = 1,
		BLEND_ONE                = 2,
		BLEND_SRCCOLOR           = 3,
		BLEND_INVSRCCOLOR        = 4,
		BLEND_SRCALPHA           = 5,
		BLEND_INVSRCALPHA        = 6,
		BLEND_DESTALPHA          = 7,
		BLEND_INVDESTALPHA       = 8,
		BLEND_DESTCOLOR          = 9,
		BLEND_INVDESTCOLOR       = 10,
		BLEND_SRCALPHASAT        = 11,
		BLEND_BOTHSRCALPHA       = 12,
		BLEND_BOTHINVSRCALPHA    = 13,
		BLEND_BLENDFACTOR        = 14, /* Only supported if PBLENDCAPS_BLENDFACTOR is on */
		BLEND_INVBLENDFACTOR     = 15, /* Only supported if PBLENDCAPS_BLENDFACTOR is on */

		/* -- 9Ex only */
		BLEND_FORCE_DWORD        = 0x7fffffff, /* force 32-bit size enum */
	} BLEND;

	typedef enum _BLEndBatch {
		BLEndBatch_ADD              = 1,
		BLEndBatch_SUBTRACT         = 2,
		BLEndBatch_REVSUBTRACT      = 3,
		BLEndBatch_MIN              = 4,
		BLEndBatch_MAX              = 5,
		BLEndBatch_FORCE_DWORD      = 0x7fffffff, /* force 32-bit size enum */
	} BLEndBatch;

	typedef enum _TEXTUREADDRESS {
		TADDRESS_WRAP            = 1,
		TADDRESS_MIRROR          = 2,
		TADDRESS_CLAMP           = 3,
		TADDRESS_BORDER          = 4,
		TADDRESS_MIRRORONCE      = 5,
		TADDRESS_FORCE_DWORD     = 0x7fffffff, /* force 32-bit size enum */
	} TEXTUREADDRESS;

	typedef enum _CMPFUNC {
		CMP_NEVER                = 1,
		CMP_LESS                 = 2,
		CMP_EQUAL                = 3,
		CMP_LESSEQUAL            = 4,
		CMP_GREATER              = 5,
		CMP_NOTEQUAL             = 6,
		CMP_GREATEREQUAL         = 7,
		CMP_ALWAYS               = 8,
		CMP_FORCE_DWORD          = 0x7fffffff, /* force 32-bit size enum */
	} CMPFUNC;

	typedef enum _CULLTYPE {
		CULLT_NONE                = 1,
		CULLT_CW                  = 2,
		CULLT_CCW                 = 3,
		CULLT_FORCE_DWORD         = 0x7fffffff, /* force 32-bit size enum */
	} CULLTYPE;

	typedef enum _ZBUFFERTYPE {
		ZB_FALSE                 = 0,
		ZB_TRUE                  = 1, // Z buffering
		ZB_USEW                  = 2, // W buffering
		ZB_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
	} ZBUFFERTYPE;

	// Values for material source
	typedef enum _MATERIALCOLORSOURCE
	{
		MCS_MATERIAL         = 0,            // Color from material is used
		MCS_COLOR1           = 1,            // Diffuse vertex color is used
		MCS_COLOR2           = 2,            // Specular vertex color is used
		MCS_FORCE_DWORD      = 0x7fffffff,   // force 32-bit size enum
	} MATERIALCOLORSOURCE;

	typedef enum _LIGHTTYPE {
		LIGHT_POINT          = 1,
		LIGHT_SPOT           = 2,
		LIGHT_DIRECTIONAL    = 3,
		LIGHT_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
	} LIGHTTYPE;

	typedef enum _IMAGE_FILEFORMAT
	{
		IFF_BMP         = 0,
		IFF_JPG         = 1,
		IFF_TGA         = 2,
		IFF_PNG         = 3,
		IFF_DDS         = 4,
		IFF_PPM         = 5,
		IFF_DIB         = 6,
		IFF_HDR         = 7,       //high dynamic range formats
		IFF_PFM         = 8,       //
		//自定义
		IFF_DXT1		= 0x14,
		IFF_DXT2		= 0x24,
		IFF_DXT3		= 0x34,
		IFF_DXT4		= 0x44,
		IFF_DXT5		= 0x54,

		IFF_FORCE_DWORD = 0x7fffffff
	} IMAGE_FILEFORMAT;

	typedef enum _texFORMAT
	{
		TFMT_UNKNOWN              =  0,

		TFMT_R8G8B8               = 20,
		TFMT_A8R8G8B8             = 21,
		TFMT_X8R8G8B8             = 22,
		TFMT_R5G6B5               = 23,
		TFMT_X1R5G5B5             = 24,
		TFMT_A1R5G5B5             = 25,
		TFMT_A4R4G4B4             = 26,
		TFMT_R3G3B2               = 27,
		TFMT_A8                   = 28,
		TFMT_A8R3G3B2             = 29,
		TFMT_X4R4G4B4             = 30,
		TFMT_A2B10G10R10          = 31,
		TFMT_A8B8G8R8             = 32,
		TFMT_X8B8G8R8             = 33,
		TFMT_G16R16               = 34,
		TFMT_A2R10G10B10          = 35,
		TFMT_A16B16G16R16         = 36,

		TFMT_A8P8                 = 40,
		TFMT_P8                   = 41,

		TFMT_L8                   = 50,
		TFMT_A8L8                 = 51,
		TFMT_A4L4                 = 52,

		TFMT_V8U8                 = 60,
		TFMT_L6V5U5               = 61,
		TFMT_X8L8V8U8             = 62,
		TFMT_Q8W8V8U8             = 63,
		TFMT_V16U16               = 64,
		TFMT_A2W10V10U10          = 67,

		TFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
		TFMT_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
		TFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
		TFMT_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
		TFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
		TFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
		TFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
		TFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
		TFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

		TFMT_D16_LOCKABLE         = 70,
		TFMT_D32                  = 71,
		TFMT_D15S1                = 73,
		TFMT_D24S8                = 75,
		TFMT_D24X8                = 77,
		TFMT_D24X4S4              = 79,
		TFMT_D16                  = 80,

		TFMT_D32F_LOCKABLE        = 82,
		TFMT_D24FS8               = 83,
		TFMT_L16                  = 81,

		TFMT_VERTEXDATA           =100,
		TFMT_INDEX16              =101,
		TFMT_INDEX32              =102,

		TFMT_Q16W16V16U16         =110,

		TFMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

		// Floating point surface formats

		// s10e5 formats (16-bits per channel)
		TFMT_R16F                 = 111,
		TFMT_G16R16F              = 112,
		TFMT_A16B16G16R16F        = 113,

		// IEEE s23e8 formats (32-bits per channel)
		TFMT_R32F                 = 114,
		TFMT_G32R32F              = 115,
		TFMT_A32B32G32R32F        = 116,

		TFMT_CxV8U8               = 117,

		TFMT_FORCE_DWORD          =0x7fffffff
	} texFORMAT;
}
