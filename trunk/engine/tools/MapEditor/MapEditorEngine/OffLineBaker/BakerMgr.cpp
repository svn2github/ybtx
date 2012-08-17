#include "StdAfx.h"
#include "BakerMgr.h"
#include "CRenderPipeline.h"

namespace sqr
{

CBakerMgr::CBakerMgr(void)
:tileWidth(0),tileDepth(0),distrubSize(3)
{
}

CBakerMgr::~CBakerMgr(void)
{
}

void CBakerMgr::SetTileSize(int width,int depth)
{
	tileWidth=width;
	tileDepth=depth;
}

void CBakerMgr::Setup()
{
	vector<CBakerBasePtr>::iterator it=MethodList.begin();
	while(it!=MethodList.end())
	{
		(*it)->Setup();
		it++;
	}
	return ;
}

void CBakerMgr::Discard()
{
	vector<CBakerBasePtr>::iterator it=MethodList.begin();
	while(it!=MethodList.end())
	{
		(*it)->Discard();
		(*it)=NULL;
		it++;
	}
	return ;
}

void CBakerMgr::OnProcess(index_t x,index_t z)
{
	//根据LEVEL级别计算相关快
	unsigned int ID=x+z*tileWidth;

	//int distrubSize=3;                       //影响范围
	int disturbNum=this->distrubSize*this->distrubSize;  //干扰块数
	
	//根据光向计算影响的TILE
	DisturbTile.clear();
	for(int j=0;j<distrubSize;j++)
	{
		for(int k=0;k<distrubSize;k++)
		{
			int offset=k+j*tileWidth;
			int basicId=ID-1-tileWidth;
			int tempId=basicId+offset;	
			if(tempId<0 || tempId>=int(tileWidth*tileDepth) )
				continue;
			if( tempId/tileWidth!=(ID/tileWidth+j-1) )
				continue;
			CVector3ud disturbVec;
			disturbVec.x=tempId%tileWidth;
			disturbVec.y=0;
			disturbVec.z=tempId/tileWidth;	
			DisturbTile.push_back(disturbVec);
		}
	}
	
	vector<CBakerBasePtr>::iterator it=MethodList.begin();
	while(it!=MethodList.end())
	{	
		//准备
		(*it)->OnPrepare();
		//Tile准备
		vector<CVector3ud>::iterator it1=DisturbTile.begin();
		while(it1!=DisturbTile.end())
		{	
			(*it)->TilePrepare(it1->x,it1->z);
			it1++;
		}
		//过程
		(*it)->OnProcess(x,z);
		it++;
	}
	return ;
}

bool CBakerMgr::AddOffLineRenderMethod(CBakerBasePtr p)
{
	Ast(p!=NULL);
	MethodList.push_back(p);
	return true;
}

bool CBakerMgr::DelOffLineRenderMethod(CBakerBasePtr p)
{
	Ast(p!=NULL);
	vector<CBakerBasePtr>::iterator it=MethodList.begin();
	while(it!=MethodList.end())
	{
		if( p==(*it) )
		{
			MethodList.erase(it);
			return true;
		}
		it++;
	}
	return false;
}

bool CBakerMgr::ClearOffLineRenderMethod()
{
	MethodList.clear();
	return true;
}

void CBakerMgr::SetdistrubSize( int distrubSize )
{
	this->distrubSize = distrubSize;
}
}