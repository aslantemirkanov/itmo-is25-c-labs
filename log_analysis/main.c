#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int conversion_date_to_sec(char *date) {
    long int second = 0;
    second += ((date[0] - 48) * 10 + (date[1] - 48) - 1) * 86400;
    second += ((date[12] - 48) * 10 + (date[13] - 48)) * 3600;
    second += ((date[15] - 48) * 10 + (date[16] - 48)) * 60;
    second += (date[18] - 48) * 10 + date[19] - 48;
    return second;
}

int errors_count() {
    FILE *file_input;
    file_input = fopen("access_log_Jul95.txt", "r");
    FILE *file_output;
    file_output = fopen("errors.txt", "w");
    int str_count = 0;
    char str[500];
    char *str_pointer;
    int error_str_count = 0;
    while (1) {
        str_pointer = fgets(str, sizeof(str), file_input);
        str_count++;
        if (str_pointer == NULL) {
            if (feof(file_input) != 0) {
                fprintf(file_output, "reading has finished\n");
                break;
            }
        }
        int str_len = strlen(str);
        int i = str_len;
        while (str[i] != '"' && i >= 0) {
            i--;
        }
        if (str[i + 2] == '5') {
            error_str_count++;
            fprintf(file_output, "%d - %s", error_str_count, str);
        }
    }
    fprintf(file_output, "%d", error_str_count);
    fclose(file_input);
    fclose(file_output);
    return str_count;
}

void str_print(int first, int last, int max_len, int time_window) {
    FILE *file_input;
    file_input = fopen("access_log_Jul95.txt", "r");
    FILE *file_output;
    file_output = fopen("max_request.txt", "w");
    fprintf(file_output, "first-%d last-%d max_request_count-%d time_window- %d\n", first, last, max_len, time_window);
    long int index = 0;
    char str[500];
    char *str_pointer;
    while (1) {
        str_pointer = fgets(str, sizeof(str), file_input);
        if (str_pointer == NULL) {
            if (feof(file_input) != 0) {
                break;
            }
        }
        if (index == first) {
            fprintf(file_output, "%d- %s", first, str);
        }
        if (index == last) {
            fprintf(file_output, "%d- %s", last, str);
        }
        index++;
    }
    if (index == last) {
            fprintf(file_output, "%d- %s", last, str);
        }
    fclose(file_input);
    fclose(file_output);
}

void max_time_window_request(int time_window, int str_count) {
    FILE *file_input;
    file_input = fopen("access_log_Jul95.txt", "r");
    int *times_in_second = (int *) (long) malloc(sizeof(long) * str_count);
    long int index = 0;
    char str[500];
    char *str_pointer;
    while (1) {
        str_pointer = fgets(str, sizeof(str), file_input);
        if (str_pointer == NULL) {
            if (feof(file_input) != 0) {
                printf("reading has finished\n");
                break;
            }
        }
        int i = 0;
        while (str[i] != '[') i++;
        char date[30];
        int date_index = 0;
        i++;
        while (str[i] != ']') {
            date[date_index++] = str[i];
            i++;
        }
        times_in_second[index++] = conversion_date_to_sec(date);
    }
    int len_arr = index;
    int max_len = 0;
    int first = 0, last = 0;
    int first_rez, last_rez;
    int buffer;
    while (last <= len_arr) {
        buffer = times_in_second[last] - times_in_second[first];
        if (buffer > time_window) {
            while (buffer > time_window) {
                first++;
                buffer = times_in_second[last] - times_in_second[first];
            }
        }
        if (buffer <= time_window) {
            if (last - first > max_len) {
                max_len = last - first;
                first_rez = first;
                last_rez = last;
            }
        }
        last++;
    }
    str_print(first_rez, last_rez, max_len + 1, time_window);
    fclose(file_input);
    free(times_in_second);
}

int main() {
    int str_count = errors_count();
    int time_window;
    printf("Input time window in seconds:\n");
    scanf("%d", &time_window);
    max_time_window_request(time_window, str_count);
    return 0;
}