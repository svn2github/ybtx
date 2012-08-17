
#################################################################################
#                                                                               #
#                           ==**帮会相关**==                                    #
#                                                                               #
#################################################################################

##帮会信息表
create table tbl_tong	 	#static tong info
(
	t_uId					    	bigint unsigned		not null		auto_increment,   #tong idntifier
  t_sName					    varchar(100) binary collate utf8_unicode_ci not null,  #tong name
	t_dtCreateTime      datetime          not null,           					#create time
  t_uHonor			      bigint unsigned		not null,           #tong honor mars
  t_uLevel						tinyint					  not null,       		#tong level
  t_uMoney           	bigint unsigned		not null ,					#tong total money
  t_uResource        	float							not null ,					#tong resourse
	t_uCamp          		tinyint					  not null,       		#tong camp
  t_sPurpose          varchar(3072)			not null,     			#tong purpose
	t_uDepotID					bigint unsigned		default null,       #tong depot id
	t_uType							tinyint						not null default 0,	#tong type
	t_uInitiatorId			bigint unsigned		not null default 0, #tong Initiator
	t_uDevelopDegree		bigint unsigned		not null default 0, #tong Develop Degree
	t_uPermitUpLevel		tinyint	unsigned	not null default 1,	#tong PermitUpLevel
  primary key(t_uId),
	unique key(t_sName),
	foreign	key(t_uDepotID)			references tbl_collectivity_depot(cds_uId) 	on	update cascade on delete cascade
)engine=innodb;

