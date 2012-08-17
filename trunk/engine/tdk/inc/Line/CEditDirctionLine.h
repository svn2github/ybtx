#pragma once
#include "CVector3.h"
#include "Line/CEditLine.h"

namespace sqr
{
	class CEditDirctionLine : public CEditLine
	{
	public:
		CEditDirctionLine();
		~CEditDirctionLine();

		void		Render() const;
	};
}