echo "Starting Copy Files================="

echo -n 'PHP files directory`s path.[/home/web_fold/] '
read phpfold
if [ "$phpfold" == "" ]
then
	phpfold="/home/web_fold/"
fi
echo -n 'Apache web files directory`s path.[/var/www/html/] '
read webfold
if [ "$webfold" == "" ]
then
	webfold="/var/www/html/"
fi
echo -n 'Crontab files directory`s path.[/home/ybtxcron/crontab/]'
read cron
if [ "$cron" == "" ]
then
	cron="/home/ybtxcron/crontab/"
fi
echo -n 'Gms language.[chinese] '
read language
if [ "$language" == "" ]
then
	language="chinese"
fi

make install phpfold=$phpfold webfold=$webfold crontab=$cron language=$language

echo ""
echo "Starting Database Config============="

echo -n 'Mysql root password? '
read dbpwd

temp=$(echo "show databases like 'gm_web'" | mysql -uroot -p${dbpwd})
if [ "$temp" != "" ]
then
	echo -n -e 'Database exists, Recreate it?[\033[1mY\033[0m/N] '
	read bool
	if [ "$bool" == "Y" ] || [ "$bool" == "y" ]
	then
		echo "drop database if exists gm_web;create database gm_web" | mysql -uroot -p${dbpwd}
		mysql -uroot -p${dbpwd} gm_web < sql/gmtools.sql
	fi
else
	echo "create database gm_web" | mysql -uroot -p${dbpwd}
	mysql -uroot -p${dbpwd} gm_web < sql/gmtools.sql
fi

echo -n 'Gms Administrator`s Account? '
read admin

action=0
while [ $action -lt 300 ]
do
	echo "insert into tbl_session_action values($action, '$admin')" | mysql -uroot -p${dbpwd} gm_web
	action=$(($action+1))
done

echo ""
echo "Starting Config GMS=============="
echo -n 'Enter to open config file. '
read t
if [ -e "${phpfold}conf/Config.php"
then
	vi ${phpfold}conf/gm_tools/Config.php
else
	cp ${phpfold}conf/gm_tools/Config.php.template ${phpfold}/conf/gm_tools/Config.php
	cp ${phpfold}conf/gm_tools/ServerList.php.template ${phpfold}/conf/gm_tools/ServerList.php
	chmod a+w ${phpfold}/conf/gm_tools/ServerList.php
	vi ${phpfold}conf/gm_tools/Config.php
fi
