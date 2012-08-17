#include "stdafx.h"
#include "CMultiLangTxtTableFile.h"
#include "CMultiLangTxtTableFileImp.h"
#include "CCurLangTxtTableFileImp.h"
#include "ExpHelper.h"
#include "TBaseTableFile.inl"

template class TBaseTableFile<CMultiLangTxtTableFileImp>;
template class TBaseTableFile<CCurLangTxtTableFileImp>;

