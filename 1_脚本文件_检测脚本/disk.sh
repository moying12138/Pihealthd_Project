#!/bin/bash
mydate=`date +"%Y-%m-%d__%H:%M:%S "`
disk=("0" "disk" )
y1=0
y2=0
xx=(`df -m  |grep -v tmpfs | tail -2| awk '{print 1,$6,$2,$3,$5}'`)
#echo ${#xx[*]}
n=$((${#xx[*]} - 1))
#echo $n

for i in `seq 0 $n`;do
    if (((i + 1) % 5 == 1)); then
        echo -n $mydate
    fi
    echo -n " ${xx[$i]} "
    if (((i + 1) % 5 == 3)); then
        y1=$(($y1+${xx[$i]}))
    fi
    if (((i + 1) % 5 == 4)); then
        y2=$(($y2+${xx[$i]}))
    fi
    if (((i + 1) % 5 == 0)); then
        echo ""
    fi
done
percsum=`echo  "scale=2;$y2*100/$y1" | bc`
echo "$mydate 0 disk $y1 $y2 ${percsum}%"




