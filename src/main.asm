INCLUDE "hardware.inc"

DEF BRICK_LEFT EQU $05
DEF BRICK_RIGHT EQU $06
DEF BLANK_TILE EQU $08

DEF GAME_TURN_PLAYER EQU $01
DEF GAME_TURN_MONSTER EQU $02

SECTION "GameState", WRAM0

wGameTurn: db

; pointer to function pointer
CurrentTurnFunction: dw
; null terminated list of function pointers
TurnFunctions: ds 4 + 1

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

  ld hl, CurrentTurnFunction
  call dereferencePointer
  call dereferencePointer
  call indirectCall
  call nz, passTurn

  jp Main

passTurn:
  ; load up the current turn function
  ; get the address
  ; increment by two
  ; if the value there is zero
  ; then loop
  ; write the address

  ret

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

  ld a, 1
  ld [wGameTurn], a

  ; init list of "turn functions"
  ld de, TurnFunctions
  ld hl, doTurnPlayer
  call updatePointer
  inc de

  ld hl, doTurnMonster
  call updatePointer
  inc de
  ld a, 0
  ld [de], a ; null terminate

  ; pointer to pointers
  ld de, CurrentTurnFunction
  ld hl, TurnFunctions
  call updatePointer

  call turnOnLCD

  ld a, IEF_VBLANK
  ld [rIE], a
  ei

  jp Main


INCLUDE "helpers.inc"
INCLUDE "player.inc"
INCLUDE "monster.inc"
INCLUDE "graphics.inc"
INCLUDE "input.inc"
INCLUDE "map-utils.inc"
INCLUDE "maps/house-1bpp-no-grid-bw-y0-x0.inc"
