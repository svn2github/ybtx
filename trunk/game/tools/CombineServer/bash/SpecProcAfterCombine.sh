#!/bin/bash

source ./args

echo "ResetUserBasicInfo.sql" >> "$CombineLogFile" 2>&1
echo "合并完后重新设置用户数据" >> "$CombineLogFile"

cat "$ExcelSqlDir"/ResetUserBasicInfo.sql | mysql -uroot -p$MysqlPasswd ybtx >> "$CombineLogFile" 2>&1
errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
	exit 0
fi

echo "ResetCharTime.sql" >> "$CombineLogFile" 2>&1
echo "重置角色登陆和删除时间" >> "$CombineLogFile"

cat "$ExcelSqlDir"/ResetCharTime.sql | mysql -uroot -p$MysqlPasswd ybtx >> "$CombineLogFile" 2>&1
errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
	exit 0
fi


