#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "common.h"
#include "timer.h"

#define CACHE_LINE_SIZE 64

// Struct to pass arguments to the thread function
typedef struct {
    int num_iterations;
    int *shared_array;
    int index;
    bool using_padding;
} thread_args;

// Function to be executed by each thread
void* thread_function(void* arg) {
    thread_args* args = (thread_args*) arg;
    for (int i = 0; i < args->num_iterations; i++) {
        // Increment only the specific element of the shared array
        if (args->using_padding)
            args->shared_array[args->index * (CACHE_LINE_SIZE / sizeof(int))]++;
        else
            args->shared_array[args->index]++;
    }
    free(args);
    pthread_exit(NULL);
}

void write_output_to_files(int num_threads, int num_iterations, double elapsed_time,
                           int *shared_array, int total_sum, int using_padding) {
    FILE* output_file = fopen("results/1_3_table.txt", "a");
    if (output_file == NULL)
        perror_exit("fopen");

    fseek(output_file, 0, SEEK_END);
    long size = ftell(output_file);
    if (size == 0)
        fprintf(output_file, "Threads\tIterations\tTime\tSum\tUsing Padding\n\n");

    fprintf(output_file, "%d\t%d\t%f\t%d\t%d\n", num_threads, num_iterations, elapsed_time, total_sum, using_padding);
    fclose(output_file);
}

void write_output_to_console(int num_threads, int num_iterations, double elapsed_time,
                             int *shared_array, int total_sum, int using_padding) {
    printf("Running with %d threads and %d iterations\n", num_threads, num_iterations);

    printf("Final values of shared array: ");
    for (int i = 0; i < num_threads; ++i){
        if (using_padding)
            printf("%d ", shared_array[i * (CACHE_LINE_SIZE / sizeof(int))]);
        else
            printf("%d ", shared_array[i]);
    }

    printf("\nTotal sum of shared array: %d\n", total_sum);
    printf("Elapsed time: %f seconds\n\n", elapsed_time);
}

int main(int argc, char* argv[]) {
    // Check for correct usage
    if (argc != 4)
        custom_error("Usage: ./1_3 <number of threads> <number of iterations> <using padding>");

    // Number of threads and iterations from command-line arguments
    int num_threads = atoi(argv[1]);
    int num_iterations = atoi(argv[2]);
    // Check for valid arguments
    if (num_threads <= 0 || num_iterations <= 0)
        custom_error("Number of threads and iterations must be positive integers");

    // Check if using padding is 0 or 1
    int using_padding = atoi(argv[3]);
    if (using_padding != 0 && using_padding != 1)
        custom_error("Invalid using padding. Use 0 for not using padding and 1 for using\n");

    // Check if the number of threads is greater than the number of iterations
    if (num_threads > num_iterations)
        num_threads = num_iterations;

    // Create an array of thread identifiers
    pthread_t threads[num_threads];

    // Create and initialize the padded shared array if using padding
    int *shared_array;
    if (using_padding)
        shared_array = (int *)calloc(num_threads * (CACHE_LINE_SIZE / sizeof(int)), sizeof(int));
    else
        shared_array = (int *)calloc(num_threads, sizeof(int));

    if (shared_array == NULL)
        perror_exit("calloc");

    double start, finish;
    // Get the start time
    GET_TIME(start);

    // Calculate iterations per thread
    int base_iterations = num_iterations / num_threads;
    int extra_iterations = num_iterations % num_threads;

    // Create threads
    for (int i = 0; i < num_threads; ++i) {
        // Allocate a unique struct for each thread
        thread_args* args = malloc(sizeof(thread_args));
        if (args == NULL)
            perror_exit("malloc");

        // Each thread gets 'base_iterations', plus an extra iteration if it's one of the first 'extra_iterations' threads
        args->num_iterations = base_iterations + (i < extra_iterations ? 1 : 0);
        args->shared_array = shared_array;
        args->index = i;  // Each thread modifies its own index
        args->using_padding = using_padding ? true : false;

        // Create the thread
        if (pthread_create(&threads[i], NULL, thread_function, args) != 0)
            perror_exit("pthread_create");
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; ++i)
        pthread_join(threads[i], NULL);

    // Get the end time
    GET_TIME(finish);
    double elapsed_time = finish - start;

    // Calculate the final sum of the shared array
    int total_sum = 0;
    for (int i = 0; i < num_threads; ++i){
        if (using_padding)
            total_sum += shared_array[i * (CACHE_LINE_SIZE / sizeof(int))];
        else
            total_sum += shared_array[i];
    }

    // Write the output to a file
    write_output_to_files(num_threads, num_iterations, elapsed_time, shared_array, total_sum, using_padding);

    // Write the output to the console
    write_output_to_console(num_threads, num_iterations, elapsed_time, shared_array, total_sum, using_padding);

    // Free the shared array
    free(shared_array);

    return 0;
}