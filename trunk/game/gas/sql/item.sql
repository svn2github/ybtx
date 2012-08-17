
################################################################
#		物品
################################################################

create table tbl_item_id
(
	ii_uId	bigint unsigned	not	null auto_increment,

	primary	key(ii_uId)
)engine=innodb;

create table tbl_item_static		#item infos, 
(
	is_uId			bigint unsigned		not	null,		#item identifiyer
	is_uType		tinyint unsigned	not	null,		#item type
	is_sName		varchar(96)		not	null,		#item name

	primary	key(is_uId)
)engine=innodb;

##物品大类顺序静态表
create table tbl_item_type_order	#item type ordering for sorting
(
	is_uType	tinyint unsigned		not	null,		#item type
	is_uOrder	tinyint unsigned		not	null,		#view order
	
	unique key(is_uType),
	key(is_uOrder)
)engine=MEMORY;

create table tbl_item_smalltype_order	#item subtype ordering for sorting
(
	is_uType	tinyint unsigned		not	null,		#item type
	is_sName	varchar(96)			not	null,		#item name
	iso_uSmallType	smallint unsigned		not	null,		#item sub-type
	iso_uBaseLevel	smallint unsigned		not	null,		#view order
	unique key(is_uType,is_sName)
)engine=MEMORY;

