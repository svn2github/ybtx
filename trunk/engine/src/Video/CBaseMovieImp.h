#pragma once

namespace sqr
{
	class ITexture;
	class  CBaseMovieImp
	{
	public:
		CBaseMovieImp();
		virtual ~CBaseMovieImp();

		virtual HRESULT LoadMemMovie(const char* name) = 0;

		virtual HRESULT LoadUrlMovie(const char* name) = 0;

		virtual void Pause() = 0;

		virtual void Play() = 0;

		virtual void Rewind() = 0;

		virtual void Stop() = 0;

		virtual bool IsPlayingMovie() = 0;

		virtual bool IsPlayCompleted(void) = 0;

		virtual float GetMovieWidth(void) = 0;

		virtual float GetMovieHeight(void) = 0;

		virtual ITexture *GetMovieTexture();

		virtual void UpdateMovieTexture() = 0;

		virtual float GetProgress(void) = 0;
		// ÌùÍ¼
		ITexture* m_pTexture;
		// Ñ­»·²¥·Å
	};
}