#防具
create table tbl_log_item_armor
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint unsigned		not null,			#物品id
	lia_uBaseLevel		tinyint unsigned 	not null, 		#起始等级
	lia_uCurLevel        tinyint unsigned 	not null,
	lia_uUpgradeAtrrValue1	int unsigned 	not null, 			#装备产出时的属性一（升级属性1~3按在配置表中的顺序）
	lia_uUpgradeAtrrValue2	int unsigned 	not null,			#装备产出时的属性二
	lia_uUpgradeAtrrValue3	int unsigned 	not null,			#装备产出时的属性三
	lia_uStaticProValue		int unsigned 	not null,       	#静态装备属性值
	lia_uIntensifyQuality    tinyint unsigned    not null default 0, 	#装备强化资质，该值决定装备可强化的最高阶段
	le_uServerId int unsigned 			not null,
	
	key (le_uId),
	key (lis_uId),
	key (le_uServerId)
)engine=myisam;

create table tbl_log_item_weapon
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint unsigned		not null,			#物品id
	liw_uBaseLevel 		tinyint unsigned 	not null, 		#起始等级（打造时修改）
	liw_uCurLevel       tinyint unsigned 	not null,
	liw_uDPS			float unsigned 	not null,			#当前DPS（打造时修改）	
	liw_uAttackSpeed float unsigned 	not null default 0,			#攻击速度
	liw_uDPSFloorRate tinyint 	not null default 0,			#攻击浮动
	liw_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
	le_uServerId int unsigned 			not null,
	
	key (le_uId),
	key (lis_uId),
	key (le_uServerId)
)engine=myisam;

create table tbl_log_item_ring
(
	le_uId			bigint unsigned 	not null,
	lis_uId			bigint unsigned 	not null,		    #物品ID
	lir_uBaseLevel		tinyint unsigned	not null, 		#起始等级
	lir_uCurLevel        tinyint unsigned 	not null,
	lir_uDPS			float unsigned	not null,			    #攻击力
	lir_uStaticProValue  float unsigned	not null,			#静态装备属性值
	lir_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
	le_uServerId int unsigned 			not null,
	
	key (le_uId),
	key (lis_uId),
	key (le_uServerId)
)engine=myisam;

#盾牌
create table tbl_log_item_shield
(
	le_uId			bigint unsigned 	not null,
	lis_uId 			bigint unsigned 	not null,		#物品ID
	lis_uBaseLevel 		tinyint unsigned 	not null, 		#起始等级（打造时修改）
	lis_uCurLevel        tinyint unsigned 	not null,
	lis_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
	le_uServerId int unsigned 			not null,
	
	key (le_uId),
	key (lis_uId),
	key (le_uServerId)
)engine=myisam;

alter table tbl_log_xiuxingta add column lx_sFloorId varchar(2) not null after lx_uSpendTimes;
alter table tbl_log_xiuxingta add key(lx_sFloorId);

##装备护甲属性
create table tbl_log_item_equip_armor (
	le_uId			bigint unsigned 	not null,
	lis_uId          bigint unsigned not null,	#物品ID
	liea_sAttr		varchar(48) not null, #护甲片(名称)
	liea_uAddTimes tinyint	unsigned   not null,
	liea_uIndex tinyint	unsigned   not null,
	le_uServerId int unsigned 			not null,
	
	key (le_uId),
	key (lis_uId),
	key (le_uServerId)
)engine = myisam;

##盾牌产出新增四种属性。
create table tbl_log_item_shield_Attr
(
	le_uId			bigint unsigned 	not null,
	lis_uId 			bigint unsigned not null,	#物品ID
	lisa_uAttrValue1	int unsigned ,				#装备产出时的属性一护甲值的值(以后升级用到)
	lisa_uAttrValue2	int unsigned ,				#装备产出时的属性二自然抗值的值(以后升级用到)
	lisa_uAttrValue3	int unsigned ,				#装备产出时的属性三破坏抗值的值(以后升级用到)
	lisa_uAttrValue4	int unsigned ,				#装备产出时的属性四暗黑抗值的值(以后升级用到)
	le_uServerId int unsigned 			not null,
	
	key (le_uId),
	key (lis_uId),
	key (le_uServerId)
)engine=myisam;

