#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 1024

int main() {
    const char *filename = "test_io.txt";
    const char *text = "System Programming Homework 6: file_io.c output.\n";
    char buffer[BUF_SIZE];
    ssize_t bytes_written, bytes_read;

    // 1. 使用 open() 以「唯寫、若不存在則建立、若存在則清空」模式開啟檔案，權限設為 0644 (Owner: rw, Group: r, Others: r)
    printf("[File I/O] Opening file '%s' for writing...\n", filename);
    int fd_write = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_write < 0) {
        perror("open for writing failed");
        exit(1);
    }
    printf("[File I/O] File opened successfully. File Descriptor (fd) is: %d\n", fd_write);

    // 2. 使用 write() 寫入資料
    bytes_written = write(fd_write, text, strlen(text));
    if (bytes_written < 0) {
        perror("write failed");
        close(fd_write);
        exit(1);
    }
    printf("[File I/O] Successfully wrote %ld bytes to '%s'.\n", bytes_written, filename);

    // 3. 使用 close() 關閉寫入的檔案描述子
    if (close(fd_write) < 0) {
        perror("close fd_write failed");
        exit(1);
    }
    printf("[File I/O] Closed write file descriptor.\n\n");

    // 4. 使用 open() 以「唯讀」模式再次開啟該檔案
    printf("[File I/O] Opening file '%s' for reading...\n", filename);
    int fd_read = open(filename, O_RDONLY);
    if (fd_read < 0) {
        perror("open for reading failed");
        exit(1);
    }
    printf("[File I/O] File opened successfully. File Descriptor (fd) is: %d\n", fd_read);

    // 5. 使用 read() 讀取資料
    memset(buffer, 0, sizeof(buffer));
    bytes_read = read(fd_read, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read failed");
        close(fd_read);
        exit(1);
    }
    printf("[File I/O] Successfully read %ld bytes. Content:\n", bytes_read);
    printf("----------------------------------------\n");
    printf("%s", buffer);
    printf("----------------------------------------\n");

    // 6. 關閉讀取的檔案描述子
    if (close(fd_read) < 0) {
        perror("close fd_read failed");
        exit(1);
    }
    printf("[File I/O] Closed read file descriptor. Program finished.\n");

    return 0;
}
