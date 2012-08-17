#include "stdafx.h"
#include "CCharacterVariantHandler.h"
#include "CCypherVariant.h"
#include "CCypherVariantCodeTable.h"
#include "CSyncVariantClient.h"
#include "BaseHelper.h"

void CCharacterVariantHandler::AddVariantAddedCallback(const string& strName, CallbackType* pCallback)
{
	return VariantAddedDlg[strName] += pCallback;
}

void CCharacterVariantHandler::AddVariantDeletedCallback(const string& strName, CallbackType* pCallback)
{
	return VariantDeletedDlg[strName] += pCallback;
}

void CCharacterVariantHandler::AddVariantChangedCallback(const string& strName, CallbackType* pCallback)
{
	return VariantChangedDlg[strName] += pCallback;
}

void CCharacterVariantHandler::AddTableClearCallback(const string& strName, CallbackType* pCallback)
{
	return TableClearDlg[strName] += pCallback;
}

void CCharacterVariantHandler::RemoveVariantAddedCallback(const string& strName, CallbackType* pCallback)
{
	return VariantAddedDlg[strName] -= pCallback;
}

void CCharacterVariantHandler::RemoveVariantDeletedCallback(const string& strName, CallbackType* pCallback)
{
	return VariantDeletedDlg[strName] -= pCallback;
}

void CCharacterVariantHandler::RemoveVariantChangedCallback(const string& strName, CallbackType* pCallback)
{
	return VariantChangedDlg[strName] -= pCallback;
}

void CCharacterVariantHandler::RemoveTableClearCallback(const string& strName, CallbackType* pCallback)
{
	return TableClearDlg[strName] -= pCallback;
}

void CCharacterVariantHandler::OnMemberAdded(CCypherVariant *pVariant)
{
//	cout << "DoOnMemberAdded " << pVariant->GetName() << endl;
	DlgMapItType it = VariantAddedDlg.find(pVariant->GetCode()->GetName());
	if (it == VariantAddedDlg.end()) return;
	(it->second)(pVariant);
}

void CCharacterVariantHandler::OnDeleteMember(CCypherVariant *pVariant)
{
//	cout << "DoOnVariantDeleted " << pVariant->GetName() << endl;
	DlgMapItType it = VariantDeletedDlg.find(pVariant->GetCode()->GetName());
	if (it == VariantDeletedDlg.end()) return;
	(it->second)(pVariant);
}

void CCharacterVariantHandler::OnValueChanged(CCypherVariant *pVariant)
{
//	cout << "DoOnVariantChanged " << pVariant->GetName() << endl;
	DlgMapItType it = VariantChangedDlg.find(pVariant->GetCode()->GetName());
	if (it == VariantChangedDlg.end()) return;
	(it->second)(pVariant);
}

void CCharacterVariantHandler::OnClearTable(CCypherVariant *pVariant)
{
//	cout << "DoOnTableClear " << pVariant->GetName() << endl;
	DlgMapItType it = TableClearDlg.find(pVariant->GetCode()->GetName());
	if (it == TableClearDlg.end()) return;
	(it->second)(pVariant);
}


CVariantCallback::CVariantCallback()
{
	AdvAddRef(this);
}

CVariantCallback::~CVariantCallback()
{
	AdvDelRef(this);
}
