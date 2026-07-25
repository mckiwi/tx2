# Module: storage

檔案：`Eep24C04.c`（驅動）、`TX-2.C`（呼叫端）

## 職責

**已證實**：24C04 EEPROM 讀寫（軟體 bit-bang I2C），持久化使用者功能設定。同檔案內另含與儲存無關的 `CT_7302`（顯示驅動 IC）I2C 寫入輔助函式，共用同一組 I2C 原語，屬職責混合。

## 對外 API（僅列名稱）

| 函式 | 用途 |
|---|---|
| `TDF24c04WriteOneByte` | 寫入 EEPROM 單一位元組 |
| `TDF24c04ReadOneByte` | 讀取 EEPROM 單一位元組 |
| `CT_7302_WriteOneByte` | 寫入 CT7302 晶片（與 EEPROM 無關，共用 I2C 原語） |

**已證實**：`CT_7302_WriteTwoByte`、`CT_7302_ReadOneByte` 於 `userdefine.h` 宣告，但實作於 `Eep24C04.c` 整段被註解停用。是否仍有呼叫端引用未確認（見 `docs/open_questions.md`）。

## 內部原語（私有，僅列名稱）

`iicSTART`,`iicSTOP`,`iicCheckAck`,`iicWriteByte`,`iicReadByte`,`I2CSS_Wait`,`NoAck`（皆 `Eep24C04.c`）

## 呼叫端（TX-2.C）

| 函式 | 用途 |
|---|---|
| `Check_EEPROM` | 驗證 machine code，不符時呼叫 `EEPROM_Write_Defaul` |
| `Call_EEPROM_Data` | 讀取所有持久化設定值 |
| `EEPROM_Write_Defaul` | 寫入預設值 |
| `Set_EEPROM_ToZero` | 清零 |

## 依賴

- 共用基礎設施（`userdefine.h` 腳位巨集 `IIC_SCL`/`IIC_SDA`）
- 函式 `T2delay1ms`（檔案 `TX-2.C`）— 供 EEPROM 寫入週期延遲。**注意**：此為一般函式而非 ISR，未收錄於 `docs/interrupts.md`（該文件僅涵蓋 5 個 ISR 本身），目前無獨立說明章節。

## 被使用

- main（`docs/modules/main.md`）— 開機時呼叫 `Check_EEPROM`
- Control Logic（`docs/modules/control_logic.md`）— 讀寫功能設定
- Communication Protocol（`docs/modules/communication_protocol.md`）— 部分命令持久化設定

## 已證實風險

- `CT_7302_WriteTwoByte`/`CT_7302_ReadOneByte` 宣告但實作停用，屬潛在死碼／未完成功能。
- 與 IO Expander 模組（`docs/modules/io_expander.md`）各自重複實作幾乎相同的軟體 I2C bit-bang 邏輯（不同物理匯流排腳位，無實際衝突，但程式碼重複）。
