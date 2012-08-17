#include <stdafx.h>
#include "COEventClient.h"
#include "CCoreObjectClient.h"
#include "ICoreObjectClientHandler.h"
#include "TCOEventMoveBegan.inl"
#include "TCOEventMoveEnded.inl"
#include "TCOEventMovePathChanged.inl"
#include "TCOEventPosChanged.inl"
#include "TBaseCOEvent.inl"
#include "PatternRef.inl"
#include "TSqrAllocator.inl"

template class TBaseCOEvent<CTraitsClient>;
template class TCOEventMoveBegan<CTraitsClient>;
template class TCOEventMoveEnded<CTraitsClient>;
template class TCOEventMovePathChanged<CTraitsClient>;
template class TCOEventPosChanged<CTraitsClient>;
template class TPtRefee< CCoreObjectClient, TBaseCOEvent<CTraitsClient> >;