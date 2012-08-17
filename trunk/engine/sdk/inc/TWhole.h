#pragma once

/**
	Part-Whole模式(合成模式).
*/

namespace sqr
{
template<typename ImpPartClass,typename ImpWholeClass>
class TPart;

template<typename ImpWholeClass,typename ImpPartClass>
class TWhole
{
private:
	typedef TPart<ImpPartClass,ImpWholeClass>	Part_t;
	typedef list<ImpPartClass*>					ImpParts_t;
	friend class TPart<ImpPartClass,ImpWholeClass>;

	ImpParts_t	m_ImpParts;

protected:
	void ClearParts();
	const ImpParts_t& GetParts()const;
	Part_t* GetFirstPart()const;
	bool HasPart()const;

public:
	TWhole(void);
	~TWhole(void);
};
}
