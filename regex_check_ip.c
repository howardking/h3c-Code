/**
 * FileName:    regex_check_ip.c
 * Description: 使用正则表达式判断IP地址的正确性。如果正确，则交付到下层使用。
 * Date:        2015-6-22
 * Author:      王帅华
 * Version:     V0.2
 * */


#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IP_PATTERN "^([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5])$"

void read_ip_from_file(char *fp, char *mac);
int check_ip(char *argv);
static void regex_emsg(int errorcode, regex_t *preg);


regmatch_t match[5]; /* 存放正则表达式匹配结果 */


/*int main(int argc, char *argv[])
{


	if(argc == 1 ){
		printf("Usage: regex_check_ip <ip> <mac>\n");
		exit(-1);
	}

	

	return 0;
}*/

int main(int argc, char *argv[])
{
	
	char *old_ip = NULL;
	char *path = NULL;
	char *mac = NULL;
	int result = 0;
	char prefixip[8]; /* 使用8的原因是前两段的最大长度为7+\0=8,正好Linux系统字节对齐提高效率 */

	if(argc <= 2){
	/*
	 * 当应用程序参数少于一个时,程序并不能执行，因为查询交换机需要源IP地址，
	 * 之后的操作需要mac地址。
	 */
		printf("Usage: regex_check_ip <ip> <mac>\n");
		exit(-1);
		
	}else if(argc >= 3){
	/*
 	 * 当多于两个参数时，现阶段只使用第一个参数（IP地址）和第二个参数（mac）。
 	 */
		old_ip = argv[1];
		mac = argv[2];
	}/*else {
		printf("此程序尚不支持多个IP地址列表的情况，请重新输入！\n");
		exit(1);
	}*/
	
	result = check_ip(old_ip);

	if (result == 1){
		/**
		 * 输入的IP地址合法
		 **/
		strncpy(prefixip, old_ip, match[2].rm_eo - match[1].rm_so);
		/*strcat(prefixip, ".");
		strcat(prefixip, old_ip + match[2].rm_so);

		printf("match[1].rm_so: %s\n", old_ip + match[1].rm_so);
		printf("match[1].rm_eo: %s\n", old_ip + match[1].rm_eo);
		printf("match[2].rm_so: %s\n", old_ip + match[2].rm_so);
		printf("match[2].rm_eo: %s\n", old_ip + match[2].rm_eo);*/

		printf("IP前缀为：%s\n", prefixip);

		if (strcmp(prefixip,"10.9") == 0){
			path = "M6ip.list";
		}else if(strcmp(prefixip, "10.5") == 0){
			path = "tj103ip.list";
		}else if (strcmp(prefixip, "192.168") == 0){
			path = "dhjip.list";
		}else if (strcmp(prefixip, "10.4") == 0){
			path = "tj101ip.list";
		}
		
	}

	read_ip_from_file(path, mac);
	

	return 0;
}

void read_ip_from_file(char *fp, char *mac)
{ 	/*
	 *从文件中读取IP地址
	 */
	char ipaddress[16];
	FILE *pfread = NULL;
	int result = 0;
	if (fp != NULL){
		pfread = fopen(fp, "r");
		if (pfread == NULL){
			perror(fp);
			exit(2);
		}
		while(fgets(ipaddress, 20, pfread) != NULL){
			result = check_ip(ipaddress);
			if (result == 1){
			/**
			 * ip合法，则执行李钊提供的接口继续进行处理。
			 */
				printf("lizhao, I am here:mac %s\n\n", mac);
			}
		}
		fclose(pfread);
	}
}

int check_ip(char *argv)
{
	/*
	 * 该函数负责判断IP地址是否合法
 	 *
	 *
 	 * 使用正则表达式的步骤为（详情请可参照：http://c.biancheng.net/cpp/html/1428.html）：
	 * (1) regcomp() 编译正则表达式，main函数中调用 
	 * (2) regexec() 匹配正则表达式，check_ip函数调用
	 * (3) regfree() 释放正则表达式，main函数中调用
 	 */
	int retval;
	regex_t reg;

	/**
 	 * 编译正则表达式
 	 */
	retval = regcomp(&reg, IP_PATTERN, REG_EXTENDED | REG_NEWLINE);
	if (retval != 0){ 
		regex_emsg(retval, &reg);
	}

	/**
	 * 执行正则表达式
 	 *
 	 */
	retval = regexec(&reg, argv, sizeof match / sizeof match[0], match, 0);
	printf("%s is %s\n", argv, retval == 0 ? "legal" : "illegal");
	
	/**
 	 * 释放正则表达式
 	 */
	regfree(&reg);

	/*
	 * 如果是正确的ip， 输出详细的匹配过程,后续的对交换机的处理工作，可以在该if语句中实现。
	 * 修改后如果IP地址符合IP规则，则返回1，否则返回0，去掉打印消息。
	 */

	if (retval == 0) {
        /*int i;
        for (i = 1; i < sizeof match / sizeof match[0]; i++) {
        	printf("ip part[%d]: %.*s\n", i, match[i].rm_eo - match[i].rm_so,
                   argv + match[i].rm_so);
    	}*/
		/**
		 *返回1表示IP地址表示格式合法
		 */
		return 1; 
    }
	/**
	 * 返回0表示IP地址不合法
	 */
	return 0;
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

