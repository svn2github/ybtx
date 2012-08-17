CREATE TABLE `tbl_rush_role_log`(
	`rrl_dtDateTime` datetime NOT NULL,
	`rrl_uServerId` int(10) unsigned NOT NULL,
	`rrl_uCamp1` int(10) unsigned NOT NULL,
	`rrl_uCamp2` int(10) unsigned NOT NULL,
	`rrl_uCamp3` int(10) unsigned NOT NULL,
	PRIMARY KEY (`rrl_dtDateTime`, `rrl_uServerId`)
) ENGINE=InnoDB;

INSERT INTO `tbl_gm_variable` VALUES('ChannelChatStemp',NOW());