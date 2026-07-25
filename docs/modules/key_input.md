# Module: key_input

檔案：`FastKey.c`（旋鈕音量掃描）、`ScanKeyAction.C`（矩陣鍵盤掃描）

兩者皆為「實體輸入掃描」職責，合併於本文件。

## FastKey.c — 旋鈕音量掃描

**已證實**：正交編碼式旋鈕輸入去彈跳與方向判斷，非 ADC，純數位 GPIO 邊緣偵測。

| 函式 | 用途 |
|---|---|
| `GetFirstRotateKeyData` | 初始化/重置旋鈕掃描狀態 |
| `MainVolumeGetRotateKey`,`MicVolumeGetRotateKey` | 讀取音樂/麥克風旋鈕目前狀態 |
| `MainRotateKeyTrigger`,`MicRotateKeyTrigger` | 觸發去彈跳計時 |
| `MainMicRotateKeyProcess` | 判斷旋轉方向並分派 |
| `RightRotary`,`LeftRotary` | 呼叫 Control Logic 對應音量增減函式 |

## ScanKeyAction.C — 矩陣鍵盤掃描

**已證實**：ROW/COL 矩陣式按鍵掃描，另含隱藏校正/預設功能鍵（透過 `Sdataout` 分派）。

| 函式 | 用途 |
|---|---|
| `ScanKeySwitch` | 一般功能鍵掃描（輸入源/音效切換等） |
| `dataout` | 一般按鍵分派至對應 Control Logic 動作 |
| `Get_SKey` | 隱藏校正/預設鍵掃描，於開機流程中無條件執行一次 |
| `Sdataout` | 校正/預設鍵分派 |

**已證實**：`Get_SKey`（`ScanKeyAction.C:72`）本身的呼叫（`TX-2.C:3423`，於 `Power_ON_Init` 內）沒有額外條件限制。但其分派的 `Sdataout`（`ScanKeyAction.C`）內，僅 case 6（`DIRECTPower_Preset_Function`）與 case 7（`INPUT_Preset_Function`）兩個分支被 `fromSTB_SW==1` 條件限制，並非整個 `Get_SKey` 掃描都受電源鍵觸發限制——先前版本文件的描述過度概括，已修正。

## 依賴

- Control Logic（`docs/modules/control_logic.md`）— 掃描結果觸發對應功能動作

## 被使用

- State Machine（`docs/modules/state_machine.md`）— `Main_Loop`（`TX-2.C:3917`）每輪呼叫 `ScanKeySwitch`，並在進入運轉狀態前呼叫 `GetFirstRotateKeyData`；`Power_ON_Init`（`TX-2.C:3423`）呼叫 `Get_SKey`
- Control Logic（`docs/modules/control_logic.md`）— `Limit_VR_Set_Action`,`Limit_VR_Set`,`Set_Power_ON_VR_Preset`（皆 `TX-2.C`）亦各自呼叫 `GetFirstRotateKeyData`

**修正說明**：先前版本誤將 `Get_SKey`/`GetFirstRotateKeyData` 標示為由 `main` 直接呼叫；經 `grep` 核對呼叫點（`TX-2.C:2626,2820,3013,3423,3917`），皆位於 State Machine 或 Control Logic 函式內部，並非 `main()` 直接呼叫。

## 已證實風險

- 建置紀錄證實 `Sdataout`（`ScanKeyAction.C`）被 `SKey_Limit_OK_Switch`、`SKey_Preset_OK_Switch` 遞迴呼叫（linker `WARNING L48`），詳見 `docs/build.md`。
