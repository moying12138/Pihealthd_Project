#!/bin/bash
nowtime=`date +"%Y-%m-%d__%H:%M:%S"`
os=`uname -a | cut -d " " -f -1`
neihe=`uname -r`
runtime=`uptime -p | tr -s " " "_"`
fuzai=`uptime | tr ":" "\n" |tail -1`
cipan_a=`df -m | grep -v tmpfs | awk '{print $2}'|sed '1d'|awk '{sum+=$1}'`
cipan_u=`df -m | grep -v tmpfs | awk '{print $2,$3}'|sed '1d'|awk '{sum+=$1;sum2+=$2};END{printf("%d",(sum2*100)/sum);}'`
neicun_a=`free -m | awk '{print $2}' | head -2|tail -1`
neicun_u=`free -m | awk '{printf("%d\n",($3 * 100)/$2)};'|head -2|tail -1`
cpu_tem=`cat /sys/class/thermal/thermal_zone0/temp| awk '{printf("%d",$1/1000)}'`
sign=0
if [[ $cipan_u -lt 80 ]];then      #磁盘
    show_cipan="normal"
elif [[ $cipan_u -lt 90 ]];then
    show_cipan="note"
else
    echo "$nowtime Disk warning Disk usage $cipan_u"
    show_cipan="warning"
fi

if [[ $neicun_u -lt 70 ]];then   #内存
    show_neicun="normal"
elif [[ $neicun_u -lt 80 ]];then
    show_neicun="note"
else
    echo "$nowtime MEM warning MEM usage $neicun_u"
    show_neicun="warning"
fi

if [[ $cpu_tem -lt 50 ]];then
    show_tem="normal"
elif [[ $cpu_tem -lt 70 ]];then
    show_tem="note"
else
    echo "$nowtime cpu warning cpu_temperature  $cpu_tem"
    show_tem="warning"
fi
echo $sign
echo "$nowtime `hostname` $os $neihe $runtime $fuzai $cipan_a ${cipan_u}% $neicun_a ${neicun_u}% $cpu_tem $show_cipan $show_neicun $show_tem"
