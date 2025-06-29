
#include <stdio.h>
#include <stdio.h>

int main() {
    FILE *inFile, *outFile;
    char ch, prev;
    int count = 1;

    inFile = fopen("input.txt", "r");
    outFile = fopen("compressed.txt", "w");

    if (inFile == NULL || outFile == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    prev = fgetc(inFile);

    while ((ch = fgetc(inFile)) != EOF) {
        if (ch == prev) {
            count++;
        } else {
            fprintf(outFile, "%c%d", prev, count);
            prev = ch;
            count = 1;
        }
    }

    // Write the last sequence
    fprintf(outFile, "%c%d", prev, count);

    fclose(inFile);
    fclose(outFile);

    printf("Compression complete. Output: compressed.txt\n");
    return 0;
}

