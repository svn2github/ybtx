#include "stdafx.h"
#include "LoadVariantCfgCommon.h"
#include "CGlobalObjVarDefCfg.h"

void LoadVariantCfgCommon(CObjVarDefCfg* pCfg, const std::string& path)
{
	CGlobalObjVarDefCfg::GetInst()->SetCfg(pCfg);
	string player = path + "PlayerVariant_Common.xml";
	CGlobalObjVarDefCfg::GetInst()->AddName2ID("Player", CGlobalObjVarDefCfg::GetInst()->Get(player.c_str()));

	string npc = path + "NpcVariant_Common.xml";
	CGlobalObjVarDefCfg::GetInst()->AddName2ID("Npc", CGlobalObjVarDefCfg::GetInst()->Get(npc.c_str()));

	string intobj = path + "IntObjVariant_Common.xml";
	CGlobalObjVarDefCfg::GetInst()->AddName2ID("IntObj", CGlobalObjVarDefCfg::GetInst()->Get(intobj.c_str()));

	string magic = path + "MagicVariant_Common.xml";
	CGlobalObjVarDefCfg::GetInst()->AddName2ID("Magic", CGlobalObjVarDefCfg::GetInst()->Get(magic.c_str()));
}
