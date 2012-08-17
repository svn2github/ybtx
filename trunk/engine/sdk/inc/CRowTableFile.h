#pragma once

#include "CDynamicObject.h"
#include "CommonDefs.h"
#include "TableFileHelper.h"
#include "TBaseTableFile.h"

namespace sqr
{
	class CTxtTableFile;
	class CRowTableFileImp;

	
	class CRowTableFile;
	class CRowTableFileNode;
	class CVariant;

	class COMMON_API CRowTableFile : virtual public CDynamicObject
								   , public TBaseTableFile<CRowTableFileImp>
	{
	public:
		typedef TBaseTableFile<CRowTableFileImp> Parent_t;
		CRowTableFile();
		~CRowTableFile();

		void Release();

		CRowTableFileNode* GetRowTableNode();
	};
};
