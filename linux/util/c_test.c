// CPP program to illustrate strstr() 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>

int main(int argc,
        char* argv[]
){ 
    FILE *file_handle;
    long int file_length;
    char* file_path;
    char *buff;
    char* substring;
    if (argc < 2){
        perror ("pleas enter file_path and key_word");
        return 1;
    }

    file_path = argv[1];
    int n = strlen(argv[1]);
    file_path[n]='\0';

    substring = argv[2];
    int m = strlen(argv[2]);
    substring[m]='\0';
    file_handle = fopen(file_path,"rb");

    if(fseek(file_handle, 0, SEEK_END)) {
        puts("Error while seeking to end of file");
        return 1;
    }
  
    file_length = ftell(file_handle);
    if (file_length < 0) {
        puts("Error while reading file position");
        return 2;
    }
    buff = calloc (1,file_length+1);
    fseek(file_handle, 0, SEEK_SET);
    fread(buff, file_length+1, 1, file_handle);

	 
	char* point; 

    int l1 = strlen(buff);
    int l2 = strlen(substring);
    int i;
    int count = 0;

    for(i = 0; i < l1 - l2; i++) {
        if(strstr(buff + i, substring) == buff + i) {
            count++;
            // if overlap is not allowed uncomment next line
            printf("%d\n", i);
            i = i + l2 - 1;
        }
    }

    printf("%d\n", count );

	/* Find first occurrence of s2 in s1 
	//point = strstr(buff, substring); 

	// Prints the result 
	if (point) { 
		printf("String found\n"); 
		printf("%*.*s", 1, strlen(substring), point); 
	} else
		printf("String not found\n"); 
    */
	return 0; 
} 
