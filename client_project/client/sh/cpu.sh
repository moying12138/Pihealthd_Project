#!/bin/bash
nowtime=`date +"%Y-%m-%d__%H:%M:%S"`
a=`cat /proc/stat |head -n 1| cut -d " " -f 6`
b=`cat /proc/stat |head -n 1 | cut -b 6-| tr " " "+"|bc`
c=`cat /proc/loadavg | cut -d " " -f -3`
count=`cat /sys/class/thermal/thermal_zone0/temp`
#echo "$a,$b,$c,$count"
sleep 0.5s
a1=`cat /proc/stat |head -n 1| cut -d " " -f 6`
b1=`cat /proc/stat |head -n 1 | cut -b 6-| tr " " "+"|bc`
x=$(($b1-$b))
y=$(($x-$a1+$a))
#echo "$x $y"
a1=`echo "scale=2;$y*100/$x" | bc`
a1=`printf "%.2f\n" "$a1"`
count=`echo "scale=2;$count/1000"|bc`
count1=`echo ${count%.*}`
#echo $count1
sign=0
if [[ $count1 -lt 50 ]];then
    show="normal"
elif [[ $count1 -lt 70 ]];then
    show="note"
else
    sign=1
    echo "$nowtime cpu warning temperature $count1"
    show="warning"
fi

echo "$sign";
echo "$nowtime $c $a1 $count $show"
