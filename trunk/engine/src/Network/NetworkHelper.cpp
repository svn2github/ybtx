#include "stdafx.h"

#include "NetworkHelper.h"
#include "Random.h"
#ifdef _WIN32
#include <WinSock.h>
#else
#include <netdb.h>
#endif

namespace sqr{

char* RandGetHostByName(const char* hostname)
{
	struct hostent* host;
	char** pp;
	struct in_addr addr;
	host = gethostbyname(hostname);

	if (!host||(pp = host->h_addr_list) == NULL)
		return NULL;

	pp = host->h_addr_list;
	addr.s_addr = *((unsigned int *)*pp);
	return inet_ntoa(addr);
}
}
