#pragma once
#include "CEventOwner.h"
#include "CVector3.h"
#include "CMatrix.h"
#include "TSingleton.h"

namespace sqr_tools
{
	class CEditModel;
}

namespace sqr
{
	class IDNAME;

	class CToolMainPlayer: public Singleton<CToolMainPlayer>
	{
		CEditModel* m_pMainPlayer;
		CMatrix m_mat;

		CVector3f m_vStart;
		CVector3f m_vEnd;
		CVector3f m_vPos;
		CVector3f m_vDir;

		float m_fSpeed;
		float m_fRoad;

		string m_ani;

		string m_strMainPlayerModel;
		string m_strMainPlayerAni;

		bool m_bStop;

		bool m_bBlock;

		float m_fRed;
		float m_fGreen;
		float m_fBlue;

		bool m_bLoadMainPlayer;
		bool m_bHideMainPlayerShadow;
		WORD m_lastKeyFrame;
		bool m_bHumanPlayer;///是不是主角
	public:
		CToolMainPlayer();
		~CToolMainPlayer();

		void SetDoesEnableBlock(bool b);
		bool GetDoesEnableBlock();
		CMatrix& GetMMat();

		void OnEvent(const CMsg& msg);
		void UpdateFPSLimited();
		void Render();
		void UnRender();
		void CreateModel();
		void SetPosition(CVector3f & v);
		void SetEnd(CVector3f & v);

		void SetMainPlayerModel(string & str);

		string GetMainPlayerModel();

		void SetMainPlayerAni(string & str);

		string GetMainPlayerAni();

		void Restart();

		void Save(FILE * fp);
		void Load(FILE * fp);

		void SetHidePlayerShadow(bool b);

		bool GetIsHidePlayerShadow();

		CVector3f GetCurPlayerDirect();

		CVector3f GetCurPlayerPosition();

		void Init(void);

	private:
		void OnKeyFrame(const IDNAME* name);
		void UpdatePlayerMaterial(const int gridIndex);
		string 	m_strCurMatCueName;
	};

	//------------------------------------------------------------------------------
	inline CMatrix&
	CToolMainPlayer::GetMMat()
	{
		return m_mat;
	}

	inline void CToolMainPlayer::SetPosition(CVector3f & v)
	{
		m_vPos = v;
	}

	inline void CToolMainPlayer::SetEnd(CVector3f & v)
	{
		m_vEnd = v;
	}

	inline void CToolMainPlayer::SetDoesEnableBlock(bool b)
	{
		m_bBlock = b;
	}

	inline bool CToolMainPlayer::GetDoesEnableBlock()
	{
		return m_bBlock;
	}

	inline string CToolMainPlayer::GetMainPlayerModel()
	{
		return m_strMainPlayerModel;
	}

	inline string CToolMainPlayer::GetMainPlayerAni()
	{
		return m_strMainPlayerAni;
	}

	inline void CToolMainPlayer::SetHidePlayerShadow( bool b )
	{
		m_bHideMainPlayerShadow = b;
	}

	inline bool CToolMainPlayer::GetIsHidePlayerShadow()
	{
		return m_bHideMainPlayerShadow;
	}

	inline CVector3f CToolMainPlayer::GetCurPlayerDirect()
	{
		return m_vDir;
	}
}
