#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 4096 // Maximum dictionary size (2^12)
#define MAX_CODE 256       // Initial ASCII codes

// Structure for dictionary entries
typedef struct {
    char *str; // Substring
    int code;  // Code assigned to substring
} DictEntry;

// Initialize dictionary with ASCII characters
void initDictionary(DictEntry *dict, int *dictSize) {
    *dictSize = 0;
    for (int i = 0; i < MAX_CODE; i++) {
        dict[*dictSize].str = malloc(2);
        dict[*dictSize].str[0] = (char)i;
        dict[*dictSize].str[1] = '\0';
        dict[*dictSize].code = i;
        (*dictSize)++;
    }
}

// Find code for a string in dictionary
int findCode(DictEntry *dict, int dictSize, char *str) {
    for (int i = 0; i < dictSize; i++) {
        if (strcmp(dict[i].str, str) == 0) {
            return dict[i].code;
        }
    }
    return -1;
}

// Compress text file using LZW
void compressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "r");
    FILE *out = fopen(outputFile, "wb"); // Binary mode for codes
    if (in == NULL || out == NULL) {
        printf("Error opening files!\n");
        return;
    }

    DictEntry dict[MAX_DICT_SIZE];
    int dictSize = 0;
    initDictionary(dict, &dictSize);

    char current[1000] = "", temp[1000];
    int currentCode;
    char ch;

    // Read input file character by character
    while ((ch = fgetc(in)) != EOF) {
        strcpy(temp, current);
        strncat(temp, &ch, 1);

        // Check if temp exists in dictionary
        if (findCode(dict, dictSize, temp) != -1) {
            strcpy(current, temp);
        } else {
            // Output code for current
            currentCode = findCode(dict, dictSize, current);
            fwrite(&currentCode, sizeof(int), 1, out);

            // Add temp to dictionary if there's space
            if (dictSize < MAX_DICT_SIZE) {
                dict[dictSize].str = strdup(temp);
                dict[dictSize].code = dictSize;
                dictSize++;
            }
            strcpy(current, &ch);
        }
    }

    // Output code for remaining current
    if (strlen(current) > 0) {
        currentCode = findCode(dict, dictSize, current);
        fwrite(&currentCode, sizeof(int), 1, out);
    }

    // Free dictionary memory
    for (int i = 0; i < dictSize; i++) {
        free(dict[i].str);
    }
    fclose(in);
    fclose(out);
    printf("File compressed successfully to %s\n", outputFile);
}

// Decompress LZW-compressed file
void decompressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb"); // Binary mode for codes
    FILE *out = fopen(outputFile, "w");
    if (in == NULL || out == NULL) {
        printf("Error opening files!\n");
        return;
    }

    DictEntry dict[MAX_DICT_SIZE];
    int dictSize = 0;
    initDictionary(dict, &dictSize);

    int code, prevCode;
    char *current, *temp;

    // Read first code
    if (fread(&prevCode, sizeof(int), 1, in) != 1) {
        fclose(in);
        fclose(out);
        return;
    }
    fprintf(out, "%s", dict[prevCode].str);

    // Read remaining codes
    while (fread(&code, sizeof(int), 1, in) == 1) {
        if (code < dictSize) {
            current = dict[code].str;
        } else {
            // Handle special case: code not yet in dictionary
            temp = malloc(strlen(dict[prevCode].str) + 2);
            strcpy(temp, dict[prevCode].str);
            strncat(temp, &dict[prevCode].str[0], 1);
            current = temp;
        }

        // Output current string
        fprintf(out, "%s", current);

        // Add new entry to dictionary
        if (dictSize < MAX_DICT_SIZE) {
            temp = malloc(strlen(dict[prevCode].str) + 2);
            strcpy(temp, dict[prevCode].str);
            strncat(temp, &current[0], 1);
            dict[dictSize].str = temp;
            dict[dictSize].code = dictSize;
            dictSize++;
        }

        prevCode = code;
    }

    // Free dictionary memory
    for (int i = 0; i < dictSize; i++) {
        free(dict[i].str);
    }
    fclose(in);
    fclose(out);
    printf("File decompressed successfully to %s\n", outputFile);
}

// Main function
int main() {
    char inputFile[100], outputFile[100];
    int choice;

    printf("Text File Compression/Decompression Tool (LZW)\n");
    printf("1. Compress a file\n");
    printf("2. Decompress a file\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);
    getchar(); // Clear newline

    if (choice == 1) {
        printf("Enter input file name (e.g., input.txt): ");
        fgets(inputFile, sizeof(inputFile), stdin);
        inputFile[strcspn(inputFile, "\n")] = 0;

        printf("Enter output file name (e.g., compressed.lzw): ");
        fgets(outputFile, sizeof(outputFile), stdin);
        outputFile[strcspn(outputFile, "\n")] = 0;

        compressFile(inputFile, outputFile);
    } else if (choice == 2) {
        printf("Enter input file name (e.g., compressed.lzw): ");
        fgets(inputFile, sizeof(inputFile), stdin);
        inputFile[strcspn(inputFile, "\n")] = 0;

        printf("Enter output file name (e.g., decompressed.txt): ");
        fgets(outputFile, sizeof(outputFile), stdin);
        outputFile[strcspn(outputFile, "\n")] = 0;

        decompressFile(inputFile, outputFile);
    } else {
        printf("Invalid choice!\n");
    }

    return 0;
}
