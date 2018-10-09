#!/bin/bash

echo  -n `date +"%Y-%m-%d__%H:%M:%S "`

#xx=`free -m |head -2|tail -1| awk '{printf("%s %s\n",$2,$3)}'`
eval `free -m |head -2|tail -1| awk '{printf("a1=%s;a2=%s\n",$2,$3)}'`
#a1=`echo $xx|cut -d " " -f -1`
#a2=`echo $xx|cut -d " " -f 2-`
a3=`echo "scale=2;$a2*100/$a1" | bc`
echo  " ${a1}M ${a2}M ${a3}%" 




