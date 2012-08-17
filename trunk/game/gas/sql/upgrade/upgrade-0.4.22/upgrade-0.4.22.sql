alter table tbl_member_tong change mt_uProffer mt_uProffer bigint unsigned  not null;

create table tbl_toolsmall_buy_detail
(
	tbd_uDetailID					bigint unsigned not null 	auto_increment,	#detail id
	tbd_sParamInfo				varchar(300) 	not null,				#parameter info
	tbd_dtBuyTime 				datetime 			not null,				#buy time
	tbd_uState						tinyint unsigned not null,    #detail state ,0--send succ,1--send failed
	primary key(tbd_uDetailID)
)engine=innodb;

alter table tbl_tong drop column t_uExploit;
##功勋表
create table tbl_tong_exploit		#tong member exploit
(       
  cs_uId							bigint unsigned		not null,                #role identifier
	te_uExploit					bigint unsigned		not null,									#exploit
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;


##国战击杀(时间)信息表
create table tbl_war_tong			#tong trunk
(       
	cs_uId		bigint unsigned 	not null,			#role
	wt_uId		bigint unsigned 	not null,			#role
	wt_uTime	bigint unsigned   not null,		#kill time
	primary key(cs_uId, wt_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;


alter table tbl_keymap_mouse add column ckm_bShowMsgBox tinyint unsigned not null default 1 after ckm_tabDistance;
alter table tbl_keymap_mouse change ckm_tabDistance ckm_tabDistance tinyint unsigned	not null default 10;

#活动房间
create table tbl_action_match_list
(
	cs_uId	bigint unsigned		not	null,		#角色id
	aml_sActionName	varchar(32) 		not null,  	#报名的活动名
	aml_uLvStep	tinyint unsigned  	not null,	#等级段
	unique key(cs_uId, aml_sActionName),
	key(aml_sActionName, aml_uLvStep),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

create table tbl_tong_get_proffer_time
(
	cs_uId			bigint unsigned 	not null,			#role
	tgpt_sTime  varchar(96)  not null,                 
	primary	key(cs_uId,tgpt_sTime),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#热更新的lua代码记录
create table tbl_hot_update_code(
	huc_id		bigint		unsigned	auto_increment	not null,
	huc_version	int(10)	unsigned	not null,
	huc_server_id	varchar(32)	not null,
	huc_type	tinyint	unsigned	not null,
	huc_code	text,
	primary key(huc_id)	
)engine=InnoDb;	

alter table tbl_char_basic drop column cb_uMoney;
alter table tbl_char_basic drop column cb_uBindingMoney;
alter table tbl_char_basic drop column cb_uTicket;
alter table tbl_char_basic drop column cb_uBindingTicket;

drop table tbl_sort_jifensai_point;
drop table tbl_sort_jifensai_point_by_camp;
drop table tbl_item_equip_armor_attr;

alter table tbl_char_money drop column cm_uTicket;

create table tbl_user_charge_detail
(
	us_uID						int unsigned not null,		#user id
	ucd_uDetailID			bigint unsigned not null,		#detail id
	ucd_uSubjectID		tinyint unsigned not null,	#subject id
	ucd_uMoney				bigint unsigned not null,		#moneynum
	ucd_dtTime 				timestamp 	not null,				#chargetime
	unique key(us_uID,ucd_uSubjectID,ucd_uDetailID),
	foreign	key(us_uID)	references tbl_user_static(us_uId) on	update cascade on delete cascade
)engine=innodb;
alter table tbl_toolsmall_buy_detail drop column tbd_dtBuyTime;
alter table tbl_toolsmall_buy_detail drop column tbd_uState;
alter table tbl_toolsmall_buy_detail add column tbd_uSubjectID tinyint unsigned not null;

##团贡表
create table tbl_tong_proffer		#tong member exploit
(       
  cs_uId							bigint unsigned		not null,                #role identifier
	tp_uProffer					bigint unsigned		not null,									#proffer
	primary key(cs_uId),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;
alter table tbl_tong_exploit add primary key(cs_uId);
alter table tbl_member_tong drop column mt_uProffer;

alter table tbl_sort_jifensai_all_wintimes drop foreign key tbl_sort_jifensai_all_wintimes_ibfk_1;
alter table tbl_sort_jifensai_all_wintimes  drop primary key;
alter table tbl_sort_jifensai_all_wintimes drop column sjaw_uType;
alter table tbl_sort_jifensai_all_wintimes add foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade ;

alter table tbl_sort_jifensai_all_wintimes_by_camp drop foreign key tbl_sort_jifensai_all_wintimes_by_camp_ibfk_1;
alter table tbl_sort_jifensai_all_wintimes_by_camp drop primary key;
alter table tbl_sort_jifensai_all_wintimes_by_camp drop column sjawbc_uType;
alter table tbl_sort_jifensai_all_wintimes_by_camp add foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade ;
