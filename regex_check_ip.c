/**
 * FileName:    regex_check_ip.c
 * Description: 使用正则表达式判断IP地址的正确性。如果正确，则交付到下层使用。
 * Date:        2015-6-22
 * Author:      王帅华
 * Version:     V0.1
 * */


#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IP_PATTERN "^([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5])$"

void read_ip_from_file(char *fp);
int check_ip(char *argv);
static void regex_emsg(int errorcode, regex_t *preg);


int retval;
regex_t reg;

int main(int argc, char *argv[])
{
	
	/*
 	 * 使用正则表达式的步骤为（详情请可参照：http://c.biancheng.net/cpp/html/1428.html）：
	 * (1) regcomp() 编译正则表达式，main函数中调用 
	 * (2) regexec() 匹配正则表达式，check_ip函数调用
	 * (3) regfree() 释放正则表达式，main函数中调用
 	 */
	char *path = NULL;

	if(argc == 1){
	/*
	 * 当应用程序没有参数时，使用默认的ip.list文件作为ip地址列表
	 */
		path = "ip.list";
	}else if(argc == 2){
	/*
 	* 当有一个参数时，使用输入的参数作为IP地址列表
 	*/
		path = argv[1];
	}else {
		printf("此程序尚不支持多个IP地址列表的情况，请重新输入！\n");
		exit(1);
	}
	
	/**
 	 * 编译正则表达式
 	 */
	retval = regcomp(&reg, IP_PATTERN, REG_EXTENDED | REG_NEWLINE);
	if (retval != 0){ 
		regex_emsg(retval, &reg);
	}

	read_ip_from_file(path);
	
	/**
 	 * 释放正则表达式
 	 */
	regfree(&reg);

	return 0;
}

void read_ip_from_file(char *fp)
{ 	/*
	 *从文件中读取IP地址
	 */
	char ipaddress[20];
	FILE *pfread = NULL;
	if (fp != NULL){
		pfread = fopen(fp, "r");
		if (pfread == NULL){
			perror(fp);
			exit(2);
		}
		while(fgets(ipaddress, 20, pfread) != NULL){
			check_ip(ipaddress);
		}
	
	}
}

int check_ip(char *argv)
{
	/*
	 * 该函数负责判断IP地址是否合法
 	 */
	regmatch_t match[5];

	/**
	 * 执行正则表达式
 	 *
 	 */
	retval = regexec(&reg, argv, sizeof match / sizeof match[0], match, 0);
	printf("%s is %s\n", argv, retval == 0 ? "legal" : "illegal");

	/*
	 * 如果是正确的ip， 输出详细的匹配过程,后续的对交换机的处理工作，可以在该if语句中实现。
	 */

	if (retval == 0) {
        	int i;
        	for (i = 1; i < sizeof match / sizeof match[0]; i++) {
        	printf("ip part[%d]: %.*s\n", i, match[i].rm_eo - match[i].rm_so,
                   argv + match[i].rm_so);
       }
    }

	return 1;
}

/*

 * 输出regexXXX调用出错消息

 */

static void regex_emsg(int errorcode, regex_t *preg)
{
    char msg[512] = {'\0'};

    regerror(errorcode, preg, msg, sizeof msg - 1);
    fputs(msg, stderr);
    fputc('\n', stderr);

   return;

}

