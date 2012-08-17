#pragma once

namespace sqr_tools
{
	class CEfxInfoReader
	{
	public:
		CEfxInfoReader(void);
		~CEfxInfoReader(void);

		uint32	GetCount(void);
		string	GetEfxName(uint32 idx);
		void	Load(const string& filename);
	protected:
		typedef vector<string> EfxNamePool;
		EfxNamePool	m_efxPool;
	};
}