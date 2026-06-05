# 《系統程式：從虛擬機器、組譯器到連結載入器的底層機制》—— 課程教科書目錄

這本書旨在為大專院校資工、資科及相關科系學生設計，以經典系統軟體（System Software）理論為核心，著重於組譯器、巨集處理器、連結器、載入器與編譯器的設計與運作原理。本書將引導學生探索程式如何從高階語言、組合語言，最終翻譯並載入至機器架構（如簡化指令電腦 SIC/XE）中執行的完整生命週期。  

---

## 📖 本書目錄

### [前言：什麼是系統程式？](CH0_preface.md)
- 系統程式 (System Software) 的定義與範疇
- 系統程式 vs. 應用程式 (Application Software) 的差異
- 本書的學習架構與預備知識

---

## 💻 第一部分：機器架構與基礎

### [第一章：簡化指令電腦 (SIC 與 SIC/XE) 架構](CH1_SIC_and_SIC_XE_Architecture.md)
- **1.1 機器架構 (Machine Architecture) 的重要性**
  - 軟體與硬體之間的橋樑
- **1.2 基礎 SIC (Simplified Instructional Computer) 架構**
  - 記憶體結構、暫存器配置、資料格式
  - 指令集與基本定址模式 (Addressing Modes)
  - 簡易輸入與輸出 (I/O) 運作方式
- **1.3 進階 SIC/XE 架構**
  - 擴充暫存器與資料格式
  - 指令格式剖析（Format 1, Format 2, Format 3, Format 4）
  - 定址模式詳解：相對定址（PC-relative, Base-relative）、立即定址、間接定址、索引定址
- **1.4 實務探討：現代機器架構對照**
  - CISC (x86) 與 RISC (ARM, RISC-V) 的定址與指令設計對照

---

## ⚙️ 第二部分：翻譯程式 (Translation Programs)

### [第二章：組譯器 (Assemblers)](CH2_Assemblers.md)
- **2.1 組譯器的基本功能與指令翻譯**
  - 機器指令翻譯（操作碼 OPTAB 與符號表 SYMTAB）
  - 組譯器虛擬指令 (Assembler Directives)：`START`, `END`, `BYTE`, `WORD`, `RESB`, `RESW`
- **2.2 二行程組譯器 (Two-Pass Assembler) 設計**
  - **Pass 1 演算法**：分配記憶體位址、計算位置計數器 (Location Counter, LOCCTR)、建立符號表 (SYMTAB)
  - **Pass 2 演算法**：產生機器碼、格式化輸出目的檔 (Object Program)
  - 目的檔格式解析：Header, Text, Modification, End 紀錄
- **2.3 機器相關的組譯器功能**
  - 程式計數器相對定址 (PC-Relative) 與基底暫存器相對定址 (Base-Relative) 的組譯處理
  - 延伸指令格式 (Format 4) 與修改紀錄 (Modification Record) 的生成
- **2.4 機器獨立的組譯器功能**
  - 字面值 (Literals) 的處理與字面值表 (LITTAB) 設計
  - 符號定義敘述：`EQU` 與 `ORG`
  - 程式區段 (Program Blocks) 的多區段位址分配
  - 控制區段 (Control Sections) 與外部參考 (External References) 的連結準備
- **2.5 單行程組譯器 (One-Pass Assembler) 與多行程組譯器**
  - 前向參考 (Forward Reference) 的動態解析與鏈結表設計

### [第三章：巨集處理器 (Macro Processors)](CH3_Macro_Processors.md)
- **3.1 巨集定義與巨集擴充 (Macro Definition and Expansion)**
  - 巨集指令 (Macro Instruction) 與子程式 (Subroutine) 的比較
  - 巨集處理器的基本功能與處理流程
- **3.2 一行程巨集處理器 (One-Pass Macro Processor) 設計**
  - 為什麼巨集定義與擴充必須在同一個行程內處理？
  - 核心資料結構設計：
    - 定義表 (DEFTAB)：儲存巨集定義內容
    - 命名表 (NAMTAB)：紀錄巨集名稱及在 DEFTAB 的起點與終點
    - 實際參數值表 (ARGTAB)：記錄執行期的參數替代值
  - 巨集擴充演算法與詳細流程
- **3.3 進階與特殊的巨集功能**
  - 巨集定義中的巨集定義 (Concatenated Macro Definitions)
  - 唯一標記的自動生成 (Generation of Unique Labels)
  - 條件式巨集擴充 (Conditional Macro Expansion)：`IF`, `ELSE`, `ENDIF`, `WHILE` 虛擬指令
  - 關鍵字參數 (Keyword Parameters) 支援

---

## 🔗 第三部分：連結與載入 (Linking and Loading)

### [第四章：載入器與連結器 (Loaders and Linkers)](CH4_Loaders_and_Linkers.md)
- **4.1 載入與連結的基本概念**
  - 載入 (Loading)、重定位 (Relocation)、連結 (Linking) 的定義與分工
