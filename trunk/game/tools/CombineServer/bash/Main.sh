#!/bin/bash

source ./args

if [ -e $CombineLogFile ]
then
	rm -f "$CombineLogFile"
fi

declare -a dumpfilelist=(${*})
dumpfilecount=${#}


#检查参数个数，至少需要两个要合并的数据库dump文件
if [ $dumpfilecount -lt 2 ]
then
	echo "usage:bash Main.sh dumpfile1 dumfile2 dumpfile3"
	echo "将dumpfile2,dumpfile3数据合并到dumpfile1上去，最终数据库名为ybtx"
	echo "ERROR:检查参数个数，至少需要两个要合并的数据库dump文件" > "$CombineLogFile"
	cat "$CombineLogFile"
	exit 0
fi

#检查参数是否为文件，是否存在
num=1
while [ $num -le $dumpfilecount ]
do
	dumpfile=${dumpfilelist[${num}-1]}
	if [ ! -e "$dumpfile" ]
	then
		echo "ERROR:要合并的数据库的dump文件不存在"
		cat "$CombineLogFile"
		exit 0
	fi
	let num=num+1
done

echo "Main.sh"
date
date > "$CombineLogFile"

#创建临时中介数据库medi_database和中介表
echo "创建临时中介数据库medi_database和中介表"
echo "创建临时中介数据库medi_database和中介表" >> "$CombineLogFile"
cat "$ExcelSqlDir"/CreateMediTbl.sql | mysql -uroot -p$MysqlPasswd >> "$CombineLogFile" 2>&1

errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
	exit 0
fi

#初始化中介数据库中的某些中介表
echo "初始化中介数据库中的某些中介表"
echo "初始化中介数据库中的某些中介表" >> "$CombineLogFile"
num=1
while [ $num -le $dumpfilecount ]
do
	database="ybtx_$num"
	if [ $num -eq 1 ]
	then
		database="ybtx"
	fi
	
	#初始化tbl_id_add_count表
	echo "insert into tbl_id_add_count(ian_sBaseName) values(\"$database\");" | mysql -uroot -p$MysqlPasswd medi_database >> "$CombineLogFile" 2>&1

	#初始化tbl_base_Id表
	echo "insert into tbl_base_Id(bi_sBaseName,bi_uId) values(\"$database\",$num)" | mysql -uroot -p$MysqlPasswd medi_database > "$CombineLogFile" 2>&1

	let num=num+1
done

errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
        cat "$CombineLogFile"
        exit 0
fi

#合并前的准备
echo "PrepareCombine.sh"
echo "  将第一个dump文件导入最终数据库ybtx，后面的分别导入到ybtx_2,ybtx_3...."
echo "  清空合服后不需要数据的表"
echo "  保存数据库有自增id的最大值，作为其他数据库偏移的偏移量"
num=1
while [ $num -le $dumpfilecount ]
do
	dumpfile=${dumpfilelist[${num}-1]}
	bash PrepareCombine.sh "$dumpfile" "$num" >> "$CombineLogFile" 2>&1 &
	let num=num+1
done
wait

errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
        exit 0
fi

#某些数据需要特殊处理
echo "SpecialProcess.sh"
echo "  对数据库ybtx_2,ybtx_3...进行自增id的偏移"
echo "  将用户的某些数据库保存到medi_database"
echo "  修改ybtx_2,ybtx_3...所有主场景id为ybtx的主场景id"
num=1
while [ $num -le $dumpfilecount ]
do
	bash SpecialProcess.sh "$num" >> "$CombineLogFile" 2>&1 &
	let num=num+1
done
wait

errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
        exit 0
fi

#进行数据库合并，把后面的数据库都合并到第一个数据库上去
echo "Combine.sh"
echo "  进行数据库合并，把后面的数据库都合并到第一个数据库上去"
echo "  把ybtx_2,ybtx_3...中跟ybtx重名的佣兵团名和角色名进行重命名"
echo "进行数据库合并，把后面的数据库都合并到第一个数据库上去" >> "$CombineLogFile"
num=2
while [ $num -le $dumpfilecount ]
do
	bash Combine.sh "$num" >> "$CombineLogFile"  2>&1 &
	let num=num+1
done
wait

errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
        exit 0
fi

#合并后的一些处理
echo "SpecProcAfterCombine.sh"
echo "  合并完后重新设置ybtx的用户数据"
echo "  重置角色登录和删除时间"
bash SpecProcAfterCombine.sh >> "$CombineLogFile" 2>&1

errstr=`grep "ERROR" "$CombineLogFile"`
if [ "$errstr" != "" ]
then
	cat "$CombineLogFile"
        exit 0
fi

echo "数据库合并完毕！"
echo "数据库合并完毕！" >> "$CombineLogFile"
date
date >> "$CombineLogFile"

