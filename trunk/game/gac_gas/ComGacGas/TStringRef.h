#pragma once
#include <string>
#include <map>
#include <list>

using namespace std;

/*
	TStringRefer和TStringRefee.具备以string作为索引查找refee,并在refee对象有修改时通知相应refer通过string作为索引更新其refee指针的功能.
	@refer删除时需要通知refee更改其refer列表
	@refee删除或改变时需要通知refer更新refee

	TStringRefee对象应该是配置对象的shared_ptr,因为TStringRefer和TStringRefee用于表间的相互引用,需要计数功能
*/

class IBaseStringRefer
{
public:
	IBaseStringRefer(){}
	virtual ~IBaseStringRefer(){}

	virtual void OnRefeeChanged()const=0;
};

template<typename ImpClass, typename ImpClassSharedPtr>
class TStringRefee
{
public:
	typedef ImpClassSharedPtr					SharedPtrType;
	typedef list<IBaseStringRefer*>				ListStringRefer;
	typedef map<string, ImpClassSharedPtr*>		MapImpClass;

	static MapImpClass& GetImpClassMap();

	const string& GetName()const;
	ListStringRefer::iterator Attach(IBaseStringRefer* pRefer);
	void Detach(ListStringRefer::iterator& it);

	void NotifyRefer()const;

protected:
	TStringRefee();
	~TStringRefee();

private:
	ListStringRefer	m_listRefer;

};

template<typename HolderType, typename RefeeType>
class TStringRefer
	: public IBaseStringRefer
	, public CConfigMallocObject
{
	typedef typename RefeeType::ListStringRefer ListRefer;
	typedef typename RefeeType::SharedPtrType	RefeeSharedPtrType;
public:
	TStringRefer(HolderType* pHolder);
	TStringRefer(HolderType* pHolder, const string& strKey);
	~TStringRefer();
	
	virtual void OnRefeeChanged()const;
	void SetStringKey(const string& strKey);
	void GetStringKey(string& strKey)const;

	const RefeeSharedPtrType& Get()const;
private:
	void ResetRefee();	//清空Refee指针

	HolderType*						m_pHolder;
	RefeeSharedPtrType				m_spRefee;	//不能直接保存Refee的指针,需要保存refee的shared_ptr
	typename ListRefer::iterator	m_itReferList;

};
