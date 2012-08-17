#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr
{
	class CModel;
	class IDNAME;
}

namespace sqr_tools
{
	class CFrameScaleDelegate;
	class CFrameScaleCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CFrameScaleCtrlImp);
	public:
		CFrameScaleCtrlImp(void* param);
		~CFrameScaleCtrlImp(void);

	private:
		int				m_nScaleLen;				//根据控件得到标尺的长度
		int				m_nFrameStart;              //起始帧
		int				m_nFrameEnd;				//结束帧
		float			m_fFrameCur;				//当前帧
		int				m_iFrameCur;
		int				m_nFramesPerGrid;			//每格的帧数
		int				m_nPixPerGrid;				//每格的象素

		map< int, int > m_KeyFrame;
		bool	m_bPlay;

	public:
		virtual void Update();

		virtual void SetFrameScaleLen(const int nLen)
		{
			m_nScaleLen = nLen;
		}
		
		virtual void SetAniStartFrame(const int start)
		{
			m_nFrameStart = start;
		}
		virtual int GetAniStartFrame()
		{
			return m_nFrameStart;
		}

		virtual void SetAniEndFrame(const int end);
		virtual int GetAniEndFrame()
		{
			return m_nFrameEnd;
		}

		virtual int GetAniFrameNum()
		{
			return m_nFrameEnd - m_nFrameStart;
		}

		virtual float GetCurFrame() 
		{
			return m_fFrameCur;
		}

		virtual int GetFramesPerGrid()
		{
			return m_nFramesPerGrid;
		}

		virtual int GetPixPerGrid()
		{
			return m_nPixPerGrid;
		}
		
		virtual void  ClearKeyFrame()						
		{ 
			m_KeyFrame.clear(); 
		}
		virtual void  SetKeyFrame( int Num )				
		{ 
			m_KeyFrame[Num] = Num; 
		}
		virtual map< int, int > GetKeyFrame()
		{
			return m_KeyFrame;
		}

		virtual bool IsPlay(void)
		{
			return m_bPlay;
		}

		virtual void CalFramePerGrid();
		virtual void CalCurrentFrame(const float x);
		virtual void Play(void);
		virtual void Stop(void);
		virtual float GetUpdateFrame(void);
		virtual void Resize(void);

	private:
		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );

	private:
		map<unsigned short,sqr::IDNAME>*  m_pKeyFrames;
		int						m_iMaxFrameNum;
		int						m_iKeyFrameNum;
		CFrameScaleDelegate*    m_pDelegate;
	};

}
