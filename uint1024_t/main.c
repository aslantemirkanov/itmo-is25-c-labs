#include <stdio.h>
#include <string.h>


struct uint1024_t {
    unsigned char digit[128];
};

int conversion_from_10_to_2(char *num, int len, int *bin) {
    int bin_len = 0;
    int i;
    int tmp_int[310];
    for (i = 0; i < len; i++) {
        tmp_int[i] = num[i] - 48;
    }
    int current_left_edge = 0;
    tmp_int[len] = 0;
    while (current_left_edge < len) {
        for (i = 0; i < len; i++) {
            tmp_int[i + 1] += (tmp_int[i] % 2) * 10;
            tmp_int[i] = tmp_int[i] / 2;
        }
        if (tmp_int[len] > 0) {
            bin[bin_len++] = 1;
            tmp_int[len] = 0;
        } else {
            bin[bin_len++] = 0;
        }
        if (tmp_int[current_left_edge] == 0) current_left_edge++;
    }
    return bin_len;
}

void conversion_from_2_to_256(int *bin, int bin_len, struct uint1024_t *A) {
    int i, j, pow_bin;
    for (i = 0; i < 128; i++) {
        A->digit[i] = 0;
    }
    int current_left_edge_256 = 0;
    int current_left_edge_bin = 0;
    while ((bin_len - current_left_edge_bin) >= 8) {
        pow_bin = 1;
        for (i = current_left_edge_bin; i < current_left_edge_bin + 9; i++) {
            A->digit[current_left_edge_256] += bin[i] * pow_bin;
            pow_bin = pow_bin * 2;
        }
        current_left_edge_bin += 8;
        current_left_edge_256 += 1;
    }
    if ((bin_len - current_left_edge_bin) < 8 && (bin_len - current_left_edge_bin) > 0) {
        pow_bin = 1;
        for (i = current_left_edge_bin; i < bin_len; i++) {
            A->digit[current_left_edge_256] += bin[i] * pow_bin;
            pow_bin = pow_bin * 2;
        }
        current_left_edge_256++;
    }
    if (current_left_edge_256 > 128) {
        printf("Undefined Behavior");
        return;
    }
}

void conversion_from_256_to_2(struct uint1024_t X, int *bin) {
    int left_edge_bin = 0;
    int i = 127;
    while (X.digit[i] == 0) i--;
    int len_X = i + 1;
    for (int i = 0; i < 1024; i++) bin[i] = 0;
    for (int i = 0; i < len_X; i++) {
        int tmp_left_edge = left_edge_bin;
        while (X.digit[i]) {
            bin[tmp_left_edge] = X.digit[i] % 2;
            X.digit[i] = X.digit[i] / 2;
            tmp_left_edge++;
        }
        left_edge_bin += 8;
    }
}

void conversion_from_2_to_10(int *bin, char *num_10_rez) {
    int i = 1023;
    while (bin[i] == 0) i--;
    int len_bin = i + 1;
    if (len_bin > 1024) {
        printf("\nUndefined Behavior");
        return;
    }
    int bin_tmp[1025];
    for (int i = len_bin - 1; i >= 0; i--) {
        bin_tmp[i] = bin[len_bin - 1 - i];
    }
    int current_left_edge = 0;
    int index_bin = 0;
    while (index_bin < len_bin) {
        bin_tmp[len_bin] = 0;
        if (bin_tmp[index_bin] == 0) index_bin++;
        else {
            for (i = index_bin; i < len_bin; i++) {
                if (i == len_bin - 1) {
                    bin_tmp[i + 1] = bin_tmp[i] % 10;
                    bin_tmp[i] = bin_tmp[i] / 10;
                } else {
                    bin_tmp[i + 1] += (bin_tmp[i] % 10) * 2;
                    bin_tmp[i] = bin_tmp[i] / 10;
                }
            }
            num_10_rez[current_left_edge++] = bin_tmp[len_bin];
        }
    }
    for (int j = current_left_edge - 1; j >= 0; j--) printf("%d", num_10_rez[j]);
}

void printf_value(struct uint1024_t X) {
    int i = 127;
    while (X.digit[i] == 0) i--;
    //имеем i+1 элементов от 0 до i
    //теперь надо перевести их в 2ю СС а оттуда в 10ю СС
    int len_X = i + 1;
    for (i = 0; i < len_X; i++) printf("%d  ", X.digit[i]);
    printf("\n");
    int bin[1025];
    conversion_from_256_to_2(X, bin);
    char num_10_rez[309];
    conversion_from_2_to_10(bin, num_10_rez);
}

void scanf_value(struct uint1024_t *X) {
    char num_X[309];
    printf("Input number: ");
    scanf("%s", num_X);
    int len_X = strlen(num_X);
    int bin_X[1024];
    for (int i = 0; i < 1024; i++) bin_X[i] = 0;
    int bin_len_X = conversion_from_10_to_2(num_X, len_X, bin_X);
    conversion_from_2_to_256(bin_X, bin_len_X, X);
}

struct uint1024_t add_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t rez;
    for (int i = 0; i < 128; i++) rez.digit[i] = 0;
    int remainder = 0;
    for (int i = 0; i < 128; i++) {
        rez.digit[i] = (x.digit[i] + y.digit[i] + remainder) % 256;
        remainder = (x.digit[i] + y.digit[i]) / 256;
    }
    return rez;
}

