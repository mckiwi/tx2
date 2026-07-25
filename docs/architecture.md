# Architecture

本文件只記錄高層架構、資料流與模組依賴。逐函式細節請至 `docs/modules/*.md`；共享變數細節請至 `docs/shared_state.md`；中斷細節請至 `docs/interrupts.md`。

## 共用基礎設施（非獨立模組）

**已證實**：全專案沒有依模組拆分的標頭檔。除 `MPC82.H`（SFR/型別擴充定義）外，所有跨檔案的全域變數宣告、函式原型、硬體腳位巨集（`sbit`）都集中在單一 `userdefine.h`，並被所有 `.c/.C` 檔案 `#include`。

- `userdefine.h`：型別定義（`U8/U16`）、時脈/鮑率常數、腳位對映（`sbit` 對 P0–P4）、硬體動作巨集（如 LED/風扇/PWM 控制巨集）
- `MPC82.H`：SFR 擴充定義（`sfr16 RCAP2/T2R` 等）、Keil 內建函式巨集（`RR8/RL8/NOP` 等）
- 外部依賴：所有 `.c` 檔案 `#include "REG_MPC82G516.H"`，此檔案**未存在於本 repo**（待確認其來源，見 `docs/open_questions.md`）

## 高層資料流

```
[IR 遙控器脈波]
   → INT0 硬體邊緣觸發 → EXint0()（interrupt 0）
       ↳ 組出 Custom_Code/Data_Code，設定 NewKeyFlag、gc_IrPowerOnOff
   → 前景輪詢（Remote_Scan/Remote_Key，State Machine/Control Logic）消費

[Timer0 溢位]
   → T0_int()（interrupt 1）→ Ticks++ → 供 EXint0 判斷 IR 位元週期

[Timer1 溢位]
   → T1_int()（interrupt 3）→ 目前無實質作用（推論：疑似冗餘，見 open_questions.md）

[RS232 收/發位元組]
   → UART_Interrupt()（interrupt 4）→ 組出 UART_RX_BUF，設定 UART_STATUS
   → 前景輪詢（Main_Loop/PowerONDetect）→ UART_main() 解析 → Communication Protocol 執行命令

[Timer2 溢位，~1ms 節拍]
   → T2_int()（interrupt 5）→ each1ms_event()→each25/55/220ms_event()
       ↳ 7-段顯示掃描（Display 模組）、LED 閃爍、音量 turbo 動作（Control Logic）皆在此中斷鏈內完成
```

完整 ISR 清單與風險見 `docs/interrupts.md`。

## 模組依賴圖

```
main (modules/main.md)
├── Keil Startup / Build（見 build.md，STARTUP.A51 實際未參與現行建置）
├── 共用基礎設施（userdefine.h, MPC82.H）
├── State Machine（modules/state_machine.md）
│   ├── Interrupt（interrupts.md）
│   ├── Communication Protocol（modules/communication_protocol.md）
│   │   └── 共用基礎設施
│   ├── Control Logic（modules/control_logic.md）
│   │   ├── Display（modules/display.md）
│   │   ├── Storage（modules/storage.md）
│   │   ├── IO Expander（modules/io_expander.md）
│   │   └── 共用基礎設施
│   └── Key Input（modules/key_input.md）
│       └── Control Logic
└── Sensor / ADC — 未發現
```

## 跨模組耦合觀察

**已證實**：
- `TX-2.C`（4491 行）單一檔案同時承擔 Application、State Machine、Control Logic、Display、部分 Timer/Interrupt 設定，缺乏檔案層級模組邊界。
- `Eep24C04.c`（Storage）與 `IOEXP6524.c`（IO Expander）各自重複實作幾乎相同的軟體 I2C bit-bang 原語（`iicSTART/STOP/...` vs `ioSTART/STOP/...`），程式碼重複但操作不同物理匯流排腳位。
- 多處各自實作性質相同的忙等延遲函式：`TX-2.C` 的 `delay10us`/`NJWDelay`/`ShortDelay`、`Eep24C04.c` 的 `I2CSS_Wait`、`IOEXP6524.c` 的 `ioCSS_Wait`。無統一延遲工具模組。

**推論**：上述集中化與重複實作，會讓任何跨模組修改（例如更換顯示晶片、更換 EEPROM 型號）需要同步改動多處分散程式碼。

## 共享狀態指標

跨模組/跨中斷共享的關鍵變數（`gc_Task`、`UART_STATUS`、`Custom_Code` 等）完整清單、讀寫方、風險說明見 `docs/shared_state.md`，本文件不重複列出。
