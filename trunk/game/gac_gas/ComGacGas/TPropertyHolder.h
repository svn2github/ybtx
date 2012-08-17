#pragma once

#include "PropertyAdapter.h"
#include "FightDef.h"
#include "CDynamicObject.h"


template<typename ImpClass>
class TPropertyHolder
	: public virtual CDynamicObject 
{
private:
	typedef TBasePropertyAdapter<ImpClass>	Adapter_t;

public:
	template<typename PropertyHolder,typename PropertyType>
	typename PropertyType::Value_t Get(PropertyType PropertyHolder::*Property)const;


	template<typename PropertyHolder,typename PropertyType>
	typename PropertyType::Value_t GetAdder(PropertyType PropertyHolder::*Property)const;

	template<typename PropertyHolder,typename PropertyType>
	void SetAdder(PropertyType PropertyHolder::*Property,typename PropertyType::Value_t Value);

	template<typename PropertyHolder,typename PropertyType>
	void FullFill(PropertyType PropertyHolder::*Property);

//protected:
	bool CppSetCtrlState(EFighterCtrlState eState,bool bSet);

public:
	template<typename PropertyHolder,typename PropertyType>
	typename PropertyType::Value_t GetMultiplier(PropertyType PropertyHolder::*Property)const;

	template<typename PropertyHolder,typename PropertyType>
	void SetMultiplier(PropertyType PropertyHolder::*Property,typename PropertyType::Value_t Value);

	static void ReserveVectors();
	static void PrepareAgileProperties();

	static void UnloadAdapterMap();

	static const TCHAR* GetPropertyName(uint32 uIndex);
	static uint32 GetPropertyIndex(const TCHAR* szName);

	//所有的属性全部用浮点表示
	float CppGetPropertyValue(const TCHAR* szName)const;
	float CppGetPropertyValue(uint32 uIndex)const;

	void CppSetPropertyValue(const TCHAR* szName,float fValue);	// 需要根据不同的PropertyValueType将float型进行转换
	void CppSetPropertyValue(uint32 uIndex,float fValue);

	void CppChangePropertyValue(const TCHAR* szName,float fDiff);
	void CppChangePropertyValue(uint32 uIndex,float fDiff);

	void CppIncreasePropertyValue(const TCHAR* szName,float fDiff);
	void CppIncreasePropertyValue(uint32 uIndex,float fDiff);

	void CppDecreasePropertyValue(const TCHAR* szName,float fDiff);
	void CppDecreasePropertyValue(uint32 uIndex,float fDiff);


	/////////////////////////////////属性更改回调, 实际上是一个空函数/////////////////////////////////////////
	void OnAgileValueChanged(uint32 uValueTypeID, float fValue)const;
	void OnCalValueChanged(uint32 uValueTypeID, float fValue)const;
	void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const;


public:
	TPropertyHolder();
	~TPropertyHolder();

	template<typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
	class TImpValueAdapter:public TValueAdapter<ImpClass,HolderType,PropertyType,pProperty>{};

	template<typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
	class TImpAdderAdapter:public TAdderAdapter<ImpClass,HolderType,PropertyType,pProperty>{};

	template<typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
	class TImpMultiplierAdapter:public TMultiplierAdapter<ImpClass,HolderType,PropertyType,pProperty>{};

	template<typename HolderType,typename PropertyType,PropertyType HolderType::*pProperty>
	class TImpAgileValueAdapter:public TAgileValueAdapter<ImpClass,HolderType,PropertyType,pProperty>{};

	template<typename AdapterType>
	static void CreateAdapter(map<string,Adapter_t*>&, vector<Adapter_t*>&, vector<string>&, uint32);

	static map<string,uint32>&		GetMapName2Index();
	static vector<string>&			GetVecName();
	static vector<Adapter_t*>&		GetVecAdapter();
	
	static map<string,Adapter_t*>& GetMapAdapter();
	static Adapter_t* GetAdapter(uint32 uIndex);

private:
	static map<string, Adapter_t*>	ms_mapAdapter;
};
