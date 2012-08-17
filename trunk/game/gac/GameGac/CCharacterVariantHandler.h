#pragma once
#include "ICypherVariantHolderHandler.h"
#include "PatternDlg.h"

namespace sqr
{
	class CSyncVariantClient;
}

class CCharacterVariantHandler : public ICypherVariantHolderHandler
{

public:
	typedef TPtManagedDlg1<CCypherVariant*> DlgType;
	typedef TPtManagedDlg1<CCypherVariant*>::CallbackType CallbackType;

	void AddVariantAddedCallback(const string& strName, CallbackType* pCallback);
	void AddVariantChangedCallback(const string& strName, CallbackType* pCallback);
	void AddVariantDeletedCallback(const string& strName, CallbackType* pCallback);
	void AddTableClearCallback(const string& strName, CallbackType* pCallback);
	void RemoveVariantAddedCallback(const string& strName, CallbackType* pCallback);
	void RemoveVariantChangedCallback(const string& strName, CallbackType* pCallback);
	void RemoveVariantDeletedCallback(const string& strName, CallbackType* pCallback);
	void RemoveTableClearCallback(const string& strName, CallbackType* pCallback);

public:
	typedef std::map<string, typename DlgType> DlgMapType;
	typedef DlgMapType::iterator DlgMapItType;
	DlgMapType VariantAddedDlg;
	DlgMapType VariantDeletedDlg;
	DlgMapType VariantChangedDlg;
	DlgMapType TableClearDlg;
private:
	void OnMemberAdded(CCypherVariant *pVariant);
	void OnDeleteMember(CCypherVariant *pVariant);
	void OnValueChanged(CCypherVariant *pVariant);
	void OnClearTable(CCypherVariant *pVariant);

};

class CVariantCallback : public CCharacterVariantHandler::CallbackType
{
public:
	CVariantCallback();
	~CVariantCallback();
	virtual int exec(CCypherVariant *pVariant) { return 0;}
};
