# 習題 6 - 行程與檔案相關系統呼叫觀念釐清

*使用 Antigravity + Gemini 3.5 Flash 協助開發與撰寫*

本作業旨在釐清 Linux/UNIX 系統程式設計中的核心系統呼叫，包含：**行程生命週期** (`fork`, `execvp`)、**檔案系統操作** (`open`, `close`, `read`, `write`) 以及**檔案描述子與重導向** (`dup2`, `stdin 0`, `stdout 1`, `stderr 2`)。

我們撰寫了三個實作範例，分別對應並深入釐清這些觀念。

---

## 📂 實作原始碼連結
*   [fork_exec.c](file:///c:/Users/angel/nqu/SystemProgramming/114b_sp/HW06/fork_exec.c) - 示範行程建立與映像檔取代。
*   [file_io.c](file:///c:/Users/angel/nqu/SystemProgramming/114b_sp/HW06/file_io.c) - 示範低階檔案存取。
*   [redirect.c](file:///c:/Users/angel/nqu/SystemProgramming/114b_sp/HW06/redirect.c) - 示範標準輸入輸出重導向與 `dup2`。
*   [Makefile](file:///c:/Users/angel/nqu/SystemProgramming/114b_sp/HW06/Makefile) - 自動化編譯設定。

---

## 📖 核心觀念詳細釐清

### 1. 行程管理篇：fork, execvp, wait
*   **`fork()`**：
    *   **定義**：用於建立一個新的子行程。
    *   **特性**：`fork()` 會複製目前父行程的所有資源（代碼段、資料段、堆疊、檔案描述子等）。
    *   **回傳值**：`fork()` 會**回傳兩次**：
        1. 在**父行程**中，回傳子行程的 PID（正整數）。
        2. 在**子行程**中，回傳 `0`。
        3. 若失敗則回傳 `-1`。
*   **`execvp(file, argv)`**：
    *   **定義**：將目前的行程映像（Image）替換為新指定的程式。
    *   **特性**：`execvp` 執行成功後**絕不返回**，因為整個行程的記憶體已被新程式完全覆蓋並從新程式的 `main` 開始執行。只有在發生錯誤（如找不到程式）時才會回傳 `-1`。
*   **`wait(status)` / `waitpid(pid, status, options)`**：
    *   當子行程終止時，若父行程沒有讀取其終止狀態，該子行程會變成**殭屍行程 (Zombie Process)** 佔用系統資源。
    *   父行程必須調用 `wait()` 來回收子行程資源並獲取其結束狀態（Status）。

---

### 2. 檔案系統篇：open, close, read, write
*   **檔案描述子 (File Descriptor, fd)**：
    *   核心是一個非負整數，為行程的檔案描述子表（File Descriptor Table）的索引，指向系統核心中開啟的檔案結構。
    *   系統預設為每個行程開啟三個描述子：
        *   `0`：標準輸入 (stdin)
        *   `1`：標準輸出 (stdout)
        *   `2`：標準錯誤 (stderr)
*   **`open(path, flags, mode)`**：
    *   開啟或建立檔案。常用參數包括 `O_RDONLY` (唯讀)、`O_WRONLY` (唯寫)、`O_CREAT` (若不存在則建立)、`O_TRUNC` (若存在則清空)、`O_APPEND` (追加寫入)。
    *   **核心規則**：`open` 成功時，核心保證指派**目前未被該行程使用的最小非負整數**作為該檔案的描述子。
*   **`close(fd)`**：
    *   關閉指定的檔案描述子，釋放對應的核心資源與 fd 索引。
*   **`read(fd, buf, count)`** & **`write(fd, buf, count)`**：
    *   Linux 最底層的低階 I/O 系統呼叫，直接對核心檔案緩衝區進行位元組層級的讀寫，屬於 **Unbuffered I/O**。與 `fread`/`fwrite`（有 C 語言標準庫用戶端緩衝）不同。

---

### 3. 重導向篇：dup2 與 fd 取代
*   **重導向原理**：
    *   `printf(...)` 的底層運作是向描述子 `1` 進行 `write()` 寫入。
    *   若我們將描述子 `1` 指向的檔案結構，從「螢幕（終端機）」改成「某個文字檔」，則所有呼叫 `printf` 的輸出都會自動流向該文字檔。
*   **實現重導向的兩種方法**：
    1.  **先 close 後 open**：
        *   先調用 `close(1)`。此時描述子 `1` 空閒。
        *   再調用 `open("log.txt", ...)`。因為 `1` 是最小且未使用的 fd，系統會指派 `1` 給 `log.txt`。
        *   自此 `stdout` (1) 被重導向到 `log.txt`。
    2.  **`dup2(oldfd, newfd)`**（更安全、更常用）：
        *   強制將 `oldfd` 的指向複製給 `newfd`。若 `newfd` 原本處於開啟狀態，系統會先自動將其關閉。
        *   例如：`dup2(fd_file, 1)` 會將標準輸出 (1) 重導向到 `fd_file` 指向的檔案。這在 Shell 實作管道（Pipe）與 `>` 重導向時是標準做法。

---

## 🛠️ 編譯與執行說明 (WSL / Linux 環境)

由於這些系統呼叫是 POSIX 標準，請在 **WSL (Windows Subsystem for Linux)** 或 Linux 環境下進行編譯：

### 1. 編譯所有程式
於 `HW06` 目錄下執行：
```bash
make
```

### 2. 執行行程管理範例 (`fork_exec`)
```bash
./fork_exec
```
**預期輸出說明**：
- 父行程列印 PID，隨後 fork。
- 子行程列印自己的 PID 及其父行程 PID。
- 子行程呼叫 `execvp` 執行 `ls -l`，子行程原本的程式碼被取代，並在螢幕印出檔案列表。
- 父行程成功等待（`wait`）子行程執行完畢後回收，並印出 Normal Exit 訊息。

### 3. 執行低階檔案 I/O 範例 (`file_io`)
```bash
./file_io
```
**預期輸出說明**：
- 開啟 `test_io.txt` 寫入內容，印出獲取的 fd（例如 3 或其他）。
- 關閉後，再次以唯讀模式開啟（fd 通常仍為 3）。
- 讀取內容並印在螢幕上。

### 4. 執行重導向範例 (`redirect`)
```bash
./redirect
```
**預期輸出說明**：
- **示範 1 (close + open)**：關閉 stdout 後，`open("redirect_stdout.txt")` 使其獲得 fd 1。隨後的 `printf` 輸出不再出現在螢幕，而是寫入 `redirect_stdout.txt` 中。接著利用複本還原 `stdout` 至螢幕。
- **示範 2 (dup2 + fork + execvp)**：子行程開啟 `redirect_exec.txt`，用 `dup2` 將標準輸出導向該檔案，並呼叫 `execvp` 執行 `echo`。父行程等待結束後，將 `redirect_exec.txt` 讀回並列印，驗證子行程外部指令的輸出成功寫入了檔案。

---

## 🧹 清除編譯產物
```bash
make clean
```
這會刪除編譯出的執行檔以及測試產生的所有 `.txt` 檔案。