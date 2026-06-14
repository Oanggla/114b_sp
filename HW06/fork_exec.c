#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    printf("[Parent] My PID is %d. Preparing to fork a child process...\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        // Fork 失敗
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // 子行程 (Child Process)
        printf("[Child] I am the child process. My PID is %d. My Parent's PID is %d.\n", getpid(), getppid());
        printf("[Child] Replacing my program image with 'ls -l' using execvp()...\n");
        fflush(stdout); // 確保在載入新程式映像前清空緩衝區，否則暫存於緩衝區的文字會遺失
        
        char *args[] = {"ls", "-l", NULL};
        execvp(args[0], args);

        // 如果 execvp 成功，控制權就轉移了，以下程式碼絕對不會被執行
        // 如果執行到這，代表 execvp 失敗
        perror("execvp failed");
        exit(1);
    } else {
        // 父行程 (Parent Process)
        printf("[Parent] Created child process with PID %d. Now waiting for it to finish...\n", pid);
        
        int status;
        pid_t child_pid = wait(&status);

        if (child_pid < 0) {
            perror("wait failed");
            exit(1);
        }

        printf("[Parent] Child process %d has finished.\n", child_pid);
        if (WIFEXITED(status)) {
            printf("[Parent] Child exited normally with status %d.\n", WEXITSTATUS(status));
        } else {
            printf("[Parent] Child exited abnormally.\n");
        }
    }

    return 0;
}
