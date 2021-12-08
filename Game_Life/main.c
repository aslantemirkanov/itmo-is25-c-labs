#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

int height, width, file_size, pixel_start;
char cur_generation_string[100];
int cur_generation = 0;
int max_iter = 500;
int frequency_of_generation_creation = 1;

void file_sizes_count(char *file_name) {
    FILE *file = fopen(file_name, "rb");
    fseek(file, 2, SEEK_SET);

    int input_byte;
    file_size = 0;
    int tmp = 1;
    for (int i = 0; i < 4; i++) {
        input_byte = fgetc(file);
        file_size += input_byte * tmp;
        tmp *= 256;
    }

    fseek(file, 10, SEEK_SET);
    tmp = 1;
    pixel_start = 0;
    for (int i = 0; i < 4; i++) {
        input_byte = fgetc(file);
        pixel_start += input_byte * tmp;
        tmp *= 256;
    }

    fseek(file, 18, SEEK_SET);
    tmp = 1;
    width = 0;
    for (int i = 0; i < 4; i++) {
        input_byte = fgetc(file);
        width += input_byte * tmp;
        tmp *= 256;
    }

    fseek(file, 22, SEEK_SET);
    tmp = 1;
    height = 0;
    for (int i = 0; i < 4; i++) {
        input_byte = fgetc(file);
        height += input_byte * tmp;
        tmp *= 256;
    }

    fclose(file);
}

void pixel_read(char *file_name, char **pixel_data) {

    FILE *file1 = fopen("out.txt", "wb");
    FILE *file = fopen(file_name, "rb");
    fseek(file, pixel_start, SEEK_SET);

    int bytes_count = height * ((width + 31) / 32) * 4;
    int height_pointer = height - 1;
    int width_pointer = 0;
    int i = 1;

    while (i <= bytes_count) {

        char bits[8];
        char cur_bytes = fgetc(file);
        int tmp_dev = 128;

        for (int q = 0; q < 8; q++) {
            bits[q] = (cur_bytes & tmp_dev) / tmp_dev;
            tmp_dev /= 2;
        }

        for (int j = 0; j < 8; j++) {
            pixel_data[height_pointer][width_pointer] = bits[j];
            width_pointer++;

            if (width_pointer >= width) {
                width_pointer = 0;
                height_pointer = height_pointer - 1;

                while (i % 4 != 0) {
                    fgetc(file);
                    i++;
                }

                if (height_pointer < 0) {
                    break;
                }

                break;
            }
        }

        if (height_pointer < 0) {
            break;
        }

        if (height_pointer <= 0 && width_pointer >= width) {
            break;
        }

        i++;
    }

    fclose(file);
    fclose(file1);
}

void output_file(char *file_name, char **pixel_data, char *output_file_name) {

    FILE *file = fopen(file_name, "rb");

    char dir_name[200];
    strcpy(dir_name, output_file_name);

    sprintf(cur_generation_string, "%d", cur_generation++);
    strcat(dir_name, cur_generation_string);
    strcat(dir_name, ".bmp");

    FILE *file_output = fopen(dir_name, "wb");

    char *data = malloc(sizeof(char) * file_size);
    fseek(file, 0, SEEK_SET);
    fread(data, 1, file_size, file);

    int cur_height_byte = pixel_start;
    int str_byte_size = ((width + 31) / 32) * 4;

    for (int i = height - 1; i >= 0; i--) {

        int cur_width_byte = 0;
        int width_pointer = 0;

        while (width_pointer < width) {
            int buffer = 0;
            int tmp = 128;

            for (int k = 0; k < 8; k++) {
                buffer += pixel_data[i][width_pointer] * tmp;
                width_pointer++;
                tmp /= 2;
                if (width_pointer >= width) {
                    while (tmp >= 1) {
                        buffer *= 2;
                        tmp /= 2;
                    }
                    break;
                }
            }

            data[cur_height_byte + cur_width_byte] = buffer;
            cur_width_byte++;

            if (width_pointer >= width) {
                while (cur_width_byte < str_byte_size) {
                    data[cur_height_byte + cur_width_byte] = 0;
                    cur_width_byte++;
                }
                break;
            }

        }
        cur_height_byte += str_byte_size;
    }

    fseek(file_output, 0, SEEK_SET);
    fwrite(data, 1, file_size, file_output);

    fclose(file);
    fclose(file_output);

    free(data);
}

int neighbors(char **pixel_data, int i, int j) {
    int n_count = 0;
    n_count += pixel_data[(height + (i - 1) % height) % height][j];
    n_count += pixel_data[(height + (i + 1) % height) % height][j];
    n_count += pixel_data[i][(width + (j - 1) % width) % width];
    n_count += pixel_data[i][(width + (j + 1) % width) % width];
    n_count += pixel_data[(height + (i - 1) % height) % height][(width + (j + 1) % width) % width];
    n_count += pixel_data[(height + (i - 1) % height) % height][(width + (j - 1) % width) % width];
    n_count += pixel_data[(height + (i + 1) % height) % height][(width + (j + 1) % width) % width];
    n_count += pixel_data[(height + (i + 1) % height) % height][(width + (j - 1) % width) % width];
    return (8 - n_count);
}

void game_life(char **pixel_data) {
    char **tmp_pixel_data = malloc(sizeof(char *) * height);
    for (int i = 0; i < height; i++) {
        tmp_pixel_data[i] = malloc(sizeof(char) * width);
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int neighbors_count = neighbors(pixel_data, i, j);

            if (pixel_data[i][j] == 0) {
                if (neighbors_count < 2 || neighbors_count > 3) tmp_pixel_data[i][j] = 1;
                else tmp_pixel_data[i][j] = 0;
            } else if (pixel_data[i][j] == 1) {
                if (neighbors_count == 3) {
                    tmp_pixel_data[i][j] = 0;
                } else tmp_pixel_data[i][j] = 1;
            }

        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            pixel_data[i][j] = tmp_pixel_data[i][j];
        }
    }

    free(tmp_pixel_data);
}

int main(int argc, char *argv[]) {
    sprintf(cur_generation_string, "%d", cur_generation++);
    file_sizes_count(argv[2]);
    printf("height-%d width-%d file_size-%d pixel_start-%d", height, width, file_size, pixel_start);
    char **pixel_data = malloc(sizeof(char *) * height);
    for (int i = 0; i < height; i++) {
        pixel_data[i] = malloc(sizeof(char) * width);
    }
    pixel_read(argv[2], pixel_data);
    output_file(argv[2], pixel_data, argv[4]);
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--max_iter")) {
            max_iter = atoi(argv[i + 1]);
        }
        if (!strcmp(argv[i], "--dump_freq")) {
            frequency_of_generation_creation = atoi(argv[i + 1]);
        }
    }
    printf("\nmax_iter = %d\n", max_iter);
    printf("frequency = %d\n", frequency_of_generation_creation);
    int generation = 0;
    for (int k = 0; k < max_iter; k++) {
        game_life(pixel_data);
        generation++;
        if (((generation) % frequency_of_generation_creation) == 0) {
            output_file(argv[2], pixel_data, argv[4]);
        }
    }
    free(pixel_data);
    return 0;
}


