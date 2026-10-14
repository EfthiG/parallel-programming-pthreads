#ifndef CUSTOM_RW_LOCK_H
#define CUSTOM_RW_LOCK_H

#include <pthread.h>
#include <stdbool.h>

// The custom_rw_lock_t structure
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    int readers_active;
    int readers_waiting;
    bool writer_active;
    int writers_waiting;
} custom_rw_lock_t;

// Common functions
void rw_lock_init(custom_rw_lock_t* lock);
void rw_lock_destroy(custom_rw_lock_t* lock);

// Reader priority functions
void rw_lock_read_readers_prior(custom_rw_lock_t* lock);
void rw_lock_write_readers_prior(custom_rw_lock_t* lock);
void rw_lock_unlock_readers_prior(custom_rw_lock_t* lock);

// Writer priority functions
void rw_lock_read_writers_prior(custom_rw_lock_t* lock);
void rw_lock_write_writers_prior(custom_rw_lock_t* lock);
void rw_lock_unlock_writers_prior(custom_rw_lock_t* lock);


#endif