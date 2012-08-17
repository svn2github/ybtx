#pragma once
#include "CFileFormatMgr.h"
#include "CEffectMallocObject.h"

namespace sqr
{
	class CEffectFormatMgr : public CFileFormatMgr
	{
	public:
		CEffectFormatMgr();
	};
	class IEffectUnitProp;
	class CEffectPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CEffectPropCoder();
		FourCC	GetChunkName(void)	{ return 'EPCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectProp( const CDataChunk& inData , OBJ_HANDLE handle );
		void	WriteEffectUnitProp(WriteDataInf& wdi , IEffectUnitProp* handle);
	};

	class CEffectUnitPropCoder : public CCoder
	{
		DECLARE_VERSION_MAP();
	public:
		FourCC	GetChunkName(void)	{ return 'EUCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CCameraFilterPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CCameraFilterPropCoder();
		FourCC	GetChunkName(void)	{ return 'CFCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CCameraShakePropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CCameraShakePropCoder();
		FourCC	GetChunkName(void)	{ return 'CSCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CDynamicLightPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CDynamicLightPropCoder();
		FourCC	GetChunkName(void)	{ return 'DLCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CFlareSpritePropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CFlareSpritePropCoder();
		FourCC	GetChunkName(void)	{ return 'FSCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CLeadingPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CLeadingPropCoder();
		FourCC	GetChunkName(void)	{ return 'LLCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CLightningPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CLightningPropCoder();
		FourCC	GetChunkName(void)	{ return 'LPCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CModelConsolePropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CModelConsolePropCoder();
		FourCC	GetChunkName(void)	{ return 'MCCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CModelRenderStylePropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CModelRenderStylePropCoder();
		FourCC	GetChunkName(void)	{ return 'MRCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CNullPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CNullPropCoder();
		FourCC	GetChunkName(void)	{ return 'NPCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CParticleSystemPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CParticleSystemPropCoder();
		FourCC	GetChunkName(void)	{ return 'PSCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CPolyTrailPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CPolyTrailPropCoder();
		FourCC	GetChunkName(void)	{ return 'PTCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CShakePropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CShakePropCoder();
		FourCC	GetChunkName(void)	{ return 'SACK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CSpritePropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CSpritePropCoder();
		FourCC	GetChunkName(void)	{ return 'SPCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

	class CTDimAudioPropCoder : public CCoder,public CCoderHelper
	{
		DECLARE_VERSION_MAP();
	public:
		CTDimAudioPropCoder();
		FourCC	GetChunkName(void)	{ return 'TDCK'; }
		uint16	GetNewVer(void)		{ return 1; }
		bool Code(WriteDataInf& wdi , OBJ_HANDLE handle );

		bool DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle );
	};

}
