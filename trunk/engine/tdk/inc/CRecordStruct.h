#pragma once

#include "CVector3.h"
#include <string>
#include <vector>

namespace sqr_tools
{
struct KeyInfo
{
	KeyInfo()
	{
		id=0;
		pos=sqr::CVector3f(0,0,0);
		dir=sqr::CVector3f(0,0,-1);
		fov=45.0f;
		speed=0.0f;
		time=0;
	};
	int				id;
	sqr::CVector3f	pos;
	sqr::CVector3f	dir;
	float			fov;
	float			speed;
	int				time;
};

struct RecordAnimate
{
	RecordAnimate()
	{
		id=0;
		timelong=0;
	};
	int				id;
	string			tip;		//∂Øª≠√Ë ˆ–≈œ¢
	int				timelong;
	vector<KeyInfo> keylist;
};

enum RecordState
{
	RECORD_PLAY,
	RECORD_PAUSE,
	RECORD_STOP
};

}