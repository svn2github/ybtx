create table tbl_attribute_panel
(        cs_uId		              bigint unsigned     not  null,	#½ÇÉ«id
         ap_uAttribute1           tinyint unsigned     not null ,   #¿Ø¼ş
         ap_uAttribute2           tinyint unsigned     not null ,    #ÊôĞÔÒ³
	 primary	key(cs_uId),
         foreign	key (cs_uId)	        references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;
   
