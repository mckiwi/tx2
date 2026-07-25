# AGENTS.md

給 AI Agent（OpenCode、Claude、Qwen 等）的專案速覽。**請先讀完本文件**，再依任務需求決定是否要讀 `docs/` 底下的細節文件。本文件不重複 `docs/` 的詳細內容，只做定位與導覽。

## 專案目的

**推論**：TX-2 是執行於 8051 相容 MCU 上的 KTV/音響擴大機控制主機板韌體，負責雙聲道（Music/Mic）音量控制、多組音效切換、輸入源/喇叭切換、7-段顯示器與狀態 LED、紅外線遙控與 RS232（ZSound 協定）雙輸入通道、EEPROM 設定持久化。
無正式需求文件佐證，細節見 `docs/project_map.md`。

## MCU

- 型號：**MPC82G516A**（Megawin，8051 相容核心）
- 記憶體：IRAM 256B、XRAM 1KB、IROM 宣告 16KB（**已證實**現行 code size 已超過此宣告值，見 `docs/build.md`）
- 完整細節：`docs/build.md`

## Compiler / Toolchain

- **Keil C51**（PK51 Prof. Developers Kit v9.60.7.0），非 GCC/Clang，非 ARM 工具鏈
- Memory Model：**Large**
- 可重入堆疊（reentrant stack）**全部停用**——這對 AI Agent 修改程式碼有直接影響，見下方「Memory Rules」
- Include Path / Define 皆為空，不要假設有任何前置巨集可用
- 完整細節：`docs/build.md`

## Coding Rules（既有慣例，修改時應遵循）

- 全專案**沒有依模組拆分的標頭檔**，除 `MPC82.H` 外，所有全域宣告集中於 `userdefine.h`。新增全域變數/函式原型請沿用此慣例，不要自行建立新標頭檔。
- 硬體腳位一律透過 `userdefine.h` 的 `sbit` 巨集存取，不要在其他檔案重新定義腳位。
- 功能開關使用 `#if 0`/`#if 1` 硬編碼於原始碼內，**沒有**透過編譯器 `-D` 巨集控制（Include Path/Define 皆為空）。
- 已知連結器行為：`TX-2.uvproj` 設定 `CaseSensitiveSymbols=0`，原始碼中已存在 `LOUD_Action`/`Loud_Action` 大小寫不一致但仍可連結成功的案例（見 `docs/modules/control_logic.md`）。**修改函式名稱時務必確認大小寫在所有呼叫端一致**，不要依賴連結器的大小寫容忍。
- 詳細模組職責與函式清單見 `docs/modules/*.md`，不要直接讀 4491 行的 `TX-2.C` 找函式，先查對應的 `docs/modules/*.md`。

## Memory Rules

- **可重入堆疊已停用**（`IBPSTACK/XBPSTACK/PBPSTACK=0`）。已證實專案中存在遞迴呼叫（`Main_Function_Loop`、`Sdataout`，見 `docs/build.md`），linker 僅發出警告並移除該路徑的堆疊分析，**未驗證實際堆疊峰值**。
- **新增程式碼時絕對不要引入新的遞迴呼叫**，8051 IRAM 僅 256 bytes，沒有安全網。
- 避免新增大型區域變數（無 XRAM 動態配置機制，靜態配置為主）。
- 中斷服務函式（ISR）應保持精簡：`T2_int`（`TX-2.C`）已被證實承擔過多業務邏輯（詳見 `docs/interrupts.md`），**不要再往任何 ISR 內增加邏輯**，新功能應放在前景輪詢。
- 5 個 ISR 目前都沒有 Keil `using n` 暫存器庫宣告，修改 ISR 前請先讀 `docs/interrupts.md`。

## File Structure

```
/                       # 原始碼皆位於根目錄（無 src/ 子目錄）
├── TX-2.C              # 最大檔案（4491 行），承擔 Application/State Machine/Control Logic/Display
├── RS232.C             # UART 傳輸 + RS232 協定
├── Eep24C04.c          # EEPROM 驅動
├── IOEXP6524.c         # IO 擴充晶片驅動
├── NJW1159.C           # 音量晶片驅動
├── FastKey.c           # 旋鈕音量掃描
├── ScanKeyAction.C     # 矩陣鍵盤掃描
├── PWM.C               # 已停用（全檔 #if 0）
├── STARTUP.A51         # Keil 標準開機碼（已證實現行建置未使用，見 docs/build.md）
├── userdefine.h        # 全域宣告/腳位巨集/常數（唯一共用標頭）
├── MPC82.H             # SFR 擴充定義
├── TX-2.uvproj/.uvopt  # Keil 專案設定
└── docs/               # 本次分析產出的完整文件（見下方索引）
    ├── project_map.md      # 文件總索引（不確定要讀哪份文件時先讀這個）
    ├── architecture.md
    ├── interrupts.md
    ├── shared_state.md
    ├── build.md
    ├── open_questions.md
    └── modules/*.md         # main, state_machine, control_logic, display,
                              # communication_protocol, storage, io_expander, key_input
```

## 開發流程

1. **先讀 `docs/project_map.md`**，找到與任務相關的模組文件。
2. 只在確認需要逐行細節時才打開對應的 `.c/.C` 原始碼，避免整份讀取 `TX-2.C`。
3. 若任務涉及全域變數，先查 `docs/shared_state.md` 確認讀寫方與已知風險。
4. 若任務涉及中斷相關程式碼，先查 `docs/interrupts.md`。
5. 若任務涉及編譯/連結設定，先查 `docs/build.md`（已記錄多項環境依賴與建置風險）。
6. 若發現任何 `docs/` 現有結論有誤或過時，**更新對應文件**，並視情況調整 `docs/open_questions.md`。

## 修改原則

- **已證實 / 推論 / 待確認必須分開標示**——這是本專案文件的既有規範（見任一 `docs/*.md`），撰寫新文件或程式註解時請延續。
- 修改 `.c/.h/.a51` 前，先確認該函式在 `docs/modules/*.md` 中的「被使用」清單，評估影響範圍。
- 不要假設 `STARTUP.A51` 會被實際建置流程使用（已證實不會，見 `docs/build.md`），修改開機流程前務必先確認。
- 不要在沒有充分理由的情況下「修正」`Loud_Action`/`LOUD_Action` 這類已知但無害的命名不一致，除非任務明確要求且會同步修正所有呼叫端。
- 涉及 IO Expander（`IOEXP6524.c`）與 Storage（`Eep24C04.c`）的修改，注意兩者各自有一套幾乎相同但獨立的軟體 I2C 原語，**不要假設修改一邊會影響另一邊**。
- 涉及 EEPROM 位址配置（`Eep24C04.c`/`userdefine.h`）的修改要特別小心，錯誤的位址偏移會直接影響出廠設定持久化。
- 任何新增的 ISR 邏輯、新增的遞迴呼叫、新增的全域變數，都必須同步更新 `docs/interrupts.md`／`docs/shared_state.md`，避免文件與程式碼再度失準。
- 本文件與 `docs/` 皆為唯讀分析產出，若程式碼有實際變更，**務必回頭更新對應文件**，保持文件與程式碼一致。
