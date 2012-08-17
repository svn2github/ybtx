#pragma once
#include "CTick.h"
#include "AutoDataHelper.h" 

namespace sqr_tools
{
	class CEditModel;
	class CEditContext;
	class CBaseUnit;
	
	class CTestModelUnit
	{
	public:
		CTestModelUnit(CEditModel* pModel = NULL):m_pModel(pModel){}

		CEditModel* operator->(){ return m_pModel; }
		operator CEditModel* () { return m_pModel; }

		bool	IsValid(void)	{ return m_pModel!=NULL; }
		CEditModel*	m_pModel;
		string		m_strPieceName;
	};

#define DEF_TEST_UNIT(class_name) \
public: \
	virtual const string&		GetName(void); \
	static CBaseUnit*			CreateNewUnit(CEditContext* pContext); \
	static const string			st_strName; \

#define IMP_TEST_UNIT(class_name,type_name) \
	const string	class_name::st_strName = type_name; \
	const string&	class_name::GetName(void) { return class_name::st_strName; } \
	CBaseUnit*		class_name::CreateNewUnit(CEditContext* pContext){ return new class_name(pContext); } \

	typedef vector<CTestModelUnit> ModelPool;
	typedef CBaseUnit* (*CreateNewUnit)(CEditContext* pContext);

#define DEFAULT_CYC 0xFFFFFFFF
	class CBaseUnit 
		: public AutoDataHelper
		, public CTick
	{
	public:
		CBaseUnit(CEditContext* pContext);
		virtual ~CBaseUnit();

	public:
		ModelPool*	GetModelPool(void);
		void		SetModelPool(ModelPool* pPool);
		void		SetTick(uint32 Tick = DEFAULT_CYC);
		void		OnTick(void);
		virtual		const string&		GetName(void) = 0;
	protected:
		virtual void	_OnTick();
		CEditContext*	m_pContext;
		uint32			m_pCyc;
		ModelPool*		m_pModelPool;

	public:
		static	size_t			InitUnit(void);
		static	const char*		GetUnitName(unsigned int idx);
		static	CBaseUnit*		CreateUnit(const char* uName,CEditContext* pContext);
		static	void			DestroyUnit(CBaseUnit* pUnit);

	protected:
		typedef map<string, CreateNewUnit>	UnitCreateMap;
		typedef vector<string>	NamePool;	
		static	NamePool		st_NamePool;
		static	UnitCreateMap	st_CreateMap;
	};
}