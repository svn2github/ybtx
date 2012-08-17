set autocommit=0;


insert into tbl_IBPay_log(tid_uDetailID,tid_uSubjectID,cs_uId,cs_uLevel,tid_sIP,tid_IBCode,tid_uBuyCount,tid_uPrice,tid_dtBuyTime,tid_uIBCodeType)
select tid_uDetailID,tid_uSubjectID,cs_uId,cs_uLevel,tid_sIP,tid_IBCode,tid_uBuyCount,tid_uPrice,tid_dtBuyTime,tid_uIBCodeType
from tbl_IBPay_detail where tid_uState = 1;

delete from tbl_IBPay_detail where tid_uState = 1;

insert into tbl_money_exchange_log(tid_uDetailID,tme_uSubjectID,cs_uId,cs_uTargetID,tme_uAmount,tme_dtTime)
select tid_uDetailID,tme_uSubjectID,cs_uId,cs_uTargetID,tme_uAmount,tme_dtTime 
from tbl_money_exchange where tme_uState = 1;

delete from tbl_money_exchange where tme_uState = 1;

delete from tbl_item_amount_limit;


commit;

insert into tbl_database_upgrade_record values("lihongling_2_upgrade-0.5.7_data.sql");

