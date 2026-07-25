# Development Workflow

本文件說明 TX2 專案的建議開發流程，供後續 AI Agent（Claude、Qwen、OpenCode）與開發者快速上手。內容彙整自本 session 已完成的分析與既有 `docs/*.md`，不重新分析專案、不重複 `architecture.md` 的架構細節。

## 1. Project Overview

- **專案定位**（推論）：TX-2 是一套執行於 8051 相容 MCU 上的 KTV/音響擴大機控制主機板韌體。完整說明見 `docs/project_map.md`。
- **MCU**：MPC82G516A（Megawin，8051 相容核心）。細節見 `docs/build.md`。
- **Compiler**：Keil C51（PK51 Prof. Developers Kit v9.60.7.0），Memory Model = Large。細節見 `docs/build.md`。
- **IDE**：µVision V5.38.0.0。
- **Build System**：Keil `.uvproj` 專案檔，單一 Source Group（8 個檔案），無 Makefile、無 CI 腳本。已證實的建置風險（`STARTUP.A51` 未參與現行建置、宣告 ROM 容量與實際連結行為不符等）見 `docs/build.md`。

## 2. Recommended Reading Order

新加入的人或 AI Agent 建議依序閱讀：

1. `AGENTS.md`（若存在）
2. `docs/project_map.md`
3. `docs/architecture.md`
4. `docs/build.md`
5. `docs/interrupts.md`
6. `docs/shared_state.md`
7. 對應的 `docs/modules/*.md`（依任務決定讀哪一份，見 `project_map.md` 的文件索引表）
8. 最後才閱讀原始碼（`.c/.C/.h/.H/.A51`）

**原則**：前 7 步足以回答「這段程式碼在做什麼、被誰呼叫、有哪些已知風險」，只有需要逐行邏輯或修改程式碼時才進入第 8 步。

## 3. 修改流程

```
理解需求
↓
閱讀 project_map.md（找到相關模組）
↓
閱讀相關 module 文件（docs/modules/*.md）
↓
修改對應 .c/.h
↓
Keil Build
↓
修正 Warning/Error
↓
同步更新 docs
```

補充說明：
- 「閱讀相關 module 文件」時，若任務涉及全域變數，另外查閱 `docs/shared_state.md`；若涉及中斷，另外查閱 `docs/interrupts.md`。
- 「Keil Build」需在原始 Windows + Keil C51 環境執行，本 repo 內無法脫離該環境重現建置（見 `docs/build.md` Build Risks）。
- 「修正 Warning/Error」時，若出現 `L48 IGNORED RECURSIVE CALL` 之類的既有警告（見 `docs/build.md`），先確認是否為已知的既有遞迴呼叫模式，不要在未理解全貌前貿然「修正」。

## 4. 修改原則

- 一次只修改必要模組，避免跨模組大規模修改。
- 不要修改與任務無關的程式碼。
- 保持既有 Coding Style（見 `AGENTS.md` 的 Coding Rules）。
- 優先維持既有行為一致，尤其是狀態機轉換（`gc_Task`）與中斷時序相關程式碼。
- Architecture 變更（新增模組、改變模組依賴關係）需同步更新 `docs/architecture.md` 與相關 `docs/modules/*.md`。

## 5. Debug Workflow

```
Compile
↓
Warning（比對 docs/build.md 已知警告清單，判斷是否為新問題）
↓
Map File（.MAP，檢查記憶體配置與呼叫樹，尤其遞迴/堆疊相關疑慮）
↓
Runtime（實機或模擬器行為觀察）
↓
Hardware Verification（實體電源鍵/遙控/RS232/EEPROM 等實測）
```

**TODO**：本 session 未取得任何實機除錯紀錄或模擬器操作步驟，上述流程之細節（如使用哪個模擬器、如何連接實機除錯）待確認。

## 6. Documentation Rules

以下情況**必須**同步更新對應文件，不得只改程式碼：

| 變更類型 | 需更新的文件 |
|---|---|
| 新增模組 | `docs/project_map.md`（模組總覽表、文件索引）、`docs/architecture.md`（依賴圖）、新增對應 `docs/modules/*.md` |
| 修改 State Machine | `docs/modules/state_machine.md` |
| 修改 ISR | `docs/interrupts.md`，若牽動共享變數也需更新 `docs/shared_state.md` |
| 修改 Shared State（新增/移除全域變數） | `docs/shared_state.md`，並檢查是否有其他文件引用該變數需連動更新 |
| 修改 Build 設定（`.uvproj`/`.uvopt`/連結參數） | `docs/build.md` |
| 修改初始化流程（`main`/`Power_ON_Init` 等） | `docs/modules/main.md`（若牽動狀態轉換也需更新 `docs/modules/state_machine.md`） |
| 修改 Module Dependency（新增/移除模組間呼叫） | 對應 `docs/modules/*.md` 的「依賴」與「被使用」段落**須雙向同步**，避免重蹈先前稽核發現的依賴關係不對稱問題 |

## 7. AI Collaboration Guide

**Claude**：
- 架構分析
- Code Review
- Project Analysis
- Documentation

**Qwen**：
- 小功能開發
- Bug Fix
- Build Error 排除
- 局部修改

## 8. Future Improvements

依據 `docs/open_questions.md` 彙整目前已知可改善方向（詳細內容見該文件，此處不重複展開）：

- 確認 `REG_MPC82G516.H`/`REG82GXX.H` 的實際關係與來源，補齊建置環境依賴。
- 確認 MPC82G516A 實體 Flash 容量，核實現行 Code Size 是否真的超出硬體上限。
- 釐清 `STARTUP.A51` 現行是否被取代，若確認無用可考慮移除或明確標註為歷史檔案。
- 評估 `T1_int` 是否為冗餘 ISR，若確認無用可考慮移除以降低不必要的中斷負擔。
- 釐清 `PWM.C` 停用狀態是否為刻意保留，決定是否移除死碼。
- 確認 `CT_7302_WriteTwoByte`/`CT_7302_ReadOneByte` 是否仍有呼叫端，避免潛在連結風險。
- 評估是否為 IR 解碼共享變數（`Custom_Code`/`Data_Code`/`NewKeyFlag`）與 UART 緩衝加上雙緩衝機制，降低競態風險。
- 補齊建置流程文件（如 `.lnp` CLASSES 設定歷史差異的原因）。

**TODO**：以上皆為既有 `open_questions.md` 項目的改善方向摘要，尚未有實際修復排期或負責人資訊，待專案維護者補充。
