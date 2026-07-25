# Open Questions

全專案「待確認」事項的唯一索引。每項附一句話描述與指回原始章節，不在此展開細節。

## 硬體/暫存器相關

1. `REG_MPC82G516.H`（所有 `.c` 檔案 `#include`）在 repo 內不存在，其確切內容/版本無法確認；與 `.uvproj` 登記的 `REG82GXX.H` 是否為同一檔案，未能確認。→ 見 `build.md`
2. MPC82G516A 實體 Flash 容量是否確實只有 `.uvproj` 宣告的 16KB，或宣告值已過時（現行 Code Size 已達 18,088 bytes）。→ 見 `build.md`
3. MPC82G516A 是否具備 ADC 硬體、專案是否使用感測器輸入——本次分析範圍內未發現任何 ADC 相關程式碼。→ 見 `project_map.md` 模組總覽表

## 建置相關

4. `STARTUP.A51` 現行是否被 Keil 內建預設 startup 模組取代，或僅為專案檔案清單遺漏所致。→ 見 `build.md`
5. 舊版 `.lnp`（`TX-2_20220208_xxxTBD.lnp`）含明確 `CLASSES` 指令，最新版 `.lnp` 卻無此指令，是否為刻意簡化或設定遺失。→ 見 `build.md`

## 程式碼相關

6. `T1_int`（`TX-2.C`）在 Timer1 已切換為 UART 鮑率自動重載模式後，是否仍有實質功能，或為遺留冗餘 ISR。→ 見 `interrupts.md`
7. `PWM.C` 全檔為 `#if 0` 停用狀態，是否為刻意保留之未來功能。→ 見 `project_map.md`
8. `CT_7302_WriteTwoByte`/`CT_7302_ReadOneByte`（宣告於 `Eep24C04.c`，實作被整段註解）是否仍被任何呼叫端引用；若有引用將導致連結錯誤。→ 見 `modules/storage.md`
9. IR 解碼共享變數（`Custom_Code`,`Data_Code`,`NewKeyFlag`）缺乏雙緩衝之競態風險，是否曾在真實硬體上實際發生過（現場故障紀錄本次分析未取得）。→ 見 `shared_state.md`

## 文件/專案管理相關

10. `docs/architecture.md`（分析開始前為空檔、未被 git 追蹤）與 `docs/prompt.txt` 的原始撰寫意圖，是否為先前分析工作的佔位檔。
