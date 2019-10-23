#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>

char flag_exit = '0';
char flag_log = '0';

FILE* fplog_cmd; 
FILE* fplog_out;

char* inputConsole()
{
	char *str;
	size_t bufsiz= 200;

	str = (char* )malloc(sizeof(char)*201);
	getline(&str, &bufsiz, stdin);

	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] == '\n')
		{
			str[i] = '\0';
			break;
		}
	}
	return str;
}

char* firstword(char* str)
{
	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] == ' ')
		{
			str[i] = '\0';
			break;
		}
	}
	return str;
}

char* cwd()
{
	char* str = (char* )malloc(100*sizeof(char));
	getcwd(str, 100);	
	return (char*)str;
}

int main()
{
	fplog_cmd = fopen("commands.log", "a+");
	fplog_out = fopen("output.log", "a+");
	while(1)
	{
		char* str = cwd();
		printf(" %s:$ ", str);
		free(str);
		char* inp_st = inputConsole();

		if(strcmp(inp_st, "enter") == 0)
		{
			while(1)
			{
				char* action = "Success";
				char* str = cwd();
				printf(" %s:$ ", str);
				free(str);

				//Command Interpretor starts, flag for exit command required
				char* inp = inputConsole();
				if(flag_log == '1')
				{
					fprintf(fplog_cmd, "%s", inp);
					time_t T = time(NULL);
					struct tm tm = *localtime(&T);
					fprintf(fplog_cmd, "\t%02d/%02d/%04d", tm.tm_mday, tm.tm_mon, tm.tm_year+1900);
					fprintf(fplog_cmd, "\t%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

				}
				if(strcmp(inp, "exit") == 0)
				{
					flag_exit = '1';	
					break;
				}
				else if(strcmp(inp, "log") == 0)
				{
					flag_log = '1';
				}
				else if(strcmp(inp, "unlog") == 0)
				{
					flag_log = '0';
				}
				else if(strcmp(inp, "viewcmdlog") == 0)
				{
					//show cmdlog from file command.log
					fseek(fplog_cmd, 0, SEEK_SET);
					char ch;
					fprintf(fplog_out, "\n[%s]\n", inp);
					while((ch = fgetc(fplog_cmd)) != EOF)
					{
						printf("%c", ch);
						if(flag_log == '1')
							fprintf(fplog_out, "%c", ch);
					}
				}
				else if(strcmp(inp, "viewoutlog") == 0)
				{
					//show outlog from file output.log
					fseek(fplog_out, 0, SEEK_SET);
					char ch;
					while((ch = fgetc(fplog_out)) != EOF)
					{
						printf("%c", ch);
					}
				}
				else
				{
					char* str_temp = (char* )malloc(sizeof(char)*30);
					strcpy(str_temp, inp);
					if(strcmp(firstword(str_temp), "changedir") == 0)
					{
						char delim[] = " ";
						char* ptr = strtok(inp, delim);
						char* dir = strtok(NULL, delim);

						chdir(dir);
						//change directory
					}
					else
					{
						printf("Wrong command!!\n");
						action = "Failure";
						//Wrong command input
					}
				}

				fprintf(fplog_cmd, "\t%s", action);
				fprintf(fplog_cmd, "%c", '\n');
			}
		}
		else
		{
			printf("Command Interpretor not started\n");
		}

		if(flag_exit == '1')
		{
			break;
		}
	}
	fclose(fplog_cmd);
}