# Module: communication_protocol

檔案：`RS232.C`（傳輸層）、`TX-2.C`（協定語意層）

## 職責

**已證實**：本模組合併「UART 硬體傳輸」與「ZSound RS232 命令協定」兩層職責，因兩者在原始碼中即混合於同一物理檔案（`RS232.C`）與呼叫關係中，未有清楚邊界。

## 傳輸層（RS232.C）

| 函式 | 用途 |
|---|---|
| `UART_init` | 串列埠硬體初始化（Mode 1，Timer1 產生鮑率） |
| `UART_Send` | 阻塞式位元組發送 |
| `UART_Variable_Reset` | 清空接收緩衝與狀態 |
| `UART_Interrupt` | ISR，逐位元組組裝 5-byte 命令幀（詳見 `docs/interrupts.md`，本文件不重複） |

## 協定語意層

| 函式 | 檔案 | 用途 |
|---|---|---|
| `UART_main` | `RS232.C` | 將已組裝完成的緩衝區拆解為 `RS232_First/RW/Addr/Data/Last_Code` |
| `RS232_Write_Data_Action` | `TX-2.C` | 處理寫入類命令（音量/聲道/電源等） |
| `RS232_Read_Data_Action` | `TX-2.C` | 處理讀取類命令 |

**已證實**：協定幀格式為固定 5-byte（Leader/RW/Addr/Data/End），常數定義於 `userdefine.h`（如 `RS232_Leader_Code`,`RS232_Write`,`RS232_Read`,`RS232_End_Code`）。

## 與前景的資料交換

**已證實**：ISR 組幀完成後設定 `UART_STATUS=2`，由 State Machine（`Main_Loop`/`PowerONDetect`，見 `docs/modules/state_machine.md`）輪詢後呼叫 `UART_main`，再依 `RS232_RW_Code` 分派至 `RS232_Write_Data_Action` 或 `RS232_Read_Data_Action`。

## 依賴

- Interrupt（`docs/interrupts.md`）— `UART_Interrupt` ISR
- Control Logic（`docs/modules/control_logic.md`）— 命令效果實際執行
- Storage（`docs/modules/storage.md`）— 部分命令持久化到 EEPROM

## 已證實風險

- `UART_Interrupt` 未處理 `TI`，發送時會觸發多餘中斷（詳見 `docs/interrupts.md`）。
- 協定解析（`UART_main`）位於傳輸層檔案 `RS232.C`，命令語意處理（`RS232_*_Data_Action`）卻位於 `TX-2.C`，兩者無共同標頭/模組邊界。
- `UART_RX_BUF`/`UART_STATUS` 缺乏雙緩衝，詳見 `docs/shared_state.md`。
