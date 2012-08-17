################################################################
# 社群系统
###############################[好友]#################################

#玩家好友组的基本信息
create table tbl_friends_class		#role friends group
(
	fc_uId bigint unsigned not null auto_increment,		#group id
	fc_sName varchar(96) collate utf8_unicode_ci not null,	#group name
	cs_uId bigint unsigned not null,			#group creator
	
	primary key (fc_uId,cs_uId),
	foreign key (cs_uId) references tbl_char_static (cs_uId) on update cascade on delete cascade
)engine=innodb;

#玩家好友
create table tbl_player_friends		#role friends
(
	cs_uId bigint unsigned not null,		#role identifier
	pf_uFriendId bigint unsigned not null,		#role friend identifier
	fc_uId bigint unsigned not null,		#group id which friend be in
							#	1 stands for default friends group
							#	2 stands for black list
	primary key (cs_uId, pf_uFriendId),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade,
	foreign key (pf_uFriendId) references tbl_char_static (cs_uId) on update cascade on delete cascade,
	foreign key (fc_uId,cs_uId) references tbl_friends_class(fc_uId,cs_uId) on update cascade on delete cascade
)engine=innodb;

#添加好友请求信息
create table tbl_request_addfriend			#role invite friend
(
	ra_uInviter  bigint unsigned not null,		#inviter identifier
	ra_uInvitee  bigint unsigned not null,		#invitee identifier
	fc_uId  bigint unsigned not null ,		#group id in which role be invite to 
	
	primary key (`ra_uInviter`, `ra_uInvitee`),
	foreign key (`ra_uInviter`) references `tbl_char_static` (`cs_uId`) on update cascade on delete cascade,
	foreign key (`ra_uInvitee`) references `tbl_char_static` (`cs_uId`) on update cascade on delete cascade,
	foreign key (fc_uId) references tbl_friends_class(fc_uId) on update cascade on delete cascade
)ENGINE=InnoDB default CHARSET=latin1;

#离线消息
create table tbl_offline_msg				#offline msg
(
	om_uID bigint unsigned  not null auto_increment,	#seqence num
	om_uSender bigint unsigned not null,			#sender identifier
	om_uReciever bigint unsigned 	not null,		#reciever identifier
	om_dtCreateTime datetime	 not null,		#msg send time
	om_sContent  varchar(3072)	 not null,		#msg content
	
	primary key(om_uID),
	foreign key (om_uSender) references tbl_char_static(cs_uId) on update cascade on delete cascade,
	foreign key (om_uReciever) references tbl_char_static(cs_uId) on update cascade on delete cascade
)ENGINE=InnoDB default CHARSET=latin1;

#########################################[好友群]###################################################
#好友群
create table tbl_friends_group				#friend circle
(
	fg_uId bigint unsigned not null auto_increment,		#friend circle id
	fg_sName varchar(96) collate utf8_unicode_ci not null,	#friend circle name
	fg_sAnnouncement varchar(765)  not null, 		#friend circle annousement
	fg_sKeyWord varchar(765)  default null, 		#key word
	fg_uGroupKind tinyint unsigned not null,		#circle type
	
	primary key (fg_uId)
)engine=innodb;

#群成员
create table tbl_group_manager				#friend circle manager
(
	fg_uId bigint unsigned not null,		#circle id
	cs_uId bigint unsigned not null,		#role idendifier
	gm_uIdentity tinyint unsigned not null,		#role status of a circle  
							#    1 stands for circle owner
							#    2 stands for circle manager
							#    3 stands for circle member
	gm_uRefuseMsgFlag tinyint unsigned not null default 0, #0-receive 1-refuse
	primary key (fg_uId,cs_uId),
	foreign key (fg_uId) references tbl_friends_group (fg_uId) on update cascade on delete cascade,
	foreign key (cs_uId) references tbl_char_static (cs_uId) on update cascade on delete cascade
)engine=innodb;

#添加好友群请求信息
create table tbl_request_add_group			#role add friends into circle
(
	rag_uManager bigint unsigned not null,		#inviter identifier( power must be no less than manager)
	rag_uInvitee bigint unsigned not null,		#role who would join circle
	fg_uId bigint unsigned not null,		#circle id				
	rag_uType tinyint	unsigned   not null  default 0,   #invite type  
									# 1 stands for active
									# 2 stands for possive
	
	primary key (`rag_uManager`, `rag_uInvitee`,fg_uId,rag_uType),
	foreign key (`rag_uManager`) references `tbl_char_static` (`cs_uId`) on update cascade on delete cascade,
	foreign key (`rag_uInvitee`) references `tbl_char_static` (`cs_uId`) on update cascade on delete cascade,
	foreign key (fg_uId) references tbl_friends_group (fg_uId) on update cascade on delete cascade
)ENGINE=InnoDB default CHARSET=latin1;

#好友群中玩家的个人信息
create table tbl_friends_info					#role desciption in friends a circle
(
	cs_uId bigint unsigned not null,			#role id
	fi_uFellState tinyint unsigned default null,		#role mode
	fi_sFellStatement varchar(96) default null,		#role mode statement
	fi_sHobby varchar(765)  default null,			#role interesting
	fi_uBodyShape tinyint unsigned default null,		#role body type
	fi_uPersonality tinyint unsigned default null,	 	#role personally character
	fi_uMakeFriendState tinyint unsigned default null,	#type which role whole make friend with
	fi_uStyle tinyint unsigned default null,		#role personally character type
	fi_sDetail varchar(765)  default null,			#role personally detailed description
	
	primary key (cs_uId),
	foreign key (cs_uId) references tbl_char_static (cs_uId) on update cascade on delete cascade
)engine=innodb;

#系统消息
create table tbl_system_message				#offline sys message
(
	sm_uId bigint unsigned not null auto_increment,		#sequence num
	sm_uMsgId bigint unsigned not null,			#message identifier
	cs_uId bigint unsigned not null,			#reciever identifier
	
	primary key(sm_uId),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;



##################GM跟玩家聊天专用表###################
create table tbl_gm_msg				
(
	gm_uSenderId bigint unsigned not null,			#sender identifier
	gm_uRecieverId bigint unsigned 	not null,		#reciever identifier
	gm_dtCreateTime datetime	 not null,		#msg send time
	gm_sContent  varchar(3072)	 not null,		#msg content
	gm_uReadFlag tinyint unsigned 	not null ,	#char readflag
	
	key(gm_uSenderId),
	key(gm_uRecieverId)
)ENGINE=InnoDB default CHARSET=latin1;