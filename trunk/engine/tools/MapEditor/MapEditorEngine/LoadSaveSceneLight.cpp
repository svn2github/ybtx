//------------------------------------------------------------------------------
//  CLocalMapLoginSaver.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "LoadSaveSceneLight.h"
#include "GraphicCommon.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "TerrainMeshObject.h"
#include "CMapEditLoginObj.h"
#include "ModelGroup.h"

enum
{
	INVALID_VERSION,
	LOGIN_DEFAULT,
	LOGIN_LOGICNAME,
	LOGIN_OFFSET,
};

namespace sqr
{

	//------------------------------------------------------------------------------
	CLoadSaveSceneLight::CLoadSaveSceneLight()
		: terrain(NULL)
	{
	}

	void CLoadSaveSceneLight::SaveSceneLightsInfo()
	{
		FILE * fp = NULL;
		fp = fopen("..\\light.info","wb");

		if (!fp)
		{
			MessageBox(NULL,"light.info 无法保存","提示",MB_OK);
			return;
		}

		int nSceneLightCount = 0;
		int nSpotLightCount  = 0;
		int nGridCnt = terrain->GetGridCount();
		set<int> pointLightGridIndexs, spotLightGridIndexs;
		for ( int n = 0; n < nGridCnt; ++n )
		{
			SGrid * pGrid = &terrain->GetGrid(n);
			if ( !pGrid->vecCenterObjects.empty() )
			{
				for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin(); iter != pGrid->vecCenterObjects.end(); ++iter)
				{
					CTObjectPtr p = (*iter);

					CEditModelGroup *pModelGroup = p->GetModelGroup();

					//记录场景点光源个数---
					if (pModelGroup->m_SL.m_bIAmbScenePointLight)
					{
						pointLightGridIndexs.insert(n);
						++nSceneLightCount;
					}

					//记录场景聚光灯个数---
					if (pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight)
					{
						spotLightGridIndexs.insert(n);
						++nSpotLightCount;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		///point light
		fwrite( &nSceneLightCount, sizeof(int), 1, fp);

		int nGridIndex = 0, nPointLightNum = 0;
		set<int>::iterator beg, end = pointLightGridIndexs.end();
		for ( beg = pointLightGridIndexs.begin(); beg != end; ++beg )
		{
			nGridIndex = *beg;
			SGrid * pGrid = &terrain->GetGrid(nGridIndex);
			if ( !pGrid->vecCenterObjects.empty())
			{
				size_t size = pGrid->vecCenterObjects.size();
				for ( size_t m = 0; m < size; ++m)
				{
					CTObjectPtr p = pGrid->vecCenterObjects[m];
					if (p->GetModelGroup()->m_SL.m_bIAmbScenePointLight)
					{
						fwrite( &nGridIndex, sizeof(nGridIndex), 1, fp);
						fwrite( &m,			 sizeof(m),			 1, fp);
						fwrite( &p->GetModelGroup()->m_SL, sizeof(SceneLight), 1, fp);
						++nPointLightNum;
					}
				}
			}
		}

		if( nPointLightNum != nSceneLightCount )
			MessageBox(NULL,"2次点光源灯数不一样","提示",MB_OK);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///spot light
		fwrite( &nSpotLightCount, sizeof(int), 1, fp);

		int nSpotLightNum = 0;
		nGridIndex = 0;
		end = spotLightGridIndexs.end();
		for ( beg = spotLightGridIndexs.begin(); beg != end; ++beg )
		{
			nGridIndex = *beg;
			SGrid * pGrid = &terrain->GetGrid(nGridIndex);
			if ( !pGrid->vecCenterObjects.empty())
			{
				size_t size = pGrid->vecCenterObjects.size();
				for ( size_t m = 0; m < size; ++m)
				{
					CTObjectPtr p = pGrid->vecCenterObjects[m];
					if (p->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight)
					{
						fwrite( &nGridIndex, sizeof(nGridIndex), 1, fp);
						fwrite( &m,			 sizeof(m),			 1, fp);
						fwrite( &p->GetModelGroup()->m_SpotLight, sizeof(SceneSpotLight), 1, fp);
						++nSpotLightNum;
					}
				}
			}
		}

		if( nSpotLightCount != nSpotLightCount )
			MessageBox(NULL,"2次点聚光源灯数不一样","提示",MB_OK);
		//////////////////////////////////////////////////////////////////////////

		fclose(fp);

		MessageBox(NULL,"保存完毕","提示",MB_OK);
	}

	void CLoadSaveSceneLight::LoadSceneLightsInfo()
	{
		FILE * fp = NULL;
		fp = fopen("..\\light.info","rb");
		if (! fp )
		{
			MessageBox(NULL,"F:\\light.info 无法打开","请换其他地图",MB_OK);
			return;
		}

		int		  nGridIndex = 0;
		size_t    nGridObjectIndex = 0;
		CVector3f vUserOffsetEx;
		string	  strEffectName = "";
		//////////////////////////////////////////////////////////////////////////
		///read point light
		int nPointLightCount = 0;
		fread( &nPointLightCount, sizeof(nPointLightCount), 1, fp);
		for ( int i=0; i < nPointLightCount; ++i )
		{
			SceneLight tempPointLight;
			fread( &nGridIndex,		  sizeof(nGridIndex),       1, fp);
			fread( &nGridObjectIndex, sizeof(nGridObjectIndex), 1, fp);
			fread( &tempPointLight,   sizeof(SceneLight),       1, fp);

			if ( terrain->IsValid(nGridIndex) )
			{
				SGrid *pGrid = NULL;
				pGrid = &terrain->GetGrid(nGridIndex);
				size_t size = pGrid->vecCenterObjects.size();

				if ( !pGrid->vecCenterObjects.empty() )
				{
					if ( nGridObjectIndex < size )
					{
						CTObjectPtr p = pGrid->vecCenterObjects[nGridObjectIndex];
						bool bEffecIsExit = p->GetModelGroup()->GetEffectName(strEffectName);
						if ( strEffectName.find("场景光源\\Standar") == 0 && bEffecIsExit )
						{
							p->GetModelGroup()->m_SL = tempPointLight;
							p->GetModelGroup()->SetIsLightEffect(true);
						}
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		///read spot light
		int nSpotLightCount = 0;
		fread( &nSpotLightCount, sizeof(nSpotLightCount), 1, fp);
		for ( int i=0; i < nSpotLightCount; ++i )
		{
			SceneSpotLight tempSpotLight;
			fread( &nGridIndex,		  sizeof(nGridIndex),       1, fp);
			fread( &nGridObjectIndex, sizeof(nGridObjectIndex), 1, fp);
			fread( &tempSpotLight,    sizeof(SceneSpotLight), 1, fp);

			if ( terrain->IsValid(nGridIndex) )
			{
				SGrid *pGrid = NULL;
				pGrid = &terrain->GetGrid(nGridIndex);
				size_t size = pGrid->vecCenterObjects.size();

				if ( !pGrid->vecCenterObjects.empty() )
				{
					if ( nGridObjectIndex < size )
					{
						CTObjectPtr p = pGrid->vecCenterObjects[nGridObjectIndex];
						bool bEffecIsExit = p->GetModelGroup()->GetEffectName(strEffectName);
						if ( strEffectName.find("场景光源\\Standar") == 0 && bEffecIsExit )
						{
							p->GetModelGroup()->m_SpotLight = tempSpotLight;
							p->GetModelGroup()->SetIsLightEffect(true);
						}
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		fclose(fp);

		MessageBox(NULL,"恢复完毕","提示",MB_OK);
	}
}