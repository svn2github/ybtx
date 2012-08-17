drop table tbl_technology_tong;
create table tbl_tong_tech_static
( 
	tts_uId           	bigint unsigned     	not null auto_increment,  	#科技id
	t_uId		 						bigint unsigned     	not null,										#帮会id
  tts_sName         	varchar(96)         	not null ,                 	#科技名称
  tts_uType         	tinyint unsigned      not null,                  	#类型，建筑科技、物品科技、战斗科技
  tts_uLevel        	tinyint unsigned      not null,      							#当前等级
	primary key(tts_uId),
	unique key(t_uId,tts_sName,tts_uType),
  foreign	key(t_uId)	        references tbl_tong(t_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##科技研发等待序列
create table tbl_tong_tech_order
(       
  tts_uId         	bigint unsigned       not null,                 	#科技id
  tto_dtStartTime   DateTime            	not null,       						#开始时间
  tto_uState        tinyint unsigned      not null,      							#状态
  primary key(tts_uId),
  foreign	key(tts_uId)	        references tbl_tong_tech_static(tts_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##幻兽蛋信息表
create table tbl_pet_egg
(        
	cs_uId	bigint unsigned	not  null,				#角色id
	pe_uIncubateTimes tinyint unsigned  not null,	#已经孵化的次数
	pe_sEggName varchar(96)	not	null,				#幻兽蛋名称
  pe_uAptitudeValue int not null,					#资质
	pe_uTotalIncubateTimes tinyint unsigned not null,#需要孵化的次数
  pe_dtIncubateDate datetime,						#孵化的时间
    
	primary	key(cs_uId),
  foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade
)engine=innodb;

##幻兽静态表
create table tbl_pet_static
(        
	ps_uId	bigint unsigned	not  null auto_increment,	#幻兽id
	ps_sPetName varchar(96)	not	null,					#幻兽名称(配置表里面的名称)
  ps_uAptitudeValue int unsigned not null,			#幻兽资质
  ps_sPetType varchar(96)	not	null,					#幻兽攻击类型
    
	primary	key(ps_uId)
)engine=innodb;

##幻兽信息表
create table tbl_char_pet
(   
	cs_uId	bigint unsigned	not  null,				#角色id
	ps_uId	bigint unsigned	not  null,				#幻兽id
	cp_sPetName varchar(96),						#幻兽名称(幻兽重命名后的名字)
  cp_uPetLevel tinyint unsigned not null,			#幻兽等级
  cp_dtHoldTime datetime not null,				#获得时间
  cp_uHealthPoint int unsigned not null,			#生命值
  cp_uDamageValue int unsigned not null,			#法力值
  cp_uExpValue	int unsigned not null,			#幻兽经验
  cp_uVimValue int unsigned not null,				#活力值
    
    
	primary	key(ps_uId),
	foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade,
	foreign	key (ps_uId) references tbl_pet_static(ps_uId) on	update cascade on delete cascade
)engine=innodb;

##幻兽技能表
create table tbl_pet_skill
(   
	cs_uId 			bigint unsigned not null,	#玩家角色ID
	ps_uId	bigint unsigned	not  null,		#幻兽id
	ps_sSkillName varchar(96),						#幻兽学会的技能名称
  ps_uIndex	tinyint unsigned, 		#技能格子位置
    
	primary	key(cs_uId,ps_uId,ps_sSkillName,ps_uIndex),
	foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade,
	foreign	key (ps_uId) references tbl_pet_static(ps_uId) on update cascade on delete cascade
)engine=innodb;

##准备状态下的幻兽
create table tbl_pet_prepare
(   
	cs_uId 			bigint unsigned not null,	#玩家角色ID
	ps_uId	bigint unsigned	not  null,		#幻兽id
    
	primary	key(cs_uId),
	foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade,
	foreign	key (ps_uId) references tbl_pet_static(ps_uId) on update cascade on delete cascade
)engine=innodb;

alter table tbl_log_skill add column ls_uIsNonSkill tinyint unsigned not null;

##物品整理小类、等级排列顺序表
create table tbl_item_smalltype_order
(
	is_uType						tinyint unsigned				not	null,		#物品大类
	is_sName						varchar(96)							not	null,		#物品名称
	iso_uSmallType			tinyint unsigned				not	null,		#小类类型
	iso_uBaseLevel			smallint unsigned				not	null,		#等级
	unique key(is_uType,is_sName)
)engine=MEMORY;


