#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define EAT_LIMIT 3

pthread_mutex_t forks[NUM_PHILOSOPHERS];

void *philosopher(void *arg) {
    int id = *((int *)arg);
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;

    for (int meal = 1; meal <= EAT_LIMIT; meal++) {
        // 1. 思考中
        printf("哲學家 %d 正在思考... (第 %d 次思考)\n", id, meal);
        usleep(100000); // 模擬思考時間

        // 2. 飢餓，準備拿筷子
        printf("哲學家 %d 餓了，準備拿起筷子\n", id);

        // 死結預防：不對稱筷子拿取策略
        if (id % 2 == 0) {
            // 偶數號哲學家先拿左筷子，再拿右筷子
            pthread_mutex_lock(&forks[left_fork]);
            printf("哲學家 %d 拿起了左邊筷子 %d\n", id, left_fork);
            pthread_mutex_lock(&forks[right_fork]);
            printf("哲學家 %d 拿起了右邊筷子 %d，開始用餐\n", id, right_fork);
        } else {
            // 奇數號哲學家先拿右筷子，再拿左筷子
            pthread_mutex_lock(&forks[right_fork]);
            printf("哲學家 %d 拿起了右邊筷子 %d\n", id, right_fork);
            pthread_mutex_lock(&forks[left_fork]);
            printf("哲學家 %d 拿起了左邊筷子 %d，開始用餐\n", id, left_fork);
        }

        // 3. 用餐中
        printf("哲學家 %d 正在用餐 (第 %d/%d 次用餐)\n", id, meal, EAT_LIMIT);
        usleep(150000); // 模擬用餐時間

        // 4. 用餐完畢，放回筷子
        pthread_mutex_unlock(&forks[left_fork]);
        pthread_mutex_unlock(&forks[right_fork]);
        printf("哲學家 %d 放下了筷子 %d 與 %d，完成此餐\n", id, left_fork, right_fork);
    }

    printf("哲學家 %d 已吃飽，離開餐桌\n", id);
    return NULL;
}

int main() {
    pthread_t threads[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // 初始化互斥鎖 (筷子)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    printf("開始哲學家用餐模擬 (無死結設計)...\n");

    // 建立哲學家執行緒
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    // 等待所有哲學家吃飽
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 銷毀互斥鎖
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    printf("所有哲學家皆已用餐完畢，模擬順利結束！\n");
    return 0;
}
