#pragma once
#include "BGCamera.h"

#define MOVE_VIEW_Y_SPEED 3.0f
#define MOVE_SPEED_RATIO 64.0f
#define ZOOM_SPEED_RATIO 10.0f

namespace sqr
{
	class CAnimCamera : public CBGCamera
	{
	protected:
		virtual void OnEvent(InputEvent & event);

	public:
		CAnimCamera(ISceneNode* pParent);
		~CAnimCamera() {}
		
		void	Rotate(float yaw, float pitch);
	};
}