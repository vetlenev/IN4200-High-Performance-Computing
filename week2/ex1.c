#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h> 

char* read_file(char *filename, int nr_reads, bool binary) {
    FILE *fr;

    /*if (binary) {
        fr = fopen(filename, "rb");
    }
    else {
        fr = fopen(filename, "r");
    }*/
    fr = fopen(filename, "r");

    clock_t clock_start, clock_stop;

    if (fr == NULL) {
        printf("Failure to open file %s\n", filename);
        exit(0);
    }

    char* storage = (char*)malloc(nr_reads*sizeof(char));

    int i=0;
    clock_start = clock();

    while (!feof(fr) && i < nr_reads) {
        fread(&storage[i], sizeof(storage), 10, fr);
        i++;
    }
    printf("i: %d\n", i);

    clock_stop = clock() - clock_start;
    printf("Clock finish: %d", clock_stop);

    fclose(fr);
    return storage;
}

int main(int narg, char **argv) {

    if (narg < 0) {
        printf("Filename and bool required.\n");
        exit(0);
    }

    char *finish;
    //finish = read_file(argv[1], 10000, argv[2]);
    finish = read_file(argv[1], 50000, true);
    return 0;
}