# Build

MCU、Toolchain、Memory Model、Source Group 設定，以及已證實的建置風險。

## Target MCU

| 項目 | 值 |
|---|---|
| Device | MPC82G516A（Megawin，DeviceId 4374） |
| CPU 記憶體宣告 | `IRAM(0-0xFF) XRAM(0x0-0x3FF) IROM(0-0x3FFF) CLOCK(45000000)` |
| 暫存器檔（Keil 專案登記） | `REG82GXX.H`，路徑 `Megawin\` |

**待確認**：原始碼所有 `.c` 檔案 `#include "REG_MPC82G516.H"`，此檔名與專案登記的 `REG82GXX.H` 不同，且皆不存在於本 repo，無法確認兩者關係與確切來源。

## Toolchain

| 項目 | 值 |
|---|---|
| IDE | µVision V5.38.0.0 |
| 工具包 | PK51 Prof. Developers Kit V9.60.7.0 |
| C Compiler | C51.exe V9.60.7.0 |
| Assembler | A51.exe V8.2.7.0 |
| Linker/Locator | LX51.exe V4.66.100.0 |
| 工具鏈路徑 | `C:\Keil_v5\C51\BIN`（Windows 絕對路徑） |

## Memory Model

| 欄位 | 值 |
|---|---|
| MemoryModel | 2（**Large**） |
| Reentrant Stack | 全部停用（`IBPSTACK/XBPSTACK/PBPSTACK=0`，`STARTUP.A51`） |
| DataOverlaying | 1（啟用） |

## Source Group

單一 Group「TX-2」，含 8 個檔案：`TX-2.C`,`FastKey.c`,`ScanKeyAction.C`,`PWM.C`,`RS232.C`,`Eep24C04.c`,`NJW1159.C`,`IOEXP6524.c`。

**已證實**：`STARTUP.A51` **不在**此 Group 清單內，也不在任何 `.lnp`／`.MAP` 的實際 LX51 連結指令中出現。

## Include Path / Defines

**已證實**：Include Path 與前置巨集（Define）在專案層級皆為空，無任何自訂設定。所有功能開關為原始碼內硬編碼（如 `#if 0`）。

## Build Output

| 項目 | 值 |
|---|---|
| 最新輸出檔名 | `TX-2_20241007-IOEXP_MCUBD` |
| 最新 Code Size | 18,088 bytes |
| 建置結果 | 0 Error(s), 8 Warning(s) |

歷史 Code Size 趨勢：15,386（2022/02）→ 16,731（2022/04）→ 16,704（2022/08）→ 18,389（2024/09）→ 18,088（現行，2024/10）。

## Build Risks（已證實）

1. **STARTUP.A51 孤兒化**：存在於 repo 且被 git 追蹤，但目前 `.uvproj` 未列入，過去 5 次留存的 `.lnp`/`.MAP` 連結指令均未包含 `STARTUP.obj`。其 `.OBJ/.LST` 時間戳（2013）與其餘模組（2025）明顯不同步，代表**現行建置不使用此檔案**。
2. **宣告的 ROM 容量與實際連結行為不符**：`.uvproj` 宣告 `IROM(0-0x3FFF)`（16,384 bytes），但最新 Code Size（18,088 bytes）已超過此上限；且實際 LX51 記憶體類別配置使用全 64KB CODE 範圍，未受宣告限制。自 2022/04 起 Code Size 即已持續超過此宣告值。
3. **遞迴呼叫警告**：建置紀錄含 8 項 `WARNING L48: IGNORED RECURSIVE CALL`，實際為兩組獨立遞迴鏈：(a) `Main_Function_Loop`（`TX-2.C`，見 `docs/modules/control_logic.md`）被 6 個 Control Logic 函式遞迴呼叫；(b) `Sdataout`（`ScanKeyAction.C`，見 `docs/modules/key_input.md`）被 `SKey_Limit_OK_Switch`/`SKey_Preset_OK_Switch` 遞迴呼叫（2 項）。Linker 已將兩組路徑皆自堆疊靜態分析中剔除，**實際堆疊峰值未被建置工具量化驗證**，而專案又未啟用可重入堆疊（見 Memory Model）。
4. **環境路徑依賴**：工具鏈與原始專案路徑皆為 Windows 專屬絕對路徑（`C:\Keil_v5\C51\BIN`,`C:\Workspace\TX2\...`），與本 git repo 路徑無關，**無法脫離原始開發機環境重現建置**。
5. **CLOCK 設定不一致**：`.uvproj` 宣告 `CLOCK(45000000)`（45MHz，僅供模擬器使用），與原始碼 `userdefine.h` 內 `Xtal_Freq 18432000`（18.432MHz，實際用於鮑率/計時計算）不符。
6. **`.gitignore` 規則對既有追蹤檔案不生效**：`.gitignore` 排除 `*.LST/*.OBJ/*.hex` 等副檔名，但這些檔案早已被 git 追蹤，規則未實際生效，導致大量建置產物被提交入版本庫。

## 待確認

- MPC82G516A 實體 Flash 容量是否確實只有 16KB，或專案宣告值已過時。
- `REG_MPC82G516.H` 與 `REG82GXX.H` 是否為同一份檔案。
- `STARTUP.A51` 現行是否由 Keil 內建預設 startup 模組取代，或僅為專案檔案清單遺漏。
