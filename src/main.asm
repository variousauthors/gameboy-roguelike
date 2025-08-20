INCLUDE "hardware.asm"
INCLUDE "dma.asm"

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
TurnFunctions: ds 4 + 2

SECTION "vblank", ROM0[$0040]
  jp DMA_ROUTINE
  reti

SECTION "Header", ROM0[$100]

	jp EntryPoint

  ds $143 - @, 0 
  db $C0
	ds $150 - @, 0 ; Make room for the header

Main:
  halt

  call drawPlayer
  call drawMonsters

  ld hl, CurrentTurnFunction
  call dereferencePointer
  call dereferencePointer
  call indirectCall
  call nz, passTurn

  jp Main

passTurn:
  ; load up the current turn function
  ld hl, CurrentTurnFunction
  call dereferencePointer

  ; move to the next one
  inc hl
  inc hl

  ; check both bytes of the pointer
  ld a, [hli]
  cp 0
  jr nz, .update

  ; check both bytes of the pointer
  ld a, [hl]
  cp 0
  jr nz, .update

.reset
  ; reset the pointer to the start
  ld de, CurrentTurnFunction
  ld hl, TurnFunctions
  call updatePointer
  ret

.update
  dec hl ; rewind to the start of the pointer
  ; otherwise write the address
  ld de, CurrentTurnFunction
  call updatePointer

  ret

EntryPoint:
  di

  dma_Copy2HRAM	; sets up routine from dma.inc that updates sprites

	; Shut down audio circuitry
	ld a, 0
	ld [rNR52], a

	; Do not turn the LCD off outside of VBlank
  call turnOffLCD

  call initCurrentMap

  call fullMapRedraw

  ; clear OAM sure why not
  ld b, 160
  ld hl, _OAMRAM
  call clearMemory

  call initPlayer
  call initMonsters

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
  inc de
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


INCLUDE "helpers.asm"
INCLUDE "player.asm"
INCLUDE "simple-queue.asm"
INCLUDE "OMA-ring-buffer.asm"
INCLUDE "monster.asm"
INCLUDE "graphics.asm"
INCLUDE "input.asm"
INCLUDE "map-utils.asm"
INCLUDE "navigation-map.asm"
INCLUDE "maps/house-1bpp-no-grid-bw-y0-x0.asm"
