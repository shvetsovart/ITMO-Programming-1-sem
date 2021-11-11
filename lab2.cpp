#include <stdio.h>

#define BASE (int)1e4
#define SIZE 78

struct uint1024_t {
    int digits[SIZE];
};

char dig[312] = "179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216";
struct uint1024_t cck;


char comp(struct uint1024_t a, struct uint1024_t b) {

    for (int i = SIZE - 1; i >= 0; --i) {
        if (a.digits[i] < b.digits[i]) {
            return 0;
        }
        else if (a.digits[i] > b.digits[i]) {
            return 1;
        }
    }

    return 2;
}

struct uint1024_t from_uint(unsigned int x) {

    struct uint1024_t res;

    for (int i = 0; i < SIZE; ++i) {
        res.digits[i] = 0;
    }

    int next = 0;
    while (x) {
        res.digits[next++] = x % BASE;
        x /= BASE;
    }

    return res;
}

struct uint1024_t add_op(struct uint1024_t* x, struct uint1024_t* y) {

    struct uint1024_t res;

    for (int i = 0; i < SIZE; i++) {
        res.digits[i] = x->digits[i] + y->digits[i];
    }

    for (int i = 0; i < SIZE - 1; i++) {
        if (res.digits[i] >= BASE) {
            res.digits[i] -= BASE;
            res.digits[i + 1]++;
        }
    }

    if (comp(from_uint(0), res) == 1 || comp(res, cck)) {
        printf("Undefined Behavior\n");
        return from_uint(0);
    }

    return res;
}

struct uint1024_t subtr_op(struct uint1024_t x, struct uint1024_t y) {

    struct uint1024_t res;

    for (int i = 0; i < SIZE; i++) {
        res.digits[i] = x.digits[i] - y.digits[i];
    }

    for (int i = 0; i < SIZE - 1; i++) {
        if (res.digits[i] < 0) {
            res.digits[i] += BASE;
            res.digits[i + 1]--;
        }
    }

    if (comp(from_uint(0), res) == 1 || comp(res, cck)) {
        printf("Undefined Behavior\n");
        return from_uint(0);
    }

    return res;
}

struct uint1024_t mult_op(struct uint1024_t x, struct uint1024_t y) {

    struct uint1024_t res;
    res = from_uint(0);

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - i; j++) {
            res.digits[i + j] += x.digits[i] * y.digits[j];
        }
    }

    for (int i = 0; i < SIZE - 1; i++) {
        if (res.digits[i] >= BASE) {
            res.digits[i + 1] += res.digits[i] / BASE;
            res.digits[i] = res.digits[i] % BASE;
        }
    }

    if (comp(from_uint(0), res) == 1 || comp(res, cck)) {
        printf("Undefined Behavior\n");
        return from_uint(0);
    }

    return res;
}


void printf_value(struct uint1024_t c) {

    short fl = 0;

    for (int i = SIZE - 1; i >= 0; --i) {
        if (c.digits[i] != 0) {
            fl++;
        }

        if (fl) {
            if (fl == 1) {
                printf("%d", c.digits[i]);
                fl++;
            } else {
                for (int j = 1000; j > 0; j /= 10)
                    if (c.digits[i] / j == 0) {
                        printf("0");
                    }

                if (c.digits[i] != 0) {
                    printf("%d", c.digits[i]);
                }
            }

        }
    }

    if (comp(from_uint(0), c) == 2) {
        printf("0");
    }

    printf("\n");
}

void scanf_value(struct uint1024_t* x)  {

    char a[312];
    for (int i = 0; i < 312; ++i) {
        a[i] = '0';
    }

    scanf("%s", a);

    int n = 0;
    for (int i = 0; i < sizeof(a) / sizeof(a[0]); ++i) {
        if (a[i] >= '0' && a[i] <= '9') {
            n++;
        } else {
            break;
        }
    }

    for (int i = 0; i <= n / 4; ++i) {
        int cnt = 4, t = 1, rs = 0;

        if (i == n / 4) {
            cnt = n % 4;
        }

        int ind = n - i * 4 - 1;
        for (int j = 0; j < cnt; ++j) {
            rs += (a[ind - j] - '0') * t;
            t *= 10;
        }

        x->digits[i] = rs;
    }
}

struct uint1024_t undef(char a[312]) {

    struct uint1024_t x = from_uint(0);
    int n = 309;

    for (int i = 0; i <= n / 4; ++i) {
        int cnt = 4;
        int t = 1;
        int rs = 0;

        if (i == n / 4) {
            cnt = n % 4;
        }

        int ind = n - i * 4 - 1;
        for (int j = 0; j < cnt; ++j) {
            rs += (a[ind - j] - '0') * t;
            t *= 10;
        }

        x.digits[i] = rs;
    }

    return x;
}

int main(int argc, char* argv[]) {
    cck = undef(dig); // 2 ^ 1024
    struct uint1024_t a = from_uint(333), b = from_uint(332);
    struct uint1024_t c = add_op(a, b);
    //printf_value(c);
    //printf("\n");
    //printf("%d", comp(from_uint(0), from_uint(0)));
    //printf_value(cck);

    //printf("\n");
    scanf_value(&a);
    scanf_value(&b);

    printf("\n");
    printf_value(add_op(a, b));
    printf_value(subtr_op(a, b));
    printf_value(mult_op(a, b));
}
