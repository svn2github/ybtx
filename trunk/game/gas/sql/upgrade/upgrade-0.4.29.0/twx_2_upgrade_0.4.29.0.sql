


##军资运输,资源点与玩家相关次数
create table tbl_tong_char_other
(
	cs_uId			bigint unsigned 	not null,			#role
	tco_uTypeId  		bigint unsigned			not null,
	tco_uNum				bigint unsigned		not null		default 0,
	primary key(cs_uId, tco_uTypeId),
	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 	on	update cascade on delete cascade
)engine=innodb;


##资源点与小队相关次数
create table tbl_tong_other
(
	t_uId					    bigint unsigned			not null,
  to_uNum						bigint unsigned		not null		default 0,
  primary key(t_uId),
	foreign	key(t_uId)		references tbl_tong(t_uId) 	on	update cascade on delete cascade
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

##跑环任务环数记录
create table tbl_quest_loop(
	cs_uId			bigint unsigned 	not null,
	ql_sLoopName		varchar(96)		not null default "",
	ql_uLoopNum		tinyint unsigned 	not null default 0,
	primary key (cs_uId),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=Innodb;

##帮会联赛中杀人次数
create table tbl_tong_jfs
(
	cs_uId			bigint unsigned 	not null,
	tj_uNum			bigint unsigned		not null,
	primary key(cs_uId),
	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 	on	update cascade on delete cascade
)engine=innodb;

create table tbl_team_static
(
	t_uId										bigint unsigned		not null		auto_increment,    #team identifier
	primary key(t_uId)
)engine=innodb;

drop table if exists tbl_hot_update_code;
#热更新的lua代码记录
create table tbl_hot_update_code(
	huc_id		bigint		unsigned	auto_increment	not null,
	huc_type	tinyint(3)	unsigned	not null	default 0,
	huc_revision	varchar(32)	not null,
	huc_patch_num	int(10)		unsigned	not null	default 1,
	huc_filename	varchar(96)	default "filename",
	huc_code	text,
	primary key(huc_id)	
)engine=InnoDb;	


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
    cmo_uProductType    tinyint     unsigned    not null,
    cmo_sProductName    varchar(96)             not null,
    cmo_tEndTime        datetime 			    not null,		# end time
    cmo_uCMMoney        bigint      unsigned    not null,
    
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

#玩家离线经验值表
create table tbl_char_off_line_exp
(
	cs_uId			bigint unsigned		not	null,
	cm_uOfflineExp		bigint unsigned	not null  default 0,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

#玩家灵感值表
create table tbl_char_afflatus_value
(
	cs_uId					bigint unsigned	not	null,
	cm_uAfflatusValue		bigint unsigned	not null  default 0,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


create table tbl_log_channel_chat
(
 cs_uId	bigint unsigned	not	null,
 lcc_sMsg	text not null,
 lcc_dtChatTime datetime	not null,
 lcc_sChannelName varchar(96) not null,

 key(cs_uId),
 key(lcc_dtChatTime)
)engine = innodb;

#拍卖底价设置
alter table tbl_team	add column t_uAuctionBasePrice      bigint unsigned  not null default 0 after t_uAuctionStandard;

alter table tbl_contract_manufacture_order add column cmo_sDisplayProductName varchar(96)  not null after cmo_uCMMoney;

#查询生活技能代工订单用到配置表中信息，生活技能等级、产出物品等级
create table tbl_contract_manufacture_cfg_info(
	cmci_sSkillName      varchar(96)             not null,
    	cmci_sDirection      varchar(96)             not null,
    	cmci_sProductName    varchar(96)             not null,
	cmci_uSkillLevel     tinyint	unsigned     not null,
	cmci_uProductLevel   tinyint	unsigned     not null,

	key(cmci_sSkillName),
	key(cmci_sDirection),
	key(cmci_sProductName)
)engine = memory;


alter table tbl_action_match_list add aml_uGroupId bigint unsigned not null after aml_uLvStep;

#玩家拍卖金钱表
create table tbl_char_auction_money
(
	cs_uId			bigint unsigned		not	null,
	cam_uObjId		bigint unsigned		not	null,
	cam_uMoney		bigint unsigned	not null  default 0,		#role money 
	cam_dtAddDate datetime not null,
	
	primary	key(cs_uId,cam_uObjId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

