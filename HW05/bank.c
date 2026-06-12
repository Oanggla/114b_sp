#include <stdio.h>
#include <pthread.h>

#define LOOPS 100000
int balance = 1000; // 初始餘額為 1000 元
pthread_mutex_t lock;

void *deposit(void *arg) {
    for (int i = 0; i < LOOPS; i++) {
        pthread_mutex_lock(&lock);
        balance = balance + 1; // 存款 1 元
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *withdraw(void *arg) {
    for (int i = 0; i < LOOPS; i++) {
        pthread_mutex_lock(&lock);
        balance = balance - 1; // 提款 1 元
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // 初始化互斥鎖
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    printf("開始銀行存提款模擬...\n");
    printf("初始存款金額: %d 元\n", balance);
    printf("同一個人，存款執行緒存入 1 元共 %d 次\n", LOOPS);
    printf("同一個人，提款執行緒提出 1 元共 %d 次\n", LOOPS);

    // 建立存提款執行緒
    pthread_create(&thread1, NULL, deposit, NULL);
    pthread_create(&thread2, NULL, withdraw, NULL);

    // 等待執行緒結束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // 銷毀互斥鎖
    pthread_mutex_destroy(&lock);

    printf("存提款完成！\n");
    printf("最後存款金額: %d 元 (預期值: 1000 元)\n", balance);

    return 0;
}
