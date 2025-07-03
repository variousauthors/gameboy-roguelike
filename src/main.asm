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
  cp a, 0
  jr z, .noInput

  call recordMoveIntentPlayer

  ; check for collision
  call checkCollisionPlayer
  call z, resetMoveIntentPlayer

  ; check for monster

  ; update player pos
  call applyMoveIntentPlayer

.noInput

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


INCLUDE "helpers.inc"
INCLUDE "player.inc"
INCLUDE "graphics.inc"
INCLUDE "input.inc"
INCLUDE "map-utils.inc"
INCLUDE "maps/house-1bpp-no-grid-bw-y0-x0.inc"
