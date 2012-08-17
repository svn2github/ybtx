#pragma once

#include "SQRWnd.h"

class CWndGroup;

enum eEditCommand
{
	eNewWnd,
	eDeleteWnd,
	eEditProp,
};

class CEditCommand
{
	struct CommandList
	{
		SQRWnd* pEditWnd;
		eEditCommand eCommand;
		string NewValue;
		string OldValue;
	};

	CWndGroup*	  m_pWndGroup;
	list<CommandList> m_CommandList;
	list<CommandList>::iterator m_it;
public:
	CEditCommand(void);
	~CEditCommand(void);
	void Init( CWndGroup* pWndGroup ){ m_pWndGroup = pWndGroup; }

	void WriteCurEditToString( SQRWnd* pWnd, string& str );
	void ReadFromString( SQRWnd* pWnd, const string& str );

	void PushCommand( SQRWnd* pEditWnd, eEditCommand eCommand, string& NewValue, string& OldValue )
	{
		m_CommandList.erase( m_it, m_CommandList.end() );
		m_it = m_CommandList.insert( m_CommandList.end(), CommandList() );
		CommandList& Cur = (*m_it);
		Cur.pEditWnd = pEditWnd;
		Cur.eCommand = eCommand;
		Cur.NewValue = NewValue;
		Cur.OldValue = OldValue;
		m_it = m_CommandList.end();

		if( m_CommandList.size() > 200 )
			m_CommandList.erase( m_CommandList.begin() );
	}

	int Size()
	{
		return (int)m_CommandList.size();
	}

	void Redo();
	void UnDo();
};
