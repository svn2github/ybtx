#pragma once
#include "FighterProperty.h"
#include "FightDef.h"

#define DefineAgileProperty(Name) \
class C##Name:public TAgileProperty<C##Name,ePID_##Name,TBaseIntegerAMBProperty,&CStaticAttribs::m_Max##Name>{}

DefineAgileProperty(HealthPoint);
DefineAgileProperty(ManaPoint);
DefineAgileProperty(ComboPoint);
DefineAgileProperty(RagePoint);
DefineAgileProperty(EnergyPoint);

#undef DefineAgileProperty