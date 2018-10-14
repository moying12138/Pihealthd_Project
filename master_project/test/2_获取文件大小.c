/*************************************************************************
	> File Name: 2_获取文件大小.c
	> Author: moying
	> Mail: 996941856@qq.com
	> Created Time: 2018年09月10日 星期一 11时24分09秒
 ************************************************************************/

#include<stdio.h>
#include<sys/stat.h>
int get_file_size(char *path) {
    struct stat st;
    stat(path, &st);
    long size = st.st_size;
    return size / 1000000;
}
int main() {
    printf("%d\n", get_file_size("/home/moying12138/3_数据结构作业/2_二叉字典树_哈夫曼编码/input"));
    return 0;
}
