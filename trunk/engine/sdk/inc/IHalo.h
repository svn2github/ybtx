// -----------------------------------------------------
// 模块名：光环接口，选定目标脚底可踩光环，或砸出的地面特效
// 作者：杨冰
// 创建时间：2005-11-14
// -----------------------------------------------------
#pragma once
#include "CVector3.h"

class IHalo
{
public:
	virtual void	Release( void )		= 0;
	virtual void	OnRender( void )	= 0;

	virtual void	SetPos( CVector3f & pos )	= 0;
	virtual void	SetSize( float fSize )		= 0;
	virtual void	SetColor( uint32 color )	= 0;
};