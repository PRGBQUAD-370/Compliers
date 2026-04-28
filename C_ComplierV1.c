#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char name[256];

    printf("Enter C file name (without .c): ");
    scanf("%255s", name);

    char inputFile[300];
    char outputFile[300];
    char command[600];

    sprintf(inputFile, "%s.c", name);
    sprintf(outputFile, "%s.exe", name);

    // Build GCC command
    sprintf(command, "gcc \"%s\" -o \"%s\"", inputFile, outputFile);

    printf("\nCompiling %s -> %s\n", inputFile, outputFile);
    printf("Command: %s\n\n", command);

    int result = system(command);

    if (result == 0) {
        printf("Compilation successful: %s\n", outputFile);
    } else {
        printf("Compilation failed.\n");
    }

    return 0;
}