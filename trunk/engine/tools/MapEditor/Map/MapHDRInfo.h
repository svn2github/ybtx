#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class COperator;
}

namespace Map
{

/// HDR信息.
class MapHDRInfo : public IO::DataChunk
{
	DeclareChunk(MapHDRInfo);
public:
	MapHDRInfo();
	virtual ~MapHDRInfo();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 加载
	void LoadFromOperator(COperator* o);

private:
	float	fBrightPass;		///<光通量 0~1
	float	fGaussMultiplier;	///<泛光强度 0~2
	float	fGaussMean;			///<泛光偏移 -1~+1
	float	fGaussStdDev;		///<泛光融和程度  0~1
	float	fExposure;			///<最终亮度0~2
	float	fSample;			///<混合度
};

}// namespace Map
