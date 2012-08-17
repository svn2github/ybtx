CREATE TABLE `tbl_rush_role_servers` (
	`rrs_uId`	int(10) unsigned NOT NULL AUTO_INCREMENT,
	`rrs_uServerId` int(10) unsigned NOT NULL,
	`rrs_dEndDate` date not null,
	PRIMARY KEY (`rrs_uId`)
) ENGINE=InnoDB;

ALTER TABLE `tbl_forbid_talk` ADD COLUMN `reason` blob NOT NULL;
ALTER TABLE `tbl_forbid_talk` ADD COLUMN `gm` varbinary(32) NOT NULL;

DROP TABLE `tbl_gm_command_log`;
CREATE TABLE `tbl_gm_command_log` (
  `gcl_sGmName` varbinary(32) NOT NULL,
  `gcl_fAction` float(8,4) unsigned NOT NULL,
  `gcl_sReason` text NOT NULL,
  `gcl_tUptime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `gcl_uServerId` int(10) unsigned NOT NULL
) ENGINE=InnoDB;