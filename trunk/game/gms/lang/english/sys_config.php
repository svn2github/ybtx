<?php
$tbl_sys_msg = array(
	"no_same_id"	 => "Returnee and Sender Game ID do not match",
	"return"			 => "Return",
	"refurbish"		 => "Refresh",
	"close"				 => "关闭",
	"all_server"	 => "All Area Information",
	"selected"		 => "选中的内容",
	"md5error"		 => "MD5Signature Error",
	"ip_error"		 => "Invalid IP",
	"unknow_err"	 => "Unknown Error",
	
	"level"				 => "Level",
	"class"				 => "Profession",
	"total"				 => "Total",
	"count"				 => "Frequency",
	"camp"	       => "Camp",
	"server"       => "Service Assistant",
	"time"				 => "Time",
	"sex"					 => "Gender",
	"skill"				 => "Skill",
	
	"online"			 => "Online",
	"offline"			 => "Offline",
	
	"on"					 => "开启",
	"off"					 => "关闭",
	
	"edit"				 => "编辑",
	"act"					 => "Operation", 
	"detail"			 => "Details",
	"delete"			 => "Delete",
	"pass"				 => "Approve",
	"cancel"			 => "Cancel",
	"status"			 => "Status",
	"desc"				 => "Description",
	"rank"				 => "排序",
	
	"success"			 => "Operation Successful",
	"nobody"			 => "没有找到指定的对象，请检查用户名、物品名称等信息是否正确。",
	"reason"			 => "Reason",
	
	"submit"			 => "Submit",
	"search"			 => "Search",
	"all"					 => "All",
	"download"		 => "Download",
	"insert"			 => "Insert",
	
	"gold"				 => "Gold",
	"silver"			 => "Silver",
	"copper"			 => "Bronze",
	
	"LastLogTime"	 => "Check Diary Update Time",

	"check_chat"	 => "检查消息记录完成。",
	"read"				 => "标为已读",
	"Max_Latency"	 => "最大延迟",
	"admin"				 => "Administrator",
	"delete_sure"	 => "确定要删除%s吗？",
	
	"MoneyType1"	 => "Unbound Gold",
	"MoneyType2"	 => "Bound Gold",
	
	"CreateTime"	 => "",
	"LastLoginTime"=> "",
	"LastLogoutTime"=> "",
);

//聊天频道
$tbl_channel = array(
	0		=> "All",
	1		=> "System",
	2		=> "World",
	3		=> "Camp",
	4		=> "Map",
	5		=> "Nearby",
	6		=> "Team",
	7		=> "Mercenary Team",
	8		=> "Mercenary Group",
	10	=> "Broadcast",
);

//积分分类、NPC商店支付类型
$tbl_jifen = array(
	"Money",
	"Dwarf Points",
	"Forest Points",
	"Nightshadow Points",
	"Mana Points",
	"Adventure Points",
	"Match Points",
	"Killing Points",
	"Training Points",
	"Confrontation Points",
	"Tribute Points",
);

//物品大类
$bid2type = array(
	1  => "Normal Items",
	2  => "Backpack",
	3  => "Skill Item",
	5  => "Weapon",
	6  => "Armour",
	7  => "Secret Weapon",
	8  => "Ring",
	9  => "Accessories",
	10 => "Guild Item",
	14 => "Gem",
	15 => "Drilling Material",
	16 => "Quest Item",
	17 => "Mail Item",
	18 => "White Gem",
	19 => "Soul Pearl",
	23 => "Guild Alchemy Item",
	24 => "Box Item",
	25 => "Document Item",
	26 => "Array Book",
	27 => "Main Smithing Material",
	28 => "Smithing-Quality Material",
	29 => "Chamber of Commerce Transport Vehicle",
	30 => "Lode Map",
	31 => "Equipment Certification Scroll",
	32 => "Gardening Item",
	34 => "Protective Plate",
	36 => "Equipment Refinery Scroll",
	37 => "Practice Tower Deck",
	38 => "Progress Stone",
	39 => "Guild Challenge Item",
	40 => "Summon NPC Tool",
	41 => "宠物蛋",
	42 => "宠物技能石",
	43 => "大重铸石",
	45 => "采矿工具",
	46 => "商城道具",
	47 => "商城道具宝箱",
	48 => "经验瓶、魂瓶",
	49 => "精炼石",
	50 => "装备追加石",
);

