#include "stdafx.h"
#include "CRefCoder.h"

namespace sqr
{
	BEGIN_VERSION_MAP(CRefCoder)
		ON_VERSION(1, Decode1)
	END_VERSION_MAP()

	bool CRefCoder::Decode1( const CDataChunk& inData, OBJ_HANDLE handle )
	{
		string refName;
		CBufFile bufFile(inData.GetBuffer(), inData.GetSize());
		int32 size = bufFile.GetBufSize();
		vector<char> temp( size + 1, 0);
		bufFile.read(&temp[0], size);
		refName = &temp[0];

		return _DecodeImpl(refName, handle);
	}

	bool CRefCoder::_EncodeImpl( WriteDataInf& wdi, PRIVATE_HANDLE handle )
	{
		string* refName = static_cast<string*>(handle);
		wdi.Write((const char*)refName->c_str(), refName->size());

		return true;
	}
}