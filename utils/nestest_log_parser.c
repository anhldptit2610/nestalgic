#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *src_fp = fopen(argv[1], "r");
    FILE *dst_fp = fopen("nestest_reduced.txt", "w");
    char c;
    int i = 0, j = 0;

    while ((c = fgetc(src_fp)) != EOF) {
        j++;
        if (c == '\n') {
            j = 0;
        }
        if (i < 7) {
            if (j > 9 && j <= 48)
                continue;
            fwrite(&c, sizeof(char), 1, dst_fp);
        }
    }
    fclose(src_fp);
    fclose(dst_fp);
    return 0;
}