#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief create an archive from files
 * 
 * @param arc_file_name
 * @param argc 
 * @param argv
 */
void create(char *arc_file_name, int argc, char *argv[]) {
    FILE *arc_file = fopen(arc_file_name, "wb"); //create an archive
    FILE *cur_file;

    int size[128]; //the program can archive up to 128 files
    
    fprintf(arc_file, "%d\n", argc - 4); //write a number of files

    for (int i = 0; i < argc - 4; i++) {
        cur_file = fopen(argv[i + 4], "rb"); //open each file
        if (cur_file == NULL) {
            printf("Couldn't open %s for reading\n", argv[i + 4]);
            continue;
        }

        fseek(cur_file, 0, SEEK_END);
        size[i] = ftell(cur_file); //count the file's size
        fseek(cur_file, 0, SEEK_SET);

        fprintf(arc_file, "%s - %d\n", argv[i + 4], size[i]); //write the file's name and size
        
        fclose(cur_file);
    }

    for (int i = 0; i < argc - 4; i++) {
        cur_file = fopen(argv[i + 4], "rb");
        if (cur_file != NULL)
            printf("File %s added\n", argv[i + 4]);
        
        char* temp = malloc(size[i] * sizeof(char));
        fread(temp, 1, size[i], cur_file);
        fwrite(temp, 1, size[i], arc_file); //write the file's content into the arc

        fclose(cur_file);
    }

    fclose(arc_file);
}

/**
 * @brief provide a list of files stored in the archive
 * 
 * @param arc_file_name 
 */
void list(char *arc_file_name) {
    FILE *arc_file = fopen(arc_file_name, "rb");

    char temp[128];

    char count[10], *end_;
    fscanf(arc_file, "%s", count);
    int cnt = strtol(count, &end_, 10); //scan the number of files

    for (int i = 0; i < cnt; i++) {
        fscanf(arc_file, "%s", temp); 
        printf("%s\n", temp); //scan and print the file's name

        fscanf(arc_file, "%s", temp);
        fscanf(arc_file, "%s", temp); //skip the "-" and file's size
    }

    fclose(arc_file);
}

/**
 * @brief extract files from an archive
 * 
 * @param arc_file_name 
 */
void extract(char *arc_file_name) {
    FILE *arc_file = fopen(arc_file_name, "rb");

    char count[10], *end;
    fscanf(arc_file, "%s", count);
    long long cnt = strtol(count, &end, 10); //scan the number of files

    int names_start = ftell(arc_file);

    char cur_file_name[128] = {0};
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
        fscanf(arc_file, "%s%s%llu", cur_file_name, dump, &cur_file_size); //scan the file's name and size

        end_ = ftell(arc_file);

        FILE *cur_file = fopen(cur_file_name, "wb"); //create current file

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

    char *arc_file_name;
    
    for (int i = 0; i < argc; i++) { //parse input

        if (!strcmp("--file", argv[i]))
            arc_file_name = argv[i + 1];

        if (!strcmp("--create", argv[i])) {
            create(arc_file_name, argc, argv);
            break;
        }

        if (!strcmp("--extract", argv[i]))
            extract(arc_file_name);

        if (!strcmp("--list", argv[i]))
            list(arc_file_name);
    }
}