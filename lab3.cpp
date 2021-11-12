#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

    FILE *fp = fopen("access_log_Jul95", "r");
    char cur[400];

    int dt;

    scanf("%d", &dt);

    int cnt_errors = 0;
    int cnt = 0;

    int *times = malloc(1900000 * sizeof(int));

    while (fgets(cur, 400, fp) != NULL) {

        int j = 0, space = 0, tl, tr;

        while (cur[j] != 0) {
            if (cur[j] == '[')
                tl = j + 1;
            if (cur[j] == '"')
                tr = j + 2;
            j++;
        }

        int days = (cur[tl] - '0') * 10 + (cur[tl + 1] - '0');
        int hours = (cur[tl + 12] - '0') * 10 + (cur[tl + 13] - '0');
        int minutes = (cur[tl + 15] - '0') * 10 + (cur[tl + 16] - '0');
        int seconds = (cur[tl + 18] - '0') * 10 + (cur[tl + 19] - '0');

        if (cur[tr] == '5') {
            printf("%s\n", cur);
            cnt_errors++;
        }

        times[cnt] = days * 86400 + hours * 3600 + minutes * 60 + seconds;

        cnt++;
    }

    int l = 0, rl, rr;
    int mx = 0;

    for (int r = 1; r < cnt; ++r) {

        while (times[r] - times[l] >= dt && l < r) {
            l++;
        }

        if (r - l + 1 > mx) {
            mx = r - l + 1;
            rl = l;
            rr = r;
        }
    }

    int r1 = times[rl], r2 = times[rr];
    int dd = r1 / 86400, dd2 = r2 / 86400;
    r1 %= 86400; r2 %= 86400;
    int hh = r1 / 3600, hh2 = r2 / 3600;
    r1 %= 3600; r2 %= 3600;
    int mm = r1 / 60, mm2 = r2 / 60;
    r1 %= 60; r2 %= 60;

    printf("%d %d [%d/Jul/1995:%d:%d:%d]-[%d/Jul/1995:%d:%d:%d]\n", cnt_errors, mx, dd, hh, mm, r1, dd2, hh2, mm2, r2);

    free(&times);
    return 0;
}