//阵营
$tbl_camp = array(
	"Null",
	"Amy",
	"Holy",
	"Hughes",
	"Passerby",
	"Monster",
	"Chaos",
	"Guild 1",
	"Guild 2",
	"Guild 3",
	"Non-quest and non-interactive passerby",
	"Opposing camps with 0,5, the rest being allies",
);

//职业
$tbl_class = array(
	"No profession",
	"Great Swordsman",
	"Wizard Swordsman",
	"Knight",
	"Mage",
	"Demon",
	"Priest",
	"Dwarf Knight",
	"Elf Archer",
	"Orc Warrior",
	"NPC",
	"Wizard",
	"Assassin",
	"Evil Mage",
	"Battle Priest",
	"Archer",
	"Great Warrior",
	"Summoner",
	"Wild Beast",
	"Summon Living Beings",
	"Attacking Knight",
	"Defense Knight",
	"Natural Element",
	"Dark Element",
	"Destruction Element",
);

//装备位置
$tbl_part = array( "Null", "Dominant Hand", "Supporting Hand", "Head", "Clothes", "Belt", "Wristguard", "Shoes", "Left Ring", "Right Ring", "Necklace", "Accessories", "Cloak", "Mantle", "Shoulder", "Armguard", "Back");

//佣兵团职位
$tbl_tong_pos = array(
	"未定义",
	"团员",
	"队长",
	"营长",
	"副团长",
	"团长",
);

//佣兵团仓库分类
$tbl_tong_page = array(
	"","Common Item","Captain's Item","Camp Leader's Item","Vice Group Leader's Item","Group Leader's Item"
);

//framework中的错误提示
$tbl_error_msg = array (
	"NoPriv" => "You do not have this authority",
	"ActionFail" => "Operation Unsuccessful",
	"PostDataFail" => "操作失败，请重试。(error code: %s)",
	"MsgTooLang" => "向服务器发送的消息过长，请减少要发送的数据量（除角色名外）。",
);

//公告状态
$tbl_list_bulletin_state=array("Submitted","Approved","Deleted","Started");

//GM申请记录 GM操作记录 审核申请
$tbl_list_compensate = array("Record does not exist","Awaiting Process","Cancel theled","Completed");

//角色信息查询 任务信息
$tbl_list_task=array("","In Progress","","Completed","");

//游戏世界查看 佣兵团 详细
$tbl_tong_detail=array("Guild Name","Guild Leader","Camp","Guild ID","Set-Up Time","Guild Reputation","","Guild Level","Guild Funds","Guild Resources","Guild Purpose","Guild Warehouse ID","Guild Member Count");

//游戏世界查看 佣兵团 成员
$tbl_tong_list_member=array(
	array("Character Name", "Character ID", "Level", "Task", "Contribution", "Guild Entry Duration"),
	array("Remove","Change Posts")
);

//游戏世界查看 佣兵团 仓库
$tbl_tong_depot=array(
	"Guild Warehouse ",
	array("Item Name","Item Category","Current Page","位置","Amount"),
);

//游戏世界查看 佣兵团
$tbl_list_tong=array(
	"Guild Information Query",
	array("Guild Name","Number of People","Creator","Camp","Reputation","Level","Check Diary"),
	array("Details","Members","Warehouse","Change Name","Change Levels"),
	array("Members","Item"),
	array("Please enter new guild name"),
	array("Reward Item Name","Warehouse","Amount","Mercenary Group Warehouse Reward")
);

$AddItemToTong = "";
$DelItemInTongDepot = "";
$ChangeTongMember = array("","");
$RemoveTongMember = "";

