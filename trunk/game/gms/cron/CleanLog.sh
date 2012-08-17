#!/usr/bin
source Config.sh
DAY=14
EVENTID=`echo "SELECT MIN(le_uId) FROM tbl_log_event WHERE le_dtDateTime > DATE(DATE_SUB(CURDATE(), INTERVAL ${DAY} DAY))" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log | cut -f1 | tail -n1`
if [ "${EVENTID}" == "NULL" ] || [ "${EVENTID}" == "" ]
then
	echo "Have no log need to be deleted."
	exit 0
fi

echo "All log before id ${EVENTID} will be deleted."

echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_static AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_binding AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_equip_advance AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_equip_durability AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_armor AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_weapon AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_ring AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_shield AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_equip_armor AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_shield_Attr AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_equip_intensify AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_equip_superaddation AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_maker AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
echo "DELETE b FROM tbl_log_item_del AS a, tbl_log_item_equip_intensifyAddAttr AS b WHERE a.lis_uId = b.lis_uId AND a.le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log

TABLES=`echo "SELECT lnd_sName FROM tbl_log_normal_delete" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log | cut -f1 | sed '1d'`
if [ "${TABLES}" == "NULL" ] || [ "${TABLES}" == "" ]
then
	exit 0
fi

for TABLE in ${TABLES}
do
	echo "DELETE FROM ${TABLE} WHERE le_uId < ${EVENTID}" | mysql -h127.0.0.1 -u$dbuser ybtx_game_log
done
