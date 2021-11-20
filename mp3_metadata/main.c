#include <stdio.h>
#include <malloc.h>
#include <string.h>

unsigned long filesize(char *filename) {
    FILE *file = fopen(filename, "r");
    unsigned long s;
    fseek(file, 0, SEEK_SET);
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        s = ftell(file);
    } else {
        return 0;
    }
    fseek(file, 0, SEEK_SET);
    return s;
}

int check(char *ident) {
    int checked = 0;
    if (!strcmp(ident, "TALB")) {
        printf("Album: ");
        checked = 1;
    }
    if (!strcmp(ident, "TDOR")) {
        printf("Original release time: ");
        checked = 1;
    }
    if (!strcmp(ident, "TIT2")) {
        printf("Song name: ");
        checked = 1;
    }
    if (!strcmp(ident, "TPE1")) {
        printf("Lead performer(s)/Soloist(s): ");
        checked = 1;
    }
    if (!strcmp(ident, "TPE2")) {
        printf("Band/orchestra/accompaniment: ");
        checked = 1;
    }
    if (!strcmp(ident, "TRCK")) {
        printf("Track number/Position in set: ");
        checked = 1;
    }
    if (!strcmp(ident, "TBPM")) {
        printf("Beats per minute: ");
        checked = 1;
    }
    if (!strcmp(ident, "TCON")) {
        printf("Kind of music: ");
        checked = 1;
    }
    if (!strcmp(ident, "TDRC")) {
        printf("Recording time: ");
        checked = 1;
    }
    if (!strcmp(ident, "TPOS")) {
        printf("Part of a set: ");
        checked = 1;
    }
    return checked;
}

void show(char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    char *data = malloc(sizeof(char) * (filesize(filename)));
    fread(data, 1, filesize(filename), file);
    unsigned long size1 = ((data[6] << 21) | ((data[7] << 14) | ((data[8] << 7) | (data[9]))));
    printf("%lu\n", size1);
    unsigned long cur = 10;
    while (cur <= size1) {
        char ident[5] = {data[cur], data[cur + 1], data[cur + 2], data[cur + 3], '\0'};
        cur = cur + 4;
        unsigned long size = ((data[cur] << 21) | ((data[cur + 1] << 14) | ((data[cur + 2] << 7) | (data[cur + 3]))));
        cur = cur + 6;
        if (check(ident)) {
            printf("(%s) ", ident);
            for (unsigned long i = 1; i < size; i++) {
                printf("%c", data[cur + i]);
            }
            printf("\n");
        } else {
            if (!strcmp("USLT",ident)){
                printf("To get Lyrics:\t--filepath=Song.mp3 --get=USLT");
            }
        }
        cur += size;
    }
    free(data);
    fclose(file);
}

void get(char *filename, char *name) {
    FILE *file;
    file = fopen(filename, "r");
    char *data = malloc(sizeof(char) * (filesize(filename)));
    fread(data, 1, filesize(filename), file);
    int size1 = ((data[6] << 21) | ((data[7] << 14) | ((data[8] << 7) | (data[9]))));
    int cur = 10;
    while (cur < size1) {
        char ident[5] = {data[cur], data[cur + 1], data[cur + 2], data[cur + 3], '\0'};
        cur = cur + 4;
        unsigned long size = ((data[cur] << 21) | ((data[cur + 1] << 14) | ((data[cur + 2] << 7) | (data[cur + 3]))));
        cur = cur + 6;
        if (!strcmp(ident, name)) {
            printf("(%s) ", ident);
            for (int i = 1; i < size; i++) {
                printf("%c", data[cur + i]);
            }
            printf("\n");
        }
        cur += size;
    }
    free(data);
    fclose(file);
}

void set(char *filename, char *name, char *value) {
    FILE *file = fopen(filename, "rb+");
    char *data = (char *) malloc(sizeof(char) * (filesize(filename)));
    fread(data, 1, filesize(filename), file);
    int cur = 10;
    while (data[cur] != 0) {
        int k = cur + 4;
        int size = data[k] * 65536 * 256 + data[k + 1] * 65536 + data[k + 2] * 256 + data[k + 3];
        if ((data[cur] == name[0]) && (data[cur + 1] == name[1]) && (data[cur + 2] == name[2]) &&
            (data[cur + 3] == name[3])) {
            cur = cur + 4;
            /*data[cur] = (svalue / (256 * 256 * 256)) % 256;
            data[cur + 1] = (svalue / (256 * 256)) % 256;
            data[cur + 2] = (svalue / 256) % 256;
            data[cur + 3] = svalue % 256;
            fseek(file, 0, SEEK_SET);
            fwrite(data, 1, cur + 6, file);
            fwrite(value, 1, svalue, file);*/
            fwrite(&data[cur + 6 + size], 1, filesize(filename) - size - cur - 6, file);
            break;

        } else cur = cur + 10 + size;
    }
    free(data);
    fclose(file);
}

int main(int argc, char *argv[]) {
    int i = 1;

    while (i < argc - 1) {
        //track_name parsing
/*        int index = 0;
        while (argv[i][index] != '=') index++;
        char track_name[256];
        index++;
        int k = index;
        int j = 0;
        while (k + j < strlen(argv[i])) {
            track_name[j] = argv[i][k + j];
            j++;
        }*/
        char *track_name = strpbrk(argv[i], "=") + 1;
        //
        if (argv[i + 1][2] == 's' && argv[i + 1][3] == 'h') {
            show(track_name);
        }
        //
        if (argv[i + 1][2] == 's' && argv[i + 1][3] == 'e') {
            char *prop_name = strpbrk(argv[i + 1], "=") + 1;
            char *prop_value = strpbrk(argv[i + 2], "=") + 1;
            set(track_name, prop_name, prop_value);
            i++;
        }

        if (argv[i + 1][2] == 'g') {
            char *prop_name = strpbrk(argv[i + 1], "=") + 1;
            get(track_name, prop_name);
        }

        i += 2;
    }

    return 0;
}