//角色信息查询 详细信息
$tbl_role_all_detail = array(
	"Character: <b>%s</b> 's detailed information is as follows:",
	"Character Details",
	"Character Location",
	"Bag Item",
	"Character Equipment",
	"Warehouse Item",
	"Character Skill",
	"Please enter reason",
	"Please enter the amount to be deleted",
	"You have entered an incorrect amount",
	"Character Detailed Information",
	"Character Ranking",
	"Character Title",
	"Mailbox Item",
	"Consignment Exchange Store Item",
	"View Character's Item Information",
	"View Character's Social Relationship",
	"Character: <b>%s</b> 's item information is as follows:",
	"Character: <b>%s</b> 's social relationship is as follows:",
	"Online/ Recent Login Information",
	"Check Character's Real Time Data",
	"Character: <b>%s</b> 's Real Time Data is as follows:",
	"Rune Condition",
);

//角色信息查询 详细信息 角色详情 性别
$tbl_sex = array("Null","Male","Female");

//角色信息查询 详细信息 角色排行
$tbl_role_sort = array(
	array("Ranking Name","Ranking","Rise and Fall","Information"),
	array(
		"level" 			=> array("Level Board","%d Level"),
		"money"				=> array("Wealth Board","%d Gold %d Silver %d Bronze"),
		"dead"				=> array("Death Board","%d Times"),
		"kill_player"	=> array("Killing Board","%d Persons"),
		"kill_npc"		=> array("Monster-Kill Total Board","%d Monsters"),
		"online"			=> array("Time Online Board","%d Days %d Hours %d Minutes %d Seconds"),
	)
);

//角色信息查询 详细信息 角色称谓
$tbl_role_appellation = array(
	"Current Title",
	array("Title")
);
  
//角色信息查询 详细信息 槽位
$tbl_role_item_room = 'In backpack no.%d, slots available are %d';
$tbl_role_item_room1= 'In main backpack, slots available are %d';

//角色信息查询 详细信息 邮箱物品
$tbl_role_mail_item = array(
	"Mail Subject","Sender"
);

//角色信息查询 详细信息 符石
$tbl_role_stone = array(
	"Rune Mark","位置","Rune stone",
	array(
		1=>"Mark of the Sun",
		2=>"Mark of the Love",
		5=>"Mark of the Wisdom",
		6=>"Mark of the Moon",
		9=>"Mark of the Ocean",
		10=>"Mark of the Sunshine",
	),
);

$tbl_role_friends = array(
	"Friend Information",
	array("Current Team","Friend Name","Camp","Profession","Level","Gender","My Friends"),
	"Blacklist",
	array("Character's Name","Camp","Profession","Level","Gender"),
	"Friend Community",
	array("Community Name","Community Post","Member Count",array("","Community Head","Administrator","Members")),
	"Guild",
	array("Guild Name","Guild Post","Member Count"),
	"Check Character's Social Relationship",
);

$tbl_user_type = array("","");
$tbl_user_role_list_function = array(
	"Delete Character","Restore Character","Change Character Name","Please Enter New Character Name"
);
  
//帐号信息查询 帐号登录信息
$tbl_user_login = array(
	"Account Login Information",
	array("Login Time","Login IP"),
	"IP Login Information",
	array("Login Time","Character Name")
);
  
$tbl_page = array(
	"Jump to Page &nbsp;%s&nbsp;","Page&nbsp;/&nbsp;%d&nbsp;","&nbsp;%d&nbsp;records in total, %d&nbsp;records&nbsp;/&nbsp;pages","Download all pages"
);
  
$tbl_hot_update_log = array(
	"Type/ Time/ Document Name/ Operation","Code","","View","Hot Updated Diaries","View Results",
);
  
$tbl_hot_update_Result = array(
	"Service Assistant Id","Status","Completion Time","View Hot Refreshed Results","Hot Refreshed Details","Results are as follows: ",
);
  
$tbl_hot_update_type = array(
	"","Main Thread","Archive Thread","Client"
);
  
