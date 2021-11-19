#include <stdio.h>
#include "malloc.h"
#include "string.h"

size_t filesize(char *filename) {
    FILE *file = fopen(filename, "r");
    size_t s;
    fseek(file,0,SEEK_SET);
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        s = ftell(file);
    } else {
        return 0;
    }
    fseek(file, 0, SEEK_SET);
    return s;
}

void show(char *filename) {
    FILE *file = fopen(filename, "r");
    char *data = (char *) malloc(sizeof(char) * (filesize(filename)));
    fread(data, 1, filesize(filename), file);
    int cur = 10;
    while (data[cur] != 0) {
        char ident[5] = {data[cur], data[cur + 1], data[cur + 2], data[cur + 3], '\0'};
        printf("%s: ", ident);
        cur = cur + 4;
        int size = data[cur] * 65536 * 256 + data[cur + 1] * 65536 + data[cur + 2] * 256 + data[cur + 3];
        cur = cur + 6;
        for (int i = 0; i < size; i++)
            printf("%c", data[cur + i]);
        cur = cur + size;
        printf("\n");
    }
    free(data);
    fclose(file);
}

void get(char *filename, char *name) {
    FILE *file = fopen(filename, "r");
    char *data = (char *) malloc(sizeof(char) * (filesize(filename)));
    fread(data, 1, filesize(filename), file);
    int cur = 10;
    while (data[cur] != 0) {
        int k=cur+4;
        int size = data[k] * 65536 * 256 + data[k+ 1] * 65536 + data[k + 2] * 256 + data[k + 3];
        if ((data[cur] == name[0]) && (data[cur + 1] == name[1]) && (data[cur + 2] == name[2]) &&
            (data[cur + 3] == name[3])) {
            printf("%s:", name);
            cur = cur + 4;
            cur = cur + 6;
            for (int i = 0; i < size; i++)
                printf("%c", data[cur + i]);
            printf("\n");
            break;
        }else cur=cur+10+size;
    }
    free(data);
    fclose(file);
}

void set(char *filename, char *name, char *value, int svalue){
    FILE *file = fopen(filename, "rb+");
    char *data = (char *) malloc(sizeof(char) * (filesize(filename)));
    fread(data, 1, filesize(filename), file);
    int cur = 10;
    while (data[cur] != 0){
        int k=cur+4;
        int size = data[k] * 65536 * 256 + data[k+ 1] * 65536 + data[k + 2] * 256 + data[k + 3];
        if ((data[cur] == name[0]) && (data[cur + 1] == name[1]) && (data[cur + 2] == name[2]) &&
            (data[cur + 3] == name[3])){
            cur = cur + 4;
            data[cur]=(svalue/(256*256*256))%256;
            data[cur+1]=(svalue/(256*256))%256;
            data[cur+2]=(svalue/256)%256;
            data[cur+3]=svalue%256;
            fseek(file, 0, SEEK_SET);
            fwrite(data,1,cur+6, file);
            fwrite(value, 1, svalue, file);
            fwrite(&data[cur+6+size],1,filesize(filename)-size-cur-6,file);
            break;

        } else cur=cur+10+size;
    }
    free(data);
    fclose(file);
}

int main(int argc, char *argv[]) {
    int i=1;
    while((i+1)<argc) {
        char *filename = strpbrk(argv[i], "=") + 1;
        if(argv[i+1][2]=='s')
            if(argv[i+1][3]=='h')
                show(filename);
            else{
                char *identification=strpbrk(argv[i+1], "=") + 1;
                char *value=strpbrk(argv[i+2], "=") + 1;
                set(filename,identification, value, strlen(value));
                i++;
            }
        else {
            char *identification=strpbrk(argv[i+1], "=") + 1;
            get(filename,identification);
        }
        i=i+2;
    }
}