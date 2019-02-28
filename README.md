## 集群健康监控系统的设计与实现
<hr>


[TOC] 
###项目实现目标 
<hr>
实时监控树莓派集群的多项健康指标,并对可能出现的故障进行预警。  
###架构 
####Master 
Master为管理理端,负责向Client端收集数据。   
####Client
Client作为客户端,运行行行在树莓派集群的各节点上,负责监控当前节点的健康状态,并向Master汇报
数据。    
>紧急状况,Client应能主动向Master告警。    
####LogManger
LogManger负责在各节点上收集,生生成,管理理重要指标数据的日日志。    
###需要监控的内容 
1. 系统运行概况,uname,uptime,last等等,内存硬件信息,磁盘温度(备选)    
2. CPU    
3. MEM    
4. Disk    
5. 进程(恶意进程)    

####MEM
1. 总量   
2. 已用   
|时间 |总量  |剩余量 | 当前占用(%)  |占用百分比 |
| :-------- | :-------- | --------:| :--: |
|2018-06-12__15:00:28  |1023M  |200M | 80%  |60% |

####DISK
1. 磁盘总量
2. 磁盘剩余量
3. 占用比例
1. 各分区占用比例
2. 各分区总量
3. 分区剩余情况
2018-01-12__16:48:23 标识(0为整个磁盘,1为分区)   
磁盘还是分区(disk| /boot , /) 磁盘/分区总量 磁盘/   
分区剩余量 占用比例   
####恶意进程
1. 被标记   
1. CPU %50   
|| MEM 50%   
2. warning   
1. 5   
2. 提交   
时间 进程(前20字符) PID 所有者 CPU占用(%) MEM占用(%)   
####CPU
1. 温度
2. 占用率
时间 负载1(1分钟) 负载2(5分钟) 负载3(15分钟) 占用用率 (时间间隔0.5) 当前温度 警告
(normal,note(50-70),warning(70~))
####系统运行概况
时间 主机名 OS版本 内核版本 运行时间 平均负载 磁盘总量 磁盘
已用% 内存大小 内存已用% CPU温度 磁盘报警级别 内存报警级
别 CPU报警级别
####用户统计
时间 用户总数(非系统用户)近期活跃用户(3个) 具时间 用户总数(非系统用户)近期活跃用户(3个) 具
有ROOT权限用户 当前在线用户_登录IP_TTY
[name_IP_TTY,sd_ip_tty]
[name,pi2]
###项目要求

####完成基本功能:
1. Master采用并行处理各节点，需要支持10000台树莓派机器
2. Client遇到Warning级别的报警， 需要马上向Master报警
3. 可自定义(迁移，扩展)的参数，  存放在配置文件中
4. Master收到数据后根据来源主机， 分别存放在不同的文件夹下。
5. 在Master和Client运行的过程中， 需要将主要的运行信息打印到一个日志文件中， 用于运维操作.
6.  实现日志功能
      - Master需要将自己的运行日志， 记录到/var/log/pihealthd.log
      - 日志包括：启动， 运行出错及出错说明， 链接客户端的地址， 接受数据存放位置
      - 每条日志信息， 包括时间，时间， 提示性说明(错误， 警告等)
7. 实现守护进程， Client按照时间对系统进行检测， 尽量减少对磁盘的真实读写
8. 当client端文件大于30M时， 对文件进行压缩， 下次传送时， 首先传送压缩的文件
####报警级别


|type|normal |note | warning |
| :-------- | :-------- | --------:| :--: |
 | 磁盘(百分比)  | 0-80  |80-90 | 90-100 |
 | CPU(百分比)  | 0-70  |70-80  |80-100 |
 | MEM(百分比) | 0-70 | 70-80 | 80-100 |

      
