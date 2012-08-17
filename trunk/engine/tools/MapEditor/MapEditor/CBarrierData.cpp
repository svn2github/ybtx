#include "stdafx.h"
#include "CBarrierData.h"
#include "ExsertStructDB.h"
#include "Console.h"

CBarrierData::CBarrierData(void)
	:m_pBarrierArray(NULL)
{
}

CBarrierData::~CBarrierData(void)
{
	if (m_pBarrierArray)
	{
		delete[] m_pBarrierArray;
		m_pBarrierArray = NULL;
	}
}


bool CBarrierData::LoadBarrierData(const string& strPathName)
{
	string sAmpFileName = strPathName + ".amp";
	FILE * fAmp = fopen(sAmpFileName.c_str(),"rb");
	if( !fAmp )
	{
		LogConsole("无法打开 " << sAmpFileName.c_str() << "\n");
		return false;
	}
	int Temp[256];
	void* pTemp = (void*)Temp;
	fread(pTemp, sizeof(int32), 1, fAmp);
	fread(pTemp, sizeof(int32), 1, fAmp);
	fread(pTemp, 64, 1, fAmp);

	uint16 u16tmp;
	fread(&u16tmp, sizeof(uint16), 1, fAmp);
	m_nWidth = u16tmp * REGION_SPACE;
	fread(&u16tmp, sizeof(uint16), 1, fAmp);
	m_nHeight = u16tmp * REGION_SPACE;
	//后面的数据这里不关心了
	fclose(fAmp);


	string sArgFileName = strPathName + "_s.arg";
	FILE * fp = fopen(sArgFileName.c_str(),"rb");
	if (!fp)
	{
		LogConsole("无法打开 "<< sArgFileName.c_str() << "\n");
		return false;
	}
	//载入Region的头
	uint32 nMagicID, nVersion, nCount, dwClient;
	fread(&nMagicID, sizeof(uint32), 1, fp);
	fread(&nVersion, sizeof(uint32), 1, fp);
	fread(&nCount, sizeof(uint32), 1, fp);
	fread(&dwClient, sizeof(uint32), 1, fp);

	m_pBarrierArray = new uint8 [m_nWidth * m_nHeight];
	int32 nOffset;
	uint32 nPos	= ftell(fp);
	uint32 nRegionHeight = m_nHeight / REGION_SPACE;
	uint32 nRegionWidth = m_nWidth / REGION_SPACE;
	for( uint32 y = 0; y < nRegionHeight; ++y )
	{
		for(uint32 x = 0; x < nRegionWidth ; ++x )
		{		
			uint32 uIdx = y*nRegionWidth + x;
			fseek(fp, nPos + uIdx*sizeof( uint32 ), SEEK_SET );
			fread( &nOffset, sizeof(uint32), 1, fp);
			if( nOffset >= 0 )
			{
				fseek(fp, nOffset, SEEK_SET );
				LoadOneRegion( fp, nCount, x, y);
			}
		}
	}
	fclose(fp);
	return true;
}

void CBarrierData::LoadOneRegion(FILE * fp, uint32 nChunk, uint32 x, uint32 y)
{
	uint32 nPos	= ftell(fp);
	// 载入每个区块
	uint32 nMagicID, nOffset, nSize;
	for(uint32 i = 0; i < nChunk; i++)
	{
		fseek(fp, nPos, SEEK_SET);
		fread(&nMagicID, sizeof(uint32), 1, fp);
		fread(&nOffset, sizeof(uint32), 1, fp);
		fread(&nSize, sizeof(uint32), 1, fp);
		nPos = ftell(fp);

		fseek(fp,nOffset, SEEK_SET);

		LoadOneChunk( fp, x, y, nMagicID );
	}
}

const uint32	BARR = 'RRAB';
const uint32	GATR = 'RTAG';///< Block
const uint32	EBLK = 'KLBE';///< Exact Block

void CBarrierData::LoadOneChunk(FILE * fp, uint32 x, uint32 y, uint32 uMagicID)
{
	uint32 nGridX, nGridY;
	
	switch(uMagicID)
	{
	case BARR:
	case GATR:
		uint8 nVal = 0, roadweight = 0;
		for(uint32 i = 0; i < REGION_SPACE; ++i)
		{
			for(uint32 j = 0; j < REGION_SPACE; ++j)
			{
				fread( &(nVal) , sizeof(uint8) , 1, fp); // 通过内存块拷贝的方式，读取障碍信息到变量(0, 1, 2, 3)
				fread( &roadweight, sizeof(uint8), 1, fp);
				nGridX = (x * REGION_SPACE) + j;
				nGridY = (y * REGION_SPACE) + i;
				m_pBarrierArray[ nGridY * m_nWidth + nGridX] = nVal;
			}
		}
		break;
	}
}

