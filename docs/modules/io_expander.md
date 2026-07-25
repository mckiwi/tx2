# Module: io_expander

檔案：`IOEXP6524.c`

## 職責

**已證實**：驅動兩顆 PCAL6524（NXP IO 擴充晶片）之一，透過軟體 bit-bang I2C 初始化與寫入輸出埠，用於承載 LED、繼電器、控制輸出等，因應主控 MCU 腳位不足而擴充。

## 對外 API（僅列名稱）

| 函式 | 用途 |
|---|---|
| `initIOPorts` | 初始化指定 IO expander（`IOEXP_FUNC`/`IOEXP_SRC`），設定輸出組態並清除中斷遮罩 |
| `ioWriteOneByte` | 寫入單一暫存器位元組 |
| `ioWriteThreePorts`,`ioWriteTwoPorts`,`ioWriteOnePorts` | 寫入 1~3 個輸出埠資料 |

## 內部原語（私有，僅列名稱）

`ioSTART`,`ioSTOP`,`ioCheckAck`,`ioWriteByte`,`ioWrite1/2/3Ports`,`ioCSS_Wait`

## 共享狀態

| 變數 | 用途 |
|---|---|
| `bXIOLock`（`TX-2.C`） | I2C 匯流排鎖，初始化/寫入期間鎖定避免其他模組同時存取 |
| `IOPORTS_DATBK[2][3]`（`IOEXP6524.c`） | 兩顆 expander 的輸出資料備份 |

## 依賴

- 共用基礎設施（`userdefine.h` 腳位巨集 `ioSCL`/`ioSDA`）

## 被使用

- main（`docs/modules/main.md`）— 開機呼叫 `initIOPorts`
- Control Logic（`docs/modules/control_logic.md`）— `PCLK_Action`（`TX-2.C`）透過本模組提交輸出
- Display（`docs/modules/display.md`）— LED bank 提交

## 已證實風險

- 與 Storage 模組（`docs/modules/storage.md`）各自重複實作幾乎相同的軟體 I2C bit-bang 原語，程式碼重複（不同物理匯流排腳位，無實際衝突）。
