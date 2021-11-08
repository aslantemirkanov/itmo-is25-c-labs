#include <stdio.h>
#include <string.h>


int conversion_date_to_sec(char *date) {
    long int second = 0;
    second += ((date[0] - 48) * 10 + (date[1] - 48) - 1) * 86400;
    second += ((date[12] - 48) * 10 + (date[13] - 48)) * 3600;
    second += ((date[15] - 48) * 10 + (date[16] - 48)) * 60;
    second += (date[18] - 48) * 10 + date[19] - 48;
    return second;
}

void errors_count() {
    FILE *file_input;
    file_input = fopen("access_log_Jul95.txt", "r");
    FILE *file_output;
    file_output = fopen("errors.txt", "w");
    char str[500];
    char *str_pointer;
    int error_str_count = 0;
    while (1) {
        str_pointer = fgets(str, sizeof(str), file_input);
        //конец файла
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

    return;
}

void max_time_window_request(int time_window) {
    FILE *file_input;
    file_input = fopen("access_log_Jul95.txt", "r");
     FILE *file_output;
     file_output = fopen("times.txt", "w");
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
        long int second = conversion_date_to_sec(date);
        fprintf(file_output,"%d\n", second);
    }
    fclose(file_input);
    fclose(file_output);
    return;

}

int main() {
    //errors_count();
    int time_window;
    printf("Input time window in seconds:\n");
    scanf("%d", &time_window);
    max_time_window_request(time_window);
    return 0;
}
