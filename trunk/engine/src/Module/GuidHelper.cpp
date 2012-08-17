#include "stdafx.h"
#include "GuidHelper.h"
#include "ExpHelper.h"
#include "CompileTimeHelper.h"

#ifdef _WIN32

#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")

#else
#include <uuid/uuid.h>
#endif


namespace sqr
{
	void CreateGuid(guid_t guid)
	{
#ifdef _WIN32
		UUID uuid;
		UuidCreate(&uuid);
		
		memcpy(guid, &uuid, sizeof(UUID));
#else
		uuid_generate(guid);
#endif
	}

	void CreateStringGuid(str_guid_t sz_guid)
	{
		guid_t guid;
		CreateGuid(guid);
		GuidToString(guid, sz_guid);
	}
	


	void GuidToString(const guid_t guid, str_guid_t sz_guid)
	{
#ifdef _WIN32
		unsigned char* szGuid;
		UUID uuid;
		memcpy(&uuid, guid, sizeof(UUID));

		UuidToString(&uuid, &szGuid);

		strcpy(sz_guid, (char*)szGuid);

		RpcStringFree(&szGuid);
#else
		uuid_unparse(guid, sz_guid);
#endif
	}


	void StringToGuid(const str_guid_t sz_guid, guid_t guid)
	{
#ifdef _WIN32
		UUID uuid;
	
		UuidFromString((unsigned char*)sz_guid, &uuid);

		memcpy(guid, &uuid, sizeof(UUID));
#else
		uuid_parse((char*)sz_guid, guid);
#endif

	}

}

