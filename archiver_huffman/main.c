#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

struct node {
    unsigned char ch; //ASCII- код
    unsigned long long freq; //частота встречаемости
    unsigned char code[8];// массив для нового кода каждого символа
    struct node *left; // левый потомок в дереве
    struct node *right;// правый потомок в дереве
};

struct decryption {
    unsigned char ch;
    unsigned char code[8];
};

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

void extract(char *archive_name) {
    FILE *archive_file = fopen(archive_name, "rb");
    char cur_file_name[256];
    unsigned long long char_file_size;
    int cur_char;
    unsigned long long data_start = 0;
    while ((cur_char = getc(archive_file)) != '?') {
        data_start++;
    }
    unsigned long long input_file_size = filesize(archive_name) - data_start;
    unsigned char *data = calloc(input_file_size, sizeof(unsigned char));
    fseek(archive_file, data_start, SEEK_SET);
    fread(data, 1, input_file_size, archive_file);
    fseek(archive_file, 0, SEEK_SET);
    unsigned long long cur_byte = 1;
    while (fscanf(archive_file, "<%s %llu>", cur_file_name, &char_file_size) != 0) {
        printf("%s\n", cur_file_name);
        FILE *cur_file = fopen(cur_file_name, "wb");
        //Получим количество закодированных байтов
        unsigned char *str_decode_byte_count = calloc(sizeof(char), 20);
        int index_decode_byte_count = 0;
        while (data[cur_byte] != '!') {
            str_decode_byte_count[index_decode_byte_count++] = data[cur_byte++];
        }
        unsigned long long decode_byte_count = atoi(str_decode_byte_count);
        //////////////////////////////////////////////////////////////////////////////////////
        cur_byte++;
        int char_coded_count = data[cur_byte];
        cur_byte += 2;
        //Запоминаем информацию о кодировании
        struct decryption char_struct[char_coded_count];
        int input_struct_arr = 0;
        for (int decode_index = 0; decode_index < char_coded_count; decode_index++) {
            char_struct[input_struct_arr].ch = data[cur_byte];
            cur_byte += 2;
            int k = 0;
            for (int q = 0; q < 8; q++) {
                char_struct[input_struct_arr].code[q] = 0;
            }
            while (data[cur_byte] != '-') {
                char_struct[input_struct_arr].code[k++] = data[cur_byte++];
            }
            input_struct_arr++;
            cur_byte++;
        }

        for (int i = 0; i < char_coded_count; i++) {
            printf("%c %s\n", char_struct[i].ch, char_struct[i].code);
        }
        //printf("%c",data[cur_byte]);
        //переводим байты в биты для дешифровки
        unsigned char *bit_translate = calloc(sizeof(char), decode_byte_count * 8);
        unsigned long long cur_bit = 0;
        for (int i = 0; i < decode_byte_count; i++) {
            int tmp = data[cur_byte + i];
            int dev = 128;
            while (dev > 0) {
                bit_translate[cur_bit++] = tmp / dev + '0';
                tmp -= (tmp / dev) * dev;
                dev /= 2;
            }
        }
        //Имеем бинарную строку, которую необходимо расшифровать по ключам в массиве
        unsigned char *result_byte_code = calloc(sizeof(char), char_file_size);
        unsigned long long result_index = 0;
        unsigned long long bin_index = 0;
        for (int i = 0; i < char_file_size; i++) {
            unsigned char buffer[8];
            for (int k = 0; k < 8; k++) buffer[k] = 0;
            int q = 0;
            int flag = 0;
            while (!flag && q < 8) {
                buffer[q++] = bit_translate[bin_index++];
                for (int index_check = 0; index_check < char_coded_count; index_check++) {
                    if (!strcmp(buffer, char_struct[index_check].code)) {
                        flag = 1;
                        result_byte_code[result_index++] = char_struct[index_check].ch;
                    }
                }
            }
        }
        fwrite(result_byte_code, 1, result_index, cur_file);
        cur_byte += decode_byte_count;
        cur_byte++;
        free(bit_translate);
        free(str_decode_byte_count);
        fclose(cur_file);
        printf("\n\n");
    }
    fclose(archive_file);
}

struct node *buildTree(struct node *node_address[], int char_count) {
    struct node *tmp = malloc(sizeof(struct node));

