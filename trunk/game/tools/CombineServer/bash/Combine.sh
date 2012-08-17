#!/bin/bash

source ./args

#执行合并
baseindex=${1}
database="ybtx_$baseindex"


echo "RenameUniqueKey.sql" >> "$CombineLogFile" 2>&1
echo "找出ybtx和$database中重复名的佣兵团和角色名，进行重命名" >> "$CombineLogFile"
cat "$ExcelSqlDir"/RenameUniqueKey.sql | mysql -uroot -p$MysqlPasswd $database>> "$CombineLogFile" 2>&1
errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
	exit 0
fi

echo "CombineTable.sql" >> "$CombineLogFile" 2>&1
echo "合并$database的所有表到ybtx" >> "$CombineLogFile"
cat "$ExcelSqlDir"/CombineTables.sql | mysql -uroot -p$MysqlPasswd $database >> "$CombineLogFile" 2>&1
errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
	exit 0
fi

