#pragma once
#include "GameDef.h"

class CCfgCalc;
class CPropertyCfg
{
	friend class CStaticAttribMgr;

public:
	~CPropertyCfg();

	typedef map<string, EClass>		MapClassType;
	static MapClassType				ms_mapClassType;

	static bool				LoadConfig(const TCHAR* szCfgFile);		// �����ñ�
	static void				UnloadConfig();							// ж�����ñ�
	static bool				IsClassInTable(EClass eClass);
	static CPropertyCfg*	GetPropertyCfg(EClass eClass,uint32 uLevel);
	static const string&	GetClassNameString(EClass eClass);

	inline static bool		InitMapClassType()		
	{
		ms_mapClassType["����ְҵ"]		=	eCL_AllClass;
		ms_mapClassType["��ְҵ"]		=	eCL_NoneClass;
		ms_mapClassType["��ʿ"]		=	eCL_Warrior;
		ms_mapClassType["ħ��ʿ"]		=	eCL_MagicWarrior;
		ms_mapClassType["������ʿ"]		=	eCL_Paladin;
		ms_mapClassType["��ʦ"]			=	eCL_NatureElf;
		ms_mapClassType["аħ"]			=	eCL_EvilElf;
		ms_mapClassType["��ʦ"]			=	eCL_Priest;
		ms_mapClassType["���鹭����"]	=   eCL_ElfHunter;
		ms_mapClassType["������ʿ"]		=   eCL_DwarfPaladin;
		ms_mapClassType["����սʿ"]		=   eCL_OrcWarrior;
		ms_mapClassType["NPC"]			=   eCL_Npc;
		ms_mapClassType["��ʦ"]			=   eCL_Wizard;
		ms_mapClassType["а��ʦ"]		=   eCL_Warlock;
		ms_mapClassType["�̿�"]			=   eCL_Assassin;
		ms_mapClassType["Ұ��"]			=   eCL_Beast;
		ms_mapClassType["ս��"]			=   eCL_WarPriest;
		ms_mapClassType["������"]		=   eCL_Archer;
		ms_mapClassType["��սʿ"]		=   eCL_Soldier;
		ms_mapClassType["�ٻ�ʦ"]		=   eCL_Summoner;
		ms_mapClassType["�ٻ�����"]		=   eCL_Summon;
		ms_mapClassType["������ʿ"]		=   eCL_Lancer;
		ms_mapClassType["������ʿ"]		=   eCL_Knight;
		ms_mapClassType["��ȻԪ��"]		=   eCL_NatureElement;
		ms_mapClassType["����Ԫ��"]		=   eCL_EvilElement;
		ms_mapClassType["�ƻ�Ԫ��"]		=   eCL_DestructionElement;
		ms_mapClassType["ְҵ001"]		=	eCL_001;
		ms_mapClassType["ְҵ002"]		=	eCL_002;
		ms_mapClassType["ְҵ003"]		=	eCL_003;
		ms_mapClassType["ְҵ004"]		=	eCL_004;
		ms_mapClassType["ְҵ005"]		=	eCL_005;
		ms_mapClassType["ְҵ006"]		=	eCL_006;
		ms_mapClassType["ְҵ007"]		=	eCL_007;
		ms_mapClassType["ְҵ008"]		=	eCL_008;
		ms_mapClassType["ְҵ009"]		=	eCL_009;
		ms_mapClassType["ְҵ010"]		=	eCL_010;
		ms_mapClassType["ְҵ011"]		=	eCL_011;
		ms_mapClassType["ְҵ012"]		=	eCL_012;
		ms_mapClassType["ְҵ013"]		=	eCL_013;
		ms_mapClassType["ְҵ014"]		=	eCL_014;
		ms_mapClassType["ְҵ015"]		=	eCL_015;
		ms_mapClassType["ְҵ016"]		=	eCL_016;
		ms_mapClassType["ְҵ017"]		=	eCL_017;
		ms_mapClassType["ְҵ018"]		=	eCL_018;
		ms_mapClassType["ְҵ019"]		=	eCL_019;
		ms_mapClassType["ְҵ020"]		=	eCL_020;
		ms_mapClassType["ְҵ021"]		=	eCL_021;
		ms_mapClassType["ְҵ022"]		=	eCL_022;
		ms_mapClassType["ְҵ023"]		=	eCL_023;
		ms_mapClassType["ְҵ024"]		=	eCL_024;
		ms_mapClassType["ְҵ025"]		=	eCL_025;
		ms_mapClassType["ְҵ026"]		=	eCL_026;
		ms_mapClassType["ְҵ027"]		=	eCL_027;
		ms_mapClassType["ְҵ028"]		=	eCL_028;
		ms_mapClassType["ְҵ029"]		=	eCL_029;
		ms_mapClassType["ְҵ030"]		=	eCL_030;
		ms_mapClassType["ְҵ031"]		=	eCL_031;
		ms_mapClassType["ְҵ032"]		=	eCL_032;
		ms_mapClassType["ְҵ033"]		=	eCL_033;
		ms_mapClassType["ְҵ034"]		=	eCL_034;
		ms_mapClassType["ְҵ035"]		=	eCL_035;
		ms_mapClassType["ְҵ036"]		=	eCL_036;
		ms_mapClassType["ְҵ037"]		=	eCL_037;
		ms_mapClassType["ְҵ038"]		=	eCL_038;
		ms_mapClassType["ְҵ039"]		=	eCL_039;
		ms_mapClassType["ְҵ040"]		=	eCL_040;
		ms_mapClassType["ְҵ041"]		=	eCL_041;
		ms_mapClassType["ְҵ042"]		=	eCL_042;
		ms_mapClassType["ְҵ043"]		=	eCL_043;
		ms_mapClassType["ְҵ044"]		=	eCL_044;
		ms_mapClassType["ְҵ045"]		=	eCL_045;
		ms_mapClassType["ְҵ046"]		=	eCL_046;
		ms_mapClassType["ְҵ047"]		=	eCL_047;
		ms_mapClassType["ְҵ048"]		=	eCL_048;
		ms_mapClassType["ְҵ049"]		=	eCL_049;
		ms_mapClassType["ְҵ050"]		=	eCL_050;
		ms_mapClassType["ְҵ051"]		=	eCL_051;
		ms_mapClassType["ְҵ052"]		=	eCL_052;
		ms_mapClassType["ְҵ053"]		=	eCL_053;
		ms_mapClassType["ְҵ054"]		=	eCL_054;
		ms_mapClassType["ְҵ055"]		=	eCL_055;
		ms_mapClassType["ְҵ056"]		=	eCL_056;
		ms_mapClassType["ְҵ057"]		=	eCL_057;
		ms_mapClassType["ְҵ058"]		=	eCL_058;
		ms_mapClassType["ְҵ059"]		=	eCL_059;
		ms_mapClassType["ְҵ060"]		=	eCL_060;
		ms_mapClassType["ְҵ061"]		=	eCL_061;
		ms_mapClassType["ְҵ062"]		=	eCL_062;
		ms_mapClassType["ְҵ063"]		=	eCL_063;
		ms_mapClassType["ְҵ064"]		=	eCL_064;
		ms_mapClassType["ְҵ065"]		=	eCL_065;
		ms_mapClassType["ְҵ066"]		=	eCL_066;
		ms_mapClassType["ְҵ067"]		=	eCL_067;
		ms_mapClassType["ְҵ068"]		=	eCL_068;
		ms_mapClassType["ְҵ069"]		=	eCL_069;
		ms_mapClassType["ְҵ070"]		=	eCL_070;
		ms_mapClassType["ְҵ071"]		=	eCL_071;
		ms_mapClassType["ְҵ072"]		=	eCL_072;
		ms_mapClassType["ְҵ073"]		=	eCL_073;
		ms_mapClassType["ְҵ074"]		=	eCL_074;
		ms_mapClassType["ְҵ075"]		=	eCL_075;
		ms_mapClassType["ְҵ076"]		=	eCL_076;
		ms_mapClassType["ְҵ077"]		=	eCL_077;
		ms_mapClassType["ְҵ078"]		=	eCL_078;
		ms_mapClassType["ְҵ079"]		=	eCL_079;
		ms_mapClassType["ְҵ080"]		=	eCL_080;
		ms_mapClassType["ְҵ081"]		=	eCL_081;
		ms_mapClassType["ְҵ082"]		=	eCL_082;
		ms_mapClassType["ְҵ083"]		=	eCL_083;
		ms_mapClassType["ְҵ084"]		=	eCL_084;
		ms_mapClassType["ְҵ085"]		=	eCL_085;
		ms_mapClassType["ְҵ086"]		=	eCL_086;
		ms_mapClassType["ְҵ087"]		=	eCL_087;
		ms_mapClassType["ְҵ088"]		=	eCL_088;
		ms_mapClassType["ְҵ089"]		=	eCL_089;
		ms_mapClassType["ְҵ090"]		=	eCL_090;
		ms_mapClassType["ְҵ091"]		=	eCL_091;
		ms_mapClassType["ְҵ092"]		=	eCL_092;
		ms_mapClassType["ְҵ093"]		=	eCL_093;
		ms_mapClassType["ְҵ094"]		=	eCL_094;
		ms_mapClassType["ְҵ095"]		=	eCL_095;
		ms_mapClassType["ְҵ096"]		=	eCL_096;
		ms_mapClassType["ְҵ097"]		=	eCL_097;
		ms_mapClassType["ְҵ098"]		=	eCL_098;
		ms_mapClassType["ְҵ099"]		=	eCL_099;
		ms_mapClassType["ְҵ100"]		=	eCL_100;
		ms_mapClassType["ְҵ101"] = eCL_101;
		ms_mapClassType["ְҵ102"] =eCL_102;
		ms_mapClassType["ְҵ103"] =eCL_103;
		ms_mapClassType["ְҵ104"] =eCL_104;
		ms_mapClassType["ְҵ105"] =eCL_105;
		ms_mapClassType["ְҵ106"] =eCL_106;
		ms_mapClassType["ְҵ107"] =eCL_107;
		ms_mapClassType["ְҵ108"] =eCL_108;
		ms_mapClassType["ְҵ109"] =eCL_109;
		ms_mapClassType["ְҵ110"] =eCL_110;
		ms_mapClassType["ְҵ111"] =eCL_111;
		ms_mapClassType["ְҵ112"] =eCL_112;
		ms_mapClassType["ְҵ113"] =eCL_113;
		ms_mapClassType["ְҵ114"] =eCL_114;
		ms_mapClassType["ְҵ115"] =eCL_115;
		ms_mapClassType["ְҵ116"] =eCL_116;
		ms_mapClassType["ְҵ117"] =eCL_117;
		ms_mapClassType["ְҵ118"] =eCL_118;
		ms_mapClassType["ְҵ119"] =eCL_119;
		ms_mapClassType["ְҵ120"] =eCL_120;
		ms_mapClassType["ְҵ121"] =eCL_121;
		ms_mapClassType["ְҵ122"] =eCL_122;
		ms_mapClassType["ְҵ123"] =eCL_123;
		ms_mapClassType["ְҵ124"] =eCL_124;
		ms_mapClassType["ְҵ125"] =eCL_125;
		ms_mapClassType["ְҵ126"] =eCL_126;
		ms_mapClassType["ְҵ127"] =eCL_127;
		ms_mapClassType["ְҵ128"] =eCL_128;
		ms_mapClassType["ְҵ129"] =eCL_129;
		ms_mapClassType["ְҵ130"] =eCL_130;
		ms_mapClassType["ְҵ131"] =eCL_131;
		ms_mapClassType["ְҵ132"] =eCL_132;
		ms_mapClassType["ְҵ133"] =eCL_133;
		ms_mapClassType["ְҵ134"] =eCL_134;
		ms_mapClassType["ְҵ135"] =eCL_135;
		ms_mapClassType["ְҵ136"] =eCL_136;
		ms_mapClassType["ְҵ137"] =eCL_137;
		ms_mapClassType["ְҵ138"] =eCL_138;
		ms_mapClassType["ְҵ139"] =eCL_139;
		ms_mapClassType["ְҵ140"] =eCL_140;
		ms_mapClassType["ְҵ141"] =eCL_141;
		ms_mapClassType["ְҵ142"] =eCL_142;
		ms_mapClassType["ְҵ143"] =eCL_143;
		ms_mapClassType["ְҵ144"] =eCL_144;
		ms_mapClassType["ְҵ145"] =eCL_145;
		ms_mapClassType["ְҵ146"] =eCL_146;
		ms_mapClassType["ְҵ147"] =eCL_147;
		ms_mapClassType["ְҵ148"] =eCL_148;
		ms_mapClassType["ְҵ149"] =eCL_149;
		ms_mapClassType["ְҵ150"] =eCL_150;
		ms_mapClassType["ְҵ151"] =eCL_151;
		ms_mapClassType["ְҵ152"] =eCL_152;
		ms_mapClassType["ְҵ153"] =eCL_153;
		ms_mapClassType["ְҵ154"] =eCL_154;
		ms_mapClassType["ְҵ155"] =eCL_155;
		ms_mapClassType["ְҵ156"] =eCL_156;
		ms_mapClassType["ְҵ157"] =eCL_157;
		ms_mapClassType["ְҵ158"] =eCL_158;
		ms_mapClassType["ְҵ159"] =eCL_159;
		ms_mapClassType["ְҵ160"] =eCL_160;
		ms_mapClassType["ְҵ161"] =eCL_161;
		ms_mapClassType["ְҵ162"] =eCL_162;
		ms_mapClassType["ְҵ163"] =eCL_163;
		ms_mapClassType["ְҵ164"] =eCL_164;
		ms_mapClassType["ְҵ165"] =eCL_165;
		ms_mapClassType["ְҵ166"] =eCL_166;
		ms_mapClassType["ְҵ167"] =eCL_167;
		ms_mapClassType["ְҵ168"] =eCL_168;
		ms_mapClassType["ְҵ169"] =eCL_169;
		ms_mapClassType["ְҵ170"] =eCL_170;
		ms_mapClassType["ְҵ171"] =eCL_171;
		ms_mapClassType["ְҵ172"] =eCL_172;
		ms_mapClassType["ְҵ173"] =eCL_173;
		ms_mapClassType["ְҵ174"] =eCL_174;
		ms_mapClassType["ְҵ175"] =eCL_175;
		ms_mapClassType["ְҵ176"] =eCL_176;
		ms_mapClassType["ְҵ177"] =eCL_177;
		ms_mapClassType["ְҵ178"] =eCL_178;
		ms_mapClassType["ְҵ179"] =eCL_179;
		ms_mapClassType["ְҵ180"] =eCL_180;
		ms_mapClassType["ְҵ181"] =eCL_181;
		ms_mapClassType["ְҵ182"] =eCL_182;
		ms_mapClassType["ְҵ183"] =eCL_183;
		ms_mapClassType["ְҵ184"] =eCL_184;
		ms_mapClassType["ְҵ185"] =eCL_185;
		ms_mapClassType["ְҵ186"] =eCL_186;
		ms_mapClassType["ְҵ187"] =eCL_187;
		ms_mapClassType["ְҵ188"] =eCL_188;
		ms_mapClassType["ְҵ189"] =eCL_189;
		ms_mapClassType["ְҵ190"] =eCL_190;
		ms_mapClassType["ְҵ191"] =eCL_191;
		ms_mapClassType["ְҵ192"] =eCL_192;
		ms_mapClassType["ְҵ193"] =eCL_193;
		ms_mapClassType["ְҵ194"] =eCL_194;
		ms_mapClassType["ְҵ195"] =eCL_195;
		ms_mapClassType["ְҵ196"] =eCL_196;
		ms_mapClassType["ְҵ197"] =eCL_197;
		ms_mapClassType["ְҵ198"] =eCL_198;
		ms_mapClassType["ְҵ199"] =eCL_199;
		ms_mapClassType["ְҵ200"] =eCL_200;
		ms_mapClassType["ְҵ201"] =eCL_201;
		ms_mapClassType["ְҵ202"] =eCL_202;
		ms_mapClassType["ְҵ203"] =eCL_203;
		ms_mapClassType["ְҵ204"] =eCL_204;
		ms_mapClassType["ְҵ205"] =eCL_205;
		ms_mapClassType["ְҵ206"] =eCL_206;
		ms_mapClassType["ְҵ207"] =eCL_207;
		ms_mapClassType["ְҵ208"] =eCL_208;
		ms_mapClassType["ְҵ209"] =eCL_209;
		ms_mapClassType["ְҵ210"] =eCL_210;
		ms_mapClassType["ְҵ211"] =eCL_211;
		ms_mapClassType["ְҵ212"] =eCL_212;
		ms_mapClassType["ְҵ213"] =eCL_213;
		ms_mapClassType["ְҵ214"] =eCL_214;
		ms_mapClassType["ְҵ215"] =eCL_215;
		ms_mapClassType["ְҵ216"] =eCL_216;
		ms_mapClassType["ְҵ217"] =eCL_217;
		ms_mapClassType["ְҵ218"] =eCL_218;
		ms_mapClassType["ְҵ219"] =eCL_219;
		ms_mapClassType["ְҵ220"] =eCL_220;
		ms_mapClassType["ְҵ221"] =eCL_221;
		ms_mapClassType["ְҵ222"] =eCL_222;
		ms_mapClassType["ְҵ223"] =eCL_223;
		ms_mapClassType["ְҵ224"] =eCL_224;
		ms_mapClassType["ְҵ225"] =eCL_225;
		ms_mapClassType["ְҵ226"] =eCL_226;
		ms_mapClassType["ְҵ227"] =eCL_227;
		ms_mapClassType["ְҵ228"] =eCL_228;
		ms_mapClassType["ְҵ229"] =eCL_229;
		ms_mapClassType["ְҵ230"] =eCL_230;
		ms_mapClassType["ְҵ231"] =eCL_231;
		ms_mapClassType["ְҵ232"] =eCL_232;
		ms_mapClassType["ְҵ233"] =eCL_233;
		ms_mapClassType["ְҵ234"] =eCL_234;
		ms_mapClassType["ְҵ235"] =eCL_235;
		ms_mapClassType["ְҵ236"] =eCL_236;
		ms_mapClassType["ְҵ237"] =eCL_237;
		ms_mapClassType["ְҵ238"] =eCL_238;
		ms_mapClassType["ְҵ239"] =eCL_239;
		ms_mapClassType["ְҵ240"] =eCL_240;
		ms_mapClassType["ְҵ241"] =eCL_241;
		ms_mapClassType["ְҵ242"] =eCL_242;
		ms_mapClassType["ְҵ243"] =eCL_243;
		ms_mapClassType["ְҵ244"] =eCL_244;
		ms_mapClassType["ְҵ245"] =eCL_245;
		ms_mapClassType["ְҵ246"] =eCL_246;
		ms_mapClassType["ְҵ247"] =eCL_247;
		ms_mapClassType["ְҵ248"] =eCL_248;
		ms_mapClassType["ְҵ249"] =eCL_249;
		ms_mapClassType["ְҵ250"] =eCL_250;
		ms_mapClassType["ְҵ251"] =eCL_251;
		ms_mapClassType["ְҵ252"] =eCL_252;
		ms_mapClassType["ְҵ253"] =eCL_253;
		ms_mapClassType["ְҵ254"] =eCL_254;
		ms_mapClassType["ְҵ255"] =eCL_255;
		ms_mapClassType["ְҵ256"] =eCL_256;
		ms_mapClassType["ְҵ257"] =eCL_257;
		ms_mapClassType["ְҵ258"] =eCL_258;
		ms_mapClassType["ְҵ259"] =eCL_259;
		ms_mapClassType["ְҵ260"] =eCL_260;
		ms_mapClassType["ְҵ261"] =eCL_261;
		ms_mapClassType["ְҵ262"] =eCL_262;
		ms_mapClassType["ְҵ263"] =eCL_263;
		ms_mapClassType["ְҵ264"] =eCL_264;
		ms_mapClassType["ְҵ265"] =eCL_265;
		ms_mapClassType["ְҵ266"] =eCL_266;
		ms_mapClassType["ְҵ267"] =eCL_267;
		ms_mapClassType["ְҵ268"] =eCL_268;
		ms_mapClassType["ְҵ269"] =eCL_269;
		ms_mapClassType["ְҵ270"] =eCL_270;
		ms_mapClassType["ְҵ271"] =eCL_271;
		ms_mapClassType["ְҵ272"] =eCL_272;
		ms_mapClassType["ְҵ273"] =eCL_273;
		ms_mapClassType["ְҵ274"] =eCL_274;
		ms_mapClassType["ְҵ275"] =eCL_275;
		ms_mapClassType["ְҵ276"] =eCL_276;
		ms_mapClassType["ְҵ277"] =eCL_277;
		ms_mapClassType["ְҵ278"] =eCL_278;
		ms_mapClassType["ְҵ279"] =eCL_279;
		ms_mapClassType["ְҵ280"] =eCL_280;
		ms_mapClassType["ְҵ281"] =eCL_281;
		ms_mapClassType["ְҵ282"] =eCL_282;
		ms_mapClassType["ְҵ283"] =eCL_283;
		ms_mapClassType["ְҵ284"] =eCL_284;
		ms_mapClassType["ְҵ285"] =eCL_285;
		ms_mapClassType["ְҵ286"] =eCL_286;
		ms_mapClassType["ְҵ287"] =eCL_287;
		ms_mapClassType["ְҵ288"] =eCL_288;
		ms_mapClassType["ְҵ289"] =eCL_289;
		ms_mapClassType["ְҵ290"] =eCL_290;
		ms_mapClassType["ְҵ291"] =eCL_291;
		ms_mapClassType["ְҵ292"] =eCL_292;
		ms_mapClassType["ְҵ293"] =eCL_293;
		ms_mapClassType["ְҵ294"] =eCL_294;
		ms_mapClassType["ְҵ295"] =eCL_295;
		ms_mapClassType["ְҵ296"] =eCL_296;
		ms_mapClassType["ְҵ297"] =eCL_297;
		ms_mapClassType["ְҵ298"] =eCL_298;
		ms_mapClassType["ְҵ299"] =eCL_299;
		ms_mapClassType["ְҵ300"] =eCL_300;
		ms_mapClassType["ְҵ301"] =eCL_301;
		ms_mapClassType["ְҵ302"] =eCL_302;
		ms_mapClassType["ְҵ303"] =eCL_303;
		ms_mapClassType["ְҵ304"] =eCL_304;
		ms_mapClassType["ְҵ305"] =eCL_305;
		ms_mapClassType["ְҵ306"] =eCL_306;
		ms_mapClassType["ְҵ307"] =eCL_307;
		ms_mapClassType["ְҵ308"] =eCL_308;
		ms_mapClassType["ְҵ309"] =eCL_309;
		ms_mapClassType["ְҵ310"] =eCL_310;
		ms_mapClassType["ְҵ311"] =eCL_311;
		ms_mapClassType["ְҵ312"] =eCL_312;
		ms_mapClassType["ְҵ313"] =eCL_313;
		ms_mapClassType["ְҵ314"] =eCL_314;
		ms_mapClassType["ְҵ315"] =eCL_315;
		ms_mapClassType["ְҵ316"] =eCL_316;
		ms_mapClassType["ְҵ317"] =eCL_317;
		ms_mapClassType["ְҵ318"] =eCL_318;
		ms_mapClassType["ְҵ319"] =eCL_319;
		ms_mapClassType["ְҵ320"] =eCL_320;
		ms_mapClassType["ְҵ321"] =eCL_321;
		ms_mapClassType["ְҵ322"] =eCL_322;
		ms_mapClassType["ְҵ323"] =eCL_323;
		ms_mapClassType["ְҵ324"] =eCL_324;
		ms_mapClassType["ְҵ325"] =eCL_325;
		ms_mapClassType["ְҵ326"] =eCL_326;
		ms_mapClassType["ְҵ327"] =eCL_327;
		ms_mapClassType["ְҵ328"] =eCL_328;
		ms_mapClassType["ְҵ329"] =eCL_329;
		ms_mapClassType["ְҵ330"] =eCL_330;
		ms_mapClassType["ְҵ331"] =eCL_331;
		ms_mapClassType["ְҵ332"] =eCL_332;
		ms_mapClassType["ְҵ333"] =eCL_333;
		ms_mapClassType["ְҵ334"] =eCL_334;
		ms_mapClassType["ְҵ335"] =eCL_335;
		ms_mapClassType["ְҵ336"] =eCL_336;
		ms_mapClassType["ְҵ337"] =eCL_337;
		ms_mapClassType["ְҵ338"] =eCL_338;
		ms_mapClassType["ְҵ339"] =eCL_339;
		ms_mapClassType["ְҵ340"] =eCL_340;
		ms_mapClassType["ְҵ341"] =eCL_341;
		ms_mapClassType["ְҵ342"] =eCL_342;
		ms_mapClassType["ְҵ343"] =eCL_343;
		ms_mapClassType["ְҵ344"] =eCL_344;
		ms_mapClassType["ְҵ345"] =eCL_345;
		ms_mapClassType["ְҵ346"] =eCL_346;
		ms_mapClassType["ְҵ347"] =eCL_347;
		ms_mapClassType["ְҵ348"] =eCL_348;
		ms_mapClassType["ְҵ349"] =eCL_349;
		ms_mapClassType["ְҵ350"] =eCL_350;
		ms_mapClassType["ְҵ351"] =eCL_351;
		ms_mapClassType["ְҵ352"] =eCL_352;
		ms_mapClassType["ְҵ353"] =eCL_353;
		ms_mapClassType["ְҵ354"] =eCL_354;
		ms_mapClassType["ְҵ355"] =eCL_355;
		ms_mapClassType["ְҵ356"] =eCL_356;
		ms_mapClassType["ְҵ357"] =eCL_357;
		ms_mapClassType["ְҵ358"] =eCL_358;
		ms_mapClassType["ְҵ359"] =eCL_359;
		ms_mapClassType["ְҵ360"] =eCL_360;
		ms_mapClassType["ְҵ361"] =eCL_361;
		ms_mapClassType["ְҵ362"] =eCL_362;
		ms_mapClassType["ְҵ363"] =eCL_363;
		ms_mapClassType["ְҵ364"] =eCL_364;
		ms_mapClassType["ְҵ365"] =eCL_365;
		ms_mapClassType["ְҵ366"] =eCL_366;
		ms_mapClassType["ְҵ367"] =eCL_367;
		ms_mapClassType["ְҵ368"] =eCL_368;
		ms_mapClassType["ְҵ369"] =eCL_369;
		ms_mapClassType["ְҵ370"] =eCL_370;
		ms_mapClassType["ְҵ371"] =eCL_371;
		ms_mapClassType["ְҵ372"] =eCL_372;
		ms_mapClassType["ְҵ373"] =eCL_373;
		ms_mapClassType["ְҵ374"] =eCL_374;
		ms_mapClassType["ְҵ375"] =eCL_375;
		ms_mapClassType["ְҵ376"] =eCL_376;
		ms_mapClassType["ְҵ377"] =eCL_377;
		ms_mapClassType["ְҵ378"] =eCL_378;
		ms_mapClassType["ְҵ379"] =eCL_379;
		ms_mapClassType["ְҵ380"] =eCL_380;
		ms_mapClassType["ְҵ381"] =eCL_381;
		ms_mapClassType["ְҵ382"] =eCL_382;
		ms_mapClassType["ְҵ383"] =eCL_383;
		ms_mapClassType["ְҵ384"] =eCL_384;
		ms_mapClassType["ְҵ385"] =eCL_385;
		ms_mapClassType["ְҵ386"] =eCL_386;
		ms_mapClassType["ְҵ387"] =eCL_387;
		ms_mapClassType["ְҵ388"] =eCL_388;
		ms_mapClassType["ְҵ389"] =eCL_389;
		ms_mapClassType["ְҵ390"] =eCL_390;
		ms_mapClassType["ְҵ391"] =eCL_391;
		ms_mapClassType["ְҵ392"] =eCL_392;
		ms_mapClassType["ְҵ393"] =eCL_393;
		ms_mapClassType["ְҵ394"] =eCL_394;
		ms_mapClassType["ְҵ395"] =eCL_395;
		ms_mapClassType["ְҵ396"] =eCL_396;
		ms_mapClassType["ְҵ397"] =eCL_397;
		ms_mapClassType["ְҵ398"] =eCL_398;
		ms_mapClassType["ְҵ399"] =eCL_399;
		ms_mapClassType["ְҵ400"] =eCL_400;
		ms_mapClassType["ְҵ401"] =eCL_401;
		ms_mapClassType["ְҵ402"] =eCL_402;
		ms_mapClassType["ְҵ403"] =eCL_403;
		ms_mapClassType["ְҵ404"] =eCL_404;
		ms_mapClassType["ְҵ405"] =eCL_405;
		ms_mapClassType["ְҵ406"] =eCL_406;
		ms_mapClassType["ְҵ407"] =eCL_407;
		ms_mapClassType["ְҵ408"] =eCL_408;
		ms_mapClassType["ְҵ409"] =eCL_409;
		ms_mapClassType["ְҵ410"] =eCL_410;
		ms_mapClassType["ְҵ411"] =eCL_411;
		ms_mapClassType["ְҵ412"] =eCL_412;
		ms_mapClassType["ְҵ413"] =eCL_413;
		ms_mapClassType["ְҵ414"] =eCL_414;
		ms_mapClassType["ְҵ415"] =eCL_415;
		ms_mapClassType["ְҵ416"] =eCL_416;
		ms_mapClassType["ְҵ417"] =eCL_417;
		ms_mapClassType["ְҵ418"] =eCL_418;
		ms_mapClassType["ְҵ419"] =eCL_419;
		ms_mapClassType["ְҵ420"] =eCL_420;
		ms_mapClassType["ְҵ421"] =eCL_421;
		ms_mapClassType["ְҵ422"] =eCL_422;
		ms_mapClassType["ְҵ423"] =eCL_423;
		ms_mapClassType["ְҵ424"] =eCL_424;
		ms_mapClassType["ְҵ425"] =eCL_425;
		ms_mapClassType["ְҵ426"] =eCL_426;
		ms_mapClassType["ְҵ427"] =eCL_427;
		ms_mapClassType["ְҵ428"] =eCL_428;
		ms_mapClassType["ְҵ429"] =eCL_429;
		ms_mapClassType["ְҵ430"] =eCL_430;
		ms_mapClassType["ְҵ431"] =eCL_431;
		ms_mapClassType["ְҵ432"] =eCL_432;
		ms_mapClassType["ְҵ433"] =eCL_433;
		ms_mapClassType["ְҵ434"] =eCL_434;
		ms_mapClassType["ְҵ435"] =eCL_435;
		ms_mapClassType["ְҵ436"] =eCL_436;
		ms_mapClassType["ְҵ437"] =eCL_437;
		ms_mapClassType["ְҵ438"] =eCL_438;
		ms_mapClassType["ְҵ439"] =eCL_439;
		ms_mapClassType["ְҵ440"] =eCL_440;
		ms_mapClassType["ְҵ441"] =eCL_441;
		ms_mapClassType["ְҵ442"] =eCL_442;
		ms_mapClassType["ְҵ443"] =eCL_443;
		ms_mapClassType["ְҵ444"] =eCL_444;
		ms_mapClassType["ְҵ445"] =eCL_445;
		ms_mapClassType["ְҵ446"] =eCL_446;
		ms_mapClassType["ְҵ447"] =eCL_447;
		ms_mapClassType["ְҵ448"] =eCL_448;
		ms_mapClassType["ְҵ449"] =eCL_449;
		ms_mapClassType["ְҵ450"] =eCL_450;
		ms_mapClassType["ְҵ451"] =eCL_451;
		ms_mapClassType["ְҵ452"] =eCL_452;
		ms_mapClassType["ְҵ453"] =eCL_453;
		ms_mapClassType["ְҵ454"] =eCL_454;
		ms_mapClassType["ְҵ455"] =eCL_455;
		ms_mapClassType["ְҵ456"] =eCL_456;
		ms_mapClassType["ְҵ457"] =eCL_457;
		ms_mapClassType["ְҵ458"] =eCL_458;
		ms_mapClassType["ְҵ459"] =eCL_459;
		ms_mapClassType["ְҵ460"] =eCL_460;
		ms_mapClassType["ְҵ461"] =eCL_461;
		ms_mapClassType["ְҵ462"] =eCL_462;
		ms_mapClassType["ְҵ463"] =eCL_463;
		ms_mapClassType["ְҵ464"] =eCL_464;
		ms_mapClassType["ְҵ465"] =eCL_465;
		ms_mapClassType["ְҵ466"] =eCL_466;
		ms_mapClassType["ְҵ467"] =eCL_467;
		ms_mapClassType["ְҵ468"] =eCL_468;
		ms_mapClassType["ְҵ469"] =eCL_469;
		ms_mapClassType["ְҵ470"] =eCL_470;
		ms_mapClassType["ְҵ471"] =eCL_471;
		ms_mapClassType["ְҵ472"] =eCL_472;
		ms_mapClassType["ְҵ473"] =eCL_473;
		ms_mapClassType["ְҵ474"] =eCL_474;
		ms_mapClassType["ְҵ475"] =eCL_475;
		ms_mapClassType["ְҵ476"] =eCL_476;
		ms_mapClassType["ְҵ477"] =eCL_477;
		ms_mapClassType["ְҵ478"] =eCL_478;
		ms_mapClassType["ְҵ479"] =eCL_479;
		ms_mapClassType["ְҵ480"] =eCL_480;
		ms_mapClassType["ְҵ481"] =eCL_481;
		ms_mapClassType["ְҵ482"] =eCL_482;
		ms_mapClassType["ְҵ483"] =eCL_483;
		ms_mapClassType["ְҵ484"] =eCL_484;
		ms_mapClassType["ְҵ485"] =eCL_485;
		ms_mapClassType["ְҵ486"] =eCL_486;
		ms_mapClassType["ְҵ487"] =eCL_487;
		ms_mapClassType["ְҵ488"] =eCL_488;
		ms_mapClassType["ְҵ489"] =eCL_489;
		ms_mapClassType["ְҵ490"] =eCL_490;
		ms_mapClassType["ְҵ491"] =eCL_491;
		ms_mapClassType["ְҵ492"] =eCL_492;
		ms_mapClassType["ְҵ493"] =eCL_493;
		ms_mapClassType["ְҵ494"] =eCL_494;
		ms_mapClassType["ְҵ495"] =eCL_495;
		ms_mapClassType["ְҵ496"] =eCL_496;
		ms_mapClassType["ְҵ497"] =eCL_497;
		ms_mapClassType["ְҵ498"] =eCL_498;
		ms_mapClassType["ְҵ499"] =eCL_499;
		ms_mapClassType["ְҵ500"] =eCL_500;
		ms_mapClassType["ְҵ501"] =eCL_501;
		ms_mapClassType["ְҵ502"] =eCL_502;
		ms_mapClassType["ְҵ503"] =eCL_503;
		ms_mapClassType["ְҵ504"] =eCL_504;
		ms_mapClassType["ְҵ505"] =eCL_505;
		ms_mapClassType["ְҵ506"] =eCL_506;
		ms_mapClassType["ְҵ507"] =eCL_507;
		ms_mapClassType["ְҵ508"] =eCL_508;
		ms_mapClassType["ְҵ509"] =eCL_509;
		ms_mapClassType["ְҵ510"] =eCL_510;
		ms_mapClassType["ְҵ511"] =eCL_511;
		ms_mapClassType["ְҵ512"] =eCL_512;
		ms_mapClassType["ְҵ513"] =eCL_513;
		ms_mapClassType["ְҵ514"] =eCL_514;
		ms_mapClassType["ְҵ515"] =eCL_515;
		ms_mapClassType["ְҵ516"] =eCL_516;
		ms_mapClassType["ְҵ517"] =eCL_517;
		ms_mapClassType["ְҵ518"] =eCL_518;
		ms_mapClassType["ְҵ519"] =eCL_519;
		ms_mapClassType["ְҵ520"] =eCL_520;
		ms_mapClassType["ְҵ521"] =eCL_521;
		ms_mapClassType["ְҵ522"] =eCL_522;
		ms_mapClassType["ְҵ523"] =eCL_523;
		ms_mapClassType["ְҵ524"] =eCL_524;
		ms_mapClassType["ְҵ525"] =eCL_525;
		ms_mapClassType["ְҵ526"] =eCL_526;
		ms_mapClassType["ְҵ527"] =eCL_527;
		ms_mapClassType["ְҵ528"] =eCL_528;
		ms_mapClassType["ְҵ529"] =eCL_529;
		ms_mapClassType["ְҵ530"] =eCL_530;
		ms_mapClassType["ְҵ531"] =eCL_531;
		ms_mapClassType["ְҵ532"] =eCL_532;
		ms_mapClassType["ְҵ533"] =eCL_533;
		ms_mapClassType["ְҵ534"] =eCL_534;
		ms_mapClassType["ְҵ535"] =eCL_535;
		ms_mapClassType["ְҵ536"] =eCL_536;
		ms_mapClassType["ְҵ537"] =eCL_537;
		ms_mapClassType["ְҵ538"] =eCL_538;
		ms_mapClassType["ְҵ539"] =eCL_539;
		ms_mapClassType["ְҵ540"] =eCL_540;
		ms_mapClassType["ְҵ541"] =eCL_541;
		ms_mapClassType["ְҵ542"] =eCL_542;
		ms_mapClassType["ְҵ543"] =eCL_543;
		ms_mapClassType["ְҵ544"] =eCL_544;
		ms_mapClassType["ְҵ545"] =eCL_545;
		ms_mapClassType["ְҵ546"] =eCL_546;
		ms_mapClassType["ְҵ547"] =eCL_547;
		ms_mapClassType["ְҵ548"] =eCL_548;
		ms_mapClassType["ְҵ549"] =eCL_549;
		ms_mapClassType["ְҵ550"] =eCL_550;
		ms_mapClassType["ְҵ551"] =eCL_551;
		ms_mapClassType["ְҵ552"] =eCL_552;
		ms_mapClassType["ְҵ553"] =eCL_553;
		ms_mapClassType["ְҵ554"] =eCL_554;
		ms_mapClassType["ְҵ555"] =eCL_555;
		ms_mapClassType["ְҵ556"] =eCL_556;
		ms_mapClassType["ְҵ557"] =eCL_557;
		ms_mapClassType["ְҵ558"] =eCL_558;
		ms_mapClassType["ְҵ559"] =eCL_559;
		ms_mapClassType["ְҵ560"] =eCL_560;
		ms_mapClassType["ְҵ561"] =eCL_561;
		ms_mapClassType["ְҵ562"] =eCL_562;
		ms_mapClassType["ְҵ563"] =eCL_563;
		ms_mapClassType["ְҵ564"] =eCL_564;
		ms_mapClassType["ְҵ565"] =eCL_565;
		ms_mapClassType["ְҵ566"] =eCL_566;
		ms_mapClassType["ְҵ567"] =eCL_567;
		ms_mapClassType["ְҵ568"] =eCL_568;
		ms_mapClassType["ְҵ569"] =eCL_569;
		ms_mapClassType["ְҵ570"] =eCL_570;
		ms_mapClassType["ְҵ571"] =eCL_571;
		ms_mapClassType["ְҵ572"] =eCL_572;
		ms_mapClassType["ְҵ573"] =eCL_573;
		ms_mapClassType["ְҵ574"] =eCL_574;
		ms_mapClassType["ְҵ575"] =eCL_575;
		ms_mapClassType["ְҵ576"] =eCL_576;
		ms_mapClassType["ְҵ577"] =eCL_577;
		ms_mapClassType["ְҵ578"] =eCL_578;
		ms_mapClassType["ְҵ579"] =eCL_579;
		ms_mapClassType["ְҵ580"] =eCL_580;
		ms_mapClassType["ְҵ581"] =eCL_581;
		ms_mapClassType["ְҵ582"] =eCL_582;
		ms_mapClassType["ְҵ583"] =eCL_583;
		ms_mapClassType["ְҵ584"] =eCL_584;
		ms_mapClassType["ְҵ585"] =eCL_585;
		ms_mapClassType["ְҵ586"] =eCL_586;
		ms_mapClassType["ְҵ587"] =eCL_587;
		ms_mapClassType["ְҵ588"] =eCL_588;
		ms_mapClassType["ְҵ589"] =eCL_589;
		ms_mapClassType["ְҵ590"] =eCL_590;
		ms_mapClassType["ְҵ591"] =eCL_591;
		ms_mapClassType["ְҵ592"] =eCL_592;
		ms_mapClassType["ְҵ593"] =eCL_593;
		ms_mapClassType["ְҵ594"] =eCL_594;
		ms_mapClassType["ְҵ595"] =eCL_595;
		ms_mapClassType["ְҵ596"] =eCL_596;
		ms_mapClassType["ְҵ597"] =eCL_597;
		ms_mapClassType["ְҵ598"] =eCL_598;
		ms_mapClassType["ְҵ599"] =eCL_599;
		ms_mapClassType["ְҵ600"] =eCL_600;

	
		ms_mapClassType["����"]			=   eCL_Special;
		ms_mapClassType["����"]			=	eCl_Building;
		return true;
	}

private:
	bool IsAccordwithLevelLimit(uint32 uLevel);

