#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int mkdir(const char *pathname);

int Pow(int num, int p) {
    int res = 1;

    for (int i = 0; i < p; ++i) {
        res *= num;
    }

    return res;
}

void game_of_life(int **life, int h, int w) {
    int tmp[h][w];
    int x, y;

    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            tmp[y][x] = life[y][x];
        }
    }

    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            int cnt_neighbours = 0;

            for (int i = -1; i < 2; ++i) {
                for (int j = -1; j < 2; ++j) {
                    if (i != 0 || j != 0) {
                        cnt_neighbours += life[(y + i + h) % h][(x + j + w) % w];
                    }
                }
            }

            if (cnt_neighbours == 3) {
                tmp[y][x] = 1; // if cnt_neighbours == 2, then tmp[y][x] = life[y][x]
            } else if (cnt_neighbours < 2 || cnt_neighbours > 3) {
                tmp[y][x] = 0;
            }
        }
    }

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            life[y][x] = tmp[y][x];
}

int main(int argc, char* argv[]) {

    char* file_name, *dir_name;
    int max_iter = 300, dump_freq = 1;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--input")) {
            file_name = argv[i + 1];
        }
        if (!strcmp(argv[i], "--output")) {
            dir_name = argv[i + 1];
            mkdir(dir_name);
        }
        if (!strcmp(argv[i], "--max_iter")) {
            max_iter = strtol(argv[i + 1], 0, 10);
        }
        if (!strcmp(argv[i], "--dump_freq")) {
            dump_freq = strtol(argv[i + 1], 0, 10);
        }
    }

    FILE* file;
    file = fopen(file_name, "rb");

    //printf("%s\n", file_name);

    int width = 0, height = 0, size = 0;
    unsigned char header[54];

    fread(header, 1, 54, file);

    for (int i = 0; i < 4; ++i) {
        width += header[18 + i] * Pow(256, i);
        height += header[22 + i] * Pow(256, i);
        size += header[2 + i] * Pow(256, i);
    }

    unsigned char img_bytes[size - 54];
    fread(img_bytes, 1, size - 54, file);

    int **img = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; ++i) {
        img[i] = (int*)malloc(width * sizeof(int));
    }

    int point = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            img[i][j] = (img_bytes[point] != 255);
            point += 3;
        }
        point += (width % 4);
    }

    int iter = 1;

    fclose(file);

    while (iter <= max_iter) {
        if (iter % dump_freq == 0) {
            char iter_name[10];
            char path[20];

            strcpy(path, dir_name);
            strcat(strcat(path, "\\"), strcat(itoa(iter, iter_name, 10), ".bmp"));

            printf("%s\n", path);

            FILE* life = fopen(path, "w");

            point = 0;

            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    double tmp = 255.0 * j / width;
                    double tmp2 = 255.0 * i / height;
                    int rs1 = (int)(tmp) * (img[i][j] == 1) % 255, rs2 = (int)(tmp2) * (img[i][j] == 1) % 255; //for chaos
                    int rz1 = (200 * (img[i][j] == 1) * j / width) % 200 + 55 * (img[i][j] == 1), rz2 = (200 * (img[i][j] == 1) * i / height) % 200 + 55 * (img[i][j] == 1); // for gradient
                    img_bytes[point] = rz1;
                    img_bytes[point + 1] = 0;
                    img_bytes[point + 2] = rz2;
                    point += 3;
                }

                point += width % 4;
            }

            fwrite(header, 1, 54, life);
            fwrite(img_bytes, 1, size - 54, life);

            fclose(life);
        }

        game_of_life(img, height, width);
        iter++;
    }

    free(img);
}
