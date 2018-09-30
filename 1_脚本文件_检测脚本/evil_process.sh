#!/bin/bash

max=50
check_count=2
mydate=`date +"%Y-%m-%d__%H:%M:%S"`
for x in `seq 1 $check_count`;do
    eval `ps -aux | awk -v max=$max '{if($3 > max || $4 > max) {printf("pidn["NR"]=%s\n", $2);}}'`
    pidnn+=(${pidn[*]})
    #echo ${pidnn[*]}
    if [[ $x -ne $check_count ]];then
        sleep 5s
    fi
done
#echo `echo ${pidnn[*]} | tr " " '\n' | sort -n | uniq -c`
#echo `echo ${pidnn[*]} | tr " " '\n' | sort -n | uniq -c | awk '{printf("%s %s\n", $1 ,$2);}'`
eval `echo ${pidnn[*]} | tr " " '\n' | sort -n | uniq -c | awk -v count=$check_count '{ if($1 == count ){printf("pid_n["NR"]=%s;", $2);}}'`
for x in ${pid_n[*]};do
    echo -n "$mydate Proc warning danger Proc  "
    echo `ps -aux | awk -v id=$x '{if($2==id){for (i = 11; i<=NF; i++)printf("%s ",$i)}}'`
done
echo ${#pid_n[@]}
for x in ${pid_n[*]};do
    echo -n "$mydate  "
    echo `ps -aux | awk -v x=$x '{if($2 == x){ for(i = 11; i <= NF ;i++) printf("%s ", $i);printf "%s %-12s %-12s%% %-12s%%",x,$1,$3,$4;}}'`
done

