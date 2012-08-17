#include "StdAfx.h"
#include "ShadowMapBakerPool.h"
#include "CGraphic.h"

#define SHAODW_BUF_MAX_NUM 9

namespace sqr
{

CShadowMapBakingPool::CShadowMapBakingPool(void)
{
	MaxNum=SHAODW_BUF_MAX_NUM;
}

CShadowMapBakingPool::~CShadowMapBakingPool(void)
{
}

bool CShadowMapBakingPool::FindShadowBuf(int tileIndex,int& bufID)
{
	int count=0;
	vector<ShadowMapBuf>::iterator it=DepthBufList.begin();
	while(it!=DepthBufList.end())
	{
		if(it->TileIndex==tileIndex)
		{
			bufID=count;
			return true;
		}
		count++;
		it++;
	}
	return false;
}

bool CShadowMapBakingPool::AddShadowBuf(int tileIndex,CAxisAlignedBox& box,BUF_FLAG flag)
{
	int BufId=0;
	if(FindShadowBuf(tileIndex,BufId))
	{
		DepthBufList[BufId].Flag=FLAG_USE;
		return true;
	}
	else
	{
		int size=DepthBufList.size();
		if(size<MaxNum)  //Î´Âú
		{
			ShadowMapBuf tempBuf;
			tempBuf.TileIndex=tileIndex;
			tempBuf.Flag=flag;
			tempBuf.Box.setNull();
			tempBuf.Box.mergeBox(box);
			tempBuf.ShadowTexture = CGraphic::GetInst()->CreateRenderTarget();
			tempBuf.ShadowTexture->Release(); //ÁÙÊ±²¹¶¡£¬Ç¿ÖÆ¼ÆÊýÆ÷¼õ1
			tempBuf.ShadowTexture->SetWidth(2048);
			tempBuf.ShadowTexture->SetHeight(2048);
			tempBuf.ShadowTexture->SetFormat(TFMT_R32F);
			tempBuf.ShadowTexture->SetUseDepthBuffer(true);
			tempBuf.ShadowTexture->Create();

			tempBuf.ShadowTexture->AutoClearColor(false);
			tempBuf.ShadowTexture->SetClearColor(CColor::White);

			tempBuf.MatrixBuild = new COrthoMatrixBuilder();
			tempBuf.MatrixBuild->SetCamera(CMainWindowTarget::GetInst()->GetCamera());
			tempBuf.MatrixBuild->SetShadowMapSize(2048);
			
			DepthBufList.push_back(tempBuf);
			return true;
		}
		else              //Âú
		{
			vector<ShadowMapBuf>::iterator it=DepthBufList.begin();
			while(it!=DepthBufList.end())
			{
				if(it->Flag==FLAG_NONE)
				{
					it->Flag=flag;
					it->Box.setNull();
					it->Box.mergeBox(box);
					it->TileIndex=tileIndex;
					return true;
				}
				it++;
			}
		}
	}
	return false;
}

bool CShadowMapBakingPool::DelShadowBuf(int tileIndex)
{
	vector<ShadowMapBuf>::iterator it=DepthBufList.begin();
	while(it!=DepthBufList.end())
	{
		if(it->TileIndex==tileIndex)
		{
			it->ShadowTexture=NULL;
			it->MatrixBuild=NULL;
			DepthBufList.erase(it);
			return true;
		}
		it++;
	}
	return false;
}

void CShadowMapBakingPool::SetShadowMapBufFlag(int tileIndex,BUF_FLAG flag)
{
	vector<ShadowMapBuf>::iterator it=DepthBufList.begin();
	while(it!=DepthBufList.end())
	{
		if(it->TileIndex==tileIndex)
		{
			it->Flag=flag;
			return ;
		}
		it++;
	}
}

void CShadowMapBakingPool::ResetShadowBufFlag(int tileIndex)
{
	vector<ShadowMapBuf>::iterator it=DepthBufList.begin();
	while(it!=DepthBufList.end())
	{
		if(it->TileIndex==tileIndex)
		{
			it->Flag=FLAG_NONE;
			return ;
		}
		it++;
	}
}

void CShadowMapBakingPool::ResetShadowBufFlag()
{
	vector<ShadowMapBuf>::iterator it=DepthBufList.begin();
	while(it!=DepthBufList.end())
	{
		it->Flag=FLAG_NONE;
		it++;
	}
}

bool  CShadowMapBakingPool::GetShadowBuf(int tileIndex,ShadowMapBuf& buf)
{
	vector<ShadowMapBuf>::iterator it=DepthBufList.begin();
	while(it!=DepthBufList.end())
	{
		if(it->TileIndex==tileIndex)
		{
			buf.Flag=it->Flag;
			buf.Box.setNull();
			buf.Box.mergeBox(it->Box);
			buf.TileIndex=it->TileIndex;
			buf.ShadowTexture=it->ShadowTexture;
			buf.MatrixBuild=it->MatrixBuild;
			return true;
		}
		it++;
	}
	return false;
}

void CShadowMapBakingPool::ClearShadowBuf()
{
	vector<ShadowMapBuf>::iterator it= DepthBufList.begin();
	while( it != DepthBufList.end() )
	{
		it->ShadowTexture=NULL;
		it->MatrixBuild=NULL;
		it++;
	}
	DepthBufList.clear();
}

void CShadowMapBakingPool::SetBufNum(int num)
{
	MaxNum=num;
}

int CShadowMapBakingPool::GetBufNum()
{
	return MaxNum;
}

}