#pragma once 
#include "CDataFileMallocObject.h"
#include "CDataFileAllocator.h"
#include "TableFileHelper.h"

namespace sqr
{
	class CVariant;

	template<typename ImpType>
	class TBaseTableFile : public CDataFileMallocObject
	{
		friend class CMultiLangTxtTableFile;
	public:
		TBaseTableFile();
		
		~TBaseTableFile();

		bool Load( const char* alias, const char* szFileName );
		bool Load( const wchar_t* alias, const char* szFileName );

		bool Load( const char* alias, vector<CVariant*>* pArgList );
		bool Load( const wchar_t* alias, vector<CVariant*>* pArgList );

		bool Reload( vector<CVariant*>* pArgList );
		bool Reload( const char* szFileName );
	
	private:
		typedef vector<const char*, CDataFileAllocator<const char*> > VecFileName_t;
		bool		IntLoad( const wchar_t* alias, VecFileName_t& vecFileName );

	protected:
		ImpType*		m_pImp;
	};

}
