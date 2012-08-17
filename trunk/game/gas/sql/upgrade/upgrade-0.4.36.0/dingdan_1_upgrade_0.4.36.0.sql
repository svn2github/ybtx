#####  order
drop table if exists tbl_contract_manufacture_order_item;
drop table if exists tbl_contract_manufacture_cfg_info;
drop table if exists tbl_contract_manufacture_order;
drop table if exists tbl_contract_manufacture_order_static;
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

insert into tbl_database_upgrade_record values("dingdan_1_upgrade_0.4.36.0.sql");
