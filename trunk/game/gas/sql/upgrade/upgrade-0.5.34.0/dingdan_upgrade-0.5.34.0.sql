#点券基本信息表
create table tbl_coupons_info(
    ci_uId          bigint  unsigned		not null,
    ci_sName        varchar(96)  collate utf8_unicode_ci           not null,
    ci_uSmallIcon   int     unsigned        not null,
    ci_uPrice       int(10) unsigned        not null,
    ci_sDesc        text             collate utf8_unicode_ci       not null,
    ci_sUrl         text            collate utf8_unicode_ci        not null,
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

insert into tbl_conf_server(sc_sVarName, sc_sVarValue) 
values ("BuyCouponsWebIP", "172.16.4.42"), ("BuyCouponsWebPort", "80");

alter table tbl_coupons_info change  ci_sDesc  ci_sDesc varchar(1024) collate utf8_unicode_ci not null;
alter table tbl_coupons_info change  ci_sUrl  ci_sUrl varchar(1024)  collate utf8_unicode_ci not null;

create table tbl_tong_tech_info_static(
    ttis_uId        tinyint unsigned                not null,
    ttis_uType      tinyint unsigned                not null,
    ttis_sName      varchar(96)                not null,
    
    primary key(ttis_uId)
)engine = innodb;

create table tbl_tong_char_tech(
    cs_uId          bigint unsigned                 not null,
    ttis_uId        tinyint unsigned                not null,
    tct_uLevel     tinyint unsigned                not null, 
  	unique key(cs_uId, ttis_uId),
  	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 		on	update cascade on	delete cascade,
  	foreign	key(ttis_uId)	 references tbl_tong_tech_info_static(ttis_uId) 		on	update cascade on	delete cascade
)engine = innodb;


create table temp_tong_tech(
    techID      bigint  unsigned    not null,
    techIndex   tinyint unsigned    not null
)engine = innodb;

create table temp_tong_tech_static			
( 
	tts_uId           	bigint unsigned     	not null,
	t_uId		 		bigint unsigned     	not null,
    tts_sName           varchar(96)         	not null,
    tts_uType         	tinyint unsigned        not null,
    tts_uLevel        	tinyint unsigned        not null
)engine=innodb;

delete from tbl_tong_tech_order;
insert into tbl_tong_tech_info_static (ttis_uId, ttis_uType, ttis_sName)  values 
(1, 1, "命中提升"),
(2, 1, "韧性提升"),
(3, 1, "生命提升"),
(4, 1, "免爆提升");

alter table tbl_tong_tech_static drop foreign key tbl_tong_tech_static_ibfk_1;
alter table tbl_tong_tech_static drop index  `t_uId`;

update tbl_tong_tech_static set tts_sName = "命中提升" where tts_sName = "物理提升";
update tbl_tong_tech_static set tts_sName = "命中提升" where tts_sName = "法术提升";
update tbl_tong_tech_static set tts_sName = "韧性提升" where tts_sName = "防御提升";
update tbl_tong_tech_static set tts_sName = "生命提升" where tts_sName = "魔法提升";

insert into temp_tong_tech_static (tts_uId, t_uId, tts_sName, tts_uType, tts_uLevel ) 
select tts_uId, t_uId, tts_sName, tts_uType, sum(tts_uLevel) from tbl_tong_tech_static group by t_uId, tts_sName ;

delete from tbl_tong_tech_static;

insert into tbl_tong_tech_static (tts_uId, t_uId, tts_sName, tts_uType, tts_uLevel )
select tts_uId, t_uId, tts_sName, tts_uType, tts_uLevel from temp_tong_tech_static  ;

insert into temp_tong_tech
(select s.tts_uId, i.ttis_uId from tbl_tong_tech_static s, tbl_tong_tech_info_static i 
where tts_sName = ttis_sName and  tts_uType = ttis_uType);

alter table tbl_tong_tech_static drop column tts_sName;
alter table tbl_tong_tech_static drop column tts_uType;
alter table tbl_tong_tech_static add column ttis_uId tinyint unsigned not null  after t_uId;

update tbl_tong_tech_static, temp_tong_tech set ttis_uId = techIndex where techID = tts_uId;

alter table tbl_tong_tech_static add unique key(t_uId, ttis_uId);
alter table tbl_tong_tech_static add foreign key(ttis_uId) 
references tbl_tong_tech_info_static(ttis_uId) on update cascade on delete cascade;
alter table tbl_tong_tech_static add foreign key(t_uId) 
references tbl_tong(t_uId) on update cascade on delete cascade;

drop table if exists temp_tong_tech;
drop table if exists temp_tong_tech_static;

