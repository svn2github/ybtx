#pragma once

#include <objbase.h>
#include <atlbase.h>
#include <atlwin.h>

namespace ShockwaveFlashObjects
{
	struct IShockwaveFlash;
};

using namespace ShockwaveFlashObjects;

enum RenderQuality
{
	RQ_LOW,
	RQ_MEDIUM,
	RQ_HIGH,
	RQ_BEST,
	RQ_AUTOLOW,
	RQ_AUTOHIGH
};

enum ScaleMode
{
	SM_SHOWALL,
	SM_NOBORDER,
	SM_EXACTFIT
};

class FlashSite;
class FlashHandler;

namespace sqr
{
	class ITexture;
	class CFlashPlayer
	{
		friend class FlashSite;
		friend class FlashHandler;

	public:
		CFlashPlayer(int w,int h);
		~CFlashPlayer(void);

		void UpdateTexture();
		bool Create();
		void ResizeRect(float w,float h);
		void InvalidateTotally();

		//Control
		void Play();
		void Stop();
		bool Load();

		void SetFlashFile(const wchar_t* filename);
		void SetTransparent(bool isTrans);
		void SetQuality(short renderQuality);
		void SetScaleMode(short scaleMode);
		//Get
		float GetTexWidth();
		float GetTexHeight();
		ITexture* GetTexture();

		//MSG PROCESS
		void ProcMouseMsg(UINT msg,WPARAM wParam,LPARAM lParam);
		void ProcKeyBoardMsg(UINT msg,WPARAM wParam,LPARAM lParam);
		//CallBack
		void HandleFlashCall(const std::wstring& xmlString);

	private:
		HWND		m_ParentHandle;
		int			m_Width;
		int			m_Height;
		wstring		m_File;
		
		ITexture*		m_pTexture;
		
		RECT			dirtyBounds;
		int				comCount;
		bool			m_autoPlay;
		bool			m_isCreated;
		bool			m_isTransparent;

		bool			isClean,isTotallyDirty;
		HANDLE			m_hThread;
		DWORD			m_threadID;
		bool			m_isLoad;

	protected:
		
		//site
		FlashSite*						m_site;
		FlashHandler*					m_handler;
		//object
		IOleObject*						m_oleObject;
		IOleInPlaceObject*				m_oleInPlaceObject;
		IShockwaveFlash*				m_shwaveflash;
		IOleInPlaceObjectWindowless*	m_msgDispatch;
};

}
