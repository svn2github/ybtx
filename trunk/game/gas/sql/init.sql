#修改心情寄语的存储过程
delimiter //
create procedure SaveShowSentence(in id bigint, in showSen varchar(96))
begin
	declare num bigint default 0;
	
	select count(*) into num from tbl_friends_info where cs_uId = id;
	if num = 0 then
		insert into tbl_friends_info(cs_uId, fi_sFellStatement) values (id, showSen);
	end if;

	if num != 0 then
		update tbl_friends_info set fi_sFellStatement = showSen where cs_uId = id;
	end if;
end; //

create procedure RequestChannelId(in id bigint)
begin
	declare num int default 0;
	#处理重复插入同一键值的错误
	#在mysql里面，重复插入键值错误码为23000
	declare continue handler for sqlstate '23000' set num = -1;
	select count(*) into num from tbl_channel_id where ci_nId = id for update;
	if num = 0 then
		insert into tbl_channel_id(ci_nId) values (id);
		select count(*) into num from tbl_channel_id where ci_nId = id for update;
	end if;
end; //

create procedure RequestChannelName(in name varchar(32))
begin
	declare num int default 0;
	declare continue handler for sqlstate '23000' set num = -1;
	select count(*) into num from tbl_channel_name where cn_sName = name for update;
	if num = 0 then
		insert into tbl_channel_name(cn_sName) values (name);
		select count(*) into num from tbl_channel_name where cn_sName = name for update;
	end if;
end; //

delimiter ;
#以上是数据库存储过程的初始化

delete from tbl_conf_server;
insert into tbl_conf_server 
(sc_sVarName, sc_sVarValue)
values
("AGIP_Version", "32"),
("AGIP_PRTL_IP", "192.168.41.89"),
("AGIP_PRTL_Port", "5091"),
("AGIP_Gateway_id", "601010"),
("AGIP_YYGameID", "7"),
("AGIP_GameID", "8"),
("AGIP_SinaGameID","9"),
("AGIP_Gateway_Code", "gw601010"),
("AGIP_Gateway_Passwd", "5iy6nmaxyjvq61u4rroa71gz140fw50j"),
("AllServerMaxUserNum","7000"),
("SingleServerMaxUserNum","1500"),
("RushRegisterAccountFlag", 0),
("MaxRushRoleNumPerServer", 20000),
("MaxRushRoleNumPerCamp", 7000),
("MaxRushRoleNumPerAccount", 1),
("BuyCouponsWebIP", "172.16.4.42"), 
("BuyCouponsWebPort", "80");

delete from tbl_server;
insert into tbl_server
(s_uId,s_sClientIp,s_sServerIp,s_uClientPort,s_uServerPort)
values
(1,'0.0.0.0','0.0.0.0',52520,52530);

delete from tbl_server_attribute;
insert into tbl_server_attribute
(s_uId,sa_bCanLogin,sa_bNoFbScene,sa_bNoMainScene)
values
(1,1,0,0);

delete from tbl_gm_validate;
insert into tbl_gm_validate
(username,us_uGameID,gv_uLevel)
values
("icek9527",8,100),
("diablosh",8,100);

delete from tbl_web_server_conf;
insert into tbl_web_server_conf
(wsc_sName,wsc_sValue)
values
("GMS_ALLOWED_IP","172.16.4.42"),
("GMS_ALLOWED_IP","172.16.1.163"),
("GMS_SUPPORT_LANG","chinese_simple");

delete from tbl_tong_tech_info_static;
insert into tbl_tong_tech_info_static 
(ttis_uId, ttis_uType, ttis_sName)
values 
(1, 1, "命中提升"),
(2, 1, "韧性提升"),
(3, 1, "生命提升"),
(4, 1, "免爆提升");
