#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "timer.h"

// Struct to pass arguments to the thread function
typedef struct {
    long long int total_throws;
    long long int circle_hits;
    unsigned int seed;
} ThreadData;


// Generate a random double between -1 and 1
double get_random_double(unsigned int* seed) {
    return (double)rand_r(seed) / RAND_MAX * 2.0 - 1.0;
}


// Increment the number of points inside the unit circle
void increment_circle_hits(long long int num_throws, long long int* circle_hits, unsigned int* seed) {
    for (long long int i = 0; i < num_throws; i++) {
        // Generate random (x, y) coordinates
        double x = get_random_double(seed);
        double y = get_random_double(seed);
        
        // Check if the point is inside the unit circle
        if (x * x + y * y <= 1.0)
            (*circle_hits)++;
    }
}


// Estimate π using Monte Carlo simulation in serial
double estimate_pi_serial(long long int num_throws) {
    // Seed the random number generator
    unsigned int seed = (unsigned int) time(NULL);

    // Count the number of points inside the unit circle
    long long int circle_hits = 0;
    increment_circle_hits(num_throws, &circle_hits, &seed);

    // Estimate π using the ratio
    return 4.0 * (double)circle_hits / (double)num_throws;
}


// Estimate π using Monte Carlo simulation in parallel
void* estimate_pi_parallel(void* arg) {
    ThreadData* data = (ThreadData*) arg;

    // Each thread uses a separate random seed
    unsigned int seed = data->seed;

    // Count the number of points inside the unit circle
    long long int local_circle_hits = 0;
    increment_circle_hits(data->total_throws, &local_circle_hits, &seed);

    // Store the result back in the thread data
    data->circle_hits = local_circle_hits;
    pthread_exit(NULL);
}


// Run parallel estimation of π
double run_parallel_estimation(long long int num_throws, int num_threads) {
    pthread_t threads[num_threads];

    // Allocate thread data
    ThreadData* thread_data = (ThreadData*) malloc(num_threads * sizeof(ThreadData));

    // Create threads
    for (int i = 0; i < num_threads; i++) {

        // Assign equal number of throws to each thread
        thread_data[i].total_throws = num_throws / num_threads;
        thread_data[i].circle_hits = 0;

        // Unique seed per thread
        thread_data[i].seed = (unsigned int) time(NULL) + i;

        if (pthread_create(&threads[i], NULL, estimate_pi_parallel, (void*) &thread_data[i]) != 0)
            perror_exit("pthread_create");
    }

    // Wait for all threads to finish
    long long int total_circle_hits = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_circle_hits += thread_data[i].circle_hits;
    }

    // Free allocated memory
    free(thread_data);

    // Calculate π estimate
    return 4.0 * (double)total_circle_hits / (double)num_throws;
}


// Write the output to a file
void write_output_to_files(int num_threads, long long int num_throws, double serial_time, double pi_serial, double parallel_time, double pi_parallel) {
    // Open a file to store the results in a tabular format
    FILE* file = fopen("results/1_1_table.txt", "a");
    if (file == NULL)
        perror_exit("fopen");

    // Check if the file is empty
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size == 0)
        fprintf(file, "Threads\tThrows\tSerial Time\tSerial π\tParallel Time\tParallel π\n\n");

    fprintf(file, "%d\t%lld\t%f\t%f\t%f\t%f\n", num_threads, num_throws, serial_time, pi_serial, parallel_time, pi_parallel);
    fclose(file);
}

void write_output_to_console(int num_threads, long long int num_throws, double serial_time, double pi_serial, double parallel_time, double pi_parallel) {
    printf("Running with %d threads and %lld throws: \n", num_threads, num_throws);
    printf("Serial execution time: %f seconds\n", serial_time);
    printf("Serial estimation of π: %f\n", pi_serial);
    printf("Parallel execution time: %f seconds\n", parallel_time);
    printf("Parallel estimation of π: %f\n\n", pi_parallel);
}


int main(int argc, char* argv[]) {
    if (argc != 3)
        custom_error("Usage: ./1_1 <num_threads> <num_throws>");

    int num_threads = atoi(argv[1]);
    long long int num_throws = atoll(argv[2]);

    if (num_throws <= 0 || num_threads <= 0)
        custom_error("Number of threads and throws must be positive integers\n");

    double start, finish;

    // Serial estimation
    GET_TIME(start);
    double pi_serial = estimate_pi_serial(num_throws);
    GET_TIME(finish);
    double serial_time = finish - start;

    // Parallel estimation
    GET_TIME(start);
    double pi_parallel = run_parallel_estimation(num_throws, num_threads);
    GET_TIME(finish);
    double parallel_time = finish - start;

    // Write the output to a file and print to stdout
    write_output_to_files(num_threads, num_throws, serial_time, pi_serial, parallel_time, pi_parallel);
    write_output_to_console(num_threads, num_throws, serial_time, pi_serial, parallel_time, pi_parallel);

    return 0;
}