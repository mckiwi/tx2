# Module: control_logic

檔案：`TX-2.C`（主要）、`NJW1159.C`（音量晶片控制，併入本模組）

## 職責

**已證實**：將使用者輸入事件（按鍵/遙控/RS232 命令）轉譯為具體硬體輸出動作（LED、繼電器、音量晶片指令、IO expander 位元組）。這是全專案函式數量最多、涵蓋業務範圍最廣的模組。

## 功能分類與代表函式（僅列函式名稱，不展開程式碼）

| 分類 | 代表函式（檔案：`TX-2.C`，除特別標註外） |
|---|---|
| 音量控制 | `Music_Volume_UP_Action`,`Music_Volume_DN_Action`,`Mic_Volume_UP_Action`,`Mic_Volume_DN_Action`,`Music_Volume_Action`,`Mic_Volume_Action` |
| 靜音 | `Mute_Action` |
| 輸入源切換 | `INPUT_Action` |
| 喇叭切換 | `SP_Action` |
| 音效功能 | `ECHO_Action`,`STECHO_Action`,`LOUD_Action`,`HICUT_Action`,`LOWCUT_Action`,`ANTI_Action`,`REV_Action` |
| 其他功能 | `ASONG_Action`,`WUTA_Action`,`VR_Stop` |
| IO 提交 | `PCLK_Action` |
| 聲道切換晶片控制 | `CT7302_ChSel_Action`,`CT7302_Init`,`SPDIF_Select_Action` |
| 出廠預設/校正 | `Call_default_Init`,`Set_Power_ON_VR_Preset_Action`,`Limit_VR_Set_Action`,`Restore_Preset_Function`,`DIRECTPower_Preset_Function`,`INPUT_Preset_Function` |

## 音量晶片控制（NJW1159.C，併入本模組）

**已證實**：檔案 `NJW1159.C` 提供函式 `NJW1159SetData`,`NJW1159VolumeControlSet`,`NJW1159Initial`，透過位元序列（bit-bang）控制 NJW1159 音量晶片。僅被本模組的音量 Action 函式呼叫，職責單純且範圍小，故不獨立成檔。

## 前景消費函式（IR 解碼結果）

**已證實**：`Remote_Scan`,`Remote_Key`（皆 `TX-2.C`）讀取 ISR `EXint0`（見 `docs/interrupts.md`）解碼出的 `NewKeyFlag`/`Custom_Code`/`Data_Code`，並分派呼叫本模組的對應 Action 函式。

## 已知命名不一致（待確認）

**已證實**：原始碼中 `LOUD_Action`（`TX-2.C:92,1858`）被以兩種大小寫呼叫：`LOUD_Action`（`TX-2.C:2115,3365`）與 `Loud_Action`（`TX-2.C:2227`,`ScanKeyAction.C:159`,`userdefine.h:220` 皆使用此拼法）。只有 `LOUD_Action`（全大寫）有實際函式定義，`Loud_Action` 從未被定義。**推論**：此專案能建置成功，可能是因為 `TX-2.uvproj` 的 Lx51 連結器設定 `CaseSensitiveSymbols=0`（符號名稱不分大小寫），使兩種拼法在連結階段被視為同一符號；此因果關係待確認。

## 讀寫共享狀態

**已證實**：讀寫 `TX-2.C` 開頭宣告的絕大多數功能旗標（`SP_Function`,`LOUD_Function`,`Mute_Function`,`ECHO_Function` 等 30+ 個），完整清單見 `docs/shared_state.md`。

## 依賴

- Display（`docs/modules/display.md`）— 設定顯示旗標
- Storage（`docs/modules/storage.md`）— 讀取 EEPROM 設定值
- IO Expander（`docs/modules/io_expander.md`）— `PCLK_Action` 提交輸出
- Key Input（`docs/modules/key_input.md`）— `Limit_VR_Set_Action`,`Limit_VR_Set`,`Set_Power_ON_VR_Preset`（皆 `TX-2.C`）呼叫 `GetFirstRotateKeyData`（`FastKey.c`）
- NJW1159.C（本模組內）

## 被使用

- State Machine（`docs/modules/state_machine.md`）— `Power_ON_Init`（`TX-2.C:3520`）呼叫 `Call_default_Init`，並非由 `main` 直接呼叫；狀態轉換時亦呼叫大量 Action 函式
- Communication Protocol（`docs/modules/communication_protocol.md`）— RS232 命令觸發對應動作
- Key Input（`docs/modules/key_input.md`）— 按鍵/旋鈕觸發對應動作
- 本模組內部自呼叫 —— `Limit_VR_Set_Action`（`TX-2.C:2605`）,`Limit_VR_Set`（`TX-2.C:2707`）,`Set_Power_ON_VR_Preset`（`TX-2.C:2894`）皆各自呼叫 `Call_default_Init`

## 已證實風險

- **已證實**：與 Application、State Machine 同置於單一 `TX-2.C`，無獨立檔案邊界。**推論**：因此形成高度耦合，任何跨功能修改都需注意連帶影響。
- **已證實**：建置紀錄證實 `Main_Function_Loop`（`docs/modules/main.md`）被本模組多個函式（`Restore_Preset_Function`,`Cancel_Remote_Key`,`Limit_VR_Set_Action`,`Set_Power_ON_VR_Preset_Action`,`DIRECTPower_Preset_Function`,`INPUT_Preset_Function`）遞迴呼叫，詳見 `docs/build.md`。
