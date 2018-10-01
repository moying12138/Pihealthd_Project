#!/bin/bash
nowtime=`date +"%Y-%m-%d__%H:%M:%S"`
userc=`cat /etc/group |tr : " " | awk -v sum=0 '{ if ($3 > 999 && $3 < 60000) {sum+=1} } END{print sum}'`

lastuser=`last | cut -d " " -f 1 | head -n -2| sort -u | head -3 | tr '\n' ','`
usermsg=`w | tail -n +3|awk '{printf("%s_%s_%s,", $1, $3,$2)}'`
root=(`cat /etc/group | grep sudo | cut -d : -f 4`)
#eval `cat /home/sudoers | grep 'ALL=(ALL:ALL)' | grep -v %sudo|grep -v '#' |awk '{printf("xx["NR"]=%s", $1)}'`
root+=(${xx[*]})
echo "$nowtime [$userc] [$lastuser] [${root[*]}] [$usermsg]"

