#include <stdio.h>
#include <string.h>

#define IN 1
#define OUT 0

int lnCount() {
    FILE *file;
    file = fopen("test.txt", "r");
    int chr, lineCount;
    if ((chr = getc(file)) != EOF) {
        lineCount = 1;
        while ((chr = getc(file)) != EOF)
            if (chr == '\n')
                lineCount++;
    } else {
        lineCount = 0;
    }
    printf("lines %d\n", lineCount);
    fclose(file);
}

int litCount() {
    FILE *file;
    file = fopen("test.txt", "r");
    long litterCount;
    litterCount = 0;
    int chr;
    while ((chr = getc(file)) != EOF)
        ++litterCount;
    printf("bytes %ld\n", litterCount);
    fclose(file);
}

int WdCount() {
    FILE *file;
    file = fopen("test.txt", "r");
    int chr, numberWord, stateWord;
    numberWord = 0;
    stateWord = OUT;
    while ((chr = getc(file)) != EOF) {
        if ((chr == ' ') || (chr == '\n') || (chr == '\t')) {
            stateWord = OUT;
        } else if (stateWord == OUT) {
            stateWord = IN;
            numberWord++;
        }
    }
    printf("words %d\n", numberWord);
    fclose(file);
}

int main(int argc, char *argv[]){
    FILE *file;
    file = fopen("test.txt", "r");
    fclose(file);
    char fileName[] = "test.txt";
    char ln1[] = "--lines";
    char ln2[] = "-l";
    char bt1[] = "--bytes";
    char bt2[] = "-c";
    char wd1[] = "--words";
    char wd2[] = "-w";
    int Flag[3];
    for (int i = 0; i < 3; i++) {
        Flag[i] = 0;
    }
    if (strcmp(argv[argc-1], fileName)!=0) {
        printf("you loser\n");
    } else {
        for (int i = 1; i < argc - 1; i++) {
            if ((!(strcmp(argv[i], ln1))) || (!(strcmp(argv[i], ln2)))) {
                if ((Flag[0] == 0)) {
                    Flag[0] = 1;
                    lnCount();
                }
            } else if (((!(strcmp(argv[i], bt1))) || (!(strcmp(argv[i], bt2))))) {
                if ((Flag[1] == 0)) {
                    Flag[1] = 1;
                    litCount();
                }
            } else if (((!(strcmp(argv[i], wd1))) || (!(strcmp(argv[i], wd2))))) {
                if ((Flag[2] == 0)) {
                    Flag[2] = 1;
                    WdCount();
                }
            } else {
                printf("you loser\n");
            }
        }
    }
    return 0;
}