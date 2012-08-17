#pragma once
//------------------------------------------------------------------------------
/**
    @class CLightMapWriter

	把CLightMapReceiver生成的surface保存下来

    (C) 2009 ThreeOGCMan
*/
#include "CRefObject.h"
#include "GraphicBase.h"
#include "CColor.h"

namespace sqr
{

	//------------------------------------------------------------------------------
	SmartPointer(CLightMapWriter);
	class CLightMapWriter : public CRefObject
	{
	public:
		virtual ~CLightMapWriter();
		/// 设置lightmap数目
		void SetNumLightMaps(uint width , uint depth);
		/// 保存
		virtual void Write(index_t x, index_t z, const ITexture* surface);

	protected:
		CLightMapWriter();
		uint width;
		uint depth;
		ITexture*		m_srcTexture;
		//ITexture*		m_srcSurface;
	};
	//------------------------------------------------------------------------------
	/// 写入文件 
	class CLMFileWriter : public CLightMapWriter
	{
	public:
		CLMFileWriter();
		/// 保存
		void Write(index_t x, index_t z, const ITexture* surface);
	};
	//------------------------------------------------------------------------------
	/// 写入逻辑阴影区域
	class CLogicalShadowWriter : public CLightMapWriter
	{
	public:
		CLogicalShadowWriter();
		/// 写入数据
		void Write(index_t x, index_t z, const ITexture* surface);
		/// 保存
		void Save(int offset);

	private:
		byte GetBlockColor(index_t x, index_t y, int pitch, const CColor* data);
	private:
		size_t blockSize;
		vector<byte> buffer;
	};
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	inline void
	CLightMapWriter::SetNumLightMaps( uint w , uint d )
	{
		this->width = w;
		this->depth = d;
	}
}