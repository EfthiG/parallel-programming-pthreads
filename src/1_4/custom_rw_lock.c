#include <pthread.h>

#include "custom_rw_lock.h"

// Common functions
void rw_lock_init(custom_rw_lock_t* lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_cond, NULL);
    pthread_cond_init(&lock->writers_cond, NULL);
    lock->readers_active = 0;
    lock->readers_waiting = 0;
    lock->writer_active = false;
    lock->writers_waiting = 0;
}

void rw_lock_destroy(custom_rw_lock_t* lock) {
    pthread_mutex_destroy(&lock->mutex);
    pthread_cond_destroy(&lock->readers_cond);
    pthread_cond_destroy(&lock->writers_cond);
}


// Reader priority functions
void rw_lock_read_readers_prior(custom_rw_lock_t* lock){
    pthread_mutex_lock(&lock->mutex);
    lock->readers_waiting++;
    
    // Wait until there is no writer active
    while(lock->writer_active)
        pthread_cond_wait(&lock->readers_cond, &lock->mutex);

    lock->readers_waiting--;
    lock->readers_active++;
    pthread_mutex_unlock(&lock->mutex);
}


void rw_lock_write_readers_prior(custom_rw_lock_t* lock){
    pthread_mutex_lock(&lock->mutex);
    lock->writers_waiting++;

    // Wait until there is no writer active and
    // there are no readers active or waiting
    while (lock->writer_active || lock->readers_active>0 || lock->readers_waiting>0 )
        pthread_cond_wait(&lock->writers_cond, &lock->mutex);

    lock->writers_waiting--;
    lock->writer_active = true;
    pthread_mutex_unlock(&lock->mutex);
}


void rw_lock_unlock_readers_prior(custom_rw_lock_t* lock){
    pthread_mutex_lock(&lock->mutex);

    // If the lock is held by a writer
    if( lock->writer_active ){
        lock->writer_active = false;
        if( lock->readers_waiting > 0 )
            pthread_cond_broadcast(&lock->readers_cond);
        else if( lock->writers_waiting > 0 )
            pthread_cond_signal(&lock->writers_cond);
    }
    else{
        lock->readers_active--;
        if(lock->readers_active == 0 && lock->writers_waiting > 0)
            pthread_cond_signal(&lock->writers_cond);
    }

    pthread_mutex_unlock(&lock->mutex);
}

// Writer priority functions
void rw_lock_read_writers_prior(custom_rw_lock_t* lock){
    pthread_mutex_lock(&lock->mutex);
    lock->readers_waiting++;

    // Wait until there are no writers active or waiting
    while(lock->writer_active || lock->writers_waiting>0)
        pthread_cond_wait(&lock->readers_cond, &lock->mutex);

    lock->readers_waiting--;
    lock->readers_active++;
    pthread_mutex_unlock(&lock->mutex);

}


void rw_lock_write_writers_prior(custom_rw_lock_t* lock){
    pthread_mutex_lock(&lock->mutex);
    lock->writers_waiting++;

    while(lock->writer_active || lock->readers_active>0)
        pthread_cond_wait(&lock->writers_cond, &lock->mutex);

    lock->writers_waiting--;
    lock->writer_active = true;
    pthread_mutex_unlock(&lock->mutex);
}


void rw_lock_unlock_writers_prior(custom_rw_lock_t* lock){
    pthread_mutex_lock(&lock->mutex);

    if( lock->writer_active ){
        lock->writer_active = false;
        if( lock->writers_waiting > 0 )
            pthread_cond_signal(&lock->writers_cond);
        else if( lock->readers_waiting > 0 )
                pthread_cond_broadcast(&lock->readers_cond);
    }
    else{
        lock->readers_active--;
        if( lock->writers_waiting > 0 && lock->readers_active == 0)
            pthread_cond_signal(&lock->writers_cond);
    }

    pthread_mutex_unlock(&lock->mutex);
}