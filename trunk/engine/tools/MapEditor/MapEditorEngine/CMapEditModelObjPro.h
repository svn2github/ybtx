#pragma once

#include "ExsertStructDB.h"

namespace sqr
{
	struct MoldeSavePropertyOld
	{
		MoldeSavePropertyOld()
		{
			nCenterGridIndex	= 0;
			nRotationCnt		= 0;
			nPitchCnt			= 0;
			nYawCnt				= 0;
			nMirrorType			= 0;

			vRotate				= CVector3f(0.0f, 0.0f, 0.0f);
			vScale				= CVector3f(1.0f, 1.0f, 1.0f);
			vUserOffsetEx		= CVector3f(0.0f, 0.0f, 0.0f);

			byRenderStyle		= 0;
			bReceiveShadow		= false;
			bPlayAnimate		= false;
			bShadow				= false;
			bTranslucent		= false;
			bVisibleByConfigure	= false;
			bBakingColorMark	= false;
			bTransState			= 0;
		}

		///model center grid index;
		int nCenterGridIndex;

		///y轴旋转次数
		int nRotationCnt;

		///z轴旋转次数
		int nPitchCnt;

		///x轴旋转次数
		int nYawCnt;

		//0: 无镜像 1：前后镜像 2： 左右镜像
		int nMirrorType;

		BYTE byRenderStyle;
		bool bPlayAnimate;///on / off animation
		///是否有影子
		bool bShadow;
		///写是否接收阴影信息
		bool bReceiveShadow;
		///是否接收消隐
		bool bTranslucent;
		///trans state 0 - auto 1 -  trans 2 - untrans
		BYTE bTransState;
		//模型可见是否依赖配置值
		bool bVisibleByConfigure;
		//模型烘培颜色不比阴影色更暗,周自强希望烘焙后的草跟地表的lightmap颜色一致, 就是color = max(bakedColor, globalShadowColor);
		bool bBakingColorMark;

		///rotate
		CVector3f vRotate;
		//缩放
		CVector3f vScale;
		///offset
		CVector3f vUserOffsetEx;
	};

	//将模型属性抽象成一个结构体
	struct MoldeSavePropertyNoPos
	{
		MoldeSavePropertyNoPos()
		{
			nCenterGridIndex	= 0;
			nRotationCnt		= 0;
			nPitchCnt			= 0;
			nYawCnt				= 0;
			nMirrorType			= 0;
			
			vRotate				= CVector3f(0.0f, 0.0f, 0.0f);
			vScale				= CVector3f(1.0f, 1.0f, 1.0f);
			vUserOffsetEx		= CVector3f(0.0f, 0.0f, 0.0f);

			byRenderStyle		= 0;
			bReceiveShadow		= false;
			bPlayAnimate		= false;
			bShadow				= false;
			bTranslucent		= false;
			bVisibleByConfigure	= false;
			bBakingColorMark	= false;
			bUseCameraCollision = false;
			bTransState			= 0;
		}

		///model center grid index;
		int nCenterGridIndex;

		///y轴旋转次数
		int nRotationCnt;

		///z轴旋转次数
		int nPitchCnt;

		///x轴旋转次数
		int nYawCnt;

		//0: 无镜像 1：前后镜像 2： 左右镜像
		int nMirrorType;

		BYTE byRenderStyle;
		bool bPlayAnimate;///on / off animation
		///是否有影子
		bool bShadow;
		///写是否接收阴影信息
		bool bReceiveShadow;
		///是否接收消隐
		bool bTranslucent;
		///trans state 0 - auto 1 -  trans 2 - untrans
		BYTE bTransState;
		//模型可见是否依赖配置值
		bool bVisibleByConfigure;
		//模型烘培颜色不比阴影色更暗,周自强希望烘焙后的草跟地表的lightmap颜色一致, 就是color = max(bakedColor, globalShadowColor);
		bool bBakingColorMark;
		///物件增加摄像机碰撞标记（有些物体不想使用消隐，而是想使用摄像机碰撞）
		bool bUseCameraCollision;