int compare(struct uint1024_t x, struct uint1024_t y) {
    int len_x, len_y;
    int i = 127;
    while (x.digit[i] == 0) i--;
    len_x = i + 1;
    i = 127;
    while (y.digit[i] == 0) i--;
    len_y = i + 1;
    if (len_x < len_y)return -1;
    if (len_x > len_y) return 1;
    for (int i = len_x - 1; i >= 0; i--) {
        if (x.digit[i] < y.digit[i])return -1;
        if (x.digit[i] > y.digit[i])return 1;
    }
    return 0;
}

struct uint1024_t subtr_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t z;
    for (int i = 0; i < 128; i++)z.digit[i] = 0;
    int comp = compare(x, y);
    int len_x, len_y;
    int i = 127;
    while (x.digit[i] == 0) i--;
    len_x = i + 1;
    i = 127;
    while (y.digit[i] == 0) i--;
    len_y = i + 1;
    if (len_x > 128 || len_y > 128) {
        printf("Undefined Behavior");
        return z;
    } else {
        if (comp == -1) {
            for (int i = 0; i < 128; i++) {
                y.digit[i] = 255 - y.digit[i];
            }
            z = add_op(x, y);
            z.digit[0]++;
            return z;
        } else if (comp == 0) {
            for (int i = 0; i < 128; i++) {
                z.digit[i] = 0;
            }
            return z;
        } else if (comp == 1) {
            int len_x;
            int i = 127;
            while (x.digit[i] == 0) i--;
            len_x = i + 1;

            int c = 0;
            for (int j = 0; j < len_x; ++j) {
                c = c + x.digit[j] - y.digit[j] + 256;
                z.digit[j] = c % 256;
                if (c <= 255)c = -1;
                else c = 0;
            }
            return z;
        }
    }
}

struct uint1024_t mult_op(struct uint1024_t x, struct uint1024_t y) {
    struct uint1024_t ans;
    int len_x, len_y;
    int tmp_ans[128];
    for (int i = 0; i < 128; i++) tmp_ans[i] = 0;
    int i = 127;
    while (x.digit[i] == 0) i--;
    len_x = i + 1;
    i = 127;
    while (y.digit[i] == 0) i--;
    len_y = i + 1;
    int len_ans = len_x + len_y + 1;
    for (int ix = 0; ix < len_x; ix++) {
        for (int iy = 0; iy < len_y; iy++) {
            tmp_ans[ix + iy] += x.digit[ix] * y.digit[iy];
        }
    }
    for (int i = 0; i < len_ans; i++) {
        tmp_ans[i + 1] += tmp_ans[i] / 256;
        tmp_ans[i] = tmp_ans[i] % 256;
    }
    for (int i = 0; i < 128; i++) {
        ans.digit[i] = tmp_ans[i];
    }
    return ans;
}

struct uint1024_t from_uint(unsigned int x) {
    struct uint1024_t rez;
    for (int i = 0; i < 128; i++) rez.digit[i] = 0;
    int i = 0;
    while (x > 0) {
        rez.digit[i++] = x % 256;
        x = x / 256;
    }
    return rez;
}

int main() {
    struct uint1024_t A, B;
    scanf_value(&A);
    printf_value(A);
    printf("\n");
    scanf_value(&B);
    printf_value(B);
    char operation[20];
    char output[] = "Input operation: A+B A-B B-A A*B generation_from_uint\n";
    char output_gen[] = "Input operation: A+B GEN+A GEN+B A-B B-A GEN-A A-GEN GEN-B B-GEN A*B A*GEN B*GEN\n";
    int flag = 0;
    printf("\n%s", output);
    struct uint1024_t uint_number;
    while (scanf("%s", operation)) {
        if (!(strcmp(operation, "A+B"))) {
            struct uint1024_t C = add_op(A, B);
            printf_value(C);
        }
        else if (!(strcmp(operation, "GEN+A"))) {
            struct uint1024_t C = add_op(uint_number, A);
            printf_value(C);
        }
        else if (!(strcmp(operation, "GEN+B"))) {
            struct uint1024_t C = add_op(uint_number, B);
            printf_value(C);
        }
        else if (!(strcmp(operation, "A-B"))) {
            struct uint1024_t C = subtr_op(A, B);
            printf_value(C);
        }
        else if (!(strcmp(operation, "B-A"))) {
            struct uint1024_t C = subtr_op(B, A);
            printf_value(C);
        }
        else if (!(strcmp(operation, "GEN-A"))) {
            struct uint1024_t C = subtr_op(uint_number, A);
            printf_value(C);
        }
        else if (!(strcmp(operation, "A-GEN"))) {
            struct uint1024_t C = subtr_op(A, uint_number);
            printf_value(C);
        }
        else if (!(strcmp(operation, "GEN-B"))) {
            struct uint1024_t C = subtr_op(uint_number, B);
            printf_value(C);
        }
        else if (!(strcmp(operation, "B-GEN"))) {
            struct uint1024_t C = subtr_op(B, uint_number);
            printf_value(C);
        }
        else if (!(strcmp(operation, "A*B"))) {
            struct uint1024_t C = mult_op(A, B);
            printf_value(C);
        }
        else if (!(strcmp(operation, "A*GEN"))) {
            struct uint1024_t C = mult_op(A, uint_number);
            printf_value(C);
        }
        else if (!(strcmp(operation, "B*GEN"))) {
            struct uint1024_t C = mult_op(B, uint_number);
            printf_value(C);
        }
        else if (!(strcmp(operation, "generation_from_uint"))) {
            flag = 1;
            printf("Input uint number:");
            unsigned int a;
            scanf("%u", &a);
            uint_number = from_uint(a);
            printf_value(uint_number);
        }
        if (!flag)printf("\n%s", output);
        else printf("\n%s", output_gen);
    }
    return 0;
}