$tbl_hot_update_state = array(
	"-","Success",
);

$tbl_get_server_date = "Array";
  
$tbl_talk = array(
	"send" => "Send Information",
	"reflesh" => "Refresh Information",
	"older" => "View Earlier Information",
	"me" => "Me",
	"AllMsg"=>"View All Information",
	"reason"=>"GM Chat",
);
  
$tbl_npc_info = array(
	"NPC Information Search",
	array("Basic Information",array("NPC Name","Remarks","Camp","Function Notes")),
	array("Item Drop",array("Monster Notes","Monster Level","Dropped Group Name","Drop Rate","Amount","Dropped Item Name","Drop Rate")),
	array("Quest",array("Current Scene","Coordinate","Quest Name")),
	array("Store",array("Store Type","Description","Payment Method","Item Category","Item Name")),
	"Non-selected NPC, perhaps because the search button has not been  clicked."
);
  
$tbl_money_binding = array(
	"All","Unbound Gold","Bound Gold"
);
  
$tbl_channel_word = array(
	"Chat Moderation - Sensitive Phrase Management",
	array("Sensitive Phrase","Delete"),
	"Add New",
	"Chat Moderation - Newly added Sensitive Phrase",
	"Successfully added New Sensitive Phrase “%s”.<br/>Recheck Information Records. May take a few minutes.",
	"Chat Moderation - Delete Sensitive Phrase",
	"Successfully deleted Sensitive Phrase “%s”.<br/>Recheck Information Records. May take a few minutes.",
);

//查看投诉
$tbl_list_complaint=array("","External Report","Skill","Quest","Monster","Money Reward","Item","Play Manual Copy","Art Resource","Other");
  
