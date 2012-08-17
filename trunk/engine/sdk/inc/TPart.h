#pragma once

#include <list>
/**
	Part-Whole模式(合成模式).
*/

namespace sqr
{
template<typename ImpWholeClass,typename ImpPartClass>
class TWhole;

template<typename ImpPartClass,typename ImpWholeClass>
class TPart
{
private:
	typedef TWhole<ImpWholeClass,ImpPartClass>		Whole_t;
	typedef list<ImpPartClass*>						ImpParts_t;
	typedef typename ImpParts_t::iterator			ItWhole_t;
	Whole_t*		m_pWhole;
	ItWhole_t		m_itWhole;
	friend class TWhole<ImpWholeClass,ImpPartClass>;

	void Delete();
protected:
	Whole_t* GetWhole()const;
public:
	TPart(Whole_t* pWhole);
	~TPart(void);
};
}



