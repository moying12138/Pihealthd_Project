/*************************************************************************
	> File Name: mkdir_test.c
	> Author: moying
	> Mail: 996941856@qq.com
	> Created Time: 2018年08月23日 星期四 10时51分00秒
 ************************************************************************/

#include<stdio.h>
#include"../common.h"

int main() {
    char str[100];
    scanf("%s", str);
    __mkdir_path(str);
    return 0;
}
