// CPP program to illustrate strstr() 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>


int check_regular_file(
    const char *path
) {
    struct stat path_stat = { 0 };
    int stat_res = stat(path, &path_stat);
    if (stat_res != 0) {
        return -1;
    }
    return S_ISREG(path_stat.st_mode);
}


int main(int argc,
        char* argv[]
) { 

    if (argc < 3) {
        perror("pleas enter file_path and keyword");
        return 1;
    }

    DIR* dir_handle;
    struct dirent* dir_entry;
    FILE    *file_handle;
    char    file_contenter[BUFSIZ];
    size_t  file_len;
    char* dir_path;
    char* file_content;
    char* keyword ;
    int dir_path_len;
    int keyword_len;

    keyword = argv[2];
    keyword_len = strlen(argv[2]);
    //keyword[keyword_len]='\0';

    dir_path = argv[1];
    dir_path_len = strlen(argv[1]);
    //dir_path[dir_path_len]='\0';
    /* Scanning the in directory */
    dir_handle = opendir (dir_path);
    if (dir_handle == NULL) {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        fclose(file_handle);
        return 1;
    }

    while (1) {
        dir_entry = readdir(dir_handle);
        if (dir_entry == NULL) {
            break;
        }
        if (dir_entry->d_name[0] == '.') {
            continue;
        }

        char* file_name = dir_entry->d_name;
        int file_name_len = strlen(file_name);
        
        char* file_path = (char*) malloc(file_name_len + dir_path_len + 1);
        if (file_path == NULL) {
            fprintf(stderr, "Error : allocation failed (error: %s)\n", strerror(errno));
            return 1;
        }
        
        strcpy(file_path, dir_path);
        if (dir_path[dir_path_len] != '/') {
            strcat(file_path, "/");
        }
        strcat(file_path, file_name);

        int check_res = check_regular_file(file_path);
        if (check_res < 0) {
            fprintf(stderr, "Error: Failed to check type of a directory entry (path: %s, error: %s)\n", file_path, strerror(errno));
            free(file_path);
            continue;
        }
        if (check_res == 0) {
            free(file_path);
            continue;
        }
        
        file_handle = fopen(file_path, "r");
        if (file_handle == NULL) {
            fprintf(stderr, "Error : Failed to open entry file (file path: %s, error: %s)\n", file_path, strerror(errno));
            fclose(file_handle);
            free(file_path);
            continue;
        }
        free(file_path);

        if (fseek(file_handle, 0, SEEK_END)) {    // movie curser end of file
            perror("Error while seeking to end of file");
            fclose(file_handle);
            continue;
        }
        
        file_len = ftell(file_handle);
        if (file_len < 0) { //find file length 
            perror("Error while reading file position");
            fclose(file_handle);
            continue;
        }
        if (file_len == 0) {
            fclose(file_handle);
            continue;
        }
        
        file_content = (char*)malloc(file_len + 1);

        if ( fseek(file_handle, 0, SEEK_SET) != 0 ) {  // movie curser  The first file
            perror("Error while seeking set of file");
            fclose(file_handle);
            free(file_content);
            continue;
        }
// inja yek test bray commit github ast
        size_t read_len = fread(file_content, 1, file_len, file_handle);
        if (read_len != file_len) {
            printf(
                "Error cannot read file (file len: %lu, read len: %lu, name: %s, error: %s) \n",
                 file_len, read_len, file_name, strerror(errno)
            );
            fclose(file_handle);
            free(file_content);
            continue;
        }
        fclose(file_handle);
        file_content[file_len]='\0';

        int word_count = 0;
        //keyword_len = strlen(keyword);

        char* pos = strstr(file_content, keyword);
        while (pos > 0) {
            word_count++;
            //size_t remaining_len = file_len - (size_t)(pos - file_content);
            pos = strstr(pos + keyword_len, keyword);   
        }
        free(file_content);

        printf("%s\n",file_name);
        printf("%d\n", word_count);
    }
    closedir(dir_handle);
} 

// hi my friends