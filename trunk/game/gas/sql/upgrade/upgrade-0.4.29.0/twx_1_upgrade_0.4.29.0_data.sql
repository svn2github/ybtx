

update tbl_item_static set is_sName = "佣兵探险装备包裹" where is_sName = "佣兵探险装备包裹（法系）" and is_uType = 24;

#####清理队伍信息
delete from tbl_team;
