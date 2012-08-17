#pragma once
#include "CRefObject.h"
#include "ExsertStructDB.h"


namespace sqr
{
	class CEditModelGroup;

	SmartPointer(CTObject);
	class CTObject : public CRefObject
	{
	public:
		CEditModelGroup		*m_pModelGroup;
		int			m_nCenterGridIndex;		// the grid the  object belongs to...
		int			m_nResSetIndex;
		int			m_nResCueIndex;//audio index
		string		m_strClassName;
		bool		m_bShadow;
		EAUTO_GENERATION_TYPE m_eAutoGenerationType;

	public:

		CTObject();
		~CTObject();

		void SetResSetIndex(int n);
		int	 GetResSetIndex();

		void SetResCueIndex(int n);
		int	 GetResCueIndex();

		void SetCenterGridIndex(int n);
		int  GetCenterGridIndex() const;

		void SetCastShadow(bool b);

		void SwitchShadow();

		bool IsCastShadow();

		void Render(/*bool bShadow*/);

		int  GetCenterGridIndex()
		{
			return m_nCenterGridIndex;
		}

		int  GetCenterGridIndexCurrent();

		void SetModelGroup(CEditModelGroup * p);
		CEditModelGroup *GetModelGroup();

		string & GetClassName()
		{
			return m_strClassName;
		}
		void SetClassName(const string & strClassName);

		sqr::EAUTO_GENERATION_TYPE GetAutoGenerationType();
		void SetAutoGenerationType(sqr::EAUTO_GENERATION_TYPE e);
	};
}
