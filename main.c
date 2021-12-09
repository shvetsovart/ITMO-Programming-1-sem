#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

unsigned int swap_endians(unsigned int n) {
	return ((n >> 24) & 0x000000ff) |
    		((n >> 8) & 0x0000ff00) |
	    	((n << 8) & 0x00ff0000) |
		   ((n << 24) & 0xff000000);
}

void copyFile(FILE* inp, FILE* outp) {
	int c;
	while ((c = getc(inp)) != EOF)
		putc(c, outp);
}

void putstr(char *str, FILE* outp) {
    while(*str != '\0') {
        putc(*str, outp);
        str++;
    }
}

struct tag_h {
    short empty;
    char version[3];
    char v1;
    char v2;
    char flags;
    int size;
};

struct frame_h {
    char name[4];
    int size;
    short flags;
};

union tag_header {
    char buffer[12];

    struct tag_h data;
};

union frame_header {
    char buffer[10];

    struct frame_h data;
};

enum call_type {
    get, 
    show
};

void read_frames(char* file_name, char* frame_name, enum call_type request) {
    FILE* file;
    file = fopen(file_name, "rb");
    
    union tag_header tag;
    fread(tag.buffer + 2, 1, 10, file);

    if (request == show) {
        printf("Version: %s%d.%d\nFrames:\n", tag.data.version, tag.data.v1, tag.data.v2);
    }

    int tag_size = swap_endians(tag.data.size) + 10;

    while (ftell(file) < tag_size) {
        union frame_header frame;
        fread(frame.buffer, 1, 10, file);

        if (frame.data.name[0] == 0)
			break;

        const int frame_size = swap_endians(frame.data.size);

        if (request == get) {
            if (strcmp(frame.data.name, frame_name)) {
                fseek(file, frame_size, SEEK_CUR);
                continue;
            }
        }

        printf("ID: %s\tValue: ", frame.data.name);

        char* frame_content;
        frame_content = malloc(frame_size);
        fread(frame_content, 1, frame_size, file);

        for (int i = 0; i < frame_size; ++i) {
            printf("%c", frame_content[i]);
        }

        if (request == get) {
            break;
        }

        printf("\n");
        free(frame_content);
    }

    fclose(file);
}

void set(char* file_name, char frame_name[4], char* frame_value) {
    FILE* file;
	file = fopen(file_name, "rb+");

	union tag_header tag;
	fread(tag.buffer + 2, sizeof(char), 10, file);

	unsigned int tag_size = swap_endians(tag.data.size);

    char frame_was_found = 0;
    int pos, old_frame_size;

    int frame_value_size = strlen(frame_value);

    union frame_header old_frame;
    char* old_value;

	while (ftell(file) < tag_size + 10) {
        union frame_header frame;
        fread(frame.buffer, 1, 10, file);

        if (frame.data.name[0] == 0) {
        	break;
        }

        int frame_size = swap_endians(frame.data.size);

        if (strcmp(frame.data.name, frame_name) == 0 && !frame_was_found) {
            frame_was_found = 1;

            pos = ftell(file) - 10;

            for (int i = 0; i < 10; ++i)
                old_frame.buffer[i] = frame.buffer[i];

            old_frame_size = swap_endians(frame.data.size);
            old_value = malloc(old_frame_size);

            fread(old_value, 1, old_frame_size, file);
        }

        fseek(file, frame_size, SEEK_CUR);
        
    }
    
    int ps = ftell(file) - 10;

    if (!frame_was_found) {
        int new_tag_size = tag_size + frame_value_size + 10 + 1;

        tag.data.size = swap_endians(new_tag_size);
        fwrite(tag.buffer + 2, 1, 10, file);

        fseek(file, ps, SEEK_SET);

        union frame_header frame;
        
        for (int i = 0; i < 4; ++i) {
            frame.data.name[i] = frame_name[i];
        }

        frame.data.size = swap_endians(frame_value_size + 1);
        frame.data.flags = 0;

        fwrite(frame.buffer, 1, 10, file);
        fseek(file, 1, SEEK_CUR);
        fwrite(frame_value, 1, frame_value_size, file);

        printf("Created new frame:\nID: %s\tValue: %s\n", frame_name, frame_value);
    } else {
        int new_tag_size = tag_size - (old_frame_size - frame_value_size) + 10 + 1;

        tag.data.size = swap_endians(new_tag_size);
        fwrite(tag.buffer + 2, 1, 10, file);

        fseek(file, pos, SEEK_SET);

        int ln = 10 + old_frame_size;

        while (ftell(file) < new_tag_size) {
            fseek(file, ln, SEEK_CUR);
            
            union frame_header frame;

            fread(frame.buffer, 1, 10, file);

            if (frame.data.name[0] == 0) {
                break;
            }
                
            int frame_size = swap_endians(frame.data.size);
            char* frame_val;

            frame_val = malloc(frame_size);
            fread(frame_val, 1, frame_size, file);

            fseek(file, -ln - 10 - frame_size, SEEK_CUR);

            fwrite(frame.buffer, 1, 10, file);
            fwrite(frame_val, 1, frame_size, file);

        }
        
        fseek(file, - ln - 10, SEEK_CUR);

        old_frame.data.flags = 0;
        old_frame.data.size = swap_endians(frame_value_size + 1);

        fwrite(old_frame.buffer, 1, 10, file);

        putc((char)(0), file);
        fwrite(frame_value, 1, frame_value_size, file);

        for (int i = 0; i < old_frame_size - frame_value_size; ++i) {
            putc((char)(0), file);
        }

        printf("New value for %s is: %s\n", frame_name, frame_value);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    char* file_name;
    char* frame_name;
    char* frame_value;

    char show_flag = 0;
    char get_flag = 0;
    char set_flag = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--show") == 0) {
            show_flag = 1;
            continue;
        }

        if (argv[i][2] == 'f') {
            file_name = strpbrk(argv[i], "=") + 1;
        }

        if (argv[i][2] == 'g') {
            frame_name = strpbrk(argv[i], "=") + 1;
            get_flag = 1;
        }

        if (argv[i][2] == 's') {
            frame_name = strpbrk(argv[i], "=") + 1;
            set_flag = 1;
        }

        if (argv[i][2] == 'v') {
            frame_value = strpbrk(argv[i], "=") + 1;
        }
    }

    if (show_flag) {
        read_frames(file_name, "", show);
    }

    if (get_flag) {
        read_frames(file_name, frame_name, get);
    }

    if (set_flag) {
        set(file_name, frame_name, frame_value);
    }
}
