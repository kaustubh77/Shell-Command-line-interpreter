#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/ipc.h>
#include<unistd.h>

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
	if(str[0] == ' ')
	{
		for(int i = 0; i < strlen(str)-1; i++)
		{
			str[i] = str[i+1];
			if(str[i+1] == ' ')
			{
				str[i] = '\0';
				break;
			}
		}
		return str;	
	}

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

int main()
{
	char* inp = inputConsole();
	char command[200];
	strcpy(command, inp);	
	int count;
	char** cmd_arr = strFactoring(command, &count, "|");

	/*for(int i = 0; i < count; i++)
	{
		printf("%s\n", cmd_arr[i]);
	}*/

	FILE* fp = fopen("file.txt", "w+");
	FILE* temp = fopen("temp.txt", "w+");
	
	for(int i = 0; i <= count; i++)
	{
		// printf("Hi %d   %d\n", i, getpid());
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
				exit(0);
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
				
				//execlp("/bin/grep", "grep", "Prob", NULL);
				//char arr[100];
				//strcpy(arr, "grep Prob");
				//char** temp = strFactoring(cmd_arr[count-i], &cnt, " ");
				//printf("%d cnt\n", cnt);
				//for(int j = 0; j < cnt; j++)
				//{
				//	printf("temp[%d]: %st\n", j, temp[j]);
				//}
				//printf("%sz\n", firstword(cmd_arr[count-i]));
				char arr[100];
				strcpy(arr, cmd_arr[count-i]);
				//execvp(firstword(cmd_arr[count-i]), strFactoring(cmd_arr[count-i], &cnt, " "));
				execvp(firstword(arr), strFactoring(cmd_arr[count-i], &cnt, " "));
				//execvp("grep", strFactoring(cmd_arr[count-i], &cnt, " "));
				//char const* argv[] = {"grep", "Prob", 0};
				//execvp("grep", argv);
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
		}
	}
}