#pragma once
#include "CVBRef.h"


namespace sqr
{
typedef map<int32, VBRefPtr > PieceColorMap;
struct PieceClassColor
{
	string className;
	PieceColorMap pieceColorMap;
};
typedef string ModelName;
typedef map<ModelName, vector<PieceClassColor> > ModelGroupColorMap;
typedef uint16 ModelGroupIndex;

}