$tbl_event_type = array(
	0		=>"All",
	1		=>"npc Store purchase",
	2		=>"npc Store sale",
	3		=>"Mail",
	4		=>"Create Mercenary Team",
	5		=>"Forging and Casting",
	6		=>"Donate",
	7		=>"Collect",
	8		=>"Cooking and Potion Making",
	9		=>"Use Item",
	10	=>"Left-click to delete ",
	11	=>"Consignment",
	12	=>"Delete Quest Item or Tool Item",
	13	=>"GM Delete Item",
	14	=>"Delete Box Item",
	15	=>"Equipment Refinement",
	16	=>"Add to Sale List",
	17	=>"Consignment Exchange Store Sale",
	18	=>"Consignment Exchange Store Purchase",
	19	=>"Manually Delete Sale List",
	20	=>"System Delete Sale List",
	22	=>"Freely pick up item",
	23	=>"Default Character Production Equipment",
	24	=>"Return Array to Array Book",
	25	=>"GM Add Item",
	26	=>"Box Item Drop",
	27	=>"Combine",
	28	=>"White Gem Analysis",
	29	=>"Remove Gem",
	30	=>"Equipment Refinement Soul Pearl",
	31	=>"New Mail",
	32	=>"Dungeon Reward",
	33	=>"Drop Auction",
	34	=>"Temper",
	35	=>"Forge",
	36	=>"Production",
	37	=>"Offline Gardening Produce",
	38	=>"Canon Tower Key Produce",
	39	=>"Tool Produce",
	40	=>"Use Revival Stone",
	41	=>"Guild Building Item Produce",
	42	=>"Quest Reward",
	43	=>"Give Quest Item or Tool Item",
	45	=>"Click on npc to give item",
	46	=>"Call Automated Beast",
	47	=>"Online Gardening Produce",
	48	=>"Won Auction Item",
	49	=>"Gather a single squared item",
	50	=>"Gather all squared items",
	51	=>"Evenly distribute collected items",
	52	=>"Need-Based Distribution",
	53	=>"Quest dropped item",
	54	=>"Click npc corpse to drop (Click a square)",
	55	=>"Equipment Whitewash",
	56	=>"Give item upon stepping on trap",
	57	=>"Click npc corpse to drop (Click all squares)",
	58	=>"Experience from killing people",
	59	=>"Experience from killing monsters",
	60	=>"Use Experience Ball to get experience",
	61	=>"GM order add experience",
	62	=>"Use Soul Pearl to add Soul Points",
	63	=>"Minigame",
	64	=>"Bursting Soul",
	65	=>"Soul drops upon death",
	66	=>"Kill to add soul",
	67	=>"Input Soul for Equipment Upgrade",
	68	=>"Add Skill Item",
	69	=>"Default Character Handcarry Skill Item",
	70	=>"Player steps on Trap give experience",
	71	=>"Breakdown Production",
	72	=>"Equipment Progress Soul", 
	73	=>"GM order added points",
	74	=>"Gladiator Arena Dungeon added points",
	75	=>"League Competition Arena Dungeon added points",
	76	=>"Kill Monster under PK Status to added points",
	78	=>"Player steps on trap added money",
	79	=>"Treasure Hunt added money",
	80	=>"Guild-sold army provisions",
	81	=>"Player Mail withdraw money",
	82	=>"Head to warehouse to save money",
	83	=>"Head to warehouse to withdraw money",
	84	=>"GM order added money",
	85	=>"Player Trading",
	86	=>"Money deducted for drink",
	87	=>"Purchase Exchange Store Order List",
	88	=>"Money deducted for fixing equipment",
	89	=>"Money deducted for skill learning",
	90	=>"Tool Trading City",
	91	=>"Mercenary currency exchange for unbound money",
	92	=>"Unbound money exchange for bound money",
	93	=>"Mercenary currency exchange for bound mercenary currency",
	94	=>"Guild Cancel Item Delete",
	95	=>"Guild Delete Member",
	96	=>"Life Skill Learning or Level-Up",
	97	=>"Gm Order to Learn Life-Skill",
	98	=>"Gm Order to enhance Life-Skill Familiarity",
	99	=>"Specialised Life-Skill Learning",
	100	=>"Expend Soul for Skill Learning",
	101	=>"Trading City Purchase",
	102	=>"Trading City Gift",
	103	=>"Guide Reward",
	104	=>"Player gets Soul points for killing monsters",
	105	=>"gm Sent Mail",
	106	=>"System Rejected Mail",
	107	=>"Consignment Exchange Store Mail",
	108	=>"Activity Reward Mail", 
	109	=>"Gardening System Mail", 
	110	=>"Points Race Reward Mail",
	111	=>"Backpack Full of Stone Combinations or Analysis Mail",
	112	=>"Order Expired Mail",
	113	=>"Purchase Order Successful Mail",
	114	=>"Order Successfully Purchased Mail",
	115	=>"Mercenary Team Building Creation",
	116	=>"Mercenary Team Cancel the Item Creation",
	117	=>"Mercenary Team Item Creation", 
	118	=>"Mercenary Team Provision Purchase", 
	119	=>"Mercenary Team Provision Sale", 
	120	=>"Mercenary Team Member Donation Money ",
	121	=>"Mercenary Team Technology Research",
	122	=>"Mercenary Team Declare War",
	123	=>"Mercenary Team Declare War Failed",
	124	=>"Mercenary Team GM Order Add Money",
	125	=>"Mercenary Team Points Race Reward",
	126	=>"Mercenary Team Resource Point Seizing Activity",
	127	=>"Mercenary Card Deck Reward",
	128	=>"Mercenary Team Kill People Add Feats ",
	129	=>"Mercenary Team Challenge Time's Up Add Glory",
	130	=>"Mercenary Team Challenge Victor Emerged Add Glory", 
	131	=>"Mercenary Team Building Destroyed Add Glory",
	132	=>"Mercenary Team npc Camp Building Destroyed Add Glory",
	133	=>"Mercenary Team Resource Drop from Building Destruction", 
	134	=>"Mercenary Team Building Repairs", 
	135	=>"Mercenary Team Update Building Level", 
	136	=>"Breakdown & Delete",
	137	=>"Boss Tussle Reward",
	138	=>"Kill Player in Pk Status Add Points", 
	139	=>"Tool Trading City Withdraw Extra Funds",
	140	=>"Massacre Add Points", 
	141	=>"Group Store Purchase Item", 
	142	=>"Quest Add Mercenary Voucher", 
	143	=>"Kill npc Add Group Contribution", 
	144	=>"Quest Guild Glory Reward", 
	145	=>"Quest Guild Group Contribution Reward", 
	146	=>"Practice Tower Give Bound Gold Currency", 
	147	=>"Auction Bargain Deduct Money", 
	148	=>"Auction Bargain Replenish Money", 
	149	=>"Auction Bargain Return Money", 
	150	=>"Auction OBJ Disappear Return Money", 
	151	=>"Auction System Return Money", 
	152	=>"Auction Even Distribution of Money", 
	153	=>"npc Store Purchase",
	154	=>"Expressions and Movements",
	155	=>"Hang Up Mining Produce",
	156	=>"Create Mercenary Group",
	157	=>"System Administrative Fee",    
	158	=>"Tribute Store Purchase Item", 
	159	=>"gm Remuneration",    
	160	=>"World Channel Chat Deduct Money", 
	161	=>"Upgrade Equipment to boost Max Limits", 
	162	=>"Gardening", 
	163	=>"Drop Item Auction", 
	164	=>"Hired Labour Factory Creation", 
	165	=>"Hired Labour Success",   
	166	=>"Killed in Pk Status Deduct Points", 
	167	=>"Offline Experience Exchange",   
	168	=>"Resource Point Captain Reward",  
	169	=>"Mercenary Team GM Order Add Level",
	170	=>"Mercenary Team  GM Order Add Glory",
	171	=>"Mercenary Team GM Order Add Resource",
	172	=>"Ally Quest Experience Sharing",
	173	=>"Special NPC Teleport",   
	174	=>"Resource Point Seizing Add Tribute",
	175	=>"Soul Bottle Full Generate Soul Pearl",
	176	=>"Guild Challenge Collect Challenge Building",
	177	=>"Hired Labour Fees",
	178	=>"System Collects",
	179	=>"Player Profession Change",
	180	=>"Mercenary Team Remove Provision Warehouse Deduct All Army provisions",
	181	=>"Exchange Store Add Order Administrative Fee",
	182	=>"Add Hired Labour Order Administrative Fee",
	183	=>"Skill Learning Administrative Fee",
	184	=>"Mail Administrative Fee",	
	185	=>"Cancel  Hired Labour Order Refund Money ",
	186	=>"Equipment Mounted Armour Plate Deducted Money",
	187	=>"Broadcast Channel Broadcast Information Deducted Money",
	188	=>"Player Trading Deducted Tax",
	189	=>"Hired Labour Success Deducted Tax",
	190	=>"Consignment Exchange Store Deducted Tax",
	191	=>"Mailing Deducted Tax",
	192	=>"Auction Money Distribution Deducted Tax",
	193	=>"Create Hired Labour Order",
	194	=>"Add Hired Labour Order",
	195	=>"Manually Cancel the Hired Labour orders",
	196	=>"Automatically Cancel the Hired Labour orders",
	197	=>"Drop Item Auction Results",
	198 =>"特殊NPC传送非绑定金币",
	199 =>"过期邮件删除",
	200 =>"装备追加消耗",
	201 =>"佣兵小队征粮扣钱",
	202 =>"佣兵小队类型提升扣钱",
	203 =>"gm工具给佣兵小队仓库加物品",
	204 =>"gm恢复装备",
	205 =>"佣兵小队补偿资金",
	206 =>"佣兵小队补偿军资",
	207 =>"WebShop购买物品",
	208 =>"添加求购订单",
	209 =>"手动删除收购订单",
	210 =>"出售收购订单",
	211 =>"收购订单成功邮件",
	212 =>"收购订单超时删除",
	213 =>"收购订单超时邮件",
	214 =>"收购订单完成删除",
	215	=>"收购订单管理费",
	
	10001	=> "佣兵小队仓库",
	10002 => "玩家交易信息",
);

