create table tbl_game_setting		#role setting
(
	cs_uId		bigint	unsigned	not	null,			#role identifier
	gs_uTeamSetting tinyint unsigned	not	null,			#whether can be invited to team or not
	gs_uTransSetting tinyint unsigned	not	null,			#whether can be invited to trade or not
	gs_uGroupSetting tinyint unsigned	not	null,			#whether can be invited to friend circle or not
	gs_uFriendSetting tinyint unsigned	not	null,			#whether can be invited as a friend of other##是否允许交友
	gs_uTroopSetting tinyint unsigned	not	null,			#whether can be invited to a mercancy group 
	gs_uAutoMakeTeam tinyint unsigned	not	null,			#whether can be force invited to a team
	gs_uSelectQuestSort tinyint unsigned	not	null,			#sort method of quest
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

######        按键设置        ######
create table tbl_keymap		#key map, 
(
	cs_uId			bigint unsigned 	not null,   	#role identifier
	ckm_sFunctionName	varchar(96)		not null,	#function name
	ckm_sFirstKey		varchar(48) 		not null,	#hot key
	ckm_uKeyPostion		tinyint unsigned 	not null,	#key position, 
	unique key (cs_uId, ckm_sFunctionName, ckm_uKeyPostion),#某功能的某位置只能被定义一次
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

######        鼠标功能设置        ######
create table tbl_keymap_mouse	#key map of mouse	
(
	cs_uId				bigint unsigned 	not null,   		#role identifier
	ckm_lockTarget		tinyint unsigned	not null default 0,	#whether lock target, bool
	ckm_movekey			tinyint unsigned 	not null default 1,	#which key to move, 1: left, 0: right
	ckm_selectLeft		tinyint unsigned	not null default 0,	#1: use left key to choose target
	ckm_selectRight		tinyint unsigned	not null default 1,	#1: use right key to choose target
	ckm_attrackLeft		tinyint unsigned	not null default 1,	#1: Left key to attack
	ckm_attrackRight	tinyint unsigned	not null default 0,	#1: right key to attack
	ckm_tabDistance		tinyint unsigned	not null default 10,#values should be in (1,20), distance that of choose target by tab
	ckm_bShowMsgBox		tinyint unsigned	not null default 1,	#1: show msgbox
	primary key (cs_uId),										#主键, 一个角色不可以
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_ui_setting
(
	us_uId							int		unsigned	not null,
	
	uis_uPlayerNameShow				tinyint	unsigned	not null,
	uis_uPlayerHonourShow			tinyint	unsigned	not null,
	uis_uPlayerTongNameShow			tinyint	unsigned	not null,
	uis_uPlayerArmyCorpsNameShow	tinyint	unsigned	not null,
	
	uis_uOtherNameShow				tinyint unsigned	not null,
	uis_uOtherHonourShow			tinyint unsigned	not null,
	uis_uOtherTongNameShow			tinyint unsigned	not null,
	uis_uOtherArmyCorpsNameShow		tinyint	unsigned	not null,
	
	uis_uNpcNameShow				tinyint unsigned	not null,
	uis_uMonsterNameShow			tinyint unsigned	not null,
	uis_uHeadPieceShow				tinyint unsigned	not null,
	uis_uLockShortcut				tinyint unsigned	not null,
	uis_uMonsterNameShowMO			tinyint unsigned	not null, #仅在鼠标悬停时显示怪物名称
	uis_uCanNotSelectFriend			tinyint	unsigned	not null,
	
	primary key(us_uId),
	foreign	key(us_uId)	references tbl_char_static(us_uId) on update cascade on delete cascade
	
)engine=innodb;