    //складываем частоты двух минимальных
    tmp->freq = node_address[char_count - 2]->freq + node_address[char_count - 1]->freq;
    tmp->left = node_address[char_count - 1];
    tmp->right = node_address[char_count - 2];
    tmp->code[0] = 0;

    //осталось 2 нерассмотренных элемента дерева - значит tmp и есть корень дерева
    if (char_count <= 2)
        return tmp;
    else {
        //если осталось не 2, то ставим tmp на свое место
        //сортируем по не возрастанию частот встреч
        //сортируем вставкой
        for (int i = 0; i < char_count; i++) {
            if (tmp->freq > node_address[i]->freq) {
                for (int j = char_count - 1; j > i; j--)
                    node_address[j] = node_address[j - 1];
                node_address[i] = tmp;
                break;
            }
        }
    }
    //рекурсивно делаем все то же самое для меньшего числа рассматриваемых узлов
    //меньше, т.к. последний элемент уже является листом, т.е. не может иметь детей
    return buildTree(node_address, char_count - 1);
}

void makeCodes(struct node *root) {
    //влево - ноль
    if (root->left) {
        strcpy(root->left->code, root->code);
        strcat(root->left->code, "0");
        makeCodes(root->left);
    }
    //вправо - один
    if (root->right) {
        strcpy(root->right->code, root->code);
        strcat(root->right->code, "1");
        makeCodes(root->right);
    }
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

void translate_bit_in_byte(unsigned char *compression_data, unsigned char *data_bit) {
    unsigned long long str_len = strlen(data_bit);
    unsigned long long index_result = 0;
    unsigned long long i = 0;
    while (i < str_len) {
        int buffer = 0;
        int tmp = 128;
        for (int j = 0; j < 8; j++) {
            buffer += (data_bit[i + j] - 48) * tmp;
            tmp /= 2;
        }
        compression_data[index_result] = buffer;
        index_result++;
        i += 8;
    }
}

void create_arch(char *archive_name, int argc, int file_name_start_index, char *file_names[]) {
    FILE *file_output = fopen(archive_name, "wb");
    //заголовок <filename filesize>
    for (int i = file_name_start_index; i < argc; i++) {
        char file_name_and_size[256] = {"\0"};
        strcat(file_name_and_size, "<");
        strcat(file_name_and_size, file_names[i]);
        strcat(file_name_and_size, " ");
        char str_file_size[20];
        sprintf(str_file_size, "%llu", filesize(file_names[i]));
        strcat(file_name_and_size, str_file_size);
        strcat(file_name_and_size, ">");
        fwrite(file_name_and_size, 1, strlen(file_name_and_size), file_output);
        //printf("%s ",file_name_and_size);
    }
    fwrite("?", 1, 1, file_output);

    //кодируем каждый файл
    for (int i = file_name_start_index; i < argc; i++) {

        /*Несколько этапов сжатия:
         1) Выделение памяти под все данные
         2) Подсчет количества частоты вхождения каждого символа
         3) Созданные массива структур и массива указателей
         4) Построение дерева по этим указателям
         5) Кодирование каждого символа исходя из глубины пути до данного листа(символа) в дереве
         6) Запись данных о кодировании и сжатых данных в архив
        */

        //выделение памяти под данные текущего файла
        size_t cur_file_size = filesize(file_names[i]);
        FILE *file = fopen(file_names[i], "rb");
        unsigned char *data = malloc(sizeof(unsigned char) * cur_file_size);
        fseek(file, 0, SEEK_SET);
        fread(data, 1, cur_file_size, file);

        //создание массива пар для подсчета частоты вхождения символов(байт) в исходный файл
        unsigned long long frequency[256][2];
        for (int j = 0; j < 256; j++) {
            frequency[j][0] = j;
            frequency[j][1] = 0;
        }

        //подсчет количества символов
        for (size_t j = 0; j < cur_file_size; j++) {
            frequency[data[j]][1]++;
        }

        // сортировка символов по убыванию их количества
        for (int j = 0; j < 255; j++) {
            for (int k = j + 1; k < 256; k++) {
                if (frequency[j][1] < frequency[k][1]) {
                    unsigned long long tmp = frequency[j][0];
                    frequency[j][0] = frequency[k][0];
                    frequency[k][0] = tmp;
                    tmp = frequency[j][1];
                    frequency[j][1] = frequency[k][1];
                    frequency[k][1] = tmp;
                }
            }
        }

        //подсчет мощности алфавита
        int char_count = 0;
        for (int j = 0; j < 256; j++) {
            if (frequency[j][1] != 0) char_count++;
        }

        /*массив структур из вершин дерева*/
        struct node bin_tree_node[char_count];
        // внесение данных о частоте символов в массив структур
        for (int j = 0; j < char_count; j++) {
            bin_tree_node[j].ch = frequency[j][0];
            bin_tree_node[j].freq = frequency[j][1];
            strcpy(bin_tree_node[j].code, "");
            bin_tree_node[j].left = NULL;
            bin_tree_node[j].right = NULL;
        }

        /*массив указатель на структуры необходим для создания дерева*/
        struct node *node_address[char_count];
        for (int j = 0; j < char_count; j++) {
            node_address[j] = &bin_tree_node[j];
        }
        /* строим бинарное дерево root- корень дерева*/
        struct node *root = buildTree(node_address, char_count);
        makeCodes(root);
        printf("Encoding %d file:\n", i - file_name_start_index + 1);
        for (int j = 0; j < char_count; j++) {
            printf("*%c*-%s\n", bin_tree_node[j].ch, bin_tree_node[j].code);
        }
        //Остался этап кодировки наших данных
        unsigned long long result_size_bit = 0;
        for (int j = 0; j < char_count; j++) {
            result_size_bit += strlen(bin_tree_node[j].code) * bin_tree_node[j].freq;
        }
        result_size_bit = ((result_size_bit + 7) / 8) * 8 + 1;
        unsigned char *data_bit = calloc(sizeof(unsigned char), result_size_bit);
        //ищем текущий символ в массиве данных о количестве и записываем битовое представление в строку
        unsigned long long bit_input_index = 0;
        for (int j = 0; j < cur_file_size; j++) {
            int cur_char_index = 0;
            //ищем нужный символ в закодированном списке
            for (int k = 0; k < char_count; k++) {
                if (data[j] == bin_tree_node[k].ch) {
                    cur_char_index = k;
                    break;
                }
            }
            //склеиваем все в одну строку, далее будем ее разделять на готовые байты (уже сжатые данные)
            for (int k = 0; k < strlen(bin_tree_node[cur_char_index].code); k++) {
                data_bit[bit_input_index] = bin_tree_node[cur_char_index].code[k];
                bit_input_index++;
            }
        }
        while (bit_input_index < result_size_bit) {
            data_bit[bit_input_index] = '0';
            bit_input_index++;
        }
        data_bit[bit_input_index++] = '\0';

        //Переводим массив битов в байты
        unsigned char *compression_data = calloc(sizeof(unsigned char), (result_size_bit / 8));
        translate_bit_in_byte(compression_data, data_bit);

        unsigned long long symbol_count = 0;
        symbol_count = result_size_bit / 8;
        char str_symbol_count[20];
        sprintf(str_symbol_count, "%lld", symbol_count);
        fwrite(str_symbol_count, 1, strlen(str_symbol_count), file_output);
        fwrite("!", 1, 1, file_output);
        putc(char_count, file_output);
        fwrite("-", 1, 1, file_output);

        /* Имеем массив compression_data, в котором содержится информация, сжатая алгоритмом Хаффмана.
        Данная информация будет идти в архиве следующий образом:
               <..><filename filesize>?x!y-a-010-b-100-... c-101-compression_data<>...
        Сначала идет заголовок, затем данные о кодировка, собственно сами данные и так для каждого файла*/

        //Вписываем данные о кодировке
        for (int k = 0; k < char_count; k++) {
            unsigned char *char_input[] = {bin_tree_node[k].ch};
            fwrite(char_input, 1, 1, file_output);
            fwrite("-", 1, 1, file_output);
            fwrite(bin_tree_node[k].code, 1, strlen(bin_tree_node[k].code), file_output);
            fwrite("-", 1, 1, file_output);
        }
        //Вставляем закодированные данные
        fwrite(compression_data, 1, result_size_bit / 8, file_output);
        fwrite("?", 1, 1, file_output);

        printf("finished encoding %d file\n\n", i - file_name_start_index + 1);
        //Очищаем выделенную память
        free(data_bit);
        free(data);
        free(compression_data);
        fclose(file);
    }

    fclose(file_output);
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