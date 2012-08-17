
#################################################################################
#                                                                               #
#                           ==**tables of market**==                                  #
#                                                                               #
#################################################################################

#####  order
create table tbl_market_order_static(			#market order static info
	mos_uId			bigint unsigned		not null 	auto_increment,		#order identifier
	mos_tCreateTime		datetime 		not null,				#create time
	primary key(mos_uId)
)engine=innodb;


#####  item referred by order
create table tbl_item_market(				#items that order refers
	is_uId			bigint unsigned		not null,				#item identifier
	mos_uId			bigint unsigned		not null,				#order identifier
	primary key (is_uId),
	foreign key (is_uId) 	references tbl_item_static(is_uId) on 	update cascade on delete cascade,
	foreign key (mos_uId)	references tbl_market_order_static(mos_uId) on  update cascade on delete cascade
)engine=innodb;


#####  acquisition orders
create table tbl_market_buy_order(			#acquisition orders
	mos_uId				bigint unsigned		not null,		# order identifier
	cs_uId				bigint unsigned		not null,		# role referred by order
	mbo_sItemName		varchar(96)			not null,		# acquired item name
	mbo_uCount			bigint unsigned		not null,		# acquired item number
	mbo_uPrice			bigint unsigned		not null,		# item price
	mbo_tEndTime		datetime 			not null,		# end time
	primary key (mos_uId),
	key (mbo_sItemName),
	key (mbo_tEndTime), 
	foreign key (mos_uId)	references tbl_market_order_static(mos_uId) on 	update cascade on delete cascade,
	foreign key (cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#####  出售部分
create table tbl_market_sell_order(
	mos_uId				bigint unsigned		not null,		#订单ID
	cs_uId				bigint unsigned		not null,		#角色ID
	mso_sItemType		varchar(36)	        not null,		#出售物品类别
	mso_sItemName		varchar(96)	 not null,		#出售物品名称
	mso_uCount			bigint	unsigned	not null,		#出售物品数目
	mso_uPrice			bigint	unsigned	not null,		#出售价
	mso_tEndTime		datetime 			not null,		#订单终止时间
	mso_uCanUseLevel	bigint	unsigned			not null,		#物品可使用等级
	mso_sSuitName		varchar(120) binary collate utf8_unicode_ci	 default "",	#套装名称
    mso_uQuality       tinyint unsigned    default 0,    #装备的魂跟属性，其他物品为0，法魔为1，战神为2
    mso_sItemDisplayName varchar(96)	binary collate utf8_unicode_ci		not null,		#出售物品显示名称
	primary key (mos_uId),
	key (mso_sItemName),
	key (mso_tEndTime),
	key (mso_uCanUseLevel),
	key(mso_uQuality),
	foreign key (mos_uId)	references tbl_market_order_static(mos_uId) on 	update cascade on delete cascade,
	foreign key (cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#### 记录出售物品的价格
create table tbl_market_sell_price(			#record item sold price
	cs_uId					bigint unsigned not null,		#role
	msp_sItemType			varchar(36)		not null,		#item type
	msp_sItemName			varchar(96)		not null,		#item name
	msp_uPrice				bigint unsigned		not null,	#sold price
	
	key(cs_uId,msp_sItemType,msp_sItemName),
	foreign key (cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#### 记录收购物品的价格
create table tbl_market_buy_price(			#record item acquired price
	cs_uId					bigint unsigned not null,		#role
	mbp_sItemType			varchar(36)		not null,		#item type
	mbp_sItemName			varchar(96)		not null,		#item name
	mbp_uPrice				bigint unsigned		not null,	#acquired price
	
	key(cs_uId,mbp_sItemType,mbp_sItemName),
	foreign key (cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


###切服的时候，将玩家售出到商店的物品，保存在数据库表
create table tbl_npcshop_player_sell_item(      #record item which player sold to npcshop when change server 
    cs_uId                  bigint unsigned not null,       #role
    npsi_nShopType          tinyint unsigned not null,      #pay type in shop like money or jifen type
    npsi_nId                bigint unsigned not null auto_increment,       #id point to one pack of item's id
    npsi_sItemType          tinyint unsigned not null,      #item type
    npsi_sItemName          varchar(36)      not null,      #item name
    npsi_nItemCount         tinyint unsigned not null, #count of a pack of item
    npsi_nBindingType       tinyint unsigned    not null,

    key(cs_uId),
    key(npsi_nId),
    foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

#售出到商店的堆叠物品id
create table tbl_player_sell_item_id(
    psii_nId bigint unsigned not null,
    psii_nItemId    bigint unsigned not null,
    unique key(psii_nItemId),
    key(psii_nId),
    foreign key (psii_nId)	references tbl_npcshop_player_sell_item(npsi_nId) on update cascade on delete cascade,
    foreign	key(psii_nItemId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine = innodb;

create table tbl_market_cfg_info(
    mci_uType						tinyint unsigned				    not	null,		#物品类型
	mci_sName						varchar(96)							not	null,		#物品名称
	mci_uAttrIndex                  tinyint unsigned                    not null,       #属性对应的index
    mci_uCamp                       tinyint unsigned,                    #可用阵营 

    primary key(mci_uType, mci_sName)
)ENGINE = MEMORY;

create table tbl_market_purchasing_cfg_info(
    mpci_sName						varchar(96)							not	null,		#物品名称
    mpci_uType						tinyint unsigned				    not	null,		#物品类型
	mpci_uChildType			tinyint unsigned				    default 0,
	mpci_uCanUseLevel	bigint	unsigned		not null,		#物品可使用等级
    mpci_uQuality       tinyint unsigned    default 0,
 	mpci_sItemDisplayName varchar(96)	binary collate utf8_unicode_ci		not null,		#物品显示名称
    key(mpci_uType),
	key(mpci_uChildType),
	key(mpci_uCanUseLevel),
	key(mpci_uQuality),
	key(mpci_sItemDisplayName),
    primary key(mpci_sName)
)ENGINE = MEMORY;

create table tbl_npcshop_buy_tip(
	cs_uId			bigint(20) 	unsigned not null,
	nbt_LiuTongShopBuyTip	tinyint 	unsigned default 1, #默认提示，0为不提示	
	nbt_CommonShopBuyTip	tinyint 	unsigned default 1, #默认提示，0为不提示	

	primary key (cs_uId),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


#拍卖行中用到装备进阶魂根、强化魂根、可进阶上限、可强化上限，加如下数据库表，满足要求
create table tbl_market_equip_info(
	mos_uId			bigint 	unsigned	not null,
	mei_uIntenQuality	tinyint		unsigned	not null,
	mei_uAdvanceQuality	tinyint 	unsigned	not null,
	mei_sIntenSoul		varchar(12)	not null,
	mei_sAdvanceSoul	varchar(12)	not null,

	key(mos_uId),
	foreign key(mos_uId)	references tbl_market_sell_order(mos_uId) 	on update cascade on delete cascade
)engine = innodb;


#####  order
create table tbl_contract_manufacture_order_static(			
	cmo_uId			    bigint unsigned		not null 	auto_increment,		#order identifier
	cmo_tCreateTime		datetime 		    not null,				#create time
	primary key(cmo_uId)
)engine=innodb;

#生活技能代工订单表
create table tbl_contract_manufacture_order(
    cmo_uId             bigint      unsigned    not null,
    cs_uId              bigint      unsigned    not null,
    cmo_sSkillName      varchar(96)             not null,
    cmo_sDirection      varchar(96)             not null,
    cmo_sPrescripName   varchar(96)             not null,
    cmo_tEndTime        datetime 			    not null,		# end time
    cmo_uCMMoney        bigint      unsigned    not null,
    cmo_sDisplayProductName varchar(96)         not null,

    
    primary key(cmo_uId),
    foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade,
    foreign key (cmo_uId) references tbl_contract_manufacture_order_static(cmo_uId) on update cascade on delete cascade
)engine = innodb;

#生活技能代工订单表关联的需要的材料信息
create table tbl_contract_manufacture_order_item(
    cmo_uId             bigint      unsigned    not null auto_increment,
    is_uId              bigint      unsigned    not null,
    
    primary key (is_uId),
    foreign key (is_uId) references tbl_item_static(is_uId) on update cascade on delete cascade,
    foreign key (cmo_uId) references tbl_contract_manufacture_order(cmo_uId) on update cascade on delete cascade

)engine = innodb;


#查询生活技能代工订单用到配置表中信息，生活技能等级、产出物品等级
create table tbl_contract_manufacture_cfg_info(
	cmci_sSkillName      varchar(96)             not null,
    cmci_sDirection      varchar(96)             not null,
    cmci_sPrescripName    varchar(96)            not null,
	cmci_uSkillLevel     tinyint	unsigned     not null,
	cmci_uProductLevel   tinyint	unsigned     not null,

	key(cmci_sSkillName),
	key(cmci_sDirection),
	key(cmci_sPrescripName)
)engine = memory;

#点券基本信息表
create table tbl_coupons_info(
    ci_uId          bigint  unsigned		not null,
    ci_sName        varchar(96)  collate utf8_unicode_ci           not null,
    ci_uSmallIcon   int     unsigned        not null,
    ci_uPrice       int(10) unsigned        not null,
    ci_sDesc        varchar(1024)           collate utf8_unicode_ci       not null,
    ci_sUrl         varchar(1024)           collate utf8_unicode_ci        not null,
    primary key (ci_uId)
)engine = innodb;

create table tbl_char_bought_coupons(
    cbc_uId     bigint unsigned		not null 	auto_increment,  
    cs_uId      bigint unsigned     not null,
    ci_uId      bigint unsigned     not null,
    cbc_sSequenceId varchar(96)     not null, 
    
    primary key (cbc_uId),
    key(cs_uId, ci_uId),
    foreign key (cs_uId)  references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;