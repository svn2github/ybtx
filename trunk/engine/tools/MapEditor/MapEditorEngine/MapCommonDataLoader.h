#pragma once
#include "TSingleton.h"
#include "CVector3.h"
namespace sqr
{
	class CTerrainMesh;
	class COperator;
	class SVertex;
	class SGrid;

	class MapCommonDataLoader : public TSingleton<MapCommonDataLoader>
	{
		COperator		*pOperator;
		CTerrainMesh	*terrain;
		vector<string>	ErrObjectList;
	public:
		friend class TSingleton<MapCommonDataLoader>;
		MapCommonDataLoader();
		virtual ~MapCommonDataLoader();

		void SetOperator(COperator *pOper);
		void SetTerrainMesh(CTerrainMesh *pTerrain);

		// 读取env 环境信息
		void LoadEnvInfo(FILE * fp);

		// 读取背景图信息
		void LoadBackPictureInfo( FILE * fp );

		// 读取wave信息
		void LoadWaveInfo(FILE * fp);

		// 读取每个顶点信息
		void LoadEachVertexInfo(FILE * fp, SVertex* pVertex);

		void PrintErrorObject()
		{
			if (ErrObjectList.size()>0)
			{
				TCHAR tt[255];
				sprintf( tt, "剔除错误场景物件 %d 个", ErrObjectList.size() );
				::MessageBox( 0, tt, "提示", MB_ICONINFORMATION);
				vector<string>::iterator eoitor = ErrObjectList.begin();
				while (eoitor!=ErrObjectList.end())
				{
					string pr;
					for (int i=0;eoitor!=ErrObjectList.end() && i<10;++i)
					{
						pr = pr + (*eoitor) + "\n";
						eoitor++;
					}
					::MessageBox( 0, pr.c_str(), "提示", MB_ICONINFORMATION);
				}

				ErrObjectList.clear();
			}
		}
	};
}
