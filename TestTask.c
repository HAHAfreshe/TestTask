#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define BUFFER_SIZE 1000


void replaceAll(char *str, const char *oldWord, const char *newWord, char *path, FILE *fLog, int i);

int main(int argc, char *argv[])
{
    if (argc == 1){
        printf("syntax : \t%s --help \n", argv[0]);
        return 0;
    }

    if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
    {
        printf("\nUtility for changing data inside files\n");
        printf("Usage:\t%s [path] [search word] [new word]\n\n", argv[0]);
    }

    DIR *d;
    FILE * fPtr;
    FILE * fTemp;
    FILE * fLog;
    char path[strlen(argv[1])+1];
    char pathToFile[1000];
    char logFileName[16];
    time_t t = time(NULL);
    struct tm* aTm = localtime(&t);
    int i = 1;

    snprintf(path, sizeof path, "%s", argv[1]);
    snprintf(logFileName, sizeof logFileName, "%04d%02d%02d-%02d%02d%02d%s", aTm->tm_year+1900, aTm->tm_mon+1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec, ".log");
    char buffer[BUFFER_SIZE];
    struct dirent *dir;

    if (argv[1] && *argv[1] && argv[1][strlen(argv[1]) - 1] == '/')
        path[strlen(path) - 1] = 0;
    
    d = opendir(path);

    fLog = fopen(logFileName, "a");

    if (d) {
        
        while ((dir = readdir(d)) != NULL) {
            snprintf(pathToFile, sizeof pathToFile, "%s/%s", path, dir->d_name);
            fPtr  = fopen(pathToFile, "r");
            if (fPtr)
            {   i = 0;
                fTemp = fopen("replace.tmp", "w"); 
                while ((fgets(buffer, BUFFER_SIZE, fPtr)) != NULL)
                {   i++;
                    replaceAll(buffer, argv[2], argv[3], pathToFile, fLog, i);
                    fputs(buffer, fTemp);
                }
                fclose(fPtr);
                fclose(fTemp);
                remove(pathToFile);
                rename("replace.tmp", pathToFile);
                remove("replace.tmp");
            }
            else
            {
                printf("\nUnable to open file: %s.\n", pathToFile);
                printf("Please check whether file exists and you have read/write privilege.\n");
            }
        }
        closedir(d);
    }
    fclose(fLog);

    return 0;
}


void replaceAll(char *str, const char *oldWord, const char *newWord, char *path, FILE *fLog, int i)
{
    char *pos, temp[BUFFER_SIZE];
    int index = 0;
    int owlen;

    owlen = strlen(oldWord);

    if (!strcmp(oldWord, newWord)) {
        return;
    }
    while ((pos = strstr(str, oldWord)) != NULL)
    {
        strcpy(temp, str);

        index = pos - str;
        str[index] = '\0';

        strcat(str, newWord);
        strcat(str, temp + index + owlen);
        
        fprintf(fLog, "\t%s: %d:%d: \"%s\" -> \"%s\"\n", path, i, pos-str+1, oldWord, newWord);
    }
}
