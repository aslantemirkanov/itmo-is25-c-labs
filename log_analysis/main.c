#include <stdio.h>
#include <string.h>


void errors_count(){
    FILE *file_input;
    file_input= fopen("access_log_Jul95.txt", "r");
    FILE *file_output;
    file_output= fopen("result.txt", "w");
    int chr;
    char str[500];
    char *str_pointer;
    int error_str_count=0;
    while (1) {
        str_pointer = fgets(str, sizeof(str), file_input);
        //конец файла
        if (str_pointer == NULL) {
            if (feof(file_input) != 0) {
                fprintf(file_output,"reading has finished\n");
                break;
            }
        }
        int str_len= strlen(str);
        int i=str_len;
        while(str[i]!='"' && i>=0){
            i--;
        }
        if (str[i+2]=='5'){
            error_str_count++;
            fprintf(file_output,"%d - %s",error_str_count,str);
        }
    }
    fprintf(file_output,"%d",error_str_count);
    fclose(file_input);
    fclose(file_output);

    return;
}

int main() {
    errors_count();

    return 0;

}
