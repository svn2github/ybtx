#!/bin/bash

source ./args

#执行合并
baseindex=${1}
database=ybtx_$baseindex

if [ $baseindex -eq 1 ]
then
	database="ybtx"
fi

if [ $baseindex != 1 ]
then
	echo "UpdateAutoIncreID.sql" >> "$CombineLogFile" 2>&1
	echo "对$database的自增ID进行偏移" >> "$CombineLogFile"
	#用前一个数据库的最大ID加上本数据库的最大ID的值（偏移值）来改变本数据库的对应ID
	cat "$ExcelSqlDir"/UpdateAutoIncreID.sql | mysql -uroot -p$MysqlPasswd $database >> "$CombineLogFile" 2>&1
	errstr=`grep "ERROR" "$CombineLogFile"`
	if [ "$errstr" != "" ]
	then
		cat "$CombineLogFile"
		exit 0
	fi
fi


echo "SaveUserDataToMediTbl.sql" >> "$CombineLogFile" 2>&1
echo "$database用户的某些数据保存到MediaTbl" >> "$CombineLogFile"
cat "$ExcelSqlDir"/SaveUserDataToMediTbl.sql | mysql -uroot -p$MysqlPasswd $database >> "$CombineLogFile" 2>&1
errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
	exit 0
fi

if [ $baseindex != 1 ]
then
	echo "ResetSceneID.sql" >> "$CombineLogFile" 2>&1
	echo "修改$database的所有主场景ID为第一个数据库的主场景ID" >> "$CombineLogFile"
	cat "$ExcelSqlDir"/ResetSceneID.sql | mysql -uroot -p$MysqlPasswd $database >> "$CombineLogFile" 2>&1
	errstr=`grep "ERROR" "$CombineLogFile"`
	if [ "$errstr" != "" ]
	then
		cat "$CombineLogFile"
		exit 0
	fi
fi
