#pragma once
#include "CEventOwner.h"
#include "CameraPathEdit.h"
#include "TSingleton.h"

namespace sqr
{
	class CMapEditCamera;
	class CRecordCamera;
	class ICamPathHandler;

	class CToolRecord: public Singleton<CToolRecord>
	{
		typedef vector< CCameraPathEdit* > CamAnimVector;
		CamAnimVector		 m_vecCamAnim;
		int					 m_nState;
		int					 m_nCurrentAnimIndex;
		int					 m_nCurrentKeyIndex;
		uint32				 m_fStartTime;			// 存储动画播放开始时间
		uint32				 m_fPauseDelta;			// 存储暂停前的已经播放的时间
		ICamPathHandler		*m_pHandler;
		CRecordCamera		*m_pRecordCam;

	public:
		static const int INVALID_KEY_POS	 = CCameraPathEdit::INVALID_KEY_POS;
		static const int PLAY_STATE_PLAYING  = 0;
		static const int PLAY_STATE_STOP	 = 1;
		static const int PLAY_STATE_PAUSE	 = 2;

		CToolRecord();
		~CToolRecord();

		virtual void		Clear();

		virtual void		AddAnim();
		virtual void		DeleteAnim( uint32 uAnimIndex );
		virtual void		SetCurrentAnim( int uAnimIndex );
		virtual int			GetCurrentAnim() const; 
		virtual uint32		GetAnimCount() const;
		virtual float		GetAnimLen( uint32 uAnimID ) const;

		virtual int			AddKey( float fNewKeyTime );
		virtual void		DeleteKey( uint32 uAnimKeyIndex );
		virtual uint32		GetKeyCount() const;
		virtual void		SetCurrentKey( int uKeyIndex );
		virtual int			GetCurrentKey() const;
		virtual float		GetKeyTime( uint32 uKeyIndex ) const;
		virtual void		GetKeyValue( CCameraPathEdit::Key &sKyeValue, uint32 uKeyIndex ) const;
		virtual float		GetKeyValueFov( uint32 uKeyIndex ) const;
		virtual void		SetKeyTime( float fNewTime, uint32 uKeyIndex );
		virtual void		SetKeyValue( uint32 uKeyIndex );

		// ---- 取得悬停帧时间
		virtual float		GetLoopKeyMask() const;
		
		virtual void		SetFov( float fFov );
		virtual float		GetFov() const;
		virtual void		SetCamSpeed( float fSpeed );
		virtual float		GetCamSpeed();
		virtual void		SetCamPos(float x,float y,float z);
		virtual CVector3f	GetCamPos();
		virtual void		SetCamTarget(float x,float y,float z);
		virtual CVector3f	GetCamTarget();

		virtual void		RePlay();
		virtual void		Pause();
		virtual void		Continue();
		virtual void		Stop();
		virtual void		ResetStartTime( uint32 fStartTime );
		virtual void		UpdateFPSLimited();
		virtual void		SetHandler( ICamPathHandler *pHandler );
		virtual void		ViewOnKey( uint nKey );
		
		virtual void		SetUseFPSCam( bool bOperateTarget );
		virtual void		PathBindCamera( uint32 uCurPath );

		virtual void		Save( FILE * fp );
		virtual void		Load( FILE * fp );

		virtual void		SaveRecordInfo(WriteDataInf* wdi);
		virtual void		LoadRecordInfo(CBufFile* tData);

		virtual void		OnEvent(const CMsg& msg);
		virtual int			GetState();
	};
}