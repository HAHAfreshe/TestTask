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
        printf("Syntax : \t%s --help \n", argv[0]);
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
    char pathToFile[BUFFER_SIZE];
    char logFileName[16];
    time_t t = time(NULL);
    struct tm* aTm = localtime(&t);
    int numbStr;

    snprintf(path, sizeof path, "%s", argv[1]);
    snprintf(logFileName, sizeof logFileName, "%04d%02d%02d-%02d%02d%02d", aTm->tm_year+1900, aTm->tm_mon+1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
    strcat(logFileName, ".log");

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
            if (fPtr){   numbStr = 0;
                fTemp = fopen("replace.tmp", "w"); 
                while ((fgets(buffer, BUFFER_SIZE, fPtr)) != NULL)
                {   numbStr++;
                    replaceAll(buffer, argv[2], argv[3], pathToFile, fLog, numbStr);
                    fputs(buffer, fTemp);
                }
                fclose(fPtr);
                fclose(fTemp);
                remove(pathToFile);
                rename("replace.tmp", pathToFile);
                remove("replace.tmp");
            }
            else{
                printf("\nUnable to open file: %s.\n", pathToFile);
                printf("Please check whether file exists and you have read/write privilege.\n");

                fprintf(fLog, "\tUnable to open file: %s.\n", pathToFile);
                fprintf(fLog, "\tPlease check whether file exists and you have read/write privilege.");
            }
        }
        closedir(d);
    }
    else{
        printf("\nUnable to find directory: %s.\n", path);
        printf("Please check whether directory exists and you have read/write privilege.\n");

        fprintf(fLog, "\tUnable to find directory: %s.\n", path);
        fprintf(fLog, "\tPlease check whether directory exists and you have read/write privilege.");
    }

    fclose(fLog);

    return 0;
}

/*
Funct "replaceAll" - finding a string with search character,
replacing search characters with new ones
and write to the log file
params:
    char *str - full string from file
    const char *oldWord - searching word
    const char *newWord - The word to replace it
    char *path - path to file (for log-file)
    FILE *fLog - log - file
    int numbStr - number of the line
*/
void replaceAll(char *str, const char *oldWord, const char *newWord, char *path, FILE *fLog, int numbStr)
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

        fprintf(fLog, "\t%s: %d:%d: \"%s\" -> \"%s\"\n", path, numbStr, index+1, oldWord, newWord);
    }
}
