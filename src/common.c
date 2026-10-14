#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void custom_error(char* message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void perror_exit(char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}