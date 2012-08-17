#include "CoreCommon.h"

#include "CPos.h"

inline void InlPixelPos2GridPos( CPos& GridPos,const CFPos& PixelPos )
{
	GridPos.x=GetGridByPixel(PixelPos.x);
	GridPos.y=GetGridByPixel(PixelPos.y);
}

inline void InlGridPos2PixelPos( CFPos& PixelPos,const CPos& GridPos )
{
	PixelPos.x=GetPixelOfGrid(GridPos.x);
	PixelPos.y=GetPixelOfGrid(GridPos.y);
}

