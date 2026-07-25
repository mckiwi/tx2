# Module: state_machine

檔案：`TX-2.C`

本文件涵蓋兩個狀態機：主任務狀態機（`gc_Task`）與紅外線解碼狀態機（`IR_STATE`）。ISR 層的硬體機制見 `docs/interrupts.md`，此處只說明狀態語意與轉換規則。

## 主任務狀態機（gc_Task）

**已證實**：`Main_Function_Loop`（檔案 `TX-2.C`）依 `gc_Task` 值分派：

| `gc_Task` | 對應函式 | 職責 |
|---|---|---|
| 0 | `PowerONDetect`（`TX-2.C`） | 待機，輪詢遙控/RS232/實體電源鍵，成立後設 `gc_Task=1` |
| 1 | `ClearAllLED`,`STB_ON_Action`,`Power_ON_Init`（皆 `TX-2.C`） | 開機初始化序列，`Power_ON_Init` 結尾設 `gc_Task=2` |
| 2 | `Main_Loop`（`TX-2.C`） | 運轉中，內含自身 `while(1)`，每輪呼叫 `PowerKeyDetect`（`TX-2.C:3684`，實體電源鍵長按去彈跳，成立後設 `gc_poweroff=1`）、`ScanKeySwitch`、`Scan_Volume`、`Remote_Scan`；`gc_poweroff` 觸發時呼叫 `ClearAllLED`（把 `gc_Task` 設回 0）後跳出返回狀態 0 |

**已證實風險**：`gc_Task` 在 `TX-2.C` 內超過 10 處被寫入（分散於 State Machine 與多個 Control Logic 函式）。`ClearAllLED` 內部會把 `gc_Task` 重設為 0，但在狀態 1 分支中隨即被 `Power_ON_Init` 覆寫為 2——最終狀態正確與否取決於函式執行順序，而非顯式防呆設計。

**推論**：若日後修改 `case 1` 分支內三個函式的呼叫順序，可能直接破壞狀態機正確性。

## 紅外線解碼狀態機（IR_STATE）

**已證實**：由 ISR `EXint0`（檔案 `TX-2.C`）驅動，狀態依序為：

```
WAITING_STATE → WAIT_NEXT_INT → GET_CUSTOM → GET_CUSTOM_BAR → GET_DATA → GET_DATA_BAR → (完成，Clear_State 重置)
```

- 函式 `Clear_State`（`TX-2.C`）：將 `IR_STATE` 重置為 `WAITING_STATE`
- 函式 `Get_Code`（`TX-2.C`）：逐位元組裝資料
- 完整命令組裝完畢後設定 `NewKeyFlag=1`，若比對到電源鍵碼則設定 `gc_IrPowerOnOff=1`

**已證實**：完成一組命令後立即重置回 `WAITING_STATE`，且無雙緩衝機制保護 `Custom_Code`/`Data_Code`/`NewKeyFlag`。**推論**：若使用者連續按鍵，下一組解碼可能在前景（`Remote_Key`，`TX-2.C`）尚未讀完前就開始覆寫（此後果未經實際執行驗證，僅由程式碼結構推導）。完整變數清單見 `docs/shared_state.md`。

## 依賴

- Interrupt（`docs/interrupts.md`）— `EXint0` 驅動 IR 狀態機
- Communication Protocol（`docs/modules/communication_protocol.md`）— RS232 命令可觸發 `gc_Task` 轉換
- Control Logic（`docs/modules/control_logic.md`）— 開機初始化呼叫大量 Action 函式
- Key Input（`docs/modules/key_input.md`）— `Main_Loop` 每輪直接呼叫 `ScanKeySwitch`（`ScanKeyAction.C`）

## 待確認

- 是否有更完整的狀態轉換圖或設計文件可交叉驗證目前歸納的三態語意，本次僅由程式碼行為反推。