- **4.2 簡易載入方案**
  - 絕對載入器 (Absolute Loader) 的運作與限制
  - 自我重定位載入器 (Bootstrap Loader) 的工作原理
- **4.3 連結載入器 (Linking Loader) 設計**
  - 控制區段 (Control Sections) 與外部符號的解析
  - 定義紀錄 (Define Record) 與參照紀錄 (Refer Record)
  - 外部符號表 (External Symbol Table, ESTAB) 設計
  - **Pass 1 演算法**：計算所有控制區段與外部符號的載入位址，建立 ESTAB
  - **Pass 2 演算法**：載入執行碼、執行地址重定位，並解析外部符號關聯
- **4.4 連結編輯器 (Linkage Editors) 運作機制**
  - 連結編輯器與連結載入器的比較
  - 靜態連結庫的合成與重定位
- **4.5 動態連結與共享庫 (Dynamic Linking & Shared Libraries)**
  - 動態連結的優缺點與設計架構
  - 執行期連結 (Run-time Linking) 與動態載入的運作

---

## 📝 第四部分：高階語言翻譯 (High-Level Language Translation)

### [第五章：編譯器 (Compilers)](CH5_Compilers.md)
- **5.1 編譯器的基本結構**
  - 前端與後端的劃分，以及編譯的六個主要階段
- **5.2 詞法分析 (Lexical Analysis / Scanning)**
  - 正規表示式 (Regular Expressions) 與有限狀態自動機 (Finite Automata)
  - 記號 (Token) 的識別與符號表管理
- **5.3 語法分析 (Syntactic Analysis / Parsing)**
  - 上下文無關文法 (Context-Free Grammar) 與語法樹 (Parse Tree)
  - 由上而下分析 (Top-Down)：遞迴下降分析、LL(1) 分析表設計
  - 由下而上分析 (Bottom-Up)：移位-歸約 (Shift-Reduce) 與 LR 分析器
- **5.4 中間碼產生與最佳化**
  - 三位址碼 (Three-Address Code)、四元組 (Quadruples) 與三元組 (Triples)
  - 程式碼最佳化 (Code Optimization) 常見技術：常數摺疊、死碼刪除、共同子運算式消除
- **5.5 目標碼產生 (Code Generation)**
  - 暫存器分配 (Register Allocation) 與機械碼生成

### [第六章：解譯器與虛擬機器 (Interpreters and Virtual Machines)](CH6_Interpreters_and_Virtual_Machines.md)
- **6.1 解譯器 (Interpreters) 的工作原理**
  - 解譯執行與編譯執行的比較
  - 抽象語法樹 (AST) 的直接解譯與評估
- **6.2 位元組碼 (Bytecode) 與虛擬機器實作**
  - 堆疊式虛擬機器 (Stack-Based VM) vs. 暫存器式虛擬機器 (Register-Based VM)
  - 設計並實作一個簡單的虛擬機器模擬器（CPU 模擬、記憶體模型、暫存器、解碼與執行迴圈）
- **6.3 即時編譯 (JIT - Just-In-Time Compilation) 簡介**

---

## 🛠️ 第五部分：系統支援工具與實務

### [第七章：文字編輯器與除錯系統 (Editors and Debuggers)](CH7_Editors_and_Debuggers.md)
- **7.1 文字編輯器的設計與結構**
  - 編輯器核心：文字緩衝區管理（Gap Buffer, Rope 等資料結構）
  - 使用者介面對應與編輯命令處理
- **7.2 除錯系統 (Debugging Systems) 的基本功能與設計**
  - 符號式除錯 (Symbolic Debugger) 的運作機制
  - 斷點 (Breakpoints) 的實現原理：軟體斷點（指令置換如 `INT 3`）與硬體斷點（偵錯暫存器）
  - 系統呼叫追蹤與除錯控制介面剖析

---

## 📚 [附錄與專案研討](Appendices.md)

- [**附錄 A：SIC/XE 指令集與操作碼 (OPTAB) 對照表**](Appendices.md#附錄-a-sicxe-指令集與操作碼-optab-對照表)
- [**附錄 B：SIC/XE 定址模式與計算公式速查**](Appendices.md#附錄-b-sicxe-定址模式與計算公式速查)
- [**附錄 C：學生手動實作專案指引 (Hands-on Lab Projects)**](Appendices.md#附錄-c-學生手動實作專案指引-hands-on-lab-projects)
  - 專案一：以 Python/C++ 實作一個 SIC/XE 虛擬機器 (Emulator/Simulator)
  - 專案二：實作一個支援完整定址模式的二行程 SIC/XE 組譯器 (Two-Pass Assembler)
  - 專案三：實作一個簡易的 One-Pass 巨集處理器 (Macro Processor)
