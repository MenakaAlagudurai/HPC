#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define MAX 10
#define NUM_CONSUMERS 3

// Task structure
typedef struct {
    int a, b;
    char op;
} Task;

// Shared buffer
Task buffer[MAX];
int in = 0, out = 0;

// Semaphores
sem_t empty, full, mutex;

// File pointer
FILE *file_ptr;

// Function to perform calculation
int calculate(int a, int b, char op) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return (b != 0) ? a / b : 0;
        default: return 0;
    }
}

// Producer thread
void *producer(void *arg) {
    char ops[] = {'+', '-', '*', '/'};

    for (int i = 0; i < 10; i++) {
        Task t;
        t.a = rand() % 100;
        t.b = rand() % 50 + 1;
        t.op = ops[rand() % 4];

        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = t;
        printf("Produced: %d %c %d\n", t.a, t.op, t.b);
        fprintf(file_ptr, "Produced: %d %c %d\n", t.a, t.op, t.b);
        fflush(file_ptr);

        in = (in + 1) % MAX;

        sem_post(&mutex);
        sem_post(&full);

        sleep(1);
    }

    pthread_exit(NULL);
}

// Consumer thread
void *consumer(void *arg) {
    int thread_id = *(int *)arg;

    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);

        Task t = buffer[out];
        out = (out + 1) % MAX;

        sem_post(&mutex);
        sem_post(&empty);

        int result = calculate(t.a, t.b, t.op);

        printf("Consumer %d: %d %c %d = %d\n",
               thread_id, t.a, t.op, t.b, result);

        fprintf(file_ptr, "Consumer %d: %d %c %d = %d\n",
                thread_id, t.a, t.op, t.b, result);

        fflush(file_ptr);

        sleep(1);
    }

    pthread_exit(NULL);
}

// Main function
int main() {
    pthread_t prod;
    pthread_t cons[NUM_CONSUMERS];
    int ids[NUM_CONSUMERS];

    // Open file
    file_ptr = fopen("output.txt", "w");
    if (file_ptr == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    srand(time(NULL));

    // Initialize semaphores
    sem_init(&empty, 0, MAX);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    // Create producer
    pthread_create(&prod, NULL, producer, NULL);

    // Create consumers
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        ids[i] = i;
        pthread_create(&cons[i], NULL, consumer, &ids[i]);
    }

    // Join producer
    pthread_join(prod, NULL);

    // Let consumers run for some time then exit (demo purpose)
    sleep(5);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    fclose(file_ptr);

    return 0;
}

