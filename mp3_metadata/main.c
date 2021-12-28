#include <stdio.h>
#include <malloc.h>
#include <string.h>

int check(char *ident) {
    int checked = 0;
    if (!strcmp(ident, "TALB")) {
        printf("Album: ");
        checked = 1;
    } else if (!strcmp(ident, "TDOR")) {
        printf("Original release time: ");
        checked = 1;
    } else if (!strcmp(ident, "TIT2")) {
        printf("Song name: ");
        checked = 1;
    } else if (!strcmp(ident, "TPE1")) {
        printf("Lead performer(s)/Soloist(s): ");
        checked = 1;
    } else if (!strcmp(ident, "TPE2")) {
        printf("Band/orchestra/accompaniment: ");
        checked = 1;
    } else if (!strcmp(ident, "TRCK")) {
        printf("Track number/Position in set: ");
        checked = 1;
    } else if (!strcmp(ident, "TBPM")) {
        printf("Beats per minute: ");
        checked = 1;
    } else if (!strcmp(ident, "TCON")) {
        printf("Kind of music: ");
        checked = 1;
    } else if (!strcmp(ident, "TDRC")) {
        printf("Recording time: ");
        checked = 1;
    } else if (!strcmp(ident, "TPOS")) {
        printf("Part of a set: ");
        checked = 1;
    }
    return checked;
}

void show(char *track_name) {
    FILE *file;
    file = fopen(track_name, "rb");

    fseek(file, 6, SEEK_SET);
    int size_arr[4];
    for (int i = 0; i < 4; i++) {
        size_arr[i] = fgetc(file);
    }
    fseek(file, 0, SEEK_SET);
    unsigned long size_file = ((size_arr[0] << 21) | (size_arr[1] << 14) | (size_arr[2] << 7) | (size_arr[3])) + 10;

    char *data = malloc(sizeof(char) * size_file);
    fread(data, 1, size_file, file);
    unsigned long index = 10;

    while (index <= size_file) {
        char ident[5] = {data[index], data[index + 1], data[index + 2], data[index + 3], '\0'};
        index = index + 4;
        unsigned long size = ((data[index] << 21) |
                              ((data[index + 1] << 14) | ((data[index + 2] << 7) | (data[index + 3]))));
        index = index + 6;
        if (check(ident)) {
            printf("(%s) ", ident);
            for (unsigned long i = 0; i < size; i++) {
                printf("%c", data[index + i]);
            }
            printf("\n");
        } else {
            if (!strcmp("USLT", ident)) {
                printf("To get Lyrics:\t--filepath=Song.mp3 --get=USLT");
            }
        }
        index = index + size;
    }
    free(data);
    fclose(file);
}

void set(char *track_name, char *prop_name, char *prop_value) {
    FILE *file;
    file = fopen(track_name, "rb+");

    fseek(file, 6, SEEK_SET);
    int size_arr[4];
    for (int i = 0; i < 4; i++) {
        size_arr[i] = fgetc(file);
    }
    unsigned long size_file = ((size_arr[0] << 21) | ((size_arr[1] << 14) | ((size_arr[2] << 7) | (size_arr[3])))) + 10;

    char *data = malloc(sizeof(char) * size_file);
    fseek(file, 0, SEEK_SET);
    fread(data, 1, size_file, file);
    fseek(file, 0, SEEK_SET);
    unsigned long index = 10;
    unsigned long input_size = strlen(prop_value);

    while (index < size_file) {
        unsigned long size = ((data[index + 4] << 21) |
                              ((data[index + 5] << 14) | ((data[index + 6] << 7) | (data[index + 7]))));
        char ident[5] = {data[index], data[index + 1], data[index + 2], data[index + 3], '\0'};
        if (!strcmp(ident, prop_name)) {
            index = index + 4;
            data[index] = (input_size >> 21) % 256;
            data[index + 1] = (input_size >> 14) % 256;
            data[index + 2] = (input_size >> 7) % 256;
            data[index + 3] = input_size % 256;
            fseek(file, 0, SEEK_SET);
            fseek(file, index, SEEK_SET);
            fwrite(&data[index], 1, 6, file);
            fwrite(prop_value, 1, input_size, file);
            fwrite(&data[index + size + 6], 1, size_file - (index + size + 6) - 1, file);
            break;
        } else {
            index += size + 10;
        }
    }
    free(data);
    fclose(file);
}

void get(char *track_name, char *name) {
    FILE *file;
    file = fopen(track_name, "rb");

    fseek(file, 6, SEEK_SET);
    int size_arr[4];
    for (int i = 0; i < 4; i++) {
        size_arr[i] = fgetc(file);
    }
    fseek(file, 0, SEEK_SET);
    unsigned long size_file = ((size_arr[0] << 21) | (size_arr[1] << 14) | (size_arr[2] << 7) | (size_arr[3])) + 10;

    char *data = malloc(sizeof(char) * size_file);
    fread(data, 1, size_file, file);
    unsigned long index = 10;

    while (index < size_file) {
        char ident[5] = {data[index], data[index + 1], data[index + 2], data[index + 3], '\0'};
        index = index + 4;
        unsigned long size = ((data[index] << 21) |
                              ((data[index + 1] << 14) | ((data[index + 2] << 7) | (data[index + 3]))));
        index = index + 6;
        if (!strcmp(ident, name)) {
            printf("(%s) ", ident);
            for (int i = 0; i < size; i++) {
                printf("%c", data[index + i]);
            }
            printf("\n");
            break;
        }
        index = index + size;
    }
    free(data);
    fclose(file);
}

int main(int argc, char *argv[]) {
    int i = 1;
    while (i < argc - 1) {
        char *track_name = strpbrk(argv[i], "=") + 1;

        if (!strcmp(argv[i + 1], "--show")) {
            show(track_name);
        }

        char tmp[256];
        strcpy(tmp, argv[i + 1]);
        char *ident = strtok(tmp, " ,=");

        if (!strcmp(ident, "--set")) {
            char *prop_name = strpbrk(argv[i + 1], "=") + 1;
            char *tmp_value = strpbrk(argv[i + 2], "=") + 1;
            char prop_value[400];
            snprintf(prop_value, sizeof(prop_value), "%c%s%c", '\x03', tmp_value, 32);
            set(track_name, prop_name, prop_value);
            i++;
        }

        if (!strcmp(ident, "--get")) {
            char *prop_name = strpbrk(argv[i + 1], "=") + 1;
            get(track_name, prop_name);
        }
        i = i + 2;
    }

    return 0;
}
