#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IP_PATTERN "^([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5])$"

void read_ip_from_file(char *fp);
int check_ip(char *argv);
static void regex_emsg(int errorcode, regex_t *preg);

int main(int argc, char *argv[])
{
	char *path = NULL;
	if(argc == 1){
	/*当应用程序没有参数时，使用默认的ip.list文件作为ip地址列表*/
		path = "ip.list";
	}else if(argc == 2){
	/*当有一个参数时，使用输入的参数作为IP地址列表*/
		path = argv[1];
	}else {
		printf("此程序尚不支持多个IP地址列表的情况，请重新输入！\n");
		exit(1);
	}
	read_ip_from_file(path);
	return 0;
}

void read_ip_from_file(char *fp)
{
	char ipaddress[20];
	FILE *pfread = NULL;
	if (fp != NULL){
		pfread = fopen(fp, "r");
		if (pfread == NULL){
			perror(fp);
			exit(2);
		}
		/*fgets(ipaddress, 20, pfread);*/
		/*printf("ip address length : %d\n", strlen(ipaddress));*/
		while(fgets(ipaddress, 20, pfread) != NULL){
		printf("ip address length : %d\n", strlen(ipaddress));
			check_ip(ipaddress);
			/*fgets(ipaddress, 20, pfread);
			printf("ip address length : %d\n", strlen(ipaddress));*/
		}
	
	}
}

int check_ip(char *argv)
{
	regex_t reg;
	regmatch_t match[5];
	int retval;

	/*if (argc != 2) {

		fprintf(stderr, "usage: %s <ipstring>\n"
		"example: %s 1.2.3.4\n",argv[0], argv[0]);

		exit(EXIT_FAILURE);
	}*/

	retval = regcomp(&reg, IP_PATTERN, REG_EXTENDED | REG_NEWLINE);
	if (retval != 0) 
		regex_emsg(retval, &reg);

	retval = regexec(&reg, argv, sizeof match / sizeof match[0], match, 0);
	printf("%s is %s\n", argv, retval == 0 ? "legal" : "illegal");
	regfree(&reg);

	/*
	 * 如果是正确的ip， 输出详细的匹配过程
	 */

	if (retval == 0) {
        	int i;
        	for (i = 1; i < sizeof match / sizeof match[0]; i++) {
        	printf("ip part[%d]: %.*s\n", i, match[i].rm_eo - match[i].rm_so,
                   argv + match[i].rm_so);
       }
    }

	return 1;
    /*exit(EXIT_SUCCESS);*/
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

