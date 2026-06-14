#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    printf("[Redirect Demo] Standard descriptors: stdin=%d, stdout=%d, stderr=%d\n\n", STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);

    // ==========================================
    // 示範 1: 使用 close(1) + open() 進行重導向 (最小可用描述子機制)
    // ==========================================
    printf("--- Demo 1: close(1) then open() ---\n");
    
    // 先複製一份原本的標準輸出，方便之後還原螢幕輸出
    int saved_stdout = dup(STDOUT_FILENO);
    if (saved_stdout < 0) {
        perror("dup failed");
        exit(1);
    }

    printf("Closing stdout (fd 1) and opening 'redirect_stdout.txt'...\n");
    close(STDOUT_FILENO); // 關閉標準輸出 (1)

    // 此時開啟檔案，系統會指派目前未使用的最小 descriptor，即是 1。
    int fd_out = open("redirect_stdout.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        // 因為此時 stdout 關閉了，我們必須將訊息印到 stderr (2)
        perror("open failed");
        exit(1);
    }

    // 呼叫 printf() 原本會寫入 fd 1 (stdout)
    // 因為 fd 1 被 redirect_stdout.txt 佔用了，以下內容將被寫入檔案！
    printf("This sentence is written into 'redirect_stdout.txt' (fd %d)!\n", fd_out);
    printf("Rule: open() always uses the smallest unused file descriptor.\n");
    fflush(stdout); // 確保緩衝區內容被寫入檔案

    // 還原標準輸出到螢幕
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    printf("Successfully restored stdout! This is printed back on the terminal.\n\n");

    // ==========================================
    // 示範 2: 使用 fork() + dup2() 重導向子行程的 execvp 輸出
    // ==========================================
    printf("--- Demo 2: Redirect child process output using dup2() ---\n");

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // 子行程：將 execvp 的輸出重導向到 redirect_exec.txt
        int fd_file = open("redirect_exec.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_file < 0) {
            perror("child open failed");
            exit(1);
        }

        // 使用 dup2 將 fd_file 複製到 fd 1 (stdout)
        // 如此一來，任何對 stdout 的寫入都會自動被引流到該檔案
        if (dup2(fd_file, STDOUT_FILENO) < 0) {
            perror("dup2 failed");
            exit(1);
        }
        
        // 關閉原本開啟的檔案描述子，因為我們已經有了複製版本 fd 1
        close(fd_file);

        // 執行外部命令。該命令的任何標準輸出（stdout）都會直接寫入 redirect_exec.txt
        char *args[] = {"echo", "Hello! This output was redirected by dup2() inside the child process.", NULL};
        execvp(args[0], args);

        perror("execvp failed");
        exit(1);
    } else {
        // 父行程：等待子行程完成
        int status;
        wait(&status);
        printf("[Parent] Child process has finished redirecting its output to 'redirect_exec.txt'.\n");

        // 讀取該檔案內容並顯示在螢幕上，以驗證重導向成功
        printf("[Parent] Verifying 'redirect_exec.txt' content:\n");
        int fd_read = open("redirect_exec.txt", O_RDONLY);
        if (fd_read >= 0) {
            char buf[256];
            memset(buf, 0, sizeof(buf));
            read(fd_read, buf, sizeof(buf) - 1);
            printf("----------------------------------------\n");
            printf("%s", buf);
            printf("----------------------------------------\n");
            close(fd_read);
        } else {
            perror("parent open log failed");
        }
    }

    return 0;
}
