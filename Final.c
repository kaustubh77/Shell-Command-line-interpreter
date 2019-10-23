#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/ipc.h>
#include<unistd.h>

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

char** strFactoring(char* command, int* count, char delim[])
{
	char cmd_cpy[200];
	strcpy(cmd_cpy, command);

	char *ptr = strtok(command, delim);

	*count = 0;
	while(ptr != NULL)
	{
		ptr = strtok(NULL, delim);
		(*count)++;
	}

	char** cmd_arr = (char** )malloc(sizeof(char*)*(*count+1));


	ptr = strtok(cmd_cpy, delim);
	int i = 0;
	while(ptr != NULL)
	{
		cmd_arr[i] = (char* )malloc(sizeof(char)*strlen(ptr));
		strcpy(cmd_arr[i], ptr);
		ptr = strtok(NULL, delim);
		i++;
	}
	cmd_arr[*count] = NULL;
	return cmd_arr;	

}

char* firstword(char* str)
{
	int offset = 0;
	while(str[offset] == ' ')
	{
		offset++;	
	}
	for(int i = 0; i < strlen(str)-offset; i++)
	{
		str[i] = str[i+offset];
		if(str[i+offset] == ' ')
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

void updatelog(char* inp, FILE* fplog_cmd)
{
	fprintf(fplog_cmd, "%s", inp);
	time_t T = time(NULL);
	struct tm tm = *localtime(&T);
	fprintf(fplog_cmd, "\t%02d/%02d/%04d", tm.tm_mday, tm.tm_mon, tm.tm_year+1900);
	fprintf(fplog_cmd, "\t%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void viewcmdlog(FILE* fplog_cmd)
{
	fseek(fplog_cmd, 0, SEEK_SET);
	char ch;
	fprintf(fplog_out, "\nviewcmdlog\n");
	while((ch = fgetc(fplog_cmd)) != EOF)
	{
		printf("%c", ch);
		if(flag_log == '1')
			fprintf(fplog_out, "%c", ch);
	}

}

void viewoutlog(FILE* fplog_out)
{
	fseek(fplog_out, 0, SEEK_SET);
	char ch;
	while((ch = fgetc(fplog_out)) != EOF)
	{
		printf("%c", ch);
	}
}

void changedir(char* inp, FILE* fplog_cmd)
{
		char delim[] = " ";
		char* ptr = strtok(inp, delim);
		char* dir = strtok(NULL, delim);

		chdir(dir);
		//change directory
}

int main()
{
	fplog_cmd = fopen("commands.log", "a+");
	fplog_out = fopen("output.log", "a+");
	printf("\033[0;32m");
	printf("Welcome to my terminal\n");
	printf("\033[0m");
	while(1)
	{
		char* str = cwd();
		printf("\033[0;33m");
		printf("%s:$ ", str);
		printf("\033[0m");
		free(str);
		char* inp_st = inputConsole();

		if(strcmp(inp_st, "enter") == 0)
		{
			while(1)
			{
				char* action = "Success";
				char* str = cwd();
				printf("\033[0;33m");
				printf("%s:$ ", str);
				printf("\033[0m");
				free(str);

				//Command Interpretor starts, flag for exit command required
				char* inp = inputConsole();
				if(flag_log == '1')
				{
					updatelog(inp, fplog_cmd);

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
					viewcmdlog(fplog_cmd);
				}
				else if(strcmp(inp, "viewoutlog") == 0)
				{
					//show outlog from file output.log
					viewoutlog(fplog_out);
				}
				else
				{
					char* str_temp = (char* )malloc(sizeof(char)*30);
					strcpy(str_temp, inp);
					if(strcmp(firstword(str_temp), "changedir") == 0)
					{
						changedir(inp, fplog_cmd);
					
					}
					else
					{
						char command[200];
						strcpy(command, inp);	
						int count;
						char** cmd_arr = strFactoring(command, &count, "|");

						FILE* fp = fopen("file.txt", "w+");
						FILE* temp = fopen("temp.txt", "w+");
						
						for(int i = 0; i <= count; i++)
						{
							int cnt;
							if(fork())
							{
								if(i == 0)
								{
									while(wait(NULL) != -1);
									fseek(fp, 0, SEEK_SET);
									char c = fgetc(fp); 
									while (c != EOF) 
									{ 
										printf ("%c", c); 
										c = fgetc(fp); 
									}
									fclose(fp);
									break;
								}
								else if(i != count)
								{
									while(wait(NULL) != -1);
									fclose(temp);
									temp = fopen("temp.txt", "w+");
									
									fseek(fp, 0, SEEK_SET);
									char c = fgetc(fp);
									while (c != EOF)
									{
										fputc(c, temp);
										c = fgetc(fp);
									}

									fclose(fp);
									fp = fopen("file.txt", "w+");
									fseek(temp, 0, SEEK_SET);
									dup2(fileno(fp), 1);
									dup2(fileno(temp), 0);
									
									char arr[100];
									strcpy(arr, cmd_arr[count-i]);
									execvp(firstword(arr), strFactoring(cmd_arr[count-i], &cnt, " "));
								}
								else
								{
									while(wait(NULL) != -1);
									dup2(fileno(fp), 1);
									char arr[100];
									strcpy(arr, cmd_arr[count-i]);
									execvp(firstword(arr), strFactoring(cmd_arr[count-i], &cnt, " "));
								}
							}
							else
							{
								if(count == 1)
								{
										dup2(fileno(fp), 1);
										execvp(firstword(cmd_arr[count-i-1]), strFactoring(cmd_arr[count-i-1], &cnt, " "));
								}
								if(i == count)
								{
									exit(0);
								}
							}
						}
					}
				}
				fprintf(fplog_cmd, "\t%s\n", action);
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