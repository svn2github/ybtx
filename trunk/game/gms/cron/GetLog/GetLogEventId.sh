#!/usr/bin
date
serv=$1
path=/home/ybtx_log/$serv
mkdir -p $path
chmod 777 -R $path
backup=`date +'%Y-%m-%d-%H-%M-%S'`

tar_path=/tmp/ybtx_log/$serv
mkdir -p $tar_path
/usr/bin/find ${path} -name "*.log" -amin +5 -exec /bin/mv {} ${tar_path} \;
mv $path/tbl_log_event*.txt ${tar_path}
cd /tmp/ybtx_log
php /home/ybtxcron/crontab/load_log.php ${tar_path} $serv 
#mv ${tar_path} /home/ybtx_log_bak/${serv}_${backup}
#rm -rf ${tar_path}
date
echo 

