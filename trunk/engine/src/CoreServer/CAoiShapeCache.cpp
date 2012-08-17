#include "stdafx.h"
#include "CAoiShapeCache.h"
#include "CQuadMallocObject.h"
#include "CPkgFile.h"
#include "TSqrAllocator.inl"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CLock.inl"
#include "CAoiShape.h"
#include "CColor.h"
#include "CColor.inl"


static const wstring PATH_ALIAS_AOI = L"res";


CAoiShapeCache::CAoiShapeCache(void)
{
}

CAoiShapeCache::~CAoiShapeCache(void)
{
	m_lkAoiShapes.Lock();

	for( ShapeMap_t::iterator it=m_mapAoiShapes.begin(); it!=m_mapAoiShapes.end(); ++it)
	{
		CAoiShape* pShape = it->second;
		SqrDelete( pShape->m_aryShape, CQuadMallocObject::GetPool());
		delete pShape;
	}

	m_mapAoiShapes.clear();

	m_lkAoiShapes.Unlock();
}

CAoiShapeCache& CAoiShapeCache::Inst()
{
	static CAoiShapeCache ls_Inst;
	return ls_Inst;
}


static uint32 ReadShapeFile( CPkgFile& File,void* pBuffer, size_t stSize)
{
	int32 nResult = File.Read( pBuffer, stSize );

	if( nResult >=0 )
		return uint32( nResult );

	GenErr("读取aoi形状文件长度不正确!");
	return uint32( -1 );
}



CAoiShape* CAoiShapeCache::GetShape(const char* szAoiFile)
{
	CAoiShape*	pShape=NULL;

	m_lkAoiShapes.Lock();

	SQR_TRY
	{
		pair<ShapeMap_t::iterator,bool> p =m_mapAoiShapes.insert( make_pair(szAoiFile,pShape) );

		if( !p.second )
		{
			pShape=p.first->second;		
		}
		else
		{
			//说明该aoi形状还没载入
			pShape=new CAoiShape;
			memset( pShape, 0, sizeof(CAoiShape) );

			p.first->second=pShape;
			
			SQR_TRY
			{				
				CPkgFile BitMap( PATH_ALIAS_AOI.c_str(), szAoiFile );

				if(!BitMap)
				{
					ostringstream strm;
					strm<<"Can't open "<<szAoiFile<<".";
					GenErr("CSynAoiScene::LoadAoiShape Error", strm.str());
				}

				BitMap.Seek( 18 );

				uint32 uWidth;
				uint32 uHeight;
				
				ReadShapeFile( BitMap, &uWidth, sizeof(uWidth) );
				ReadShapeFile( BitMap, &uHeight, sizeof(uHeight) );

				bool* aryShape=(bool*)SqrNew(sizeof(bool) * uWidth * uHeight, CQuadMallocObject::GetPool());

				const uint32 uMore = uWidth % 4;

				VerNe( BitMap.Seek( 54 ), -1 );

				CColor PixelColor = 0;			
				CPos posCenter;
				
				// 找到中心点
				uint32 i = 0;
				for( ; i < uHeight; i++ )
				{
					uint32 j = 0;
					for( ; j < uWidth; j++ )
					{
						ReadShapeFile( BitMap,  &PixelColor, 3 );
						if( PixelColor == 0xff0000 )
						{
							posCenter.x = j;
							posCenter.y = i;
							break;
						}
					}
					BitMap.Seek( uMore, SEEK_CUR );

					if(j<uWidth)
						break;
				}
				if( i>=uHeight )
					GenErr("Can't find the center point in aoi file.");
				
				VerNe( BitMap.Seek( 54 ), -1 );
				// 计算偏移
				for( uint32 i = 0; i < uHeight; i++ )
				{
					for( uint32 j = 0; j < uWidth; j++ )
					{
						ReadShapeFile( BitMap,  &PixelColor, 3 );

						aryShape[i*uHeight+j] = ( PixelColor == 0x000000 || PixelColor == 0xff0000 );
					}
					BitMap.Seek( uMore, SEEK_CUR );
				}

				pShape->m_uWidth=uWidth;
				pShape->m_uHeight=uHeight;
				pShape->m_aryShape=aryShape;
				pShape->m_posCenter=posCenter;
			}
			SQR_CATCH(exp)
			{
				LogExp( exp );
			}
			SQR_TRY_END;
		}
	}
	SQR_CATCH(exp)
	{
		LogExp( exp );
	}
	SQR_TRY_END;

	m_lkAoiShapes.Unlock();

	return pShape;
}
