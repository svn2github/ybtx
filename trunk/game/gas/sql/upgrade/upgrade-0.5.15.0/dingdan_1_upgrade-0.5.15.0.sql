#####×°±¸×·¼Ó
create table tbl_item_equip_superaddation(
    is_uId          bigint unsigned not null,
    ies_uCurSuperaddPhase tinyint unsigned not null,
    ies_uMaxSuperaddPhase tinyint unsigned not null,
    primary key (is_uId),
    foreign key(is_uId) references tbl_item_static(is_uId) on update cascade on delete cascade
)engine = innodb;

alter table tbl_item_equip_superaddation drop column ies_uMaxSuperaddPhase;