$tbl_log_vir_wealth = array(
	"View Virtual Wealth Diary",
	"Character Name:%s(CharacterID:%s)",
	"Character Level Up Status:  ",
	array("Level","Time"),
	"Character Experience Status",
	array("Source","Experience"),
	"Character Money Status",
	array("Operation Type","Money Type","Operation Frequency","Total Operations Money","Average Operations Money"),
	"Character Soul Status",
	array("Operation Type","Operation Frequency","Total Operations Soul Points","Average Operations Soul Points"),
	"Character Points Status",
	array("Points Type","Operation Type","Operation Frequency","Total Operations Points","Average Operations Points"),
	"Character Item Status",
	array("Obtained","Lost","Operation Type","Operation Frequency"),
);

$tbl_log_item_flow = array("Obtained","Lost");

$tbl_market = array(
	"title"							=> "Consignment Exchange Store&nbsp; - &nbsp;%s",
	"giver_item_list"		=> array("Sale Item Distribution",	array("Item Name","Order Amount","Total Amount","Total Sale Price")),
	"taker_item_list"		=> array("Purchase Item Distribution",	array("Item Name","Order Amount","Total Amount","Total Sale Price")),
	"order_list"				=> array("Order Status",			array("Operation Type","Order Amount","Total Sale Item Amount","Total Sale Price")),
);

