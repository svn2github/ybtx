#include "stdafx.h"
#include "BaseHelper.h"
#include "CGraphic.h"
#include "CWinPkgFile.h"
#include "CBaseMovieImp.h"
#include "CDShowMovieImp.h"
#include "CMovie.h"

CMovie::CMovie()
:m_pMovieImp(NULL)
{
	m_pMovieImp = new CDShowMovieImp();
}

CMovie::~CMovie()
{
	SafeDelete(m_pMovieImp);
}

void CMovie::Load(const char* name)
{
	switch(JudyType(name))
	{
	case MEM_TYPE:
		m_pMovieImp->LoadMemMovie(name);
		break;
	case URL_TYPE:
		m_pMovieImp->LoadUrlMovie(name);
		break;
	}
}

void CMovie::Play()
{
	m_pMovieImp->Play();
}

void CMovie::Stop()
{
	m_pMovieImp->Stop();
}

void CMovie::Pause()
{
	m_pMovieImp->Pause();
}

void CMovie::Replay()
{
	m_pMovieImp->Rewind();
}

float CMovie::GetMovieWidth()
{
	return m_pMovieImp->GetMovieWidth();
}

float CMovie::GetMovieHeight()
{
	return m_pMovieImp->GetMovieHeight();
}

void CMovie::UpdateMovieTexture()
{
	m_pMovieImp->UpdateMovieTexture();
}

ITexture *CMovie::GetMovieTexture()
{
	return m_pMovieImp->GetMovieTexture();
}

bool CMovie::IsPlayingMovie()
{
	return m_pMovieImp->IsPlayingMovie();
}

bool CMovie::IsPlayCompleted( void )
{
	return m_pMovieImp->IsPlayCompleted();
}

MovieType CMovie::JudyType(const char* name)
{
	std::string _name = name;
	if (-1 != _name.find("://"))
	{
		return URL_TYPE;
	}
	else
	{
		return MEM_TYPE;
	}
}


float CMovie::GetProgress( void )
{
	return m_pMovieImp->GetProgress();
}