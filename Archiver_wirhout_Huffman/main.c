#include <stdio.h>
#include <string.h>

/*
Устройство архив файла:
В начале идет заголовок
<filename_1 filesize_1>...<<filename_n filesize_n>
Затем идут последовательно все данные файлов
 */

size_t filesize(char *filename) {
    FILE *file = fopen(filename, "r");
    size_t s;
    fseek(file, 0, SEEK_SET);
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        s = ftell(file);
    } else {
        return 0;
    }
    fseek(file, 0, SEEK_SET);
    fclose(file);
    return s;
}

void create_arch(char *archive_name, int argc, int file_name_start_index, char *file_names[]) {
    FILE *archive_file = fopen(archive_name, "wb");
    fseek(archive_file, 0, SEEK_SET);

    //Заголовок
    for (int i = file_name_start_index; i < argc; i++) {
        size_t cur_file_size = filesize(file_names[i]);
        char char_file_size[30];
        sprintf(char_file_size, "%zu", cur_file_size);
        fwrite("<", 1, 1, archive_file);
        fwrite(file_names[i], 1, strlen(file_names[i]), archive_file);
        fwrite(" ", 1, 1, archive_file);
        fwrite(char_file_size, 1, strlen(char_file_size), archive_file);
        fwrite(">", 1, 1, archive_file);
    }
    fwrite("?", 1, 1, archive_file);

    //Данные файлов
    for (int i = file_name_start_index; i < argc; i++) {
        FILE *cur_file = fopen(file_names[i], "rb");
        int cur_char;
        while ((cur_char = getc(cur_file)) != EOF) {
            putc(cur_char, archive_file);
        }
        fclose(cur_file);
    }

    fclose(archive_file);
}

void extract(char *archive_name) {
    FILE *archive_file = fopen(archive_name,"rb");
    unsigned long long cur_file_size;
    char cur_file_name[256];
    int cur_char;
    unsigned long long data_start = 0;
    while ((cur_char = getc(archive_file)) !='?'){
        data_start++;
    }
    data_start++;
    fseek(archive_file, 0, SEEK_SET);
    while(fscanf(archive_file,"<%s %llu>",cur_file_name,&cur_file_size) != 0){
        unsigned long long file_position = ftell(archive_file);
        fseek(archive_file, data_start, SEEK_SET);
        FILE *new_file = fopen(cur_file_name, "wb");
        for (unsigned long long i = 0; i < cur_file_size; i++){
            int cur_byte;
            cur_byte = getc(archive_file);
            putc(cur_byte, new_file);
        }
        data_start += cur_file_size;
        fseek(archive_file,file_position,SEEK_SET);
        fclose(new_file);
    }
    fclose(archive_file);
}

void list(char *archive_name) {
    FILE *archive_file = fopen(archive_name, "rb");
    char cur_file_name[256];
    unsigned long long char_file_size;
    while (fscanf(archive_file, "<%s %llu>", cur_file_name, &char_file_size) != 0) {
        printf("%s ", cur_file_name);
    }
    fclose(archive_file);
}

int main(int argc, char *argv[]) {
    char *archive_name;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--file")) {
            archive_name = argv[i + 1];
        }
        if (!strcmp(argv[i], "--create")) {
            create_arch(archive_name, argc, i + 1, argv);
        }
        if (!strcmp(argv[i], "--extract")) {
            extract(archive_name);
        }
        if (!strcmp(argv[i], "--list")) {
            list(archive_name);
        }
    }
    return 0;
}