		///rotate
		CVector3f vRotate;
		//缩放
		CVector3f vScale;
		///offset
		CVector3f vUserOffsetEx;
	};

	//将模型属性抽象成一个结构体
	struct MoldeSaveProperty
	{
		MoldeSaveProperty()
		{
			nCenterGridIndex	= 0;
			nRotationCnt		= 0;
			nPitchCnt			= 0;
			nYawCnt				= 0;
			nMirrorType			= 0;
			
			vPosition			= CVector3f(-1.0f, -1.0f, -1.0f);
			vRotate				= CVector3f(0.0f, 0.0f, 0.0f);
			vScale				= CVector3f(1.0f, 1.0f, 1.0f);
			vUserOffsetEx		= CVector3f(0.0f, 0.0f, 0.0f);

			byRenderStyle		= 0;
			bReceiveShadow		= false;
			bPlayAnimate		= false;
			bShadow				= false;
			bTranslucent		= false;
			bVisibleByConfigure	= false;
			bBakingColorMark	= false;
			bUseCameraCollision = false;
			bTransState			= 0;
		}

		MoldeSaveProperty MoldeSaveProperty::operator = (const MoldeSavePropertyOld& modelProRight)
		{
			this->nCenterGridIndex		= modelProRight.nCenterGridIndex;
			this->nRotationCnt			= modelProRight.nRotationCnt;
			this->nPitchCnt				= modelProRight.nPitchCnt;
			this->nYawCnt				= modelProRight.nYawCnt;
			this->nMirrorType			= modelProRight.nMirrorType;

			this->vRotate				= modelProRight.vRotate;
			this->vScale				= modelProRight.vScale;
			this->vUserOffsetEx			= modelProRight.vUserOffsetEx;

			this->byRenderStyle			= modelProRight.byRenderStyle;
			this->bReceiveShadow		= modelProRight.bReceiveShadow;
			this->bPlayAnimate			= modelProRight.bPlayAnimate;
			this->bShadow				= modelProRight.bShadow;
			this->bTranslucent			= modelProRight.bTranslucent;
			this->bVisibleByConfigure	= modelProRight.bVisibleByConfigure;
			this->bBakingColorMark		= modelProRight.bBakingColorMark;
			this->bTransState			= modelProRight.bTransState;

			this->bUseCameraCollision	= false;

			return *this;
		}

		MoldeSaveProperty MoldeSaveProperty::operator = (const MoldeSavePropertyNoPos& modelProRight)
		{
			this->nCenterGridIndex		= modelProRight.nCenterGridIndex;
			this->nRotationCnt			= modelProRight.nRotationCnt;
			this->nPitchCnt				= modelProRight.nPitchCnt;
			this->nYawCnt				= modelProRight.nYawCnt;
			this->nMirrorType			= modelProRight.nMirrorType;

			this->vRotate				= modelProRight.vRotate;
			this->vScale				= modelProRight.vScale;
			this->vUserOffsetEx			= modelProRight.vUserOffsetEx;

			this->byRenderStyle			= modelProRight.byRenderStyle;
			this->bReceiveShadow		= modelProRight.bReceiveShadow;
			this->bPlayAnimate			= modelProRight.bPlayAnimate;
			this->bShadow				= modelProRight.bShadow;
			this->bTranslucent			= modelProRight.bTranslucent;
			this->bVisibleByConfigure	= modelProRight.bVisibleByConfigure;
			this->bBakingColorMark		= modelProRight.bBakingColorMark;
			this->bTransState			= modelProRight.bTransState;

			this->bUseCameraCollision	= modelProRight.bUseCameraCollision;

			return *this;
		}

