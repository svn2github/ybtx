#pragma once
#include "TRefPointer.h"
#include "Cmd.h"
#include "ExsertStructDB.h"


namespace sqr
{
	SmartPointer(CTObject);

	class CCmdBornObject : public CCmd
	{
		struct SDiffBornInfo
		{
			CTObjectPtr pObject;
			CTObjectPtr pObjectPre;
			vector<string> veCMapEditModelFileNames;
			SDiffBornInfo() : pObject(NULL),pObjectPre(NULL),bSubUnit(false),bCenterType(sqr::ECT_NONE),
				bTranslucent(true), bResGroupMark(0), bAudio(false), bLoginModel(false) { }
			~SDiffBornInfo() {}
			bool bSubUnit;
			sqr::ECenter_Type bCenterType;
			vector< vector<sqr::SBlockInfo> > vvBlock;
			vector< vector<sqr::SLogicHeightInfo> > vvHeight;
			CVector3f vUserOffset;
			CVector3f vUserOffsetEx;
			string strItemName;
			CVector3f vGroupOffset;
			float fRotation;
			int nRotationCnt;
			float fPitch;
			int nPitchCnt;
			float fYaw;
			int nYawCnt;
			float xScale;
			float yScale;
			float zScale;
			bool bCameraOriented;
			bool bEffectBase;
			bool bTranslucent;
			bool bShadow;
			BYTE bResGroupMark;//是否是npc
			bool bVisibleByConfigure;//是否受配置限制决定是否隐藏
			bool bAudio;
			bool bLoginModel;
		};

		SDiffBornInfo m_diff;

	public:
		CCmdBornObject() { }

		~CCmdBornObject() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void SetModelFileNames(vector<string> const & vec);

		void SetSubUnit(bool b);

		void SetTranslucent(bool b);

		void SetCastShadow(bool b);

		void SetCenterType(sqr::ECenter_Type eCenterType);

		void SetBlockInfo(vector< vector<sqr::SBlockInfo> > & vvBlock);

		void SetHeightInfo(vector< vector<sqr::SLogicHeightInfo> > & vvHeight);

		void SetUserOffset(float fLift, float fShiftX, float fShiftZ);

		void SetUserOffsetEx(float fLiftEx, float fShiftXEx, float fShiftZEx);

		void SetItemName(string & str);

		void SetGroupOffset(CVector3f vGroupOffset);

		void SetRotationCount(int nRotationCnt);

		void SetRotation(float fRotation);

		void SetYawCount(int nRotationCnt);
		void SetYaw(float fR);
		void SetPitchCount(int nRotationCnt);
		void SetPitch(float fR);
		void SetScaleX(float x);
		void SetScaleY(float y);
		void SetScaleZ(float z);

		void SetCameraOriented(bool b);

		void SetIsEffectBase(bool b);

		void SetResGroupMark(BYTE bResGroupMark);
		void SetIsVisibleByConfigure(bool b);
		void SetIsAudio(bool b);
		void SetIsLoginModel(bool b);
	};
}