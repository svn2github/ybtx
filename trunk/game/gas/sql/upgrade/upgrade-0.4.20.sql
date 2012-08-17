

##装备护甲属性
create table tbl_item_equip_armor (
		is_uId          bigint unsigned not null,	#物品ID
		iea_sAttr		varchar(48) not null, #护甲片(名称)
		iea_uAddTimes tinyint	unsigned   not null,
		iea_uIndex tinyint	unsigned   not null,
		
		primary key (is_uId,iea_uIndex),
    foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine = innodb;


insert into tbl_item_equip_armor select is_uId,ieaa_sAttr1,1,1 from tbl_item_equip_armor_attr where ieaa_sAttr1 is not null;
insert into tbl_item_equip_armor select is_uId,ieaa_sAttr2,1,2 from tbl_item_equip_armor_attr where ieaa_sAttr2 is not null;
insert into tbl_item_equip_armor select is_uId,ieaa_sAttr3,1,3 from tbl_item_equip_armor_attr where ieaa_sAttr3 is not null;
insert into tbl_item_equip_armor select is_uId,ieaa_sAttr4,1,4 from tbl_item_equip_armor_attr where ieaa_sAttr4 is not null;

create table tbl_gm_msg				
(
	gm_uSenderId bigint unsigned not null,			#sender identifier
	gm_uRecieverId bigint unsigned 	not null,		#reciever identifier
	gm_dtCreateTime datetime	 not null,		#msg send time
	gm_sContent  varchar(3072)	 not null,		#msg content
	
	key(gm_uSenderId),
	key(gm_uRecieverId)
)ENGINE=InnoDB default CHARSET=latin1;

alter table tbl_gm_msg add column gm_uReadFlag tinyint unsigned 	not null after gm_sContent;

#道具商城商品类型静态表
create table tbl_toolsmall_tabtype_memory
(
	ttm_uTabType 	tinyint unsigned			not	null,
	is_uType			tinyint unsigned			not	null,		#item type
	is_sName			varchar(96)						not	null,		#item name
	
	unique key(ttm_uTabType,is_uType,is_sName)
)engine=MEMORY;


