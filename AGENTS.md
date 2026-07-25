# AGENTS.md

本文件是 AI Agent（OpenCode、Claude、Qwen、Codex 等）的專案入口。

**開始任務前先讀本文件。**

除非使用者明確要求，否則：

- 不要掃描整個 Repository。
- 不要直接完整讀取 `TX-2.C`。
- 不要修改與任務無關的檔案。
- 先讀文件，再讀必要原始碼。

---

## Standard Workflow

1. 閱讀 `AGENTS.md`。
2. 閱讀 `docs/project_map.md`。
3. 依任務選擇相關模組文件。
4. 必要時閱讀 `docs/shared_state.md`、`docs/interrupts.md` 或 `docs/build.md`。
5. 最後才閱讀對應原始碼。
6. 先確認影響範圍，再進行修改。
7. 程式碼變更後，同步更新受影響的文件。
8. 未執行 Build 或硬體測試時，必須明確標示未驗證。

完整功能開發、除錯或 Code Review 時，再閱讀：

`docs/development_workflow.md`

---

## Task Routing

| 任務 | 優先閱讀 |
|---|---|
| 初始化、`main()` | `docs/modules/main.md` |
| State Machine | `docs/modules/state_machine.md` |
| 音量、音效、輸入源、Action | `docs/modules/control_logic.md` |
| UART、RS232、ZSound | `docs/modules/communication_protocol.md` |
| EEPROM | `docs/modules/storage.md` |
| IO Expander | `docs/modules/io_expander.md` |
| 按鍵、旋鈕 | `docs/modules/key_input.md` |
| 顯示器、LED | `docs/modules/display.md` |
| ISR | `docs/interrupts.md` |
| 全域變數、共享狀態 | `docs/shared_state.md` |
| Build、Linker、Memory | `docs/build.md` |
| 架構、模組依賴 | `docs/architecture.md` |
| 未確認問題 | `docs/open_questions.md` |

不確定時先讀：

`docs/project_map.md`

---

## Project Summary

**推論：**

TX-2 是執行於 MPC82G516A 8051 相容 MCU 的 KTV／音響擴大機控制韌體，主要負責：

- Music／Mic 音量控制
- 音效與輸入源切換
- 喇叭切換
- 7 段顯示器與狀態 LED
- 紅外線遙控
- RS232／ZSound 協定
- EEPROM 設定持久化

詳細資訊見：

- `docs/project_map.md`
- `docs/architecture.md`

---

## Toolchain

- MCU：**MPC82G516A**
- Compiler：**Keil C51**
- Toolchain：PK51 v9.60.7.0
- Memory Model：**Large**
- IRAM：256 Bytes
- XRAM：1 KB
- Reentrant Stack：全部停用
- Include Path：空
- Project Define：空

本專案不是 GCC、Clang、ARM、CMake 或一般桌面 C/C++ 專案。

不要自行使用：

- C99／C11 特性
- GCC／Clang Extension
- 動態記憶體配置
- 現代桌面標準函式庫假設

完整資訊見：

`docs/build.md`

---

## Coding Rules

### Header

除 `MPC82.H` 外，共用宣告主要集中於：

`userdefine.h`

因此：

- 不要自行建立新模組 Header。
- 新增全域函式原型或變數宣告時，沿用現有結構。
- 除非使用者要求重構，否則不要拆分既有宣告。

### GPIO

GPIO 透過 `userdefine.h` 中的 `sbit` 與巨集存取。

- 不要重複定義腳位。
- 不要猜測硬體連線。
- 修改前先確認相關文件與 `userdefine.h`。

### Feature Switch

現有功能開關使用：

```c
#if 0
#endif

#if 1
#endif
```

除非任務明確要求，否則不要引入新的 Compiler Define 管理方式。

### Function Name Case

Keil Project 設定：

```text
CaseSensitiveSymbols = 0
```

已存在 `LOUD_Action`／`Loud_Action` 大小寫不一致的案例。

- 修改函式名稱時同步檢查所有呼叫端。
- 不要依賴 Linker 的大小寫容忍。
- 不要無故整理既有命名。

### TX-2.C

`TX-2.C` 約 4491 行，包含多種職責。

先讀 `docs/modules/*.md`，再定位需要的函式。不要一開始完整掃描此檔。

---

## Memory and ISR Rules

### Recursion

Reentrant Stack 已停用，且專案已有 Linker 偵測到的遞迴呼叫：

- `Main_Function_Loop`
- `Sdataout`

因此：

- 絕對不要新增遞迴。
- 不要建立循環呼叫鏈。
- 修改呼叫關係後檢查 Build Warning 與 MAP。
- 不要把現有 Warning 視為安全保證。

### Local Variables

8051 IRAM 僅 256 Bytes。

- 避免大型區域變數與陣列。
- 避免不必要的深層呼叫。
- 優先維持現有靜態或全域配置方式。

### ISR

`T2_int` 已承擔過多邏輯。

因此：

- 不要再向 ISR 增加業務邏輯。
- 新功能優先放在前景輪詢。
- ISR 只處理必要旗標、計數器與硬體狀態。
- 修改 ISR 前先讀 `docs/interrupts.md`。
- 涉及 ISR／前景共享資料時，再讀 `docs/shared_state.md`。

目前 ISR 沒有使用 Keil `using n`。不要在未理解影響前自行加入。

---

## Important Project Rules

- `STARTUP.A51` 已證實未參與現行 Build，不要假設修改它會生效。
- `IOEXP6524.c` 與 `Eep24C04.c` 各自有獨立的軟體 I2C 實作。
- 不要假設修改其中一邊會影響另一邊。
- EEPROM 位址與資料格式不可任意調整。
- 不要自行拆分 `TX-2.C`。
- 不要進行無關的大規模格式化或重新命名。
- 優先維持既有行為。

---

## Documentation Rules

文件中的結論必須區分：

- **已證實**
- **推論**
- **待確認**

不要將推論或建議寫成已證實事實。

程式碼變更若影響下列內容，必須同步更新對應文件：

| 變更 | 文件 |
|---|---|
| 架構、資料流 | `docs/architecture.md` |
| 初始化 | `docs/modules/main.md` |
| State Machine | `docs/modules/state_machine.md` |
| 控制邏輯 | `docs/modules/control_logic.md` |
| 顯示 | `docs/modules/display.md` |
| RS232／協定 | `docs/modules/communication_protocol.md` |
| EEPROM | `docs/modules/storage.md` |
| IO Expander | `docs/modules/io_expander.md` |
| 按鍵、旋鈕 | `docs/modules/key_input.md` |
| ISR | `docs/interrupts.md` |
| 全域或共享狀態 | `docs/shared_state.md` |
| Build、Memory、Linker | `docs/build.md` |
| 未確認問題 | `docs/open_questions.md` |

若文件與程式碼不一致：

1. 確認目前檔案是否屬於現行 Build。
2. 以有效原始碼與 Build 證據為準。
3. 修正文件。
4. 無法確認時更新 `docs/open_questions.md`。
5. 不要猜測原開發者意圖。

---

## Completion Report

任務完成後簡要回報：

- 閱讀了哪些文件與原始碼
- 修改了哪些檔案
- 修改內容與影響範圍
- 是否影響 ISR、Shared State、Memory 或 Build
- 是否更新文件
- 是否執行 Keil Build
- Build Error／Warning 結果
- 是否完成硬體驗證
- 尚待確認事項