# Vendor Purchase Debug Notes (2026-04-11)

## 1) ç°è±¡
- æœ¬åœ°ä»“åº“ä¸­ç‚¹ `E` è´­ä¹°ä¼šå‘èµ·ç½‘ç»œè¯·æ±‚ï¼Œä½†è§’è‰²ä»“åº“ä¸å¢åŠ ã€‚
- æ—¥å¿—æ˜¾ç¤º `CMD_880` è¿”å›ç  `3005`ï¼ˆèµ„é‡‘ä¸è¶³ï¼‰ï¼Œä¸æ˜¯åŒ…æœªå‘ã€UI æœªåˆ·ã€‚

## 2) å…³é”®æ—¥å¿—ä½ç½®
- å®¢æˆ·ç«¯ï¼š`Saved/Logs/MMOARPG.log`
- æ¸¸æˆç½‘å…³ï¼š`\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\build\bin\2026-04-11\gate.txt`
- æ¸¸æˆæœï¼š`\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\build\bin\2026-04-11\game.txt`
- æ•°æ®æœï¼š`\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\build\bin\2026-04-11\db.txt`

## 3) å…³é”®ä»£ç ä¿®æ”¹
- å®¢æˆ·ç«¯:
  - `Source/MMOARPG/Inventory/UI_SplitStack.cpp`
    - è´­ä¹°è¯·æ±‚å‘é€æ—¶ `count` æ”¹ä¸º `int32`
    - è§£æ `SP_ItemBuy` å¤±è´¥ç ï¼Œ`3005` æ˜ å°„ä¸º `not enough gold`
    - ä»…åœ¨è´­ä¹°æˆåŠŸå›åŒ…æ—¶å…³é—­è´­ä¹°å¼¹çª—
    - æˆåŠŸåæ›´æ–°èƒŒåŒ…æ•°æ®ï¼Œä¸å†ä»…æœ¬åœ°æ·»åŠ 
  - `Source/MMOARPG/Inventory/UI_Vendor.cpp`
    - ä¿®å¤å–è´§åè®® `sell` çš„ `pos` å†™å…¥
  - `Source/MMOARPG/Core/Game/MMOARPGGameState.cpp/.h`
    - å¢åŠ è“å›¾ `VendorItemIdToPropId` æ˜ å°„å¹¶åœ¨ `BeginPlay` åº”ç”¨
    - å…¼å®¹å­—ç¬¦ä¸²å½’ä¸€åŒ–æ˜ å°„

- æœåŠ¡ç«¯:
  - `\\wsl.localhost\Ubuntu-22.04\root\server_mmorpg\GameServer\code\AppSelectRole.cpp`
    - ç™»å½•è§’è‰²åï¼Œå¦‚æœ `gold < 1000000`ï¼Œåˆ™è¡¥åˆ° `1000000`
    - è®°å½•è¡¥é‡‘å¸æ—¥å¿— `[login gold seed]`
  - é‡å¯æœåŠ¡ç«¯åç”Ÿæ•ˆï¼ˆè§ä¸‹ï¼‰

## 4) ç¼–è¯‘ä¸é‡å¯
- åªé‡æ–°ç¼–è¯‘äº† GameServerï¼š
  - `cd /root/server_mmorpg/build && ninja GameServer`
- é‡å¯æœåŠ¡è„šæœ¬ï¼š
  - `cd /root/server_mmorpg && ./restart.sh`
- å¯åŠ¨é¡ºåºè„šæœ¬ä¸­ä¼šæŒ‰ `DBServer -> CenterServer -> GameServer -> GateServer -> LoginServer` å¯åŠ¨

## 5) è¯´æ˜
- `CMD_880` çš„ `3005` åˆ¤å®šè·¯å¾„ï¼š`InventoryService.cpp` ä¸­ `if (user->role.base.econ.gold < gold) return 3005;`
- å½“å‰ä¼šè¯çš„æ ¹å› æ˜¯ç™»å½•åè§’è‰² `gold` ä¸º 0ï¼Œå¯¼è‡´å•†åº—è´­ä¹°ç›´æ¥å¤±è´¥ã€‚
- ä»¥ä¸Šæ”¹åŠ¨ç›®æ ‡æ˜¯è®©è´­ä¹°é“¾è·¯å¯ç”¨å¹¶å¿«é€ŸéªŒè¯ï¼›å¦‚éœ€â€œé•¿æœŸè§„åˆ™åŒ–â€ï¼Œå»ºè®®åç»­æŠŠåˆå§‹é‡‘å¸ç­–ç•¥æ”¹åˆ°è´¦å·åˆ›å»º/æ–°æ‰‹å¼•å¯¼æµç¨‹ï¼Œè€Œä¸æ˜¯æ¯æ¬¡ç™»å½•è¡¥é½ã€‚
## 6) 2026-04-11 ¹ºÂò 3006 ÔÙ´ÎÅÅ²é½á¹û
- ÏÖÏó: `CMD_880` ÈÔ·µ»Ø `childcmd:3006`£¬·şÎñ¶Ë `game.txt` ´ò `WorldProp Create Equip:200900101`£¬²Ö¿âÎŞµÀ¾ßÈë°ü¡£
- ¸ùÒò: `prop.csv` Ê³ÎïµÀ¾ß (`200900101~200900105`) Îª `type=2`£¨ÏûºÄÆ·£©£¬µ« `consume.csv` È±ÉÙ¶ÔÓ¦ĞĞ£¬`WorldScript` ¼ÓÔØºó `SCRIPT_PROP::consume.id` Îª 0¡£
- ¶ÔÓ¦´úÂëÂ·¾¶:
  - `InventoryService.cpp: buyItem() -> TS_Tools::createProp(...)`
  - `WorldTools_Prop.cpp: createProp() -> createConsume()`
  - `createConsume()` ·µ»ØÌõ¼ş `if(s_prop->consume.id < 100000) return false;`
- ÒÑĞŞ:
  - ÒÑ²¹Æë `consume.csv` Îå¸öÌõÄ¿: `200900101~200900105`
  - ÒÑÓÅ»¯ `WorldTools_Prop.cpp` ÈÕÖ¾ÎÄ±¾£¬Çø·Ö `EQUIP/CONSUME/GOLD` ·ÖÖ§Ê§°ÜÔ­Òò
- ½á¹ûËµÃ÷: ÇĞ»Ø·şÎñ¶ËÈÕÖ¾ºó£¬ÈôÒÑÖØÆô·şÎñ£¬¹ºÂòÓ¦²»ÔÙ·µ»Ø 3006£»ÈôÈÔÓĞÒì³££¬ÇëÓÅÏÈ×¥ `gate.txt` + `game.txt` ÖĞ `CMD_880` ºÍ `WorldProp Create*` Ïà¹ØÈÕÖ¾¡£
