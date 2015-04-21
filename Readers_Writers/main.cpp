//
//  main.cpp
//  Readers_Writers
//
//  Created by Serhii Onopriienko on 4/9/15.
//  Copyright (c) 2015 com.purchase.test1. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>





#define READERS_NUMBER 10
#define WRITERS_NUMBER 3


pthread_t readers[READERS_NUMBER];
pthread_t writers[WRITERS_NUMBER];


pthread_mutex_t condition_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_var       = PTHREAD_COND_INITIALIZER;


pthread_mutex_t writers_mutex       = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readers_count_mutex = PTHREAD_MUTEX_INITIALIZER;


bool isWritingNow = false;
int readingNowCount = 0;








void beginReading(long threadId) {
    
    printf("reader> %ld is reading\n", threadId);
    
    pthread_mutex_lock( &readers_count_mutex );
    readingNowCount++;
    pthread_mutex_unlock( &readers_count_mutex );
}




void finishReading(long threadId) {
    
    printf("reader< %ld has finished reading\n", threadId);
    
    pthread_mutex_lock( &readers_count_mutex );
    readingNowCount--;
    pthread_mutex_unlock( &readers_count_mutex );
}




void *read(void *args) {
    
    for(;;) {
        
        // Lock mutex and then wait for signal to relase mutex
        pthread_mutex_lock( &condition_mutex );
        if (isWritingNow) {
            
            // Wait while write() operates on isWritingNow var
            // mutex unlocked if condition varialbe in write() signaled.
            pthread_cond_wait( &condition_var, &condition_mutex );
            
        }
        pthread_mutex_unlock( &condition_mutex );
        
        
        beginReading((long)args);
        usleep(1000);
        finishReading((long)args);
    }
}






void *write(void *args) {
    
    for(;;) {
        
        pthread_mutex_lock( &writers_mutex );
        
        pthread_mutex_lock( &condition_mutex );
        isWritingNow = true;
        printf("----writer %ld trying to write\n", (long)args);
        
        
        while (readingNowCount != 0) {
           //  printf("-----------readers count - %d\n", readingNowCount);
        }
        
        
        printf("----writer> %ld is writing\n", (long)args);
        usleep(1500);
        printf("----writer< %ld has finished writing\n", (long)args);
        // Condition of if statement has been met.
        // Signal to free waiting thread by freeing the mutex.
        // Note: functionCount1() is now permitted to modify "count".
        
        pthread_cond_broadcast( &condition_var );
        pthread_mutex_unlock( &condition_mutex );
        
        pthread_mutex_unlock( &writers_mutex );
    }
    
    return(NULL);
}








int main(int argc, const char * argv[]) {

    
    for (int i = 0 ; i < READERS_NUMBER; i++) {
        pthread_create(&readers[i], NULL, read, (void*)(long)i);
    }
    
    
    for (int i = 0 ; i < WRITERS_NUMBER; i++) {
        pthread_create(&writers[i], NULL, write, (void*)(long)i);
    }
    
    
    for (int i = 0 ; i < READERS_NUMBER; i++) {
        pthread_join(readers[i], NULL);
    }
    
    
    for (int i = 0 ; i < WRITERS_NUMBER; i++) {
        pthread_join(writers[i], NULL);
    }
    
    
    exit(EXIT_SUCCESS);
}
