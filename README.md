# 課程：系統程式 -- 筆記、習題與報告

欄位 | 內容
-----|--------
學期 | 114 學年下學期
學生 |  歐子綺
學號末兩碼 | 48
教師 | [陳鍾誠](https://www.nqu.edu.tw/educsie/index.php?act=blog&code=list&ids=4)
學校科系 | [金門大學資訊工程系](https://www.nqu.edu.tw/educsie/index.php)
課程教材 | https://github.com/ccc114b/cpu2os<br/>https://github.com/cccbook/ai-teach-you/blob/main/sp/tw/README.md<br/>https://github.com/ccc-c/c0computer

---

**全部功課都是AI原創，程式碼沒有複製或參考同學或網路的**

## 習題與作業總結 (只做了HW02 ~ HW06)

下面是本學期系統程式課程中，各個作業的實作內容與主題摘要：

### [習題 2 -- Lucy 程式語言與棧式虛擬機器實作 (Rust 版)](https://github.com/Oanggla/114b_sp/tree/master/HW02)
* **主題**：自訂程式語言編譯與虛擬機器設計
* **說明**：使用 **Rust** 實作自訂語言 **Lucy**。包含手寫詞法分析器 (Lexer)、遞迴下降語法分析器 (Parser)、將抽象語法樹 (AST) 編譯為 JSON 格式位元組碼 (`.lucyc`) 的編譯器，以及一個支援 Call Frame、區域/全域變數及遞迴呼叫的堆疊式虛擬機器 (LumiVM/LucyVM)。特別支援管道操作符 (`|>`)。

### [習題 3 -- dc機器人 (gbot)](https://github.com/Oanggla/114b_sp/tree/master/HW03)
* **主題**：Discord 機器人開發
* **說明**：使用 AI 工具協助開發與實作的 Discord 機器人。專案連結：[gbot](https://github.com/Oanggla/gbot)。

### [習題 4 -- 系統程式教科書目錄與大綱](https://github.com/Oanggla/114b_sp/blob/master/HW04/README.md)
* **主題**：系統程式理論與架構規劃
* **說明**：編寫《系統程式：從虛擬機器、組譯器到連結載入器的底層機制》的教科書大綱與各章節詳細目錄。內容涵蓋簡化指令電腦 (SIC/XE) 架構、組譯器、巨集處理器、載入器與連結器、編譯器與虛擬機器等底層翻譯與執行機制。

### [習題 5 -- 執行緒、競爭情況與同步問題實作](https://github.com/Oanggla/114b_sp/blob/master/HW05/README.md)
* **主題**：多執行緒同步與互斥 (POSIX Threads)
* **說明**：使用 **C 語言** 實作三個作業系統中經典的並行控制問題：
  * **銀行存提款模擬 (`bank.c`)**：使用互斥鎖 (`pthread_mutex_t`) 解決 Race Condition，確保共享餘額修改的原子性。
  * **生產者消費者問題 (`producer_consumer.c`)**：使用環狀緩衝區，配合三個訊號量 (`empty`, `full`, `mutex`) 實現多線程同步。
  * **哲學家用餐問題 (`philosopher.c`)**：採用不對稱拿取法（奇數/偶數哲學家拿筷順序不同）打破循環等待鏈以預防死結 (Deadlock)。

### [習題 6 -- 行程與檔案相關系統呼叫](https://github.com/Oanggla/114b_sp/blob/master/HW06/README.md)
* **主題**：Linux 行程管理、檔案系統與重導向
* **說明**：使用 **C 語言** 探討 POSIX 標準核心系統呼叫：
  * **行程管理 (`fork_exec.c`)**：使用 `fork()` 建立子行程、`execvp()` 載入並取代為外部指令 (`ls -l`)，並呼叫 `wait()` 回收子行程以避免殭屍行程。
  * **檔案系統 I/O (`file_io.c`)**：利用 `open()`、`close()`、`read()`、`write()` 執行 Unbuffered 低階檔案讀寫，釐清系統分配最小非負整數為檔案描述子 (fd) 的規則。
  * **重導向 (`redirect.c`)**：利用 `close + open` 以及 `dup2()` 複製檔案描述子，將標準輸出重導向至文字檔。
