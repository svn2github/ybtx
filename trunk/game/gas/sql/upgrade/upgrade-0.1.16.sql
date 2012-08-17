##增加召唤兽基本信息表
create table tbl_char_servant_outline
(
	cs_uId					bigint 			unsigned	not null,
	cso_sServantResName			varchar(32)		not null,
	cso_uServantType			tinyint(1)		not null,
	cso_uCurHP				int	unsigned	not	null,
	cso_uCurMP				int	unsigned	not	null,
	primary	key( cs_uId, cso_sServantResName),
	foreign 	key(cs_uId) 	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

##把物品大类改为tinyint类型
alter table tbl_item_static change is_sType is_uType tinyint unsigned;
alter table tbl_item_boxitem_pickup change ibp_sType ibp_uType tinyint unsigned;

##寄售交易所收购和出售订单中添加物品魂根属性字段
alter table tbl_market_buy_order add column mbo_uSoulRoot tinyint unsigned  default 0;
alter table tbl_market_sell_order add column mso_uSoulRoot tinyint unsigned default 0;

##清理掉包裹里重复的物品id信息
delete from tbl_item_store_room 
	where is_uId in
		(
			select is_uId from tbl_item_equip 
			union 
				select is_uId from tbl_mail_item
			);
create table tbl_quest_mercenarylist(
	cs_uId					bigint unsigned 	not null,
	qm_sMapName			varchar(32)		not null,
	qm_uType				tinyint unsigned 	not null,
	qm_sQuestList		varchar(256)		not null,
	qm_tTime				datetime	not	null,
	primary key (cs_uId,qm_sMapName,qm_uType)
)engine=Innodb;

create table tbl_gm_validate(
	username 	varchar(32)	not null,
	gv_uLevel tinyint not null,
	primary key 	(`username`)
)engine = InnoDb;

##log系统技能表结构修改
alter table tbl_log_skill add column ls_sName varchar(96) not null;
alter table tbl_log_skill drop column  ls_uSkillId;
alter table tbl_log_skill drop column  ls_uLevel;

#未删除的角色列表
create table tbl_log_char
(
	lcs_uId					bigint	unsigned	not	null, 
	lc_sName					char(18)					not	null,
	
	primary	key(lcs_uId),
	foreign	key(lcs_uId)	references tbl_log_user_static(lcs_uId) on	update cascade on delete cascade,
	unique key(lc_sName)
)engine=myisam;

#寄售交易所
create table tbl_log_market(
	le_uId		bigint unsigned	not	null,
	mos_uId		bigint unsigned	not null,#订单ID
	lm_uCount	int	unsigned	not null,	#物品数目
	lm_uPrice	int	unsigned	not null,	#物品单价
	
	foreign key (mos_uId)	references tbl_market_order_static(mos_uId) on 	update cascade on delete cascade,
	foreign	key(le_uId)     references tbl_log_event(le_uId)on 	delete cascade on update cascade
)engine=myisam;

#自然积分
create table tbl_log_point_nature
(
	le_uId bigint unsigned not null,
	lpn_uNaturePoint bigint unsigned not null,
	
	foreign key(le_uId) references tbl_log_event(le_uId) on delete cascade on update cascade
)engine=myisam;
#暗黑积分
create table tbl_log_point_evil
(
	le_uId bigint unsigned not null,
	lpe_uEvilPoint bigint unsigned not null,
	
	foreign key(le_uId) references tbl_log_event(le_uId) on delete cascade on update cascade
)engine=myisam;
#破坏积分
create table tbl_log_point_destroy
(
	le_uId bigint unsigned not null,
	lpd_uDestroyPoint bigint unsigned not null,
	
	foreign key(le_uId) references tbl_log_event(le_uId) on delete cascade on update cascade
)engine=myisam;

#护甲积分
create table tbl_log_point_armor
(
	le_uId bigint unsigned not null,
	lpa_uArmorPoint bigint unsigned not null,
	
	foreign key(le_uId) references tbl_log_event(le_uId) on delete cascade on update cascade
)engine=myisam;

#大逃杀积分
create table tbl_log_point_dataosha
(
	le_uId bigint unsigned not null,
	lpd_uDaTaoShaPoint bigint unsigned not null,
	
	foreign key(le_uId) references tbl_log_event(le_uId) on delete cascade on update cascade
)engine=myisam;

#积分赛积分
create table tbl_log_point_jifensai
(
	le_uId bigint unsigned not null,
	lpj_uJiFenSaiPoint bigint unsigned not null,
	
	foreign key(le_uId) references tbl_log_event(le_uId) on delete cascade on update cascade
)engine=myisam;


#穿装备
create table tbl_log_equip_puton
(
	le_uId			bigint unsigned 	not null,
	foreign	key(le_uId)	references tbl_log_event(le_uId) on	delete cascade on	update cascade
)engine=myisam;

#脱装备
create table tbl_log_equip_putoff
(
	le_uId			bigint unsigned 	not null,
	foreign	key(le_uId)	references tbl_log_event(le_uId) on	delete cascade on	update cascade
)engine=myisam;

#限制金钱流通
create table tbl_money_corrency_limit(
	mcl_sFunName 	varchar(96)	not null,
	mcl_sModule		varchar(96)	not null,
	primary key 	(`mcl_sFunName`,`mcl_sModule`)
)engine = InnoDb;

#积分限制
create table tbl_jifen_use_limit(
	jul_uType 	tinyint 	not null,
	primary key 	(`jul_uType`)
)engine = InnoDb;

#物品使用限制
create table tbl_item_use_limit(
	iul_sItemName 	varchar(96)	not null,
	primary key 	(`iul_sItemName`)
)engine = InnoDb;

 alter table tbl_log_skill  add column ls_uLevel tinyint unsigned not null;
 alter table tbl_log_skill  add column ls_uType tinyint unsigned not null;

 alter table tbl_request_add_group add column rag_uType tinyint   unsigned   not null  default 0;
 
 
 #########################################################33
  alter table tbl_item_armor drop column ia_uAddAttr3;
  alter table tbl_item_armor drop column ia_uAddAttr4;
  alter table tbl_item_armor drop column ia_uAddAttrValue3;
  alter table tbl_item_armor drop column ia_uAddAttrValue4;
  
  alter table tbl_item_weapon drop column iw_uAddAttr3;
	alter table tbl_item_weapon drop column iw_uAddAttr4;
	alter table tbl_item_weapon drop column iw_uAddAttr5;
	alter table tbl_item_weapon drop column iw_uAddAttr6;
	
	alter table tbl_item_weapon drop column iw_uAddAttrValue3;
	alter table tbl_item_weapon drop column iw_uAddAttrValue4;
	alter table tbl_item_weapon drop column iw_uAddAttrValue5;
	alter table tbl_item_weapon drop column iw_uAddAttrValue6;
	
	alter table tbl_item_ring drop column ir_uAddAttr3;
	alter table tbl_item_ring drop column ir_uAddAttr4;
	alter table tbl_item_ring drop column ir_uAddAttrValue3;
	alter table tbl_item_ring drop column ir_uAddAttrValue4;
	
	alter table tbl_item_shield drop column is_uAddAttr3;
	alter table tbl_item_shield drop column is_uAddAttr4;
	alter table tbl_item_shield drop column is_uAddAttrValue3;
	alter table tbl_item_shield drop column is_uAddAttrValue4;
	
##装备护甲属性
create table tbl_item_equip_armor_attr(
    is_uId          bigint unsigned not null,	#物品ID
    ieaa_sAttr1		varchar(48) default null, #护甲片1(名称)
		ieaa_sAttr2		varchar(48) default null, #护甲片2(名称)
		ieaa_sAttr3		varchar(48) default null, #护甲片3(名称)
		ieaa_sAttr4		varchar(48) default null, #护甲片4(名称)
		
    primary key (is_uId),
    foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine = innodb;