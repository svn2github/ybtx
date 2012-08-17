#pragma once

namespace sqr
{
	enum MovieType
	{
		MEM_TYPE = 0,
		URL_TYPE
	};

	class CBaseMovieImp;
	class CMovie
	{
	public:
		CMovie();
		~CMovie();

		virtual void Load(const char* name);

		virtual void Play(void);

		virtual void Stop(void);

		virtual void Pause(void);

		virtual void Replay(void);

		virtual float GetMovieWidth(void);

		virtual float GetMovieHeight(void);

		virtual void UpdateMovieTexture(void);

		virtual ITexture* GetMovieTexture(void);

        virtual bool IsPlayingMovie();

		virtual bool IsPlayCompleted(void);

		virtual float GetProgress(void);

	private:
		MovieType JudyType(const char* name);

	protected:
		CBaseMovieImp* m_pMovieImp;
	};
}