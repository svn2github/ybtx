#GM工具任务完成限制表
create table tbl_quest_finish_limit(
	qfl_sQuestName 	varchar(96)	not null,
	primary key 	(`qfl_sQuestName`)
)engine = InnoDb;

#GM工具OBJ掉落限制表
create table tbl_obj_drop_limit(
	odl_sObjName 	varchar(96)	not null,
	primary key 	(`odl_sObjName`)
)engine = InnoDb;

delete from tbl_keymap;


alter table tbl_action_team_member add column atm_uState tinyint unsigned 	not null default 0 after at_uId;

#玩家金钱表
create table tbl_char_money
(
	cs_uId			bigint unsigned		not	null,
	cm_uMoney		bigint unsigned	not null  default 0,		#role money 
	cm_uBindingMoney	bigint unsigned	not null	default 0,	#role binding money
	cm_uTicket		bigint unsigned	not null	default 0,	#role tickets
	cm_uBindingTicket	bigint unsigned	not null 	default 0,	#role binding tickets
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

insert into tbl_char_money select cs_uId,cb_uMoney,cb_uBindingMoney,cb_uTicket,cb_uBindingTicket from tbl_char_basic;

##小队组队器
create table tbl_team_list (			#list of players apply for a team 
		cs_uId			bigint unsigned		not	null,		#role identifier
		tl_nType    tinyint unsigned	not	null,
		tl_sMsg      char(128) collate utf8_unicode_ci not null,
		tl_dtTime   datetime,
		#tl_dtUGTime   datetime,
		#tl_dtActTime   datetime,
		#tl_dtTaskTime   datetime,
		primary key(cs_uId,tl_nType),
		foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade		
)engine=innodb;

