#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	struct ColorAnimNode
	{
		ColorAnimNode()
			: r(0), g(0), b(0), a(0)
		{
		}

		ColorAnimNode( float in_r, float in_g, float in_b, float in_a )
			: r(in_r), g(in_g), b(in_b), a(in_a)
		{
		}
		
		union
		{
			struct
			{
				float r, g, b, a; // Red, Green, and Blue color data
			};
			float c[4];
		};
	};

	struct TrackSize
	{
		TrackSize()
			: width(0), height(0)
		{
		}
		int	width;
		int height;
	};

	class CColorAnimTypeBase;
	class ColorDelCmd;
	class ColorInsCmd;
	class ColorEditCmd;
	class CColorAnimImp : public CCtrlPropBase
	{
		REG_UI_IMP(CColorAnimImp)
		friend class ColorDelCmd;
		friend class ColorInsCmd;
		friend class ColorEditCmd;
	public:
		typedef map<float, ColorAnimNode>	TrackNodeMap;
		typedef TrackNodeMap::iterator		TrackNodeMapIter;

	public:
		CColorAnimImp(void* param);
		~CColorAnimImp();

		virtual void			
				SetEditCtrlData(void* ctrldata);

		virtual void	
				SetColorMap( ECtrlDataType type, void* inMap );

		virtual int		
				GetKeyNum();
		
		virtual bool	
				GetNode( ColorAnimNode& node, float& key, int idx );

		virtual bool
				GetSelPointIdx( int& cur_idx );

		virtual bool 
				OnCtrlMouseMove( int x, int y );

		virtual bool 
				OnCtrlLBtDown( int x, int y );

		virtual bool 
				OnCtrlKeyDown(MSG_KEY_INF key);

		virtual bool 
				OnCtrlSize( int width,int height );

		virtual void
				SetColorAtPos( const ColorAnimNode& color, int x, int y );

	private:
		ColorAnimNode	
				getColor( float pos );

		float	calcAlpha( int y );

		virtual void	OnDataChanged(void);

	private:
		void*					m_wndHandle;
		float					m_fCurSelPos;
		float					m_fCtrlPointHalfSize;
		TrackNodeMap			m_ColorMap;
		CColorAnimTypeBase*		m_pTypeImp;
		TrackSize				m_clientSize;
		bool					m_bUpdateLock;
	};
}
