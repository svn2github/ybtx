#pragma once
#include "ColorAnimCtrl/CColorAnimImp.h"

namespace sqr_tools
{
	class CColorAnimTypeBase
	{
	public:
		typedef CColorAnimImp::TrackNodeMap TrackNodeMap;
		typedef TrackNodeMap::iterator		 TrackNodeMapIter;

	public:
		CColorAnimTypeBase( TrackNodeMap& nodeMap );

		virtual void 
				SetData( void* dataIn ) = 0;

		virtual void
				Update() = 0;

		static CColorAnimTypeBase*
				CreateImp( ECtrlDataType type, TrackNodeMap& nodeMap );

	protected:
		TrackNodeMap& m_nodeMap;
	};
}