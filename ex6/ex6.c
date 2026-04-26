#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX 5
#define NUM_THREADS 3
#define WORD_LEN 50

char input_buffer[MAX][WORD_LEN];
char result_buffer[100][WORD_LEN + 20];

int in = 0, out = 0, count = 0, result_count = 0;

char *dict[]={"apple","banana","cat","dog","elephant","grape"};
int dict_size=6;

pthread_mutex_t mutex;
pthread_cond_t not_full, not_empty;

/* Dummy spell check function */
int spell_check(char *word) {
	for(int i=0;i<dict_size;i++){
		if(strcmp(word,dict[i])==0)
			return 1;
	}
	return 0;
}

/* Producer thread */
void *producer(void *arg) {
    char *words[] = {"apple", "bat", "cat", "dog", "egg", "grape"};

    for (int i = 0; i < 6; i++) {
        pthread_mutex_lock(&mutex);

        while (count == MAX)
            pthread_cond_wait(&not_full, &mutex);

        strcpy(input_buffer[in], words[i]);
        printf("Produced: %s\n", words[i]);

        in = (in + 1) % MAX;
        count++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }

    /* Poison pill */
    pthread_mutex_lock(&mutex);
    while (count == MAX)
        pthread_cond_wait(&not_full, &mutex);

    strcpy(input_buffer[in], "STOP");
    in = (in + 1) % MAX;
    count++;

    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

/* Consumer thread */
void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == 0)
            pthread_cond_wait(&not_empty, &mutex);

        char word[WORD_LEN];
        strcpy(word, input_buffer[out]);

        /* Check for STOP */
        if (strcmp(word, "STOP") == 0) {
            pthread_cond_broadcast(&not_empty);
            pthread_mutex_unlock(&mutex);
            break;
        }

        out = (out + 1) % MAX;
        count--;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        int result = spell_check(word);

        pthread_mutex_lock(&mutex);
        if (result)
            sprintf(result_buffer[result_count++], "%s - Correct", word);
        else
            sprintf(result_buffer[result_count++], "%s - Incorrect", word);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t prod, cons[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&prod, NULL, producer, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);

    pthread_join(prod, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(cons[i], NULL);

    printf("\nFinal Results:\n");
    for (int i = 0; i < result_count; i++) {
        printf("%s\n", result_buffer[i]);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
