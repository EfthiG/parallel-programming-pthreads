#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#include "common.h"
#include "timer.h"

// Structs to pass arguments to the thread function
typedef struct {
    int num_iterations;
    int *shared_variable;
} thread_atomic_args;

typedef struct {
    int num_iterations;
    int *shared_variable;
    pthread_mutex_t *mutex;
} thread_mutex_args;


// Function to be executed by each thread using atomics
void* thread_function_atomic(void* arg) {
    thread_atomic_args* args = (thread_atomic_args*) arg;

    for (int i = 0; i < args->num_iterations; i++)
        // Atomic increment of the shared variable
        atomic_fetch_add(args->shared_variable, 1);

    free(args);
    pthread_exit(NULL);
}

// Function to be executed by each thread using mutexes
void* thread_function_mutex(void* arg) {
    thread_mutex_args* args = (thread_mutex_args*) arg;
    for (int i = 0; i < args->num_iterations; i++) {
        // Lock the mutex before changing the shared variable
        pthread_mutex_lock(args->mutex);
        (*args->shared_variable)++;
        // Unlock the mutex after changing
        pthread_mutex_unlock(args->mutex);
    }
    free(args);
    pthread_exit(NULL);
}

void write_output_to_files(int num_threads, int num_iterations, double elapsed_time, int shared_variable, int sync_method) {
    // Open a file to store the results in a tabular format
    FILE* output_file = fopen("results/1_2_table.txt", "a");
    if (output_file == NULL)
        perror_exit("fopen");

    // Check if the file is empty
    fseek(output_file, 0, SEEK_END);
    long size = ftell(output_file);
    if (size == 0)
        fprintf(output_file, "Threads\tIterations\tTime\tShared Variable\tSync Method\n\n");

    // Write the results to the file (difference is in the sync method used)
    if (sync_method == 0)
        fprintf(output_file, "%d\t%d\t%f\t%d\tAtomics\n", num_threads, num_iterations, elapsed_time, shared_variable);
    else
        fprintf(output_file, "%d\t%d\t%f\t%d\tMutexes\n", num_threads, num_iterations, elapsed_time, shared_variable);

    fclose(output_file);
}


void write_output_to_console(int num_threads, int num_iterations, double elapsed_time, int shared_variable, int sync_method) {
    printf("Running with %d threads and %d iterations and sync method %d\n", num_threads, num_iterations, sync_method);
    printf("Final value of shared variable: %d\n", shared_variable);
    printf("Elapsed time: %f seconds\n\n", elapsed_time);
}


int main(int argc, char* argv[]) {
    // Check for correct usage
    if (argc != 4)
        custom_error("Usage: ./1_2 <number of threads> <number of iterations> <sync method>\n");

    // Number of threads from command-line argument
    int num_threads = atoi(argv[1]);
    int num_iterations = atoi(argv[2]);
    int sync_method = atoi(argv[3]);

    // Check for valid arguments
    if (sync_method != 0 && sync_method != 1)
        custom_error("Invalid sync method. Use 0 for atomics and 1 for mutexes\n");

    if (num_threads <= 0 || num_iterations <= 0)
        custom_error("Number of threads and iterations must be positive integers\n");

    // Create an array of thread identifiers
    pthread_t threads[num_threads];

    // Initialize shared variable
    int shared_variable = 0;
    pthread_mutex_t mutex;

    double start, finish;

    // Get the start time
    GET_TIME(start);

    // Initialize the mutex if used
    if (sync_method == 1)
        pthread_mutex_init(&mutex, NULL);

    // Create threads
    // Use different arguments for the thread function based on the sync method
    for (int i = 0; i < num_threads; ++i) {

        // Using atomics
        if (sync_method == 0) {
            // Pass the shared variable to the thread function
            thread_atomic_args* args = malloc(sizeof(thread_atomic_args));
            if (args == NULL)
                perror_exit("malloc");

            args->num_iterations = num_iterations;
            args->shared_variable = &shared_variable;
            pthread_create(&threads[i], NULL, thread_function_atomic, args);
        }

        // Using mutexes
        else {
            // Pass the shared variable and the mutex to the thread function
            thread_mutex_args* args = malloc(sizeof(thread_mutex_args));
            if (args == NULL)
                perror_exit("malloc");

            args->num_iterations = num_iterations;
            args->shared_variable = &shared_variable;
            args->mutex = &mutex;
            pthread_create(&threads[i], NULL, thread_function_mutex, args);
        }
    }


    // Wait for all threads to finish
    for (int i = 0; i < num_threads; ++i)
        pthread_join(threads[i], NULL);

    // Destroy the mutex if used
    if (sync_method == 1)
        pthread_mutex_destroy(&mutex);

    // Calculate the elapsed time
    GET_TIME(finish);
    double elapsed_time = finish - start;

    // // Write the output to a file
    write_output_to_files(num_threads, num_iterations, elapsed_time, shared_variable, sync_method);

    // Write the output to the console
    write_output_to_console(num_threads, num_iterations, elapsed_time, shared_variable, sync_method);

    return 0;
}