INCLUDE "hardware.inc"

DEF BRICK_LEFT EQU $05
DEF BRICK_RIGHT EQU $06
DEF BLANK_TILE EQU $08

SECTION "GameState", WRAM0

wPlayerTurn: db

SECTION "vblank_interrupt", ROM0[$0040]
  reti

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0 ; Make room for the header


Main:
  halt

  nop

  ld a, [wPlayerTurn]
  cp a, 0
  jr z, .updateMonster

.updatePlayer
  ; update graphics
  call playerUpdateSprite

  ; process intents

  call playerLacksIntent
  jr z, .doneUpdate

  call playerBump
  jr z, .rollback

  call playerAttack
  call z, playerCommitAttack

  call playerCommitMove

  ; pass to monster turn
  ld a, 0
  ld [wPlayerTurn], a

  jr .doneUpdate

.rollback
  call playerRollbackMove
  jr .doneUpdate

.updateMonster
  ; call monsterUpdateSprite
  call monsterListUpdateSprites

  ; process intents

  call monsterListBump
  ; call monsterBump
  ; call z, monsterRollbackMove

  ; pass to player turn
  ld a, 1
  ld [wPlayerTurn], a

  jr .doneUpdate

.doneUpdate

  ; plan nextturn

  ld a, [wPlayerTurn]
  cp a, 0
  jr z, .planMonsterTurn
.planPlayerTurn
  ; update intents
  call UpdateKeys
  ld a, [wNewKeys]
  cp a, 0
  jr z, Main ; no new inputs

  call playerMove
  jp Main
.planMonsterTurn
  call monsterListPlanMoves
  jp Main


EntryPoint:
	; Shut down audio circuitry
	ld a, 0
	ld [rNR52], a

	; Do not turn the LCD off outside of VBlank
  call turnOffLCD

  ; copy the 1bpp stuff
	ld de, OverworldTiles
	ld hl, $9000
	ld bc, OverworldTilesEnd - OverworldTiles
  call Memcopy1bpp

	; Copy the tilemap
	ld de, Tilemap
	ld hl, $9800
	ld bc, TilemapEnd - Tilemap
  call Memcopy

  ; copy the player sprites
	ld de, PlayerSprites
	ld hl, $8000
	ld bc, PlayerSpritesEnd - PlayerSprites
  call Memcopy

  ; copy the monsters sprites
	ld de, MonsterSprites
	ld hl, $8040
	ld bc, MonsterSpritesEnd - MonsterSprites
  call Memcopy

  ; clear OAM sure why not
  ld a, 0
  ld b, 160
  ld hl, _OAMRAM
ClearOam:
  ld [hli], a
  dec b
  jp nz, ClearOam

  ; init entities
  call initPlayer
  ; call initMonster
  call monsterListInit

  ; start on the player turn
  ld a, 1
  ld [wPlayerTurn], a

  ; initial draw
  call playerUpdateSprite
  ; call monsterUpdateSprite
  call monsterListUpdateSprites

  ; Initialize global variables
  ld a, 0
  ld [wCurKeys], a
  ld [wNewKeys], a

  call turnOnLCD

  ld a, IEF_VBLANK
  ld [rIE], a
  ei

  jp Main


SECTION "Input", ROM0
UpdateKeys:
  ; poll half the controller
  ld a, P1F_GET_BTN
  call .onenibble
  ld b, a

  ; poll the other hald
  ld a, P1F_GET_DPAD
  call .onenibble
  swap a
  xor a, b
  ld b, a

  ld a, P1F_GET_NONE
  ldh [rP1], a

  ld a, [wCurKeys]
  xor a, b ; a gets keys that changed state
  and a, b ; a gets keys that changed to pressed
  ld [wNewKeys], a
  ld a, b
  ld [wCurKeys], a
  ret

.onenibble
  ldh [rP1], a
  call .knownret ; burn 10 cycles calling a known ret
  ldh a, [rP1] ; waiting for the key matrix to settle
  ldh a, [rP1]
  ldh a, [rP1] ; this read counts
  or a, $F0

.knownret
  ret


SECTION "Input Variables", WRAM0
wCurKeys: db
wNewKeys: db

INCLUDE "helpers.inc"
INCLUDE "graphics.inc"
INCLUDE "player.inc"
INCLUDE "utilities.inc"
INCLUDE "monsters.inc"
INCLUDE "monsterList.inc"
