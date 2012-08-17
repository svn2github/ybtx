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

ALTER TABLE `tbl_channel_chat` ADD COLUMN `cc_sUser` varchar(32) NOT NULL;