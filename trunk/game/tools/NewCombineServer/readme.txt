配置表1：auto_increment.txt 记录需要进行偏移的有自增ID的表
	这个表还是手工产生，因为会有一些特殊的表：
	tbl_item_static没有自增id，但是需要偏移；
	tbl_scene,tbl_war_zone有自增id，但是因为只需要保留一区的，所有不需要偏移；

配置表2：ClearTables.txt 记录需要清空的表，手工记录

配置表3：MainSceneName.txt 记录主场景名，其他非主场景都可以在合服时删掉

配置表4：AllTables.txt 按顺序记录需要合并的表,这个配置表现在改为由脚本分析产生，如果行不通再改为手工输入
	
配置表5：KeepMainDbTables.txt 记录只保留一区的表，即把后面区的表清空即可
