#pragma once

#include "TSqrAllocator.h"
#include "CNetworkMallocObject.h"

DeclarePoolAllocator(TNetworkAllocator, CNetworkMallocObject);

DeclarePoolAllocator(TNetworkRecvBufAllocator, CNetworkRecvBufferMallocObject);
DeclarePoolAllocator(TNetworkSendBufAllocator, CNetworkSendBufferMallocObject);


