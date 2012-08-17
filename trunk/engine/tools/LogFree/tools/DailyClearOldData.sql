/bin/echo "
insert 	into 
	tbl_client_log_common_back 
select * 
from 
	tbl_client_log_common 
where 
	happened_time < date(date_add(now(), interval -7 day));


delete 
from 
	tbl_client_log_common 
where 
	happened_time < date(date_add(now(), interval -7 day));


insert 
	into tbl_server_log_common_back 
select * 
from 
	tbl_server_log_common 
where happened_time < date(date_add(now(), interval -7 day));


delete 
from 
	tbl_server_log_common 
where 
	happened_time < date(date_add(now(), interval -7 day));

update
	tbl_guid_info
set
	is_special_client=0;

" | /usr/local/bin/mysql -uroot -p1 LogFree -h10.10.43.142
