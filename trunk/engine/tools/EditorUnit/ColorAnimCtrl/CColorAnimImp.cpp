#include "stdafx.h"
#include "ColorAnimCtrl/CColorAnimImp.h"
#include "ColorAnimTypes/ColorAnimTypes.h"
#include "BaseHelper.h"
#include "CEventState.h"
#include <math.h>
#include "CCombinCmd.h"
#include "CmdMgr.h"

namespace sqr_tools
{
	class ColorDelCmd : public CPropBaseCmd<CColorAnimImp>
	{
	public:
		ColorDelCmd( CColorAnimImp* pCtrl, float pos ,bool update = false )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			m_Execute = true;
			m_bUpdate = update;
			if(GetCtrl()->m_ColorMap.find(pos) == GetCtrl()->m_ColorMap.end())
			{
				m_Execute = false;
			}
			else
				m_cData = GetCtrl()->m_ColorMap[m_fPos];
		}

		void _Execute(void)
		{
			if(m_Execute)
				GetCtrl()->m_ColorMap.erase(m_fPos);
			if(m_bUpdate)
				GetCtrl()->OnDataChanged();
		}

		void _UnExecute(void)
		{
			if(m_Execute)
				GetCtrl()->m_ColorMap[m_fPos] = m_cData;
			GetCtrl()->OnDataChanged();
		}

