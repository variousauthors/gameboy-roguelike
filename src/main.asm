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

  ds $143 - @, 0 
  db $C0
	ds $150 - @, 0 ; Make room for the header

Main:
  halt

  call drawPlayer

  ; read input
  call UpdateKeys

  ld a, [wNewKeys]

.checkLeft
  bit BUTTON_LEFT_BIT, a
  jr z, .checkRight

  ld hl, playerWorldX
  ld b, [hl]
  dec b
  ld [hl], b

.checkRight
  bit BUTTON_RIGHT_BIT, a
  jr z, .checkUp

  ld hl, playerWorldX
  ld b, [hl]
  inc b
  ld [hl], b

.checkUp
  bit BUTTON_UP_BIT, a
  jr z, .checkDown

  ld hl, playerWorldY
  ld b, [hl]
  dec b
  ld [hl], b

.checkDown
  bit BUTTON_DOWN_BIT, a
  jr z, .doneCheck

  ld hl, playerWorldY
  ld b, [hl]
  inc b
  ld [hl], b

.doneCheck
  ; check for collision
  ; check for monster
  ; update player pos

  nop

  jp Main

EntryPoint:
	; Shut down audio circuitry
	ld a, 0
	ld [rNR52], a

	; Do not turn the LCD off outside of VBlank
  call turnOffLCD

  call initCurrentMap

  call fullMapRedraw

  ; clear OAM sure why not
  ld a, 0
  ld b, 160
  ld hl, _OAMRAM
ClearOam:
  ld [hli], a
  dec b
  jp nz, ClearOam

  call initPlayer

  ; Initialize global variables
  ld a, 0
  ld [wCurKeys], a
  ld [wNewKeys], a

  call turnOnLCD

  ld a, IEF_VBLANK
  ld [rIE], a
  ei

  jp Main

SECTION "PlayerState", WRAM0

playerWorldX: db
playerWorldY: db

SECTION "Player", ROM0

drawPlayer:
  ; draw player sprite
  ld hl, _OAMRAM

  ld a, [playerWorldY]
  inc a
  inc a ; add 16, sprites start off-screen
  sla a
  sla a
  sla a
  ld [hli], a

  ld a, [playerWorldX]
  inc a ; add 8 sprites start off-screen
  sla a
  sla a
  sla a
  ld [hli], a

  ld a, 0
  ld [hli], a
  ld [hli], a

  ret

initPlayer:
	; Copy the player sprite
	ld de, PlayerSprite
	ld hl, $8000
	ld bc, PlayerSpriteEnd - PlayerSprite
  call Memcopy

  ; initialize player state
  ld a, 12
  ld [playerWorldX], a
  ld [playerWorldY], a

  ret


INCLUDE "helpers.inc"
INCLUDE "graphics.inc"
INCLUDE "input.inc"
INCLUDE "map-utils.inc"
INCLUDE "maps/house-1bpp-no-grid-bw-y0-x0.inc"
