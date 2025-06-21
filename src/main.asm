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

  jp Main

EntryPoint:
	; Shut down audio circuitry
	ld a, 0
	ld [rNR52], a

	; Do not turn the LCD off outside of VBlank
  call turnOffLCD

  ; copy the 1bpp stuff
  ld bc, TestMapTileset
  call LoadMapTileset
  /*
	ld de, OverworldTiles
	ld hl, $9000
	ld bc, OverworldTilesEnd - OverworldTiles
  call Memcopy1bpp
  */

	; Copy the tilemap
	ld de, Tilemap
	ld hl, $9800
	ld bc, TilemapEnd - Tilemap
  call Memcopy

  ; clear OAM sure why not
  ld a, 0
  ld b, 160
  ld hl, _OAMRAM
ClearOam:
  ld [hli], a
  dec b
  jp nz, ClearOam

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
INCLUDE "graphics.inc"
INCLUDE "input.inc"
INCLUDE "maps/test-map.inc"
