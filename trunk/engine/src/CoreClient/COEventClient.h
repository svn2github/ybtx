#pragma once

#include "TCOEventMoveBegan.h"
#include "TCOEventMoveEnded.h"
#include "TCOEventMovePathChanged.h"
#include "TCOEventPosChanged.h"
#include "CTraitsClient.h"

typedef TCOEventMoveBegan<CTraitsClient> CCOEventMoveBeganClient;
typedef TCOEventMoveEnded<CTraitsClient> CCOEventMoveEndedClient;
typedef TCOEventMovePathChanged<CTraitsClient> CCOEventMovePathChangedClient;
typedef TCOEventPosChanged<CTraitsClient> CCOEventPosChangedClient;