alter table tbl_log_event_type add key (le_uServerId);

##装备强化信息表
create table tbl_log_item_equip_intensify
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint unsigned		not null,			#物品id
	liei_uIntensifySoulNum	int unsigned		not null default 0,#同上，不过适是用于强化的魂数目
	liei_uPreAddAttr1	varchar(48) not null default "" , 					#强化上一阶段的附加属性1（强化时修改）
	liei_uPreAddAttrValue1	int unsigned not null default 0,   			#附加属性1的值		（强化时修改）
	liei_uPreAddAttr2	varchar(48) not null default "", 					#强化上一阶段的附加属性2（强化时修改）
	liei_uPreAddAttrValue2	int unsigned not null default 0,   			#强化上一阶段的附加属性2的值（强化时修改）
	liei_uAddAttr1		varchar(48) not null default "" , 					#强化当前阶段附加属性1	（强化时修改）
	liei_uAddAttrValue1	int unsigned not null default 0,   				#强化当前阶段附加属性1的值（强化时修改）
	liei_uAddAttr2		varchar(48) not null default "", 					#强化当前阶段附加属性2	（强化时修改）
	liei_uAddAttrValue2	int unsigned not null default 0,   				#强化当前阶段附加属性2的值（强化时修改）
	liei_uIntensifyPhase	tinyint unsigned not null default 0,				#装备强化到的阶段（初始为0，到1、2、3、4）
	liei_uEuqipSuitType	tinyint unsigned not null default 0,				#装备强化出的套装类型，0表示无，1表示2件套，2表示3件套，3表示4件套，4表示六件套，5表示8件套
	liei_sSuitName		varchar(120) not null default "",					#套装名称
	liei_uIntensifyBackState tinyint unsigned not null default 0, 			#装备是否可进行重铸，0表示不能重铸，1表示可以
	liei_uIntensifyTimes      smallint unsigned not null default 0, 		#记录装备强化次数
	liei_uIntensifyBackTimes  smallint unsigned not null default 0, 		#记录装备重铸次数
	liei_uIntenTalentIndex     smallint unsigned not null default 0,        #强化到三阶段产生的额外天赋或技能名称
	le_uServerId int unsigned 			not null,
	
	key(le_uId),
	key(lis_uId),
	key(le_uServerId)
)engine=myisam;

#############装备强化4~9阶段属性表######################
create table tbl_log_item_equip_intensifyAddAttr
(
	le_uId			bigint unsigned 	not null,
  lis_uId      bigint unsigned 	not null,		                #物品ID
  liei_sAddAttr4       varchar(48)   not null default "",        					#强化4阶段附加属性
  liei_uAddAttr4Value  int unsigned  not null default 0,          				#强化4阶段附加属性值
  liei_sAddAttr5       varchar(48)   not null default "",       					#强化5阶段附加属性
  liei_uAddAttr5Value  int unsigned  not null default 0,          				#强化5阶段附加属性值
  liei_sAddAttr6       varchar(48)   not null default "",        					#强化6阶段附加属性
  liei_uAddAttr6Value  int unsigned  not null default 0,          				#强化6阶段附加属性值
  liei_sAddAttr7       varchar(48)   not null default "",        					#强化7阶段附加属性
  liei_uAddAttr7Value  int unsigned  not null default 0,          				#强化7阶段附加属性值
  liei_sAddAttr8       varchar(48)   not null default "",        					#强化8阶段附加属性
  liei_uAddAttr8Value  int unsigned  not null default 0,          				#强8四阶段附加属性值
  liei_sAddAttr9       varchar(48)   not null default "",        					#强化9阶段附加属性
  liei_uAddAttr9Value  int unsigned  not null default 0,          				#强化9阶段附加属性值
  le_uServerId int unsigned 	not null,
  
  key(le_uId),
  key(lis_uId),
  key(le_uServerId)
)engine=myisam;

create table tbl_log_dataosha_point
	(
		le_uId bigint unsigned not null,
		ldp_uPoint bigint not null,
		le_uServerId int unsigned not null,
		
		key(le_uId),
		key(le_uServerId)
	)engine=myisam;
