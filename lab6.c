#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pos = 0;

/**
 * @brief create an archive from files
 * 
 * @param file1 
 * @param argc 
 * @param argv 
 */
void create(char *file1, int argc, char *argv[]) {
    FILE *arc = fopen(file1, "wb");
    FILE *file2;

    int size[128]; //the program can archive up to 128 files
    
    fprintf(arc, "%d\n", argc - 4); //write a number of files

    for (int i = 0; i < argc - 4; i++) {
        file2 = fopen(argv[i + 4], "rb"); //open each file
        if (file2 == NULL) {
            printf("Couldn't open %s for reading\n", argv[i + 4]);
            continue;
        }

        fseek(file2, 0, SEEK_END);
        size[i] = ftell(file2); //count the file's size
        fseek(file2, 0, SEEK_SET);

        fprintf(arc, "%s - %d\n", argv[i + 4], size[i]); //write the file's name and size
        
        fclose(file2);
    }
        
    pos = ftell(arc);

    for (int i = 0; i < argc - 4; i++) {
        file2 = fopen(argv[i + 4], "rb");
        if (file2 != NULL)
            printf("File %s added\n", argv[i + 4]);
        
        char* temp = malloc(size[i] * sizeof(char));
        fread(temp, 1, size[i], file2);
        fwrite(temp, 1, size[i], arc); //write the file's content into the arc

        fclose(file2);
    }
}

/**
 * @brief provide a list of files stored in the archive
 * 
 * @param file1 
 */
void list(char *file1) {
    FILE *file = fopen(file1, "rb");

    char temp[128];

    char count[10], *end_;
    fscanf(file, "%s", count);
    int cnt = strtol(count, &end_, 10); //scan the number of files

    for (int i = 0; i < cnt; i++) {
        fscanf(file, "%s", temp); 
        printf("%s\n", temp); //scan and print the file's name

        fscanf(file, "%s", temp);
        fscanf(file, "%s", temp); //skip the "-" and file's size
    }

    fclose(file);
}

/**
 * @brief extract files from an archive
 * 
 * @param file1 
 */
void extract(char *file1) {
    FILE *arc_file = fopen(file1, "rb");

    char count[10], *end;
    fscanf(arc_file, "%s", count);
    long long cnt = strtol(count, &end, 10); //scan the number of files

    int names_start = ftell(arc_file);

    char file2[128] = {0};
    char dump[128] = {0};
    int cur_pos = 0;
    
    char temp[128];

    for (int i = 0; i < cnt; i++) { //print files' names
        fscanf(arc_file, "%s", temp);
        printf("%s\n", temp);

        fscanf(arc_file, "%s", temp);
        fscanf(arc_file, "%s", temp);
    }

    int cur_file_size, content_start = ftell(arc_file), end_ = 0;

    fseek(arc_file, names_start, SEEK_SET);

    for (int i = 0; i < cnt; i++) {
        fscanf(arc_file, "%s%s%llu", file2, dump, &cur_file_size); //scan the file's name and size

        end_ = ftell(arc_file);

        FILE *cur_file = fopen(file2, "wb"); //create current file

        fseek(arc_file, content_start + 1, SEEK_SET); //go to file's content

        char* temp = malloc(cur_file_size * sizeof(char*));
        fread(temp, 1, cur_file_size, arc_file);
        fwrite(temp, 1, cur_file_size, cur_file); //copy the content into file

        content_start += cur_file_size; //move the content pointer to another file

        fseek(arc_file, end_, SEEK_SET); //go to the next file

        fclose(cur_file);
    }

    printf("DONE!\n");
    fclose(arc_file);
}

int main(int argc, char *argv[]) {

    char *file1;
    
    for (int i = 0; i < argc; i++) { //parse input

        if (!strcmp("--file", argv[i]))
            file1 = argv[i + 1];

        if (!strcmp("--create", argv[i])) {
            create(file1, argc, argv);
            break;
        }

        if (!strcmp("--extract", argv[i]))
            extract(file1);

        if (!strcmp("--list", argv[i]))
            list(file1);
    }
}