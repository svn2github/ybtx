drop table tbl_market_buy_order;

create table tbl_market_buy_order(      #acquisition orders
  mos_uId       bigint unsigned   not null,   # order identifier
  cs_uId        bigint unsigned   not null,   # role referred by order
  mbo_sItemName   varchar(96)     not null,   # acquired item name
  mbo_uCount      bigint unsigned   not null,   # acquired item number
  mbo_uPrice      bigint unsigned   not null,   # item price
  mbo_tEndTime    datetime      not null,   # end time
  primary key (mos_uId),
  key (mbo_sItemName),
  key (mbo_tEndTime), 
  foreign key (mos_uId) references tbl_market_order_static(mos_uId) on  update cascade on delete cascade,
  foreign key (cs_uId)  references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_market_purchasing_cfg_info(
    mpci_sName            varchar(96)             not null,   #物品名称
    mpci_uType            tinyint unsigned            not null,   #物品类型
  mpci_uChildType     tinyint unsigned            default 0,
  mpci_uCanUseLevel bigint  unsigned    not null,   #物品可使用等级
    mpci_uQuality       tinyint unsigned    default 0,
  mpci_sItemDisplayName varchar(96) binary collate utf8_unicode_ci    not null,   #物品显示名称
    key(mpci_uType),
  key(mpci_uChildType),
  key(mpci_uCanUseLevel),
  key(mpci_uQuality),
  key(mpci_sItemDisplayName),
    primary key(mpci_sName)
)ENGINE = MEMORY;

alter table tbl_market_sell_order modify  mso_uCanUseLevel bigint unsigned      not null;