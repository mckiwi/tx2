# Module: main（Entry Point / Initialization / Main Loop 殼層）

檔案：`TX-2.C`

## 進入點

**已證實**：函式 `main`，檔案 `TX-2.C`。

**推論**：Keil C51 專案的標準流程為 `?C_STARTUP` → `?C_START` → `main`。但**已證實**現行建置實際上並未包含 `STARTUP.A51`（不在 Source Group、不在任何 `.lnp`/`.MAP` 連結指令中，見 `docs/build.md`），因此 `main` 實際由哪個啟動模組呼叫進入待確認（見 `docs/open_questions.md`）。

## 初始化流程（依序執行，已證實）

`main` 內依序執行：

1. Port 初始值設定（`P0-P3` 全設高電位）
2. 中斷/計時器相關 SFR 設定（`IE`,`TMOD`,`TCON`,`T2CON`,`RCAP2`,`IP`）
3. 全域中斷致能
4. 函式 `Clear_State`（檔案 `TX-2.C`）— 重置紅外線解碼狀態機
5. 函式 `init_taskTPreset`（檔案 `TX-2.C`）— 初始化排程計數器
6. 全域功能旗標歸零（含 `gc_Task=0`,`isBoot=1`,`toInit=1`）
7. 函式 `UART_init`、`UART_Variable_Reset`（檔案 `RS232.C`）
8. 忙等延遲
9. 函式 `PowerPlugMCU_init`（檔案 `TX-2.C`）— 主板 GPIO 腳位上電/解除 reset（**注意**：此函式本身位於 `TX-2.C`，非 IO Expander 模組檔案，僅是為後續 IO expander 初始化預備電氣條件）
10. 函式 `initIOPorts`（檔案 `IOEXP6524.c`）— 分別初始化兩顆 IO expander
11. 函式 `Check_EEPROM`（檔案 `TX-2.C`）— 驗證 EEPROM machine code
12. 函式 `Display_7SEG_init`（檔案 `TX-2.C`）— 初始化七段顯示器
13. 函式 `ClearAllLED`（檔案 `TX-2.C`）— 關閉所有 LED/輸出，並將 `gc_Task` 重設為 0
14. 清除音量計數器等旗標，`toInit=0`
15. 呼叫函式 `Main_Function_Loop`（檔案 `TX-2.C`）— **此後不再返回 `main`**

## Main Loop 殼層

**已證實**：`Main_Function_Loop`（檔案 `TX-2.C`）是一個以全域變數 `gc_Task` 為狀態碼的 `while(1)` 迴圈殼層，依 `gc_Task` 值分派至三個狀態對應的函式執行。

殼層本身僅負責 dispatch，**不包含**狀態轉換邏輯與業務細節——完整狀態機說明（三態轉換規則、所有寫入點、風險）見 `docs/modules/state_machine.md`。

## 依賴

- State Machine（`docs/modules/state_machine.md`）— `main` 呼叫 `Main_Function_Loop` 進入狀態機
- Communication Protocol（`docs/modules/communication_protocol.md`）— `UART_init`/`UART_Variable_Reset`
- IO Expander（`docs/modules/io_expander.md`）— `initIOPorts`（`PowerPlugMCU_init` 不屬於此模組，見上方第 9 步註記）
- Storage（`docs/modules/storage.md`）— `Check_EEPROM`
- Display（`docs/modules/display.md`）— `Display_7SEG_init`/`ClearAllLED`

## 待確認

- `main` 內大量忙等延遲（`while(gw_TimerBase);`）期間若中斷延遲或被搶佔，是否影響開機時序，未實測驗證。
- `main` 實際由哪個啟動模組呼叫進入（見上方「進入點」章節），待確認。