		MoldeSaveProperty MoldeSaveProperty::operator = (const MoldeSaveProperty& modelProRight)
		{
			this->nCenterGridIndex		= modelProRight.nCenterGridIndex;
			this->nRotationCnt			= modelProRight.nRotationCnt;
			this->nPitchCnt				= modelProRight.nPitchCnt;
			this->nYawCnt				= modelProRight.nYawCnt;
			this->nMirrorType			= modelProRight.nMirrorType;

			this->vPosition				= modelProRight.vPosition;
			this->vRotate				= modelProRight.vRotate;
			this->vScale				= modelProRight.vScale;
			this->vUserOffsetEx			= modelProRight.vUserOffsetEx;

			this->byRenderStyle			= modelProRight.byRenderStyle;
			this->bReceiveShadow		= modelProRight.bReceiveShadow;
			this->bPlayAnimate			= modelProRight.bPlayAnimate;
			this->bShadow				= modelProRight.bShadow;
			this->bTranslucent			= modelProRight.bTranslucent;
			this->bVisibleByConfigure	= modelProRight.bVisibleByConfigure;
			this->bBakingColorMark		= modelProRight.bBakingColorMark;
			this->bTransState			= modelProRight.bTransState;

			this->bUseCameraCollision	= modelProRight.bUseCameraCollision;

			return *this;
		}

		bool MoldeSaveProperty::operator == (const MoldeSaveProperty& modelProRight) const
		{
			return this->nCenterGridIndex	== modelProRight.nCenterGridIndex &&
				this->nRotationCnt			== modelProRight.nRotationCnt &&
				this->nPitchCnt				== modelProRight.nPitchCnt &&
				this->nYawCnt				== modelProRight.nYawCnt &&
				this->nMirrorType			== modelProRight.nMirrorType &&
				this->vRotate				== modelProRight.vRotate &&
				this->vScale				== modelProRight.vScale &&
				this->vUserOffsetEx			== modelProRight.vUserOffsetEx &&
				this->byRenderStyle			== modelProRight.byRenderStyle &&
				this->bReceiveShadow		== modelProRight.bReceiveShadow &&
				this->bPlayAnimate			== modelProRight.bPlayAnimate &&
				this->bShadow				== modelProRight.bShadow &&
				this->bTranslucent			== modelProRight.bTranslucent &&
				this->bVisibleByConfigure	== modelProRight.bVisibleByConfigure &&
				this->bBakingColorMark		== modelProRight.bBakingColorMark &&
				this->bTransState			== modelProRight.bTransState &&
				this->bUseCameraCollision	== modelProRight.bUseCameraCollision;
		}

		///model center grid index;
		int nCenterGridIndex;

		///y轴旋转次数
		int nRotationCnt;
		
		///z轴旋转次数
		int nPitchCnt;

		///x轴旋转次数
		int nYawCnt;

		//0: 无镜像 1：前后镜像 2： 左右镜像
		int nMirrorType;

		BYTE byRenderStyle;
		bool bPlayAnimate;///on / off animation
		///是否有影子
		bool bShadow;
		///写是否接收阴影信息
		bool bReceiveShadow;
		///是否接收消隐
		bool bTranslucent;
		///trans state 0 - auto 1 -  trans 2 - untrans
		BYTE bTransState;
		//模型可见是否依赖配置值
		bool bVisibleByConfigure;
		//模型烘培颜色不比阴影色更暗,周自强希望烘焙后的草跟地表的lightmap颜色一致, 就是color = max(bakedColor, globalShadowColor);
		bool bBakingColorMark;
		///物件增加摄像机碰撞标记（有些物体不想使用消隐，而是想使用摄像机碰撞）
		bool bUseCameraCollision;

		///position
		CVector3f vPosition;
		///rotate
		CVector3f vRotate;
		//缩放
		CVector3f vScale;
		///offset
		CVector3f vUserOffsetEx;
	};