$tbl_gm_log_user = array(
	"role" => array("role_detail_"=>"View Character Details","role_select"=>"Select Character","role_filter"=>"View records relevant to the character"),
	"user" => array("user_detail"=>"View Account Details","user_select"=>"Select Account","user_filter"=>"View records relevant to the account"),
	"tong" => array("tong_detail"=>"View Guild","tong_filter"=>"View records relevant to the Guild")
);

$tbl_item_is_binding = array("","Bound once equipped","Bound");
$tbl_role_equip_info = array(
	"Durability: %d/%d",
	array(
		array("%d - %d Damage","Speed %.1f","(Damage per second %.1f)"),
		array("Armour Value","Natural Resistance","Destruction Resistance"),
		array("Attack Power: %.1f"),
	),
	"%s Progress",
	"%s Buff",
	"Accumulative Buff %d times",
	"幸运值&nbsp;%d/150",
	"追加",
	"级",
	"(%d)套装：",
	"隐藏效果："
);

$tbl_server_info = array(
	array("Service Assistant Change Amount","Time","Number of People","Service Assistants in Queue: ","Persons in Queue: ","Dungeon Amount","People in Dungeon","Realm Dungeon Amount"),
	array("People in Progress Distribution","Number of People","Process","Service Assistant"),
	array("Main Scene People Distribution","Process","Number of People (Including people teleported into the dungeon)","Activity Character Amount","Scene Name"),
	array("Dungeon Total","Dungeon Name","Dungeon Amount","Number of People"),
	array("Dungeon in Progress Total","Process","Dungeon Amount","Number of People"),
);

$tbl_item_inten_soul = array(
	"1b3J8Q" => "Physical Attack",
	"t6jEpw" => "Magic Damage Attack ",
);

