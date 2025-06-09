INCLUDE "hardware.inc"

DEF BRICK_LEFT EQU $05
DEF BRICK_RIGHT EQU $06
DEF BLANK_TILE EQU $08

SECTION "vblank_interrupt", ROM0[$0040]
  reti

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0 ; Make room for the header


Main:
  halt

  nop

  ; update graphics
  call playerUpdateSprite
  call monsterUpdateSprite

  ; process intents

  call playerBump
  jr z, .rollbackMove

  call playerAttack
  jr z, .commitAttack

  ; call lootCorpse
  ; jr z, .loopCorpse

  jr .commitMove

.commitAttack
  call playerCommitAttack

  jr .rollbackMove

.commitMove
  ld a, [wPlayerNextX]
  ld [wPlayerX], a
  ld a, [wPlayerNextY]
  ld [wPlayerY], a

  jr .doneUpdate

.rollbackMove
  ld a, [wPlayerX]
  ld [wPlayerNextX], a
  ld a, [wPlayerY]
  ld [wPlayerNextY], a

.doneUpdate

  ; update intents
  call UpdateKeys
  call playerMove

.done
  jp Main

EntryPoint:
	; Shut down audio circuitry
	ld a, 0
	ld [rNR52], a

	; Do not turn the LCD off outside of VBlank
  call turnOffLCD

	; Copy the tile data
	ld de, Tiles
	ld hl, $9000
	ld bc, TilesEnd - Tiles
  call Memcopy

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
  call initMonster

  ; initial draw
  call playerUpdateSprite
  call monsterUpdateSprite

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