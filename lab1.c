#include <stdio.h>

int main(int argc, char* argv[]) {
    FILE* input_txt;
    char* filename = argv[2];
    char* option = argv[1];

    if (fopen(filename, "r") == NULL) {
        printf("Could not open the file");
        return 1;
    }

    input_txt = fopen(filename, "r");

    if ((strcmp(option, "-l") == 0) || (strcmp(option, "--lines") == 0)) {
        int lines = 1;
        int ch = fgetc(input_txt);
        int cnt_chars = 0;
        while (ch != EOF) {
            cnt_chars++;
            if (ch == '\n') { //windows
                lines++;
            }
            ch = fgetc(input_txt);
        }
        if (cnt_chars == 0) {
            fclose(input_txt);
            printf("%d", 0);
            return 0;
        }
        fclose(input_txt);
        printf("%d", lines);
        return 0;
    }

    if ((strcmp(option, "-c") == 0) || (strcmp(option, "--bytes") == 0)) {
        int bytes = 0;
        int ch = fgetc(input_txt);
        while (ch != EOF) {
            bytes++;
            if (ch == '\n') { //windows
                bytes++;
            }
            ch = fgetc(input_txt);
        }
        fclose(input_txt);
        printf("%d", bytes);
        return 0;
    }

    if ((strcmp(option, "-w") == 0) || (strcmp(option, "--words") == 0)) {
        int words = 0;
        int non_space = 0;
        int ch = fgetc(input_txt);
        while (ch != EOF) {
            if (ch == '\n' || ch == ' ' || ch == '\t' || ch == '\f') {
                if (non_space == 1) {
                    non_space = 0;
                    words++;
                }
            } else {
                non_space = 1;
            }
            ch = fgetc(input_txt);
        }
        if (non_space) {
            words++;
        }
        fclose(input_txt);
        printf("%d", words);
        return 0;
    }
}
