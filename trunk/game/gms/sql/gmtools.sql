CREATE TABLE `tbl_channel_chat` (
  `Name` char(32) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `Msg` text NOT NULL,
  `ChatTime` datetime NOT NULL,
  `ChannelName` varchar(96) NOT NULL,
  `ServerId` int(10) unsigned NOT NULL,
  `cc_sUser` varchar(32) NOT NULL
) ENGINE=InnoDB;

CREATE TABLE `tbl_channel_word` (
  `word` varbinary(50) NOT NULL,
  PRIMARY KEY (`word`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_daily_money` (
  `dm_dDate` date NOT NULL,
  `dm_uEventType` smallint(5) unsigned NOT NULL,
  `dm_uMoneyType` tinyint(4) NOT NULL,
  `dm_uLevel` tinyint(3) unsigned NOT NULL,
  `dm_uServerId` int(10) unsigned NOT NULL,
  `dm_uMoney` bigint(20) NOT NULL,
  `dm_uCount` bigint(20) NOT NULL,
  `dm_uCharId` bigint(20) unsigned NOT NULL,
  `dm_uCharCamp` tinyint(3) unsigned NOT NULL
) ENGINE=InnoDB;

CREATE TABLE `tbl_gm_anousment` (
  `ga_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `begin_time` datetime NOT NULL,
  `end_time` datetime NOT NULL,
  `step` int(10) unsigned NOT NULL,
  `serverid` int(10) unsigned NOT NULL,
  `title` blob NOT NULL,
  `content` blob NOT NULL,
  `state` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ga_id`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_gm_command_log` (
  `gcl_sGmName` varbinary(32) NOT NULL,
  `gcl_fAction` float(8,4) unsigned NOT NULL,
  `gcl_sReason` text NOT NULL,
  `gcl_tUptime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `gcl_uServerId` int(10) unsigned NOT NULL
) ENGINE=InnoDB;

CREATE TABLE `tbl_gm_compensate_action` (
  `gca_uId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `gm` varbinary(32) NOT NULL,
  `action_id` int(10) unsigned NOT NULL,
  `server_id` int(10) unsigned NOT NULL,
  `action_state` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `role_name` blob,
  `arg1` blob,
  `arg2` blob,
  `reason` blob NOT NULL,
  PRIMARY KEY (`gca_uId`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_gm_variable` (
  `gv_name` varbinary(32) NOT NULL,
  `gv_value` varbinary(32) NOT NULL,
  PRIMARY KEY (`gv_name`)
) ENGINE=InnoDB;

INSERT INTO `tbl_gm_variable` VALUES('ChannelChatStemp',NOW());

CREATE TABLE `tbl_hot_update` (
  `hu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `hu_type` int(11) NOT NULL,
  `hu_code` blob,
  `hu_file` varbinary(255) DEFAULT NULL,
  `hu_datetime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`hu_id`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_hot_update_result` (
  `hu_id` int(10) unsigned NOT NULL,
  `hur_serverid` int(11) NOT NULL,
  `hur_state` int(11) NOT NULL DEFAULT '0',
  `hur_datetime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`hu_id`,`hur_serverid`),
  CONSTRAINT `tbl_hot_update_result_ibfk_1` FOREIGN KEY (`hu_id`) REFERENCES `tbl_hot_update` (`hu_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB;

CREATE TABLE `tbl_output_money_warning` (
  `DateTime` datetime NOT NULL,
  `ServerId` int(10) unsigned NOT NULL,
  `CharId` bigint(20) unsigned NOT NULL,
  `EventType` smallint(5) unsigned DEFAULT NULL,
  `Money` bigint(20) unsigned NOT NULL,
  `Count` int(10) unsigned NOT NULL
) ENGINE=InnoDB;

CREATE TABLE `tbl_server_money_warning` (
  `DateTime` datetime NOT NULL,
  `ServerId` int(10) unsigned NOT NULL,
  `CharCount` bigint(20) unsigned NOT NULL,
  `Money` bigint(20) unsigned NOT NULL,
  `BindingMoney` bigint(20) unsigned NOT NULL
) ENGINE=InnoDB;

CREATE TABLE `tbl_session` (
  `UID` varbinary(128) NOT NULL,
  `gm_name` varbinary(32) NOT NULL,
  `sess_time` datetime NOT NULL,
  PRIMARY KEY (`UID`),
  KEY `gm_name` (`gm_name`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_session_action` (
  `action_id` float(10,2) NOT NULL,
  `gm_name` varbinary(32) NOT NULL,
  KEY `gm_name` (`gm_name`),
  KEY `action_id` (`action_id`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_talk_log` (
  `role_name` char(32) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `last_update` datetime NOT NULL,
  `serverid` int(11) NOT NULL,
  PRIMARY KEY (`role_name`,`serverid`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_forbid_talk` (
	`id` int(20) unsigned NOT NULL AUTO_INCREMENT,
	`role_name` char(32) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
	`channal_id` smallint(5) unsigned NOT NULL,
	`end_date` datetime NOT NULL,
	`server_id` int(10) unsigned NOT NULL,
	`reason` blob NOT NULL,
	`gm` varbinary(32) NOT NULL,
	PRIMARY KEY (`id`),
	KEY `end_date` (`end_date`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_gm_tools_notice` (
	`gmn_uId` int(20) unsigned NOT NULL AUTO_INCREMENT,
	`gmn_sTitle` blob NOT NULL,
	`gmn_sContent` blob NOT NULL,
	`gmn_dDate` date not null,
	`gms_sName` varbinary(32) NOT NULL,
	PRIMARY KEY (`gmn_uId`),
	FOREIGN KEY `gms_sName`(`gms_sName`) REFERENCES `tbl_session`(`gm_name`) ON DELETE NO ACTION ON UPDATE CASCADE
) ENGINE=InnoDB;

CREATE TABLE `tbl_gm_tools_notice_unread` (
	`gmn_uId` int(20) unsigned NOT NULL,
	`gms_sName` varbinary(32) NOT NULL,
	FOREIGN KEY `gms_sName`(`gms_sName`) REFERENCES `tbl_session`(`gm_name`) ON DELETE CASCADE ON UPDATE CASCADE,
	FOREIGN KEY `gmn_uId`(`gmn_uId`) REFERENCES `tbl_gm_tools_notice`(`gmn_uId`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB;

#角色抢注服务器
CREATE TABLE `tbl_rush_role_servers` (
	`rrs_uId`	int(10) unsigned NOT NULL AUTO_INCREMENT,
	`rrs_uServerId` int(10) unsigned NOT NULL,
	`rrs_dEndDate` date not null,
	PRIMARY KEY (`rrs_uId`)
) ENGINE=InnoDB;

#合作伙伴销售
CREATE TABLE `tbl_joint_sales_items`(
	`jsi_uId` int(10) unsigned NOT NULL AUTO_INCREMENT,
	`jsi_uPartnerId` smallint(5) unsigned NOT NULL default 1,
	`jsi_sItemName` varchar(96) NOT NULL,
	`jsi_sTooltips` text NOT NULL,
	`jsi_uSmallIcon` smallint(5) NOT NULL default 0,
	`jsi_sURL` text NOT NULL,
	`jsi_uPrice` int(10) unsigned NOT NULL,
	`jsi_uState` enum('0','1') default '0',
	PRIMARY KEY (`jsi_uId`),
	KEY (`jsi_sItemName`),
	KEY (`jsi_uState`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_joint_sales_sn`(
	`jsi_uId` int(10) unsigned NOT NULL,
	`jss_sSN` varchar(255) NOT NULL,
	PRIMARY KEY (`jsi_uId`, `jss_sSN`),
	KEY(`jsi_uId`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_joint_sales_log`(
	`jsl_uId` int(10) unsigned NOT NULL AUTO_INCREMENT,
	`jsi_uId` int(10) unsigned NOT NULL,
	`jss_sSN` varchar(255) NOT NULL,
	`jsl_uServerId` int(10) unsigned NOT NULL,
	`jsl_uUser` varchar(32) NOT NULL,
	`jsl_uChar` varchar(32) NOT NULL,
	`jsl_dtDateTime` datetime NOT NULL,
	PRIMARY KEY (`jsl_uId`),
	KEY (`jsi_uId`)
) ENGINE=InnoDB;

CREATE TABLE `tbl_rush_role_log`(
	`rrl_dtDateTime` datetime NOT NULL,
	`rrl_uServerId` int(10) unsigned NOT NULL,
	`rrl_uCamp1` int(10) unsigned NOT NULL,
	`rrl_uCamp2` int(10) unsigned NOT NULL,
	`rrl_uCamp3` int(10) unsigned NOT NULL,
	PRIMARY KEY (`rrl_dtDateTime`, `rrl_uServerId`)
) ENGINE=InnoDB;