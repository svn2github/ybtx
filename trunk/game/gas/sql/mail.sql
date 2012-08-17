################################################################
#邮件
################################################################
#邮件表
create table tbl_mail			#mails info
(
	ms_uID			bigint unsigned not null auto_increment,	#mail identifiler
	cs_uSenderID		bigint unsigned ,				#sender identifiler, gm mail when null
	cs_sSender		varchar(96) collate utf8_unicode_ci not null,	#sender name
	cs_uReciever		bigint unsigned not null,			#reciever identifier
	m_sTitle		varchar(384) not null,				#mail title
	m_sContent		varchar(1024)		not null,			#mail content
	m_state			tinyint unsigned 	not null,		#mail state
	m_uMoney		bigint unsigned 	not null,		#attached money
	m_tUpdateTime		datetime		not null,		#time of last changing state
	m_tCreateTime		datetime		not null,		#time of creation
	m_nMailType		tinyint unsigned 	not null,		#mail type 1: common mail  2: sys mail   3: outdate posted mail
	m_uLogMailType bigint unsigned 	not null,
	m_nStrType  tinyint unsigned not null default 0, #客户端邮件字符串读取方式，以后会删掉的，暂时用着
	primary key(ms_uID),
	foreign	key(cs_uSenderID)	references tbl_char_static(cs_uId) on update cascade on delete cascade,
	foreign	key(cs_uReciever)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


#邮件包裹表
create table tbl_mail_item		#items contented in mails
(
	ms_uID			bigint unsigned		not null,		#mail identifier
	is_uId			bigint unsigned		not null,		#item identifier
	mi_uSlotIndex		tinyint unsigned	not null,		#slot index
	unique key(is_uId),
	key(ms_uID,mi_uSlotIndex),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on update cascade on delete cascade,
	foreign	key(ms_uID)	references tbl_mail(ms_uID) on delete cascade on update cascade
)engine=innodb;


#邮件文本附件
create table tbl_item_mail_letter	#mail content, treated as item when get from mail
(
	is_uId			bigint unsigned not null,			#item identifier
	iml_uMailContext	varchar(1024)	not null, 				#mail content
	cs_uSenderName		varchar(96) 	not null, 			#sender name
	cs_uRecieverName	varchar(96) 	not null,			#reciever name
	iml_sTitle		varchar(384) 	not null,			#mail title
	iml_tCreateTime		datetime		not null,		#item create time
	iml_nType		tinyint			not null,		#item type     0: from common mails; 1: from system mails;
	ms_uID			bigint unsigned		not null,		#mail identifier
	iml_nStrType tinyint unsigned not null default 0,#客户端邮件字符串读取方式，以后会删掉的，暂时用着
	primary key(is_uId),
	unique key(ms_uID),
	foreign key(is_uId) references tbl_item_static(is_uId) on delete cascade on update cascade
)engine=innodb;