	typedef multimap<EClass, CPropertyCfg*>		MultimapPropertyCfg;
	static MultimapPropertyCfg					ms_MultimapProperty;

	EClass			m_eClass;					// ְҵ
	CCfgCalc*		m_pLevel;					// �ȼ�
	CCfgCalc*		m_pMaxHP;					// ��������
	CCfgCalc*		m_pMaxMP;					// ħ������
	float			m_pHPUpdateRate;			// ��Ѫ��
	float			m_pMPUpdateRate;			// ��ħ��
	float			m_pRevertPer;				// ս��״̬�ظ��ٶ�
	CCfgCalc*		m_pDPS;						// DPS
	CCfgCalc*		m_pMagicDamage;				// ����
	CCfgCalc*		m_pDefence;					// ����ֵ
	CCfgCalc*		m_pPhysicalDodge;			// �������ֵ
	CCfgCalc*		m_pParryValue;				// �м�ֵ
	CCfgCalc*		m_pStrikeValue;				// ����ֵ
	CCfgCalc*	    m_pStrikeMultiValue;		// ��������
	CCfgCalc*		m_pMagicDodge;				// �������ֵ
	CCfgCalc*		m_pMagicHitValue;			// ��������
	CCfgCalc*		m_pAccuratenessValue;		// ��׼ֵ						
	CCfgCalc*		m_pResilienceValue;			// ����ֵ
	CCfgCalc*		m_pStrikeResistanceValue;			// �Ⱪֵ
	CCfgCalc*		m_pNatureResistance;		// ��Ȼ����
	CCfgCalc*		m_pDestructionResistance;	// �ƻ�����
	CCfgCalc*		m_pEvilResistance;			// ���ڿ���	
	int				m_pValidityCoefficient;		// ��Ч��ϵ��
	CCfgCalc*		m_NatureSmashValue;			// ��Ȼ��ѹֵ 
	CCfgCalc*		m_DestructionSmashValue;	// �ƻ���ѹֵ
	CCfgCalc*		m_EvilSmashValue;			// ������ѹֵ
	CCfgCalc*		m_DefenceSmashValue;		// ������ѹֵ
};