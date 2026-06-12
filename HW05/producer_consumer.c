#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 20

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t empty;
sem_t full;
sem_t mutex;

int items_produced = 0;
int items_consumed = 0;

void *producer(void *arg) {
    int id = *((int *)arg);
    while (1) {
        // 檢查是否已達到生產上限
        sem_wait(&mutex);
        if (items_produced >= TOTAL_ITEMS) {
            sem_post(&mutex);
            break;
        }
        items_produced++;
        int item = items_produced;
        sem_post(&mutex);

        // 等待空位
        sem_wait(&empty);
        sem_wait(&mutex);

        // 放入緩衝區
        buffer[in] = item;
        printf("生產者 %d: 在槽位 %d 放入產品 %d\n", id, in, item);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full);

        // 模擬生產耗時
        usleep(100000); // 0.1秒
    }
    printf("生產者 %d 結束執行\n", id);
    return NULL;
}

void *consumer(void *arg) {
    int id = *((int *)arg);
    while (1) {
        // 檢查是否已達到消費上限
        sem_wait(&mutex);
        if (items_consumed >= TOTAL_ITEMS) {
            sem_post(&mutex);
            break;
        }
        sem_post(&mutex);

        // 等待產品
        sem_wait(&full);
        sem_wait(&mutex);

        // 檢查二次確認，防止最後退出時的超額消費
        if (items_consumed >= TOTAL_ITEMS) {
            sem_post(&mutex);
            sem_post(&full); // 把信號量還回去，避免其他線程卡死
            break;
        }

        // 取出產品
        int item = buffer[out];
        items_consumed++;
        printf("消費者 %d: 從槽位 %d 取出產品 %d (累計消費: %d/%d)\n", 
               id, out, item, items_consumed, TOTAL_ITEMS);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&empty);

        // 模擬消費耗時
        usleep(150000); // 0.15秒
    }
    printf("消費者 %d 結束執行\n", id);
    return NULL;
}

int main() {
    pthread_t prod_threads[2];
    pthread_t cons_threads[2];
    int prod_ids[2] = {1, 2};
    int cons_ids[2] = {1, 2};

    // 初始化訊號量
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    printf("開始生產者消費者模擬...\n");

    // 建立生產者與消費者執行緒
    pthread_create(&prod_threads[0], NULL, producer, &prod_ids[0]);
    pthread_create(&prod_threads[1], NULL, producer, &prod_ids[1]);
    pthread_create(&cons_threads[0], NULL, consumer, &cons_ids[0]);
    pthread_create(&cons_threads[1], NULL, consumer, &cons_ids[1]);

    // 等待所有執行緒結束
    for (int i = 0; i < 2; i++) {
        pthread_join(prod_threads[i], NULL);
        pthread_join(cons_threads[i], NULL);
    }

    // 銷毀訊號量
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    printf("生產者消費者模擬結束，所有執行緒安全退出！\n");
    return 0;
}