	protected:
		bool			m_Execute;
		bool			m_bUpdate;
		float			m_fPos;
		ColorAnimNode	m_cData;
	};

	class ColorInsCmd : public CPropBaseCmd<CColorAnimImp>
	{
	public:
		ColorInsCmd( CColorAnimImp* pCtrl, float pos,ColorAnimNode& node )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			//m_bInsert = GetCtrl()->m_ColorMap.find(m_fPos) == GetCtrl()->m_ColorMap.end();
			m_cData = node;
		}

		void _Execute(void)
		{
			GetCtrl()->m_ColorMap[m_fPos] = m_cData;
			GetCtrl()->OnDataChanged();
		}

		void _UnExecute(void)
		{
			GetCtrl()->m_ColorMap.erase(m_fPos);
		}

	protected:
		float			m_fPos;
		ColorAnimNode	m_cData;
	};

	class ColorEditCmd :  public CPropBaseCmd<CColorAnimImp>
	{
	public:
		ColorEditCmd( CColorAnimImp* pCtrl, float pos,ColorAnimNode& node )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			m_cOld = GetCtrl()->m_ColorMap[m_fPos];
			m_cData = node;
		}

		void _Execute(void)
		{
			GetCtrl()->m_ColorMap[m_fPos] = m_cData;
			GetCtrl()->OnDataChanged();
		}

		void _UnExecute(void)
		{
			GetCtrl()->m_ColorMap[m_fPos] = m_cOld;
			GetCtrl()->OnDataChanged();
		}

	protected:
		float			m_fPos;
		ColorAnimNode	m_cData;
		ColorAnimNode	m_cOld;
	};


	const float CONTRL_POINT_HALF_SIZE = 3.0f;

	DEF_UI_IMP(CColorAnimImp);

	CColorAnimImp::CColorAnimImp( void* param )
		: m_pTypeImp(NULL)
		, m_wndHandle(param)
		, m_fCurSelPos(0.0f)
	{
		m_ColorMap.insert(make_pair(0.0f, ColorAnimNode(0,0,0,0)));
		m_ColorMap.insert(make_pair(1.0f, ColorAnimNode(1,1,1,1)));

		OnCtrlSize(0, 0);
	}

	CColorAnimImp::~CColorAnimImp()
	{
		SafeDelete(m_pTypeImp);
	}

	void CColorAnimImp::SetColorMap( ECtrlDataType type, void* inMap )
	{
		if ( m_pTypeImp )
			SafeDelete(m_pTypeImp);

		m_pTypeImp = CColorAnimTypeBase::CreateImp(type, m_ColorMap);
		if ( m_pTypeImp )
			m_pTypeImp->SetData(inMap);

		CCtrlBase::Update();
	}

	int CColorAnimImp::GetKeyNum()
	{
		return (int)m_ColorMap.size();
	}

	bool CColorAnimImp::GetNode( ColorAnimNode& node, float& key, int idx )
	{
		TrackNodeMap::iterator it = m_ColorMap.begin();
		advance(it, idx);
		if ( it != m_ColorMap.end() )
		{
			key  = it->first;
			node = it->second;
			return true;
		}
		else
			return false;
	}

	bool CColorAnimImp::GetSelPointIdx( int& cur_idx )
	{
		int idx = 0;
		TrackNodeMapIter it = m_ColorMap.begin();
		while ( it != m_ColorMap.end() )
		{
			if ( it->first == m_fCurSelPos )
			{
				cur_idx = idx;
				return true;
			}

			++idx;
			++it;
		}

		cur_idx = -1;
		return false;
	}

	bool CColorAnimImp::OnCtrlMouseMove( int x, int y )
	{
		if ( CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) != MGT_MOUSE_LBNDOWN )
			return false;

		ColorAnimNode new_node = getColor(m_fCurSelPos);
		CCombinCmd* pCtrlMove = new CCombinCmd;
		m_bUpdateLock = true;
		CCmdMgr::GetInst()->UnDo();
		m_bUpdateLock = false;

		if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) != MGT_KEY_DOWN )
		{
			pCtrlMove->Push(new ColorDelCmd(this,m_fCurSelPos));
			if ( m_fCurSelPos > m_fCtrlPointHalfSize &&
				 m_fCurSelPos < 1.0f - m_fCtrlPointHalfSize )
			{
				m_fCurSelPos = float(x)/m_clientSize.width;
			}
		}

		if ( m_fCurSelPos <= m_fCtrlPointHalfSize )
			m_fCurSelPos = 0.0f;
		if ( m_fCurSelPos >= 1.0f - m_fCtrlPointHalfSize )
			m_fCurSelPos = 1.0f;

		new_node.a = calcAlpha(y);
		pCtrlMove->Push(new ColorInsCmd(this,m_fCurSelPos,new_node));
	
		pCtrlMove->Execute();
		return true;
	}

	bool CColorAnimImp::OnCtrlLBtDown( int x, int y )
	{
		RECT rect;
		::GetClientRect((HWND)m_wndHandle, &rect);
	
		m_fCurSelPos = float(x)/rect.right;
		if ( m_fCurSelPos < m_fCtrlPointHalfSize*2 )
		{
			m_fCurSelPos = 0.0f;
			return true;
		}
		if ( m_fCurSelPos > 1.0f - m_fCtrlPointHalfSize*2 )
		{
			m_fCurSelPos = 1.0f;
			return true;
		}

	
		TrackNodeMap::iterator it, eit = m_ColorMap.end();
		for ( it = m_ColorMap.begin(); it != eit; ++it )
		{
			if ( fabs(m_fCurSelPos - it->first) < m_fCtrlPointHalfSize )
			{
				m_fCurSelPos = it->first;
				break;
			}
		}
		CCombinCmd* pCtrlMove = new CCombinCmd;
		pCtrlMove->Execute();		
		return true;
	}

	bool CColorAnimImp::OnCtrlKeyDown( MSG_KEY_INF key )
	{
		if ( key == MGI_KEY_Delete && m_fCurSelPos != 0.0f && m_fCurSelPos != 1.0f )
		{
			(new ColorDelCmd(this,m_fCurSelPos,true))->Execute();
		}
		return true;
	}

	ColorAnimNode CColorAnimImp::getColor( float pos )
	{
		ColorAnimNode new_node;
		TrackNodeMapIter it = m_ColorMap.end();
		new_node = (--it)->second;

		for ( it = m_ColorMap.begin(); it != m_ColorMap.end(); ++it )
		{
			TrackNodeMapIter pre;
			if ( it->first > pos )
			{
				pre = it;
				--pre;
				float w = (pos - pre->first) / (it->first - pre->first);
				for ( int i=0; i<4; ++i )
				{
					new_node.c[i] = pre->second.c[i]*(1.0f-w) + it->second.c[i]*w;
				}
				break;
			}
		}

		return new_node;
	}

	void CColorAnimImp::SetColorAtPos( const ColorAnimNode& color, int x, int y )
	{
		// x info had been processed by OnCtrlLBtDown()
		ColorAnimNode tColor = color;
		tColor.a = calcAlpha(y);
		(new ColorEditCmd(this,m_fCurSelPos,tColor))->Execute();
	}

	float CColorAnimImp::calcAlpha( int y )
	{
		return min(1.0f, max( 0.0f, 1.0f - float(y)/m_clientSize.height));
	}

	bool CColorAnimImp::OnCtrlSize( int width, int height )
	{
		RECT client_rect;
		::GetClientRect((HWND)m_wndHandle, &client_rect);
		m_clientSize.width  = client_rect.right;
		m_clientSize.height = client_rect.bottom;
		m_fCtrlPointHalfSize = CONTRL_POINT_HALF_SIZE/m_clientSize.width;
		return true;
	}

	void CColorAnimImp::SetEditCtrlData( void* ctrldata )
	{
		if (ctrldata)
		{
			m_pCtrlData = static_cast<ECtrlData*>(ctrldata);
			SetColorMap(m_pCtrlData->m_eDataType, m_pCtrlData->m_pData);
			CCtrlBase::Update();
		}
		else
			m_pCtrlData = NULL;
	}

	void CColorAnimImp::OnDataChanged( void )
	{
		if(m_bUpdateLock)
			return;

		if ( m_pTypeImp )
			m_pTypeImp->Update();
		CCtrlPropBase::OnDataChanged();
	}
}
