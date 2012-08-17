#pragma once


template<typename FinalHolderType>
class TBasePropertyAdapter
{
public:
	virtual float Get(const FinalHolderType* pHolder)const=0;
	virtual void Set(FinalHolderType* pHolder,float fValue)=0;
	virtual ~TBasePropertyAdapter(){}
};

template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
class TValueAdapter
	:public TBasePropertyAdapter<FinalHolderType>
{
private:
	float Get(const FinalHolderType* pHolder)const;
	void Set(FinalHolderType* pHolder,float fValue);
public:
	static string GetName();
};


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
class TAdderAdapter
	:public TBasePropertyAdapter<FinalHolderType>
{
private:
	float Get(const FinalHolderType* pHolder)const;
	void Set(FinalHolderType* pHolder,float fValue);
public:
	static string GetName();
};


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
class TMultiplierAdapter
	:public TBasePropertyAdapter<FinalHolderType>
{
private:
	float Get(const FinalHolderType* pHolder)const;
	void Set(FinalHolderType* pHolder,float fValue);
public:
	static string GetName();
};


template<typename FinalHolderType,typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
class TAgileValueAdapter
	:public TBasePropertyAdapter<FinalHolderType>
{
private:
	float Get(const FinalHolderType* pHolder)const;
	void Set(FinalHolderType* pHolder,float fValue);
public:
	static string GetName();
};

