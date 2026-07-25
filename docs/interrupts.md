# Interrupts

**已證實**：全專案共掃描出 5 個中斷服務函式（ISR），全部位於 `TX-2.C` 與 `RS232.C`。`STARTUP.A51` 不含向量表（8051 中斷向量由 Keil C51 依 `interrupt N` 關鍵字自動產生）。未發現任何 ADC/PWM 相關中斷。

## ISR 總表

| # | 函式 | 檔案 | interrupt N（向量位址） | 優先權 | 硬體週邊 |
|---|---|---|---|---|---|
| 1 | `EXint0` | `TX-2.C` | 0（0x0003） | HIGH（`PX0=1`） | 外部中斷 INT0，紅外線遙控解碼 |
| 2 | `T0_int` | `TX-2.C` | 1（0x000B） | HIGH（`PT0=1`） | Timer0，IR 位元計時 |
| 3 | `T1_int` | `TX-2.C` | 3（0x001B） | LOW | Timer1（同時作為 UART 鮑率產生器） |
| 4 | `UART_Interrupt` | `RS232.C` | 4（0x0023） | LOW | UART，RS232 命令收發 |
| 5 | `T2_int` | `TX-2.C` | 5（0x002B） | LOW | Timer2，~1ms 系統節拍 |

優先權判定依據：`main` 函式內 `IP = 0x03` 設定（`TX-2.C`），僅 Timer0 與 INT0 為高優先權。

## 逐項說明

### 1. `EXint0`（TX-2.C）
- 職責：紅外線遙控訊號位元級解碼狀態機（`WAITING_STATE→...→GET_DATA_BAR`）
- 讀寫共享變數：`IR_STATE`,`Ticks`,`TempData`,`BitCount`,`Custom_Code*`,`Data_Code*`,`NewKeyFlag`,`gc_IrPowerOnOff`（詳見 `shared_state.md`）
- 呼叫函式：`Start_Timer0`,`Get_Code`,`Clear_State`,`VR_Stop`（皆在 `TX-2.C`）
- 與前景資料交換：設定旗標供 `Remote_Scan`/`Remote_Key`（皆 `TX-2.C`，前景輪詢消費，見 `modules/control_logic.md`「前景消費函式」章節）
- **已證實**：完整遙控碼需橫跨 5 次中斷觸發組成，中介狀態全域變數無雙緩衝保護。**推論**：連續按鍵時新一輪解碼可能在前景讀完舊資料前覆寫（未經實測驗證，完整說明見 `docs/shared_state.md`）。
- 耗時操作：無，僅位元運算。

### 2. `T0_int`（TX-2.C）
- 職責：Timer0 溢位計時，供 IR 解碼位元週期判斷
- 讀寫共享變數：`Ticks`
- 呼叫函式：無
- 與前景資料交換：無（僅供 `EXint0` 內部使用）
- 耗時操作：無

### 3. `T1_int`（TX-2.C）
- 職責：Timer1 溢位（該 Timer 同時被 `UART_init` 設為 UART 鮑率自動重載來源）
- 讀寫共享變數：僅重啟 `TR1`
- **推論**：Timer1 已切換為自動重載模式，理論上不需中斷；此 ISR 目前無實質功能，可能為冗餘/遺留碼（待確認，見 `open_questions.md`）
- 耗時操作：無，但若真為冗餘則每次鮑率 tick 都會產生不必要的中斷負擔

### 4. `UART_Interrupt`（RS232.C）
- 職責：逐位元組組裝 RS232 五位元組命令幀，完成時設定 `UART_STATUS=2`
- 讀寫共享變數：`UART_STATUS`,`UART_INDEX`,`UART_RX_DATA`,`UART_RX_BUF`
- 呼叫函式：`UART_Variable_Reset`（`RS232.C`）
- 與前景資料交換：`Main_Loop`/`PowerONDetect`（`TX-2.C`）輪詢 `UART_STATUS==2` 後呼叫 `UART_main`
- **已證實風險**：未檢查/清除 `TI`，而發送函式 `UART_Send`（`RS232.C`）以前景忙等清除 `TI`，導致每次發送觸發一次多餘中斷進出。
- 耗時操作：無

### 5. `T2_int`（TX-2.C）
- 職責：Timer2 溢位，系統 ~1ms 節拍來源，呼叫排程鏈 `each1ms_event→each25/55ms_event→each220ms_event`（皆在 `TX-2.C`）
- 讀寫共享變數：`x1ms`,`gw_TimerBase`,及排程計數器（`x25/55/220ms_counter`），並間接驅動顯示與 LED 相關旗標
- 呼叫函式：`each1ms_event`（內部再呼叫 `each25ms_event`,`each55ms_event`,`each220ms_event`）
- 與前景資料交換：`gw_TimerBase` 供前景 `while(gw_TimerBase);` 忙等使用；顯示/LED 效果直接在中斷內完成，無需前景再處理
- **已證實風險**：此 ISR 在中斷情境內執行多層巢狀呼叫與大量業務邏輯（7-段顯示掃描、LED 閃爍時序、音量 turbo 動作），是 5 個 ISR 中工作量最大者。
- 耗時操作：**是**，相較其餘 ISR 明顯偏重。

## 共通觀察

**已證實**：
- 5 個 ISR 均未使用 Keil `using n` 暫存器庫宣告。
- 建置紀錄（`TX-2_20241007-IOEXP_MCUBD.build_log.htm`）證實部分函式間存在遞迴呼叫（如 `Main_Function_Loop` 被多個 Control Logic 函式遞迴呼叫），linker 已將其自堆疊分析中剔除，代表 ISR 與一般函式共用的堆疊實際峰值未被建置工具驗證。詳見 `docs/build.md`。