	struct ModelCommonProperty
	{
		ModelCommonProperty()
		{
			eEditState = EES_DANGLING;
			eEditStatePlaced = EESP_NONE;
			matUserOffset.Identity();
			matSelfOff.Identity();
			matSelfOffXMirror.Identity();
			matSelfOffZMirror.Identity();
			matWorldOff.Identity();
			mat.Identity();
			matPieceOff.Identity();

			bSelected = false;
	
			vBlockCenter = CVector3f(0.0f,0.0f,0.0f);
			vUserOffset  = CVector3f(0.0f,0.0f,0.0f);
			bLocked = false;
			fRotationCurrent = 0.0f;
			fLift = 0.0f;
			fShiftX = 0.0f;
			fShiftZ = 0.0f;

			bSubUnit = false;
			bIsChanged = true;
			bFixedHeight = false;
	
			bCameraOriented = false;
			bEffectBase = false;
			bResGroupMark = 0;

			bHideModelVertexColor = false;
			bEffectLight = false;
			bStopCue = false;

			fRedOverallAmbientRatio = 1.0f;
			fBlueOverallAmbientRatio = 1.0f;
			fGreenOverallAmbientRatio = 1.0f;

			bNotGroup = TRUE;//%%%%%
			nCenterGridIndexCurrent = INVALID;//%%%%%

			strFileName		= "";
			strModelAgpName = "";
			strModelSkeName = "";
		}

		//是否是复合图素
		bool		bSubUnit;
		// 是否隐藏顶点色对模型的影响
		bool		bHideModelVertexColor;
		// 是否为npc
		BYTE		bResGroupMark;
		bool		bStopCue;
		bool		bEffectLight;
		bool		bLocked;
		bool		bFixedHeight;
		bool		bCameraOriented;
		bool		bEffectBase;
		bool		bIsChanged;//是否需要进行实时矩阵计算
		bool		bSelected;
		// 是否是群组
		BOOL		bNotGroup;

		float		fLift;
		float		fShiftX;
		float		fShiftZ;
		float		fHeight;
		float		fWidth;
		float		fDepth;
		float		fRedOverallAmbientRatio;
		float		fBlueOverallAmbientRatio;
		float		fGreenOverallAmbientRatio;
		float		fRotationCurrent;// 用于实时计算当前中心格子索引

		int			nWidthDimInGrids;
		int			nDepthDimInGrids;
		int			nXGroupOffset;
		int			nZGroupOffset;
		// center grid
		int			nPreCenterGridIndex;
		// derived from "scale and offset"
		int			nWidthRadius;
		int			nDepthRadius;
		int			nCenterGridIndexCurrent;// 实时计算得到的当前中心格子索引（用于群组旋转）

		EEdit_State eEditState;
		sqr::EEdit_State_Placed eEditStatePlaced;
		sqr::ECenter_Type eCenterType;

		string		strFileName;///model name
		string		strModelAgpName;
		string		strModelSkeName;

		CVector3f	m_vCorners[8];
		CVector3f	vCorners[8];
// 		CVector3f	vScale; ///模型本身根据大小的scale
// 		CVector3f	vOffset;
		CVector3f	vOffsetOverall;
		CVector3f	vBlockCenter;
		CVector3f	vImageCenter;
		CVector3f	vGroupOffset;
		CVector3f   vUserOffset;///在保存物件偏移信息后，该值作为打开地图后，物体移动的偏移量，加这个值是为了保存vUserOffsetEx从制作到最终在地图里进行的偏移
		// 物件群组的中心
		CVector3f vGroupCenter;

		CMatrix		matSelfOff;///SetOffsetOverall
		CMatrix		matSelfOffXMirror;
		CMatrix		matSelfOffZMirror;
		CMatrix		matUserOffset;
		CMatrix		matWorldOff;///格子索引给的偏移
		CMatrix		matRotY;	///xyz旋转后的矩阵
		CMatrix		matScale;	///缩放
		CMatrix		mat;
		CMatrix		WireBoxMat;
		CMatrix		matPieceOff;
	};
}