$tbl_money_log = array(
	"AUN"					=> "Active User Amount",
	"RUN"					=> "Relevant User Amount",
	"SUN"					=> "User Amount",
	"RUP"					=> "Relevant User Proportion",
	"AC"					=> "Average Frequency",
	"CMS"					=> "Unbound Gold Total",
	"CMA"					=> "Average Unbound Gold Per Person",
	"BMS"					=> "Bound Gold Total",
	"BMA"					=> "Average Bound Gold Per Person",
	"CM"					=> "Unbound Gold",
	"BM"					=> "Bound Gold",
	"CMC"					=> "Unbound Gold Frequency",
	"BMC"					=> "Bound Gold Frequency",
	"CMM"					=> "Unbound Gold Distribution",
	"BMM"					=> "Bound Gold Distribution",
	"MM"					=> "Money Distribution",
	"MV"					=> "Average Money Per Person",
	"P"						=> "Produce",
	"R"						=> "Recycle",
	"CMP"					=> "Unbound Gold Produce",
	"CMPM"				=> "Unbound Gold Produce Distribution",
	"BMP"					=> "Bound Gold Produce",
	"BMPM"				=> "Bound Gold Produce Distribution",
	"CMR"					=> "Unbound Gold Recycling",
	"CMRM"				=> "流通金回收分布",
	"BMR"					=> "Bound Gold Recycling",
	"CMRM"				=> "绑定金回收分布",
	"CS"					=> "Unbound Amount Total",
	"CSA"					=> "Average Unbound Amount per person",
	"CSM"					=> "Unbound Amount Distribution",
	"CC"					=> "Unbound Frequency",
	"CI"					=> "Unbound Income",
	"CIM"					=> "Unbound Income Distribution",
	"CO"					=> "Unbound Expense",
	"COM"					=> "Unbound Expense Distribution",
	"CN"					=> "Unbound Net Income",
	"CNM"					=> "Unbound Net Income Distribution",
	
	"Market"			=> "Exchange Store",
	"Manufacture"	=> "Hired Labour",
	"Mail"				=> "Mail",
	"Trade"				=> "Face-to-Face Delivery",
	"Bidding"			=> "Auction Receipt",
	"Prepaid"			=> "Recharge",
	"task"				=> "Quest",
	"shop"				=> "Store",
	"Xiuxingta"		=> "Practice Tower",
	"Stone"				=> "Rune stone",
	"Talk"				=> "Chat",
	"Exp"					=> "Offline Experience",
	"Skill"				=> "Skill",
	"Life"				=> "Life Skills",
	"Tex"					=> "Tax",
	"Cost"				=> "Administrative Fee",
	"Bag"					=> "Bag",
	"Depot"				=> "Warehouse",
);
$tbl_log_desc = array(
	1		=> array("Approve NPC[%s]","Purchase[%d]个[%s]","花费[%s][%s]"),
	2		=> array("Approve NPC[%s]","Sell[%d]个[%s]","Get[%s][%s]"),
	3		=> array("Player[%s]","To Player[%s]","Send Mail"),
	16	=> array("At Consignment Exchange Store, issue Order List to<br/>Sell [%s] [%s]，Priced at [%s]","该订单于[%s]被[%s]购得"),
	18	=> array("At Consignment Exchange Store, from Player [%s]'s Order List<br/>Purchase [%s] [%s], spending[%s]"),
	42	=> "Complete Quest [%s]",
	85	=> array("Trade between player [%s] and player [%s]，<br/>","使用“%s”，<br/>交换“%s”","[%1\$d]个[%2\$s]","[流通金][%1\$s]","、"),
	153	=> "Through NPC[%s]，<br/>repurchase [%d] [%s],<br/>spending [%s][%s]",
	10002=> "请查看与当前记录同时发生的金钱数量为空的“玩家交易”记录，这两条记录为同一事件。",
);
$tbl_equip_upgrade_type = array(
	"intensify"					=>	array("装备强化",		""),
	"intensifyAddAttr"	=>	array("精炼后强化",	""),
	"armor"							=>	array("镶嵌护甲片",	"在位置%d，装入第%d个护甲片[%s]"),
	"advance"						=>	array("装备进阶",		""),
	"superaddation"			=>	array("装备追加",		"装备追加至第%d阶段"),
);
$tbl_market_buy_order = array(
	"由[%s]出售[%d]个物品",
	"总计购得[%d]个物品",
	"订单由于[%s]被删除",
	array(
		209 => "玩家操作",
		212 => "超过时间",
		214 => "满足数量"
	),
);
$tbl_JingLing_type = array(
	"u-A"	=>	"火",
	"tdg"	=>	"地",
	"t+c"	=>	"风",
	"y64"	=>	"水",
	"sfk"	=>	"冰",
);
$tbl_tong_depot_log = array("","取出","存入","奖励");
?>
