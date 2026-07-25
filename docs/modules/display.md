# Module: display

檔案：`TX-2.C`

## 職責

**已證實**：7-段顯示器移位暫存器驅動（多工掃描顯示音樂/麥克風音量數字）、電源狀態 LED（紅/藍/綠）閃爍時序控制。

## 代表函式（僅列名稱）

| 函式 | 用途 |
|---|---|
| `Display_7SEG_init` | 初始化 7-段顯示器相關腳位 |
| `Music_Display_7SEG`,`Mic_Display_7SEG` | 對應聲道的顯示驅動（移位暫存器 CLK/LATCH 操作） |
| `ClearAllLED` | 關閉所有 LED/顯示輸出，並重置 `gc_Task`（見 `docs/modules/state_machine.md`） |

**已證實**：`TX-2.C` 中存在兩份 `Music_Display_7SEG` 定義，其中一份以 `#if 0`/`#endif` 整段停用（舊版實作），實際生效版本為未被停用的另一份。此為確認過的死碼保留，非重複定義錯誤。

## 驅動來源

**已證實**：顯示刷新（多工掃描）與 LED 閃爍完全由 Timer2 中斷鏈驅動，非前景輪詢。ISR 機制見 `docs/interrupts.md`（`T2_int`）。

## 讀寫共享狀態

`Music_Display_Flag`,`Mic_Display_Flag`,`Music/Mic_Character_tens/units`,`Music/Mic_Scan_Display_counter`,`Power_*LED_Flash_Flag` — 完整說明見 `docs/shared_state.md`。

## 依賴

- Interrupt（`docs/interrupts.md`）— 由 `T2_int` 排程鏈觸發
- IO Expander（`docs/modules/io_expander.md`）— `PCLK_Action` 提交 LED bank

## 被使用

- Control Logic（`docs/modules/control_logic.md`）— 音量動作設定顯示旗標後由本模組渲染
- main（`docs/modules/main.md`）— 開機時呼叫 `Display_7SEG_init`/`ClearAllLED`

## 已證實風險

- **已證實**：顯示刷新邏輯完全內嵌於 Timer2 中斷鏈，中斷層與顯示層職責無明確邊界（見 `docs/interrupts.md` 中 `T2_int` 風險說明）。
- **推論**：因此任何顯示邏輯修改都需一併評估對中斷執行時間預算的影響，惟實際是否曾造成節拍延遲未經量測驗證。
