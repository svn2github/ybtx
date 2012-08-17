##################################################################################
##                                                                               #
##                                 ==**ÉÌ»áÏà¹Ø**==                              #
##                                                                               #
##################################################################################
###ÉÌ»áĞÅÏ¢±í
#create table tbl_cofc
#(
#	c_uId				bigint unsigned		not null auto_increment,	##ÉÌ»áid
#	c_sName				varchar(100) collate utf8_unicode_ci not null,					##ÉÌ»áÃû×Ö
#	c_dtCreateTime		datetime			not null,					##ÉÌ»á´´½¨Ê±¼ä
#	c_uLevel			int unsigned		not null default 1,			##ÉÌ»áµÈ¼¶
#	c_uMoney			bigint unsigned		not null default 0,			##ÉÌ»á×Ê½ğ
#	c_sPurpose			varchar(600)		not null default '',		##ÉÌ»á×ÚÖ¼
#	c_nStockSum			int unsigned		not null default 0,			##¸ÃÉÌ»áµÄ¹ÉÆ±×ÜÊı
#	tc_nTechId      int unsigned    not null  default 0,    #¿Æ¼¼´úºÅ(µ±Ç°¿ÆÑĞÏîÄ¿)£»Îª0Ê±Ã»ÓĞ¿ÆÑĞÏîÄ¿
#	primary key(c_uId),
#	unique key(c_sName)
#)engine=innodb;
#
###ÉÌ»áÈËÆø±í£¬ÒÔÈÕÎª·ÖÀà´æÖü
#create table tbl_day_popular_cofc
#(
#	c_uId				bigint unsigned		not null,					##ÉÌ»áid
#	dpc_uYear			smallint unsigned	not null,					##Äê
#	dpc_uDayOfYear		smallint unsigned	not null,					##¸ÃÄêÖĞµÄÌìÊı£¬Í¨¹ıdayofyear()»ñÈ¡
#	dpc_uWeekOfYear		smallint unsigned	not null,					##¸ÃÈÕÔÚ¸ÃÄêÖĞµÄĞÇÆÚÊı£¬Í¨¹ıweekofyear()»ñÈ¡
#	dpc_uPopular		bigint				not null default 0,			##ÉÌ»áµÄÈËÆøÖµ
#	primary key(c_uId, dpc_uYear, dpc_uDayOfYear),
#	foreign	key(c_uId)		references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
#
###ÉÌ»áÈËÔ±±í
#create table tbl_member_cofc
#( 
#	cs_uId				bigint unsigned		not null,					##³ÉÔ±µÄ½ÇÉ«id
#	c_uId				bigint unsigned		not null,					##ÉÌ»áid
#	mc_uPosition		varchar(96)			not null,					##ÔÚÉÌ»áÖĞµÄÖ°Î»
#	mc_uProffer			tinyint unsigned	not null default 0,			##°ï¹±
#	mc_dtJoinTime		datetime			not null,					##¼ÓÈëÊ±¼ä
#	primary key(cs_uId),
#	foreign	key (cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade,
#	foreign	key(c_uId)		references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
#
###ÉÌ»áÈÕÖ¾ĞÅÏ¢±í
#create table tbl_log_cofc
#( 
#	lc_uId				bigint unsigned		not null auto_increment,	##ÈÕÖ¾id
#	c_uId				bigint unsigned		not null,					##ÉÌ»áid
#	lc_sContent			varchar(300)		not null,					##ÈÕÖ¾ÄÚÈİ
#	lc_Type				tinyint unsigned	not null,					##ÈÕÖ¾Àà±ğ
#	lc_dtCreateTime		datetime			not null,					##Ê±¼ä
#	primary key(lc_uId),
#	foreign	key(c_uId)	references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
#
###ÉêÇëĞÅÏ¢±í
#create table tbl_request_cofc
#(
#	cs_uId				bigint unsigned		not null,					##ÉêÇëÕßid
#	c_uId				bigint unsigned		not null,					##ÉÌ»áid
#	rc_uRecomId		bigint unsigned  not null,				##Òı¼öÕßid
#	rc_dtRequestTime	datetime			not null,					##·¢³öÉêÇëµÄÊ±¼ä
#	rc_sExtraInfo		varchar(300)		not null,					##¸½¼ÓµÄÇëÇóĞÅÏ¢
#	primary key(cs_uId,c_uId),
#	foreign	key(cs_uId)			references tbl_char_static(cs_uId) on update cascade on delete cascade,
#	foreign	key(rc_uRecomId)	references tbl_char_static(cs_uId) on update cascade on delete cascade,
#	foreign	key(c_uId)			references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
#
#
###³ÉÔ±ÍË³öÉÌ»áĞÅÏ¢±í
#create table tbl_leave_cofc
#(
#	cs_uId				bigint unsigned		not null,					##ÍË³öÕßid
#	lc_dtQuitTime		datetime			not null,					##ÍË³öÉÌ»áÊ±¼ä
#	foreign	key (cs_uId)		references tbl_char_static(cs_uId) on update cascade on delete cascade
#)engine=innodb;
#
##################################################################################
##                                    ¹ÉÆ±Ïà¹Ø                                   #
##################################################################################
###¹ÉÆ±³ÖÓĞ±í
#create table tbl_stock_have_cofc
#(
#	c_uId				bigint unsigned		not null,					##¹ÉÆ±µÄ´úÂë£¨ÉÌ»áµÄid£©
#	cs_uId				bigint unsigned		not null default 0,			##¹ÉÆ±ËùÓĞÕßid
#	
#	shc_uId				bigint unsigned		not null auto_increment,	##¼ÇÂ¼µÄÁ÷Ë®ºÅ
#	shc_uNumber			int unsigned		not null,					##³ÖÓĞµÄ¹ÉÆ±ÊıÁ¿
#	shc_uPrice			bigint unsigned		not null default 0,			##¹ºÈë¼Û¸ñµÄ100±¶É
#	shc_dtTradeTime		datetime			not null,					##¹ºÈëÈÕÆÚ
#	
#	primary key(shc_uId),
#	foreign key(c_uId)			references tbl_cofc(c_uId) on update cascade on delete cascade,
#	foreign key(cs_uId)			references tbl_char_static(cs_uId) on update cascade on delete cascade
#)engine=innodb;
#
###¹ÉÆ±½»Ò×ÀúÊ·±í
#create table tbl_stock_exchange_cofc
#(
#	c_uId				bigint unsigned		not null,					##¹ÉÆ±µÄ´úÂë£¨ÉÌ»áµÄid£©
#	sec_uId				bigint unsigned		not null auto_increment,	##¼ÇÂ¼µÄÁ÷Ë®ºÅ
#	
#	sec_uFromId			bigint unsigned		not null,					##¹ÉÆ±Á÷¶¯µÄÆğÊ¼·½id£¬ÈôÎª0±íÊ¾ÉÌ»á±¾Éí
#	sec_uToId			bigint unsigned		not null,					##¹ÉÆ±Á÷¶¯µÄÖÕ½á·½id£¬ÈôÎª0±íÊ¾ÉÌ»á±¾Éí
#	
#	sec_dtTraceTime		datetime			not null,					##½»Ò×Ê±¼ä´Á
#	sec_nNumber			bigint unsigned		not null,					##½»Ò×¶î
#	sec_nPrice			bigint unsigned		not null,					##½»Ò×¼Û¸ñµÄ100±¶
#	
#	primary key(sec_uId),
#	foreign key(c_uId)			references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
#
###¹ÉÆ±½»Ò×Í³¼ÆÀúÊ·£¨ÍêÈ«ÊÇ¶Ô±ítbl_stock_exchange_cofc¼ä¸ôÊ±¼äµÄÍ³¼ÆÖµ£©
#create table tbl_stock_exchange_statistics_cofc
#(
#	c_uId				bigint unsigned		not null,					##¹ÉÆ±µÄ´úÂë£¨ÉÌ»áµÄid£©
#	sesc_uId			bigint unsigned		not null auto_increment,	##¼ÇÂ¼µÄÁ÷Ë®ºÅ
#	sesc_dtEndTime		datetime			not null,					##Í³¼ÆÊ±¼ä¶ÎµÄÖÕµãÊ±¼ä
#	
#	sesc_nNumber		bigint unsigned		not null default 0,			##×Ü½»Ò×Á¿
#	sesc_nSumPrice		bigint unsigned		not null default 0,			##×Ü½»Ò×½ğ¶îµÄ100±¶£¨Ê±¼ä¶ÎÄÚ ½»Ò×Á¿*¼Û¸ñ³É¼¨ µÄ¼ÓºÏ£©
#	sesc_nDeltaType		int unsigned		not null,					##Ê±¼ä¼ä¸ôÀàĞÍ¡£15Îª15·ÖÖÓ£¬120Îª2h£¬480Îª8h
#	
#	primary key(sesc_uId),
#	foreign key(c_uId)			references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
#
###¹ÉÆ±¶©µ¥±í
#create table tbl_stock_order_cofc
#(
#	c_uId				bigint unsigned		not null,					##¹ÉÆ±µÄ´úÂë£¨ÉÌ»áµÄid£©
#	cs_uId				bigint unsigned,								##ÏÂµ¥ÓÃ»§id
#	soc_uId				bigint unsigned		not null auto_increment,	##¶©µ¥Á÷Ë®ºÅ
#	soc_uType			tinyint unsigned	not null,					##¶©µ¥ÀàĞÍ£¬0ÎªÂòµ¥£¬1ÎªÂôµ¥
#	soc_dtCreateTime	datetime			not null,					##¶©µ¥Éú³ÉÊ±¼ä
#	soc_uPrice			bigint unsigned		not null,					##¹Òµ¥µÄ¼Û¸ñ(ÏëÒªÂò»òÕßÂôµÄµ¥Ö§¹ÉµÄ¼Û¸ñ)
#	soc_uCostPrice  bigint unsigned		not null default 0,	##¹ÒÂôµ¥Ö®Ç°µÄ³É±¾¼Û¸ñ(Âòµ¥µÄ³É±¾¼Û¸ñÊÇ0)
#	soc_uNumber			bigint unsigned		not null,					##Ô¤ÆÚ½»Ò×Á¿
#	
#	primary key(soc_uId),
#	foreign key(c_uId)			references tbl_cofc(c_uId) on update cascade on delete cascade,
#	foreign key(cs_uId)			references tbl_char_static(cs_uId) on update cascade on delete cascade
#)engine=innodb;
#
###²Æ±¨±í
#create table tbl_stock_report_cofc
#(
#	c_uId				bigint unsigned 	not null,					##¹ÉÆ±µÄ´úÂë£¨ÉÌ»áµÄid£©
#	
#	src_uId				bigint unsigned		not null auto_increment,	##²Æ±¨Á÷Ë®ºÅ
#	src_dtEndTime		datetime			not null,					##½áËãÊ±¼ä
#	
#	src_uLevel			int unsigned		not null,					##ÉÌ»áµÈ¼¶
#	src_uMemberNum		int unsigned		not null,					##ÉÌ»á³ÉÔ±Êı
#	src_uMoneyAll		bigint unsigned		not null,					##ÉÌ»á×Ü×Ê½ğ
#	src_uMoneyIncome	bigint unsigned		not null,					##ÆÚ¼äÊÕÈë
#	src_uExchangeNum	bigint unsigned		not null,					##ÆÚ¼ä×Ü½»Ò×Á¿
#	src_uBonusAll		bigint unsigned		not null,					##·Ö³öµÄ×ÜºìÀû
#	src_uStockNum		int					not null,					##ÊÛ³öµÄ×Ü¹ÉÊı
#	src_uActivePoint	bigint unsigned		not null,					##»îÔ¾¶È
#	
#	primary key(src_uId),
#	foreign key(c_uId)			references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
#
###²Æ±¨´ó¹É¶«±í
#create table tbl_stock_report_shareholder_cofc
#(
#	src_uId				bigint unsigned		not null,					##²Æ±¨±íµÄÁ÷Ë®ºÅ
#	cs_uId				bigint unsigned		not null,					##¹ÉÆ±ËùÓĞÕßid
#	
#	srsc_uNumber		bigint unsigned		not null,					##³ÖÓĞÁ¿
#	
#	primary key(src_uId, cs_uId),
#	foreign key(src_uId)	references tbl_stock_report_cofc(src_uId)	on update cascade on delete cascade,
#	foreign key(cs_uId)		references tbl_char_static(cs_uId)			on update cascade on delete cascade
#)engine=innodb;
##################################################################################
##                                    ÉÌ»áÔËÊä³µ                             #
##################################################################################
##
#create table tbl_cofc_truck
#(
#	ct_uId				bigint unsigned		not null auto_increment,	##ÔËÊä³µµÄÁ÷Ë®ºÅ
#	ct_uCarTyp smallint unsigned	not null,			##ÔËÊä³µµÄĞÍºÅ 1-´óĞÍ³µ £»2 -ÖĞĞÍ³µ£»3 - Ğ¡ĞÍ³µ
#	ct_uCapacity	bigint unsigned		not null,		##ÔËÊä³µµÄµ±Ç°ÈİÁ¿
#	ct_uHP			bigint unsigned		not null,			##ÔËÊä³µµÄµ±Ç°ÉúÃüÖµ
#	cs_uId				bigint unsigned		default null,		##ÔËÊä³µµÄ³ÖÓĞÕßid
#	c_uId				bigint unsigned 	default null,			##ÔËÊä³µµÄ³ÖÓĞÉÌ»á´úÂë
#	
#	primary key(ct_uId),
#	foreign key(cs_uId)	references tbl_char_static(cs_uId)	on update cascade on delete cascade,
#	foreign key(c_uId)	references tbl_cofc(c_uId)				on update cascade on delete cascade
#)engine = innodb;
##################################################################################
##                                    ÉÌ»á¿Æ¼¼                                  #
##################################################################################
#create table tbl_technology_cofc
#(       
#	c_uId		 					bigint unsigned     	not null,										#ÉÌ»áid
#	tc_nTechId        int unsigned        	not null,                		#¿Æ¼¼´úºÅ
#	tc_uPoint   			bigint unsigned     	not null   default 0,      	#µ±Ç°µãÊı£¨Íê³É¶È£©
#	tc_uLevel        	int unsigned        	not null   default 0,      	#µ±Ç°µÈ¼¶
#	primary	key(c_uId,tc_nTechId),
#	foreign key(c_uId)			references tbl_cofc(c_uId) on update cascade on delete cascade
#)engine=innodb;