create table tbl_item_bag_in_use	#baggage in used
(
	cs_uId		bigint unsigned	not	null, 				#baggage owner
	is_uId		bigint unsigned not null, 				#baggage identifier
	ibiu_uBagSlotIndex		tinyint unsigned not null,		#in whish slot it used
	ibiu_uRoomIndex				tinyint unsigned not null,	#in which room it used

	unique key(is_uId),
	unique key(cs_uId,ibiu_uBagSlotIndex),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_grid_in_room	
(
	gir_uGridID		bigint unsigned		not	null auto_increment,#position sequence id
	cs_uId					bigint 	unsigned	not null,		#pos owner
	gir_uRoomIndex 	tinyint	unsigned 	not null,	#index of baggage which item is in
	gir_uPos				tinyint	unsigned 	not null,		#position
	primary key(gir_uGridID),
	key(cs_uId,	gir_uRoomIndex, gir_uPos),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_item_in_grid	#item index that of main baggage
(
	is_uId		bigint 	unsigned	not	null,		#item identifier
	gir_uGridID		bigint unsigned		not	null,
	unique key(is_uId),
	foreign	key(gir_uGridID)	references tbl_grid_in_room(gir_uGridID) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_grid_info	
(
	gir_uGridID		bigint unsigned		not	null,
	gi_uCount	int		unsigned 	not null,		#item count
	is_uType	tinyint unsigned		not	null,		#item type
	is_sName	varchar(96)			not	null,		#item name
	unique key(gir_uGridID),
	foreign	key(gir_uGridID)	references tbl_grid_in_room(gir_uGridID) on	update cascade on delete cascade
)engine=innodb;

#物品一段时间可以获得的最大数量
create table tbl_item_amount_limit	
(
	cs_uId		bigint 	unsigned	not null,		#item owner
	is_sName	varchar(96)		not	null,		#item name	
	key(is_sName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#包裹仓库锁
create table tbl_item_store_room_lock	
(
	cs_uId		bigint 	unsigned	not null,		
	isrl_uTime	int 	unsigned not null,			
	isrl_dtLockTime datetime  not null,

	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

##物品绑定属性
create table tbl_item_is_binding    	#binding info of a item
(
	is_uId		bigint 	unsigned	not	null,		#item identifier
	isb_bIsbind	tinyint	unsigned	default 0, 		#item binding type
									#	0 stands for unbindinged
									#	1 stands for would be bindinged when be used
									#	2 stands for bindinged
	unique key(is_uId),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;
##物品生命周期
create table tbl_item_life		#item left time
(
	is_uId		bigint 		unsigned	not	null,	#item identifier
	il_nLeftTime	bigint  	not null,   			#left time when style is online tick
									#lasts time when style is continuous tick
	il_nLoadTime	datetime  not null,				#load time
	il_uStyle	tinyint		unsigned,			#tick style. 1 stands for online tick, 2 stands for continuous tick.
	unique key(is_uId),
	KEY `il_uStyle` (`il_uStyle`),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

##分解熟练度
create table tbl_item_break_exp	#item left time
(
	cs_uId		bigint 		unsigned	not	null,	#char identifier
	ibp_uExp  bigint 		unsigned	not	null,	#break exp
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

##########################################################
## 盒子物品掉落的物品信息表
create table tbl_item_boxitem_pickup 	#item box info
(
	cs_uId		bigint 	unsigned	not null,		#item owner
	ibp_uBoxitemId	bigint 	unsigned	not null,		#item that is in the box
	ibp_uId		bigint 	unsigned	not null auto_increment,#sequence id
	ibp_uType	tinyint unsigned	not null,		#drop type
	ibp_sName	varchar(96)		not null,		#item name dropped
	ibp_uCount	tinyint			not null,		#item count dropped
	ibp_uBindingType	tinyint	default null,

	primary key(ibp_uId),
	key(ibp_uBoxitemId),
	foreign	key(cs_uId)     	references tbl_char_static(cs_uId)  on	update cascade on delete cascade,
	foreign key(ibp_uBoxitemId) references tbl_item_static(is_uId) on delete cascade on update cascade
)engine=innodb;

########################################################
#宝石镶嵌孔
########################################################

create table tbl_stone_hole		#stone hole info
(
	sh_uId		bigint unsigned not null auto_increment,	#sequence id
	cs_uId		bigint unsigned not null, 			#owner
	sh_uPanel	tinyint unsigned not null,			#panel that the hole belongs to
	sh_uDepart	tinyint unsigned not null ,			#part that the hole belongs to
	sh_uSlot	tinyint unsigned not null,			#slot of a part that hole in
	primary key  (sh_uId),
	unique key (cs_uId, sh_uPanel, sh_uDepart, sh_uSlot),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#########################################################
#宝石
#########################################################
create table tbl_stone_in_use  		#in used stone
(
	cs_uId			bigint unsigned 	not null, 	#owner
	sh_uId			bigint unsigned 	not null,       #hole identifier that stone be in
	siu_uType		tinyint unsigned	not	null,	#item type
	siu_sName		varchar(96)		not	null,	#item name
	siu_bIsbind		tinyint unsigned	default 0, 	#item binding type
	primary key (cs_uId, sh_uId),
	foreign key (sh_uId) references tbl_stone_hole(sh_uId)	on update cascade on delete cascade,
	foreign key (cs_uId) references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_stone_frame_use	#current used stone frame
(
	cs_uId			bigint unsigned not null, 		#owner
	us_uPanel			tinyint unsigned not null default 1,	#current using stone frame
	primary key  (cs_uId),
	unique key (cs_uId, us_uPanel),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#################################################################################
#                                                                               #
#                           ==**集体仓库相关：譬如帮会、商会等的仓库**==         #
#                                                                               #
#################################################################################
##集体仓库静态表，所有的集体仓库都存在这里
create table tbl_collectivity_depot	#global orgnization depot
(
	cds_uId			bigint unsigned not null auto_increment, 	#depot identifier
	cds_dtCreateTime	datetime             not null,			#create time
	primary key(cds_uId)
)engine=innodb;

##集体仓库物品表
create table tbl_item_collectivity_depot	#items that in depot
(
	cds_uId			bigint 	unsigned	not null,		#depot identifier
	is_uId			bigint 	unsigned	not null,		#item identifier
	icd_uPageIndex		tinyint	unsigned	not null,		#in which page
	icd_uPos		tinyint	unsigned 	not null,		#slot

	unique key(is_uId),
	key(cds_uId,	icd_uPageIndex, icd_uPos),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade,
	foreign	key(cds_uId)	references tbl_collectivity_depot(cds_uId) on	update cascade on delete cascade
)engine=innodb;

##个人仓库存钱表
create table tbl_depot_money			#roles' money in depot
(
	cs_uId				bigint unsigned		not	null,	#owner
	dm_uMoney			bigint unsigned		not	null,	#count
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#################################################################################
#                                                                               #
#                           ==**武器装备相关**==                                  #
#                                                                               #
#################################################################################

#已经装备的武器防具等
#EEquipPart.eWeapon					= 1	#weapon
#EEquipPart.eHead					= 3	#head
#EEquipPart.eWear					= 4	#armor
#EEquipPart.eSash					= 5	#sash
#EEquipPart.eCuff					= 6	#cuff
#EEquipPart.eShoe					= 7	#shoe
#EEquipPart.eRingLeft					= 8	#left ring
#EEquipPart.eRingRight					= 9	#right ring
#EEquipPart.eNecklace					= 10	#necklace
#EEquipPart.eAccouterment				= 11	#Accouterment
#EEquipPart.eManteau					= 12	#manteau
#EEquipPart.eCoat					= 13	#coat
create table tbl_item_equip
(
	cs_uId				bigint unsigned		not	null,
	is_uId				bigint unsigned		not	null,
	ce_uEquipPart	tinyint unsigned  not null,   #装备位置
	primary key (is_uId),
	unique key(cs_uId,ce_uEquipPart),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_item_armor
(
	is_uId		bigint unsigned		not null,			#物品id
	ia_sName	varchar(48)		not null default "",		#显示名称
	ia_uBaseLevel		tinyint unsigned 	not null, 		#起始等级
	ia_uCurLevel        tinyint unsigned 	not null,
	ia_uUpgradeAtrrValue1	int unsigned 	not null, 			#装备产出时的属性一（升级属性1~3按在配置表中的顺序）
	ia_uUpgradeAtrrValue2	int unsigned 	not null,			#装备产出时的属性二
	ia_uUpgradeAtrrValue3	int unsigned 	not null,			#装备产出时的属性三
	ia_uStaticProValue		int unsigned 	not null,       	#静态装备属性值
	ia_uIntensifyQuality    tinyint unsigned    not null default 0, 	#装备强化资质，该值决定装备可强化的最高阶段
	ia_sIntenSoul varchar(6),
	
	primary key (is_uId),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_item_weapon
(
	is_uId 			bigint unsigned 	not null,		#物品ID
	iw_sName 		varchar(48) 		not null default "",	#显示名称（打造时修改）
	iw_uBaseLevel 		tinyint unsigned 	not null, 		#起始等级（打造时修改）
	iw_uCurLevel       tinyint unsigned 	not null,
	iw_uDPS			float unsigned 	not null,			#当前DPS（打造时修改）	
	iw_uAttackSpeed float unsigned 	not null default 0,			#攻击速度
	iw_uDPSFloorRate tinyint 	not null default 0,			#攻击浮动
	iw_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
    iw_sIntenSoul varchar(6),
    
	primary key (is_uId),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_item_ring
(
	is_uId			bigint unsigned 	not null,		    #物品ID
	ir_sName		varchar(48)		not null default "",	#显示名称
	ir_uBaseLevel		tinyint unsigned	not null, 		#起始等级
	ir_uCurLevel        tinyint unsigned 	not null,
	ir_uDPS			float unsigned	not null,			    #攻击力
	ir_uStaticProValue  float unsigned	not null,			#静态装备属性值
	ir_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
	ir_sIntenSoul varchar(6),
	
	primary key (is_uId),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_item_shield
(
	is_uId 			bigint unsigned 	not null,		#物品ID
	is_sName 		varchar(48) 		not null default "",	#显示名称（打造时修改）
	is_uBaseLevel 		tinyint unsigned 	not null, 		#起始等级（打造时修改）
	is_uCurLevel        tinyint unsigned 	not null,
	is_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
    is_sIntenSoul varchar(6),
    
	primary key (is_uId),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

##盾牌产出新增四种属性。
create table tbl_item_shield_Attr
(
	is_uId 			bigint unsigned not null,	#物品ID
	isa_uAttrValue1	int unsigned ,				#装备产出时的属性一护甲值的值(以后升级用到)
	isa_uAttrValue2	int unsigned ,				#装备产出时的属性二自然抗值的值(以后升级用到)
	isa_uAttrValue3	int unsigned ,				#装备产出时的属性三破坏抗值的值(以后升级用到)
	isa_uAttrValue4	int unsigned ,				#装备产出时的属性四暗黑抗值的值(以后升级用到)
	
	primary key (is_uId),
	foreign	key(is_uId)	references tbl_item_shield(is_uId) on	update cascade on delete cascade
)engine=innodb;

############装备进阶信息表#######################
create table tbl_item_equip_advance
(
	is_uId		bigint unsigned		not null,			#物品id
	iea_uCurAdvancePhase tinyint unsigned	not null,			#装备当前进阶的阶段
	iea_uTotalAdvancedTimes int  unsigned,					#装备的总进阶次数
	iea_uSkillPieceIndex1 	tinyint	unsigned,				#技能激活项的名称1索引
	iea_uSkillPieceIndex2 	tinyint	unsigned,				#技能激活项的名称2索引
	iea_uSkillPieceIndex3 	tinyint	unsigned,				#技能激活项的名称3索引
	iea_uSkillPieceIndex4 	tinyint	unsigned,				#技能激活项的名称4索引		
	iea_uChoosedSkillPieceIndex 	tinyint unsigned,			#当前选择的技能激活项名称索引
	iea_sJingLingType               varchar(3),                 #精灵类型：火、风、地、冰、水
	iea_uAdvanceSoulNum             smallint unsigned default 0,          #注入的进阶魂数目
	iea_uAdvancedTimes              int unsigned default 0,               #进阶次数
	iea_uAdvancedAttrValue1         int unsigned,
	iea_uAdvancedAttrValue2         int unsigned,
	iea_uAdvancedAttrValue3         int unsigned,
	iea_uAdvancedAttrValue4         int unsigned,
	iea_sAdvancedSoulRoot           varchar(12),
	iea_sAdvancedAttr1	            varchar(48),
	iea_sAdvancedAttr2	            varchar(48),
	iea_sAdvancedAttr3	            varchar(48),
	iea_sAdvancedAttr4	            varchar(48),
	
	primary key (is_uId),
	foreign key (is_uId) references tbl_item_static(is_uId) on update cascade on delete cascade
)engine=innodb;

#############玩家当前激活技能表####################
create table tbl_char_equip_advancedActiveSkill
(
	cs_uId		bigint unsigned	not	null,				#玩家id
	iea_uActiveSkillIndex	int unsigned,					#玩家选择可用的技能名称索引
	primary key (cs_uId),
	foreign key (cs_uId) references tbl_char_static(cs_uId)  on update cascade on delete cascade
)engine=innodb;

#############装备强化信息表#########################
create table tbl_item_equip_intensify
(
	is_uId		bigint unsigned		not null,			#物品id
	iei_uIntensifySoulNum	int unsigned		not null default 0,#同上，不过适是用于强化的魂数目
	iei_uPreAddAttr1	varchar(48) , 					#强化上一阶段的附加属性1（强化时修改）
	iei_uPreAddAttrValue1	int unsigned,   			#附加属性1的值		（强化时修改）
	iei_uPreAddAttr2	varchar(48) , 					#强化上一阶段的附加属性2（强化时修改）
	iei_uPreAddAttrValue2	int unsigned,   			#强化上一阶段的附加属性2的值（强化时修改）
	iei_uAddAttr1		varchar(48) , 					#强化当前阶段附加属性1	（强化时修改）
	iei_uAddAttrValue1	int unsigned,   				#强化当前阶段附加属性1的值（强化时修改）
	iei_uAddAttr2		varchar(48), 					#强化当前阶段附加属性2	（强化时修改）
	iei_uAddAttrValue2	int unsigned,   				#强化当前阶段附加属性2的值（强化时修改）
	iei_uIntensifyPhase	tinyint unsigned,				#装备强化到的阶段（初始为0，到1、2、3、4）
	iei_uEuqipSuitType	tinyint unsigned,				#装备强化出的套装类型，0表示无，1表示2件套，2表示3件套，3表示4件套，4表示六件套，5表示8件套
	iei_sSuitName		varchar(120),					#套装名称
	iei_uIntensifyBackState tinyint unsigned, 			#装备是否可进行重铸，0表示不能重铸，1表示可以
	iei_uIntensifyTimes      smallint unsigned, 		#记录装备强化次数
	iei_uIntensifyBackTimes  smallint unsigned, 		#记录装备重铸次数
	iei_uIntenTalentIndex     smallint unsigned,        #强化到三阶段产生的额外天赋或技能名称
	primary key (is_uId),
	foreign key (is_uId) references tbl_item_static(is_uId) on update cascade on delete cascade
#	foreign key (is_uId) references tbl_item_armor(is_uId) on update cascade on delete cascade,
#	foreign key (is_uId) references tbl_item_ring(is_uId) on update cascade on delete cascade,
#	foreign key (is_uId) references tbl_item_shield(is_uId) on update cascade on delete cascade
)engine=innodb;

#############装备强化4~9阶段属性表######################
create table tbl_item_equip_intensifyAddAttr
(
    is_uId      bigint unsigned 	not null,		                #物品ID
    iei_sAddAttr4       varchar(48)   ,        					#强化4阶段附加属性
    iei_uAddAttr4Value  int unsigned  ,          				#强化4阶段附加属性值
    iei_sAddAttr5       varchar(48)   ,       					#强化5阶段附加属性
    iei_uAddAttr5Value  int unsigned  ,          				#强化5阶段附加属性值
    iei_sAddAttr6       varchar(48)   ,        					#强化6阶段附加属性
    iei_uAddAttr6Value  int unsigned  ,          				#强化6阶段附加属性值
    iei_sAddAttr7       varchar(48)   ,        					#强化7阶段附加属性
    iei_uAddAttr7Value  int unsigned  ,          				#强化7阶段附加属性值
    iei_sAddAttr8       varchar(48)   ,        					#强化8阶段附加属性
    iei_uAddAttr8Value  int unsigned  ,          				#强8四阶段附加属性值
    iei_sAddAttr9       varchar(48)   ,        					#强化9阶段附加属性
    iei_uAddAttr9Value  int unsigned  ,          				#强化9阶段附加属性值
    primary key (is_uId),
    foreign	key(is_uId)	references tbl_item_equip_intensify(is_uId) on	update cascade on delete cascade
)engine=innodb;


##装备护甲属性
create table tbl_item_equip_armor (
		is_uId          bigint unsigned not null,	#物品ID
		iea_sAttr		varchar(48) not null, #护甲片(名称)
		iea_uAddTimes tinyint	unsigned   not null,
		iea_uIndex tinyint	unsigned   not null,
		
		primary key (is_uId,iea_uIndex),
    foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine = innodb;

####装备耐久
create table tbl_item_equip_durability(
    is_uId          bigint unsigned not null,	#物品ID
    ied_uMaxDuraValue float unsigned,          #耐久上限值
    ied_uCurDuraValue float unsigned,         #当前耐久值
    primary key (is_uId),
    foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine = innodb;


#####装备追加
create table tbl_item_equip_superaddation(
    is_uId          bigint unsigned not null,
    ies_uCurSuperaddPhase tinyint unsigned not null,
    primary key (is_uId),
    foreign key(is_uId) references tbl_item_static(is_uId) on update cascade on delete cascade
)engine = innodb;


##装备制作者
create table tbl_item_maker
(
	is_uId						bigint unsigned not null, 	#物品id
	cs_uId 						bigint unsigned 		not null,		#制造者
	unique key(is_uId),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade,
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

##### 魂珠
create table tbl_item_pearl
(
	is_uId			bigint unsigned 	not null,		#物品ID
	ip_uSoulNum		bigint unsigned 	not null, 		#魂的数目
	primary key (is_uId),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

##### 魂珠、经验球使用次数限制
create table tbl_item_pearl_limit
(
	cs_uId 						bigint unsigned 		not null,		#玩家id
	ipl_uItemType			tinyint	unsigned   default null,	##类型，1魂珠，2经验球
	ipl_uLimitType		tinyint	unsigned   default null,	##限制类型
	ipl_uLimitTime		bigint unsigned 	not null, 		#使用次数
	unique key(cs_uId,ipl_uItemType,ipl_uLimitType),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

##阵法书信息表
create table tbl_item_battlebook
(
	is_uId      bigint unsigned    not null,	 	                    ##阵法书流水号
	ib_uPos1			tinyint	unsigned   default null,					          ##1号位置
	ib_uPos2			tinyint	unsigned 	 default null,					          ##2号位置
	ib_uPos3			tinyint	unsigned	 default null,					          ##3号位置
	ib_uPos4			tinyint	unsigned   default null,					          ##4号位置
	ib_uPos5			tinyint	unsigned	 default null,					          ##5号位置
	primary key(is_uId),
	foreign key(is_uId)	references tbl_item_static(is_uId) on update cascade on delete cascade
)engine=innodb;

##矿藏图隐藏属性表
create table tbl_item_oremap
(
	is_uId      bigint unsigned				not null,
	io_sName		varchar(96)						default null,					          ##矿藏图位置
	io_uPosX		smallint unsigned			not	null default 1,					        ##坐标X
	io_uPosY		smallint unsigned			not	null default 1,					        ##坐标Y

	primary key(is_uId),
	foreign key(is_uId)	references tbl_item_static(is_uId) on update cascade on delete cascade
)engine=innodb;


create table tbl_item_cool_down
(
	cs_uId					bigint unsigned	not	null,
	icd_sName				varchar(96)			not	null,		#物品名称
	icd_dataUseTime	datetime  			not null,		#使用时间	
	primary key(cs_uId, icd_sName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

##采矿工具耐久度
create table tbl_item_pickore_dura
(
	is_uId      bigint unsigned				not null,
	ipd_uMaxDura int unsigned		not	null default 1,          #耐久上限值
	ipd_uCurDura int unsigned		not	null default 1,          #当前耐久值

	primary key(is_uId),
	foreign key(is_uId)	references tbl_item_static(is_uId) on update cascade on delete cascade
)engine=innodb;

#魂瓶
create table tbl_item_soul_bottle
(
	cs_uId	bigint unsigned 	not null,	
	is_uId	bigint unsigned 	not null,	
	isb_uState tinyint unsigned not null,  #激活状态 1--激活  2--关闭
	isb_uStorageNum int unsigned not null, #存储量
	
	primary key(cs_uId,is_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

#经验瓶
create table tbl_item_exp_bottle
(
	cs_uId	bigint unsigned 	not null,	
	is_uId	bigint unsigned 	not null,	
	ieb_uState tinyint unsigned not null,  #激活状态 1--激活  2--关闭
	ieb_uStorageNum int unsigned not null, #存储量
	
	primary key(cs_uId,is_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;