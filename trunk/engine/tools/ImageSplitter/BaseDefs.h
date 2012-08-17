#pragma once

// ---- 错误代码定义 --------------------------------------------------------------------------

#define ERR_BASE						1
#define	ERR_CREATE_WINDOW_FAILED		ERR_BASE + 1
#define	ERR_INIT_DIRECT3D9_FAILED		ERR_BASE + 2
#define ERR_PROCESS_FAILED				ERR_BASE + 3

// ---- 数据类型定义 --------------------------------------------------------------------------

	typedef unsigned __int8		uint8;
	typedef unsigned __int16	uint16;
	typedef unsigned __int32	uint32;
	typedef unsigned __int64	uint64;
	typedef __int8				int8;
	typedef __int16				int16;
	typedef __int32				int32;
	typedef __int64				int64;


// ---- 数据结构定义 --------------------------------------------------------------------------

	struct GraphicDeviceDesc
	{
		int		m_nX, m_nY; // position
		int		m_nWidth, m_nHeight; // size
		int		m_nMultiSamples;
		bool	m_bFullScreen; // fullscreen or windowed
		char	*m_szDevice;

		GraphicDeviceDesc(void)
			: m_nX(0)
			, m_nY(0)
			, m_nWidth(256)
			, m_nHeight(256)
			, m_nMultiSamples(0)
			, m_szDevice(0)
			, m_bFullScreen(false)
		{
		}
	};

