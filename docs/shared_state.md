# Shared State

本文件是全專案跨模組／跨中斷共享全域變數的唯一權威來源。其他文件提及這些變數時只列名稱並連結回本文件，不重複解釋語意。

## 為何獨立成檔

**已證實**：
- 共享旗標數量龐大（`TX-2.C` 開頭宣告區即有 30+ 個功能旗標）。
- `gc_Task` 被 10 餘處分散寫入，橫跨 State Machine、Control Logic、Communication Protocol。
- 已具體證實兩處跨 ISR/前景競態風險（見下方風險章節）。

## 狀態機控制旗標

| 變數 | 位置 | 用途 | 讀寫方 |
|---|---|---|---|
| `gc_Task` | `TX-2.C` | 主狀態機碼：0=待機偵測,1=開機初始化,2=運轉中 | State Machine、Control Logic、Communication Protocol 多處寫入 |
| `gc_poweroff` | `TX-2.C` | 關機請求旗標 | 三處設定：State Machine `PowerKeyDetect`（`TX-2.C:3741`，實體電源鍵長按）、Control Logic `Remote_Key`（`TX-2.C:2142,2164`）、Communication Protocol `RS232_Write_Data_Action`（`TX-2.C:1984`）；State Machine（`Main_Loop`）輪詢消費 |
| `gc_IrPowerOnOff` | `TX-2.C` | IR 電源鍵事件旗標 | ISR `EXint0` 寫入，State Machine 讀取 |
| `gw_TimerBase` | `TX-2.C` | 通用忙等延遲計數器 | ISR `T2_int` 遞減，幾乎所有模組的前景忙等讀取 |

## IR 解碼共享變數（EXint0 專用）

| 變數 | 用途 |
|---|---|
| `IR_STATE` | 解碼狀態機當前狀態 |
| `Ticks` | 位元計時（由 `T0_int` 遞增） |
| `Custom_Code`,`Custom_Code_BAR`,`Data_Code`,`Data_Code_BAR` | 解碼結果 |
| `NewKeyFlag` | 完整命令就緒旗標 |

**已證實**：由 ISR `EXint0`（`TX-2.C`）寫入，前景 `Remote_Key`（`TX-2.C`）讀取消費，**無雙緩衝機制**。

**推論**：若使用者連續快速按鍵，新一輪解碼可能在前景讀完舊資料前開始覆寫；此後果為依程式碼結構推導，未經實際執行或現場故障紀錄驗證（見 `docs/open_questions.md` 第 9 項）。

## RS232 / UART 共享變數

| 變數 | 用途 |
|---|---|
| `UART_STATUS` | 0=待命,1=接收中,2=命令就緒 |
| `UART_INDEX` | 目前接收位元組索引 |
| `UART_RX_BUF[5]` | 命令幀緩衝 |
| `RS232_First/RW/Addr/Data/Last_Code` | 解析後的命令欄位（由 `UART_main` 填入，`RS232.C`） |

**已證實**：由 ISR `UART_Interrupt`（`RS232.C`）寫入，前景 `UART_main`（`RS232.C`）讀取，**無雙緩衝機制**。

**推論**：理論上新一幀可能在前景尚未讀完舊幀前開始覆寫，但實務風險較低，因協定固定 5-byte 且需 `UART_STATUS==2` 才被消費（此評估未經實測驗證）。

## 顯示 / LED 相關旗標

| 變數 | 用途 |
|---|---|
| `Music_Display_Flag`,`Mic_Display_Flag` | 是否啟用對應顯示器掃描 |
| `Music/Mic_Character_tens/units` | 待顯示的數字字元 |
| `Music/Mic_Scan_Display_counter` | 多工掃描計數器 |
| `Power_RedLED_Flash_Flag`,`Power_BlueLED_Flash_Flag`,`Power_GreenLED_Flash_Flag` | 電源狀態 LED 閃爍控制 |

讀寫方：ISR `T2_int` 排程鏈（`each1ms/55ms_event`，`TX-2.C`）驅動效果；Control Logic 各 `*_Action` 函式設定顯示內容。

## 音量與功能旗標（節錄）

**已證實**：以下變數皆宣告於 `TX-2.C` 開頭，由 Control Logic 模組的對應 `*_Action` 函式讀寫，細節見 `modules/control_logic.md`：

`SP_Function`,`LOUD_Function`,`Mute_Function`,`ECHO_Function`,`STECHO_Function`,`HICUT_Function`,`LOWCUT_Function`,`ANTI_Function`,`REV_Function`,`ASONG_Function`,`WUTA_Function`,`CH_SEL`,`BT_SPDIF_CTL`,`Music_Volume_Counter`,`Mic_Volume_Counter`,`Turbo_Volume_Counter`

（因數量多，不逐一展開語意，需要時請對照 `modules/control_logic.md` 與原始碼。）

## IO Expander 共享狀態

| 變數 | 位置 | 用途 |
|---|---|---|
| `bXIOLock` | `TX-2.C` | IO expander I2C 匯流排鎖（`volatile bit`） |
| `IOPORTS_DATBK[2][3]` | `IOEXP6524.c` | 兩顆 IO expander 的輸出資料備份 |

細節見 `modules/io_expander.md`。
