
##幻兽蛋信息表
#table of eudemous eggs
create table tbl_pet_egg	
(        
	cs_uId	bigint unsigned	not  null,				#owner id
	pe_uIncubateTimes tinyint unsigned  not null,	#incubated times
	pe_sEggName varchar(96)	not	null,				#egg's name
  pe_uAptitudeValue int not null,					#aptitude value
	pe_uTotalIncubateTimes tinyint unsigned not null,	#total needed incubate times
  pe_dtIncubateDate datetime,						#incubated time
    
	primary	key(cs_uId),
  foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade
)engine=innodb;

##幻兽静态表
create table tbl_pet_static		#eudemous static info
(        
	ps_uId	bigint unsigned	not  null auto_increment,		#eudemous id
	ps_sPetName varchar(96)	not	null,						#eudemous's name
  ps_uAptitudeValue int unsigned not null,			#aptitude value
  ps_sPetType varchar(96)	not	null,					#attack type(magical or physical)
    
	primary	key(ps_uId)
)engine=innodb;

##幻兽信息表
#table of pets that belong to roles
create table tbl_char_pet
(   
	cs_uId	bigint unsigned	not  null,				#role identifier
	ps_uId	bigint unsigned	not  null,				#eudemous identifier
	cp_sPetName varchar(96) collate utf8_unicode_ci,						#eudemous's name renamed by role
  cp_uPetLevel tinyint unsigned not null,			# eudemous level
  cp_dtHoldTime datetime not null,				#hold time
  cp_uHealthPoint int unsigned not null,			#health point
  cp_uDamageValue int unsigned not null,			#damage value
  cp_uExpValue	int unsigned not null,			#experience number
  cp_uVimValue int unsigned not null,				#active value
    
    
	primary	key(ps_uId),
	foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade,
	foreign	key (ps_uId) references tbl_pet_static(ps_uId) on	update cascade on delete cascade
)engine=innodb;

##幻兽技能表
#eudemous' skill
create table tbl_pet_skill
(   
	cs_uId 			bigint unsigned not null,	#owner of eudemous
	ps_uId	bigint unsigned	not  null,		#eudemous id
	ps_sSkillName varchar(96) not  null,						#skill of eudemous
  ps_uIndex	tinyint unsigned not  null, 		#hot key slot of the skill
    
	primary	key(cs_uId,ps_uId,ps_sSkillName,ps_uIndex),
	foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade,
	foreign	key (ps_uId) references tbl_pet_static(ps_uId) on update cascade on delete cascade
)engine=innodb;

##准备状态下的幻兽
#eudemous of prepared status
create table tbl_pet_prepare		
(   
	cs_uId 			bigint unsigned not null,	#owner ID of eudemous
	ps_uId	bigint unsigned	not  null,		#eudemous id
    
	primary	key(cs_uId),
	foreign	key (cs_uId) references tbl_char_static(cs_uId) on	update cascade,
	foreign	key (ps_uId) references tbl_pet_static(ps_uId) on update cascade on delete cascade
)engine=innodb;




