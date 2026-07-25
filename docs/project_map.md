# TX2 Project Map

本文件是整個文件集的入口。若你只能讀一份文件，讀這份，再依需求跳轉到對應文件。

## 專案一句話定位

**推論**：TX-2 是一套執行於 8051 相容 MCU（MPC82G516A）上的 KTV/音響擴大機控制主機板韌體，負責音樂/麥克風雙聲道音量控制、多組音效切換（ECHO/LOUD/HICUT/LOWCUT/ANTI/REV）、輸入源選擇、喇叭切換、7-段顯示器與狀態 LED 顯示、紅外線遙控與 RS232（ZSound 協定）雙輸入通道、EEPROM 設定持久化。
（依據：`TX-2.C` 標頭註解、功能函式命名、`userdefine.h` 輸入源常數。無正式需求文件佐證，故列為推論。）

## 文件索引

| 文件 | 內容 | 何時該讀 |
|---|---|---|
| `docs/architecture.md` | 高層資料流、模組依賴圖 | 想了解「系統整體怎麼串起來」 |
| `docs/interrupts.md` | 5 個中斷服務函式完整清單 | 想了解硬體中斷相關行為 |
| `docs/shared_state.md` | 跨模組/跨中斷共享全域變數總表 | 想確認某個全域變數被誰讀寫 |
| `docs/build.md` | MCU/Toolchain/Memory Model/建置風險 | 想了解如何編譯、或編譯是否可重現 |
| `docs/open_questions.md` | 全專案待確認事項索引 | 想知道哪些結論還沒有100%把握 |
| `docs/modules/main.md` | 進入點、初始化流程、主迴圈殼層 | 想了解開機流程 |
| `docs/modules/state_machine.md` | `gc_Task` 三態機、IR 解碼狀態機 | 想了解執行期狀態如何切換 |
| `docs/modules/control_logic.md` | 音量/音效/輸入源等功能動作函式 | 想了解某個按鍵/遙控按鍵做了什麼 |
| `docs/modules/display.md` | 7-段顯示器、狀態 LED 閃爍 | 想了解顯示相關邏輯 |
| `docs/modules/communication_protocol.md` | RS232 傳輸與 ZSound 協定 | 想了解序列埠通訊協定 |
| `docs/modules/storage.md` | EEPROM 讀寫 | 想了解設定值如何持久化 |
| `docs/modules/io_expander.md` | PCAL6524 IO 擴充晶片驅動 | 想了解 LED/繼電器輸出如何透過 IO expander 提交 |
| `docs/modules/key_input.md` | 旋鈕音量掃描、矩陣鍵盤掃描 | 想了解實體按鍵輸入如何被讀取 |

## 模組總覽表

| 模組 | 對應檔案 | 狀態 |
|---|---|---|
| Application / Entry Point | `TX-2.C` | 已證實存在，見 `modules/main.md` |
| State Machine | `TX-2.C` | 已證實存在，見 `modules/state_machine.md` |
| Control Logic | `TX-2.C`、`NJW1159.C` | 已證實存在，見 `modules/control_logic.md` |
| Display / UI | `TX-2.C` | 已證實存在，見 `modules/display.md` |
| Communication Protocol | `RS232.C`、`TX-2.C` | 已證實存在，見 `modules/communication_protocol.md` |
| Storage | `Eep24C04.c` | 已證實存在，見 `modules/storage.md` |
| IO Expander | `IOEXP6524.c` | 已證實存在，見 `modules/io_expander.md` |
| Key Input | `FastKey.c`、`ScanKeyAction.C` | 已證實存在，見 `modules/key_input.md` |
| Interrupt | `TX-2.C`、`RS232.C` | 已證實存在，見 `interrupts.md` |
| Board / Hardware Definition | `userdefine.h`、`MPC82.H` | 已證實存在（共用標頭，無獨立模組文件），見 `architecture.md` |
| PWM | `PWM.C` | 已證實：全檔為 `#if 0` 停用狀態，非功能性模組 |
| Sensor / ADC | — | 未發現任何相關程式碼 |

## Toolchain 一句話摘要

Keil C51（PK51 v9.60.7.0），MCU=MPC82G516A（Megawin），Memory Model=Large。細節見 `docs/build.md`。
