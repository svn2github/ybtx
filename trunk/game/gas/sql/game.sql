################################################################
#
#		表格命名规则:
#
#			@.必须以‘tbl_’作为前缀
#			@.全小写
#			@.单词之间以下划线区分
#			@.tbl_前缀后的第一个单词代表表中一行数据的含义。例如，tbl_char_static, 前缀
#				之后第一个单词是'char',说明表中每一行数据都是一个角色
#			@.sql语法关键字全部用小写
#
#
#		字段命名规则:
#
#			普通字段：
#				@.以表名称的所有单词的首字母和'_'再加一个代表类型的字母作为前缀
#
#			外键字段：
#				@.如果某个字段是其他表（表a)的外键,则该字段的前缀
#					不得使用当前表单词首字母的缩写组合，而应该使表a的单词首字母的缩写组
#					合作为前缀,以突出表的外键联系
#
#		更多注意的内容：
#			@.存储时间和日期的字段务必使用datetime类型，不要使用int	unsigned(ctime)
#
################################################################


source server.sql

source scene.sql

source user.sql

source char.sql

source skill.sql

source liveskill.sql

source item.sql

source toolbar.sql

source mail.sql

source relation.sql

source quest.sql

source market.sql

source tong.sql

source gm_command.sql

source sort.sql

source scene_fb.sql

source team.sql

source player_panel.sql

source battle.sql

source association.sql

source channel.sql


source area.sql

source setting.sql

source war_zone.sql


source direct.sql

source novice_direct.sql

source multiserver.sql

source pet.sql

source user_log.sql

source toolsmall.sql

source tong_rob.sql
source army_corps.sql
source erating.sql
source revenue.sql


#######数据库初始化放在所有table创建完之后，所以下面一行放在最后
source init.sql