##声望(不可消耗)
create table tbl_tong_exploit		#tong member exploit
(       
  cs_uId							bigint unsigned		not null,                #role identifier
	te_uExploit					bigint unsigned		not null,									#exploit
	primary key(cs_uId),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##帮会人员表
create table tbl_member_tong		#tong members
(       
  cs_uId							bigint unsigned		not null,                #role identifier
	t_uId								bigint unsigned		not null,                #tong identifier
 	mt_sPosition       	tinyint unsigned  not null default 0,      #tong position
 	mt_uPorfferExpend		bigint unsigned		not null,								 #expend proffer
 	mt_uTongProffer			bigint unsigned   not null,								 #tong proffer
  mt_dtJoinTime       datetime          not null,                #join time
	primary key(cs_uId),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade,
	foreign	key(t_uId)	  references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

##国贡(可消耗)
create table tbl_tong_proffer		#tong member exploit
(       
  cs_uId							bigint unsigned		not null,                #role identifier
	tp_uProffer					double(20,2) unsigned not null,									#proffer
	primary key(cs_uId),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##国战声望获得，国战结束即清理一次
create table tbl_tong_battle_exploit #tong battle war exploit
(
	cs_uId							bigint unsigned		not null,								#role identifier
	be_uGetExploit			bigint unsigned		not null default 0,     #get exploit
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##国战阵营声望获得，国战结束即清理一次
create table tbl_battle_camp_exploit #tong camp war exploit
(
	ce_uAmCampExploit			bigint unsigned		not null default 0,     #camp exploit
	ce_uSsCampExploit			bigint unsigned		not null default 0,     #camp exploit
	ce_uXsCampExploit			bigint unsigned		not null default 0,     #camp exploit
	primary	key(ce_uAmCampExploit)
)engine=innodb;

##金钱捐献记录,只记录当天的，每天0点清理一次
create table tbl_tong_contribute_money		#tong member exploit
(       
  cs_uId									bigint unsigned		not null,    	#role identifier
	tcm_uContriNum					bigint unsigned		not null default 0,			#contribute number
	tcm_uContriNumforExp		bigint unsigned		not null default 0,			#contribute number
	tcm_uContriNumforSoul		bigint unsigned		not null default 0,			#contribute number
	tcm_uContriNumforResource	bigint unsigned		not null default 0,			#contribute number
	
	primary key(cs_uId),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##帮会日志信息表
create table tbl_log_tong			#tong log
(       
  lt_uId							bigint unsigned					not null auto_increment, #log sequence id
	t_uId								bigint unsigned					not null,                #tong id
  lt_sContent     		varchar(300)						not null,                #log conteny
  lt_Type             tinyint unsigned        not null,          	     #log type, mayby item or member
  lt_dtCreateTime     datetime                not null,                #log create time
	primary key(lt_uId),
	key(lt_Type),
	foreign	key(t_uId)	references tbl_tong(t_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##申请信息表
create table tbl_request_tong		#role request to join tongs
(       
  rt_uId       			bigint unsigned   not null auto_increment,  #request sequence id
  cs_uId						bigint unsigned		not  null,	          		#role id
	t_uId							bigint unsigned		not null,                 #tong id
  rt_dtRequestTime  datetime         	not null,                 #request time
  primary key(rt_uId),
  foreign	key (cs_uId)	     	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade,
	foreign	key(t_uId)	        references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

##成员退出帮会信息表
create table tbl_leave_tong		#messages that members leave tongs
(
	cs_uId				      			bigint unsigned	    not null,        #role id
	lt_dtQuitTime             datetime            not null,        #time that event happened
	foreign	key (cs_uId)	    references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##佣兵团权限表
create table tbl_tong_purview		#purviews of tong
(
	t_uId				      	bigint unsigned	    not null,                  #tong id
	tp_uPurview         tinyint           	not null,                  #purview type
	tp_uPos							varchar(48)					not null,									 #if have purview
	foreign	key(t_uId)	        references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

create table tbl_tong_type_change			#tongType changed record
(
	t_uId				      	bigint unsigned	    not null,                #tong id
	ttc_uTypeChanged    tinyint           	not null,                #changed record num 0,1,2
	primary key(t_uId),
	foreign	key(t_uId)	        references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;


###################################################×建筑部分×################################################
##帮会建筑信息表
create table tbl_building_tong	#tong building
(
	bt_uId		 				bigint	unsigned				not null auto_increment,	#tong building id
	t_uId		 					bigint 	unsigned				not null,			        		#tong id
  bt_sBuildName    	varchar(192)            not null,                 #building name
	bt_uStep        	tinyint unsigned        not null,       					#building step, defines level that a building can be leveled up
	bt_uPosX        	float        						not null,                 #position of x-axis
	bt_uPosY        	float 				        	not null,                 #position of y-axis
  bt_uState       	tinyint unsigned       	not null,                	#building type
  bt_uLevel					tinyint unsigned				not	null default 1,				#build level
	key (bt_sBuildName),
	KEY `bt_uState` (`bt_uState`),
	primary	key(bt_uId),
  foreign	key(t_uId)	 references tbl_tong(t_uId) 	on	update cascade on	delete cascade
)engine=innodb;


create table tbl_building_life	#values of tong build life
(
	bt_uId		 				bigint	unsigned				not null,	              #tong building id
  bl_uLife   				float       						not null,               #value of current life
  bl_dtLastTime    datetime              	not null,                 #time of that value first changes after the time that last full of life
  primary	key(bt_uId),
  foreign	key(bt_uId)	 references tbl_building_tong(bt_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##模型建造和等待建造的表
create table tbl_building_item	#tong item that waited to be created
(
	bt_uId		 				bigint	unsigned				not null,	                #tong building id
  bi_uDepotIndex    tinyint unsigned       	not null,                 #slot of tong depot
  bi_dtStartTime    datetime              	not null,                 #beginning time
  primary	key(bt_uId),
  foreign	key(bt_uId)	 references tbl_building_tong(bt_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##################################################*粮草领取*#################################
create table tbl_tong_draw_forage
(
	tdf_uId   bigint unsigned     	not null,										#char id
	tdf_uTotal    bigint unsigned    	not null  default 50,			#forage total num
	primary	key(tdf_uId),
  foreign	key(tdf_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


create table tbl_tong_draw_forage_time
(
	tdft_dtTime  datetime  not null,                 
	primary	key(tdft_dtTime)
)engine=innodb;

create table tbl_tong_draw_forage_temp
(
	tdft_uId   bigint unsigned     	not null,										#char id
	tdft_uTotal    bigint unsigned    	not null,			#forage total num
	primary	key(tdft_uId),
  foreign	key(tdft_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


###################################################×粮草收购×##############################################
create table tbl_tong_market	#tong market
(       
	t_uId		 					bigint unsigned     	not null,										#tong id
	tm_uPrice		 			float     						not null,										#acquirement price
	tm_uBuyNumber		 	bigint unsigned     	not null,										#acquirement number
	primary	key(t_uId),
  foreign	key(t_uId)	        references tbl_tong(t_uId) 	on	update cascade on	delete cascade
)engine=innodb;
###################################################×物品制造×##############################################
create table tbl_tong_item_create	#tong item creating info
(       
	tic_uId		 				bigint	unsigned			not null auto_increment,		#sequence id
	bt_uId		 				bigint unsigned     	not null,										#tong id
	tic_uItemType			tinyint unsigned			not null,										#itme type
	tic_sItemName		 	varchar(96)    				not null,										#item name
	tic_dtStartTime   datetime              not null,                 	#start time
	tic_uState				tinyint unsigned			not null,										#being created or waiting
	tic_nDepot				tinyint unsigned			not null,										#in which sloy
	primary	key(tic_uId),
  foreign	key(bt_uId)	 references tbl_building_tong(bt_uId) 	on	update cascade on	delete cascade
)engine=innodb;

###################################################×抢夺资源点管理×##############################################
create table tbl_tong_rob_res		#active that fight for resource
(
	trr_sObjName      varchar(96)	     	not null,							      		#Application place
	trr_uId		 				bigint unsigned     	not null,										#tong id
	trr_sName	 				varchar(96)	     			not null,										#tong name
	trr_uTongCamp     bigint unsigned     	not null,										#tong camp
	primary	key(trr_uId,trr_sObjName),
	foreign	key(trr_uId) references tbl_tong(t_uId) on update cascade on	delete cascade
)engine=innodb;

#####################################################帮会资源点信息表##############################
create table tbl_tong_rob_resource	#tong resource info
(
	
	trr_uId    	 bigint unsigned		not null,  			#tong id
	trr_sObjName     varchar(96)    		not null,   #application place
	primary key(trr_uId),
	unique key(trr_sObjName),
	foreign	key(trr_uId) references tbl_tong(t_uId) on update cascade on delete cascade
)engine=innodb;

#####################################################帮会资源点临时信息表##############################
create table tbl_tong_rob_resource_temp	 #tong resource info
(
	
	trrt_uId    	     bigint unsigned		not null,  			#tong id
	trrt_nCampId      bigint unsigned		not null,  			#tong camp type
	trrt_sObjName     varchar(96)    		not null,   #application place
	primary key(trrt_uId ,trrt_sObjName)
)engine=innodb;

#####################################################帮会资源点报名信息存储表##############################
create table tbl_tong_rob_res_time	 #tong resource info
(	
	trrti_sObjName			varchar(96)    		not null, 
	trrti_uCamp 			bigint unsigned		not null,  			#tong camp
	trrti_uId    	     bigint unsigned		not null,  			#tong id
	trrti_dtSignTime        DateTime          not null,     #signup time
	trrti_uServerId    bigint unsigned		not null,  			#current server id
	primary key(trrti_sObjName, trrti_uCamp,trrti_uId ,trrti_dtSignTime),
	foreign	key(trrti_uId) references tbl_tong(t_uId) on update cascade on	delete cascade
)engine=innodb;

###################################################×挑战管理×##############################################
create table tbl_tong_challenge			#challenge between tongs
(       
	tc_nActiveTongID		bigint 	unsigned     	not null,										#Sponsor tong id
	tc_nPassiveWzId			int unsigned     			not null, 									#war zone place
	tc_nPassiveWzsIndex int unsigned					not null,										#subindex of war zone place
	tc_nChlgMoney			  bigint unsigned 			not null,										#bet money of challenge
	tc_nState						tinyint unsigned			not null,										#bid state
	tc_nTime						DateTime							not null,										#bid time
	tc_nActiveCsId			bigint unsigned		    not null,                   #sponsor role id
	primary	key(tc_nActiveTongID),
  foreign	key(tc_nActiveTongID)	 references tbl_tong(t_uId) 	on	update cascade on	delete cascade	
)engine=innodb;

create table tbl_tong_challenge_battle	#fight between tongs
(       
	tcb_nActiveTongID		 bigint 	unsigned     	not null,									#sponsor tong id
	tcb_nActiveCsId			 bigint   unsigned		  not null,                 #sponsor role id
	tcb_nPassiveTongID   bigint 	unsigned			not null,									#tong id of passive side
	tcb_nPassiveWzId		 int unsigned     			not null, 								#war zone id that be challenged
	tcb_nPassiveWzsIndex int unsigned					  not null,									#index of war zone station that be challenged
	tcb_nSceneId				 int unsigned						not null,									#scene id
	tcb_nServerId				 int unsigned						not null,									#server id
	tcb_nState					 int unsigned						not null,									#callenge state
	tcb_nCamp					 	 int unsigned						not null,									#
	primary	key(tcb_nActiveTongID),
  foreign	key(tcb_nActiveTongID)	 references tbl_tong(t_uId) 	on	update cascade on	delete cascade	
)engine=innodb;

###################################################×科技×##############################################
create table tbl_tong_tech_info_static(
    ttis_uId        tinyint unsigned                not null,
    ttis_uType      tinyint unsigned                not null,
    ttis_sName      varchar(96)                 not null,
    
    primary key(ttis_uId)
)engine = innodb;

create table tbl_tong_tech_static			#tong technology
( 
	tts_uId           	bigint unsigned     	not null auto_increment,  	#tong technology static info
	t_uId		 						bigint unsigned     	not null,										#tong id
    ttis_uId        tinyint unsigned                not null,
    tts_uLevel        	tinyint unsigned      not null,      							#current level
	primary key(tts_uId),
	unique key(t_uId, ttis_uId),
  foreign	key(t_uId)	        references tbl_tong(t_uId) 	on	update cascade on	delete cascade,
  foreign key(ttis_uId) references tbl_tong_tech_info_static(ttis_uId) on update cascade on delete cascade
)engine=innodb;

##科技研发等待序列
create table tbl_tong_tech_order		#tong technology production time
(       
  tts_uId         	bigint unsigned       not null,                 	#tong technology id
  tto_dtStartTime   DateTime            	not null,       						#begin time
 	tto_dtUpdateTime  DateTime            	not null, 									#update time
  tto_uState        tinyint unsigned      not null,      							#state
  primary key(tts_uId),
  foreign	key(tts_uId)	        references tbl_tong_tech_static(tts_uId) 	on	update cascade on	delete cascade
)engine=innodb;


################################################×运输车×#######################################################
##运输车信息表
create table tbl_truck_tong			#tong trunk
(       
	cs_uId			bigint unsigned 	not null,			#role
	tt_sName		varchar(96)       not null,			#trunk name
	tt_uResNum	bigint unsigned   not null 	default 0,		#resource num
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;


################################################×国战击杀时间记录×#######################################################
##×国战击杀(时间)信息表
create table tbl_war_tong			#tong trunk
(       
	cs_uId		bigint unsigned 	not null,			#role
	wt_uId		bigint unsigned 	not null,			#role
	wt_uTime	bigint unsigned   not null,		#kill time
	primary key(cs_uId, wt_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

##×国战击杀获得声望(时间)信息表
create table tbl_war_exploit_time			#tong trunk
(       
	cs_uId		bigint unsigned 	not null,			#role
	we_uId		bigint unsigned 	not null,			#role
	we_uTime	bigint unsigned   not null,		#get exploit time
	primary key(cs_uId, we_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

create table tbl_tong_get_proffer_time
(
	cs_uId			bigint unsigned 	not null,			#role
	tgpt_sTime  Datetime  not null,                 
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_tong_header_award_time
(
	t_uId				bigint unsigned			not null,
	cs_uId			bigint unsigned 	not null,			#role
	that_dtTime datetime not null,                 
	primary	key(t_uId,cs_uId),
	foreign	key(t_uId)	references tbl_tong(t_uId) on	update cascade on delete cascade,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

##Boss争夺战佣兵小队击杀Boss次数
create table tbl_tong_boss
(
	t_uId					    bigint unsigned			not null,
  tb_uNum						tinyint unsigned		not null		default 0,
  primary key(t_uId),
	foreign	key(t_uId)		references tbl_tong(t_uId) 	on	update cascade on delete cascade
)engine=innodb;

##Boss争夺战玩家击杀Boss次数
create table tbl_tong_char_boss
(
	cs_uId					bigint unsigned 	not null,
	tcb_uNum				bigint unsigned		not null		default 0,
	primary key(cs_uId),
	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 	on	update cascade on delete cascade
)engine=innodb;

##军资运输,资源点与玩家相关次数
create table tbl_tong_char_other
(
	cs_uId			bigint unsigned 	not null,			#role
	tco_uTypeId  		bigint unsigned			not null,
	tco_uNum				bigint unsigned		not null		default 0,
	primary key(cs_uId, tco_uTypeId),
	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 	on	update cascade on delete cascade
)engine=innodb;


##资源点与小队相关次数
create table tbl_tong_other
(
	t_uId					    bigint unsigned			not null,
  to_uNum						bigint unsigned		not null		default 0,
  primary key(t_uId),
	foreign	key(t_uId)		references tbl_tong(t_uId) 	on	update cascade on delete cascade
)engine=innodb;

##帮会联赛中杀人次数
create table tbl_tong_jfs
(
	cs_uId			bigint unsigned 	not null,
	tj_uNum			bigint unsigned		not null,
	primary key(cs_uId),
	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 	on	update cascade on delete cascade
)engine=innodb;
create table tbl_tong_station_time
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tst_uStationId	smallint unsigned 	not null,			#role
	tst_uWsId  tinyint unsigned 	not null,			#wsId 
	tst_uTime  smallint unsigned 	not null default 0,			#role 
	primary	key(t_uId),
	foreign	key(t_uId)	  references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

create table tbl_tong_line_time
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tlt_uLineId	tinyint unsigned 	not null,			#role
	tlt_uWsId  tinyint unsigned 	not null,			#wsId 
	tlt_uTime  smallint unsigned 	not null default 0,			#role       
	primary	key(t_uId),
	foreign	key(t_uId)	  references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;


create table tbl_tong_rob_res_log
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	trrl_sObjName	varchar(96)         	not null,			#role
	primary	key(t_uId, trrl_sObjName)
)engine=innodb;


create table tbl_tong_issue_record
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tir_uNum	tinyint    not null,			
	primary	key(t_uId),
	foreign	key(t_uId)	 references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#佣兵小队篝火表
create table tbl_tong_needfire
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tn_uServerId tinyint unsigned	not null,
	
	primary	key(t_uId,tn_uServerId),
	foreign	key(t_uId)	 references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#添加木柴表
create table tbl_char_firewood		
(
	cs_uId		bigint	unsigned	not	null,	#role identifier
	cf_dtDateTime	datetime  not null,#add firewood time
	cf_uAddTimes tinyint not null,
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_tong_char_tech(
    cs_uId          bigint unsigned                 not null,
    ttis_uId        tinyint unsigned                not null,
    tct_uLevel     tinyint unsigned                not null, 
  	unique key(cs_uId, ttis_uId),
  	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 		on	update cascade on	delete cascade,
  	foreign	key(ttis_uId)	 references tbl_tong_tech_info_static(ttis_uId) 		on	update cascade on	delete cascade
)engine = innodb;

#龙穴活动龙头建筑奖励
create table tbl_building_dragon(
		bt_uId		bigint unsigned			not null,
		t_uId			bigint unsigned			not null,
		bt_buildTime		datetime			not null,
		primary key(bt_uId),
		foreign key(bt_uId)	references tbl_building_tong(bt_uId)  on update cascade on delete cascade,
		foreign key(t_uId)	references tbl_tong(t_uId)  on update cascade on delete cascade
)engine = innodb;


#记录资源点奖励数据清理时间
create table tbl_tong_res_award_time(
		rat_dtTime		DateTime        not null       	#clear time
)engine = innodb;
