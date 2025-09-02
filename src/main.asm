INCLUDE "hardware.asm"
INCLUDE "dma.asm"

DEF BRICK_LEFT EQU $05
DEF BRICK_RIGHT EQU $06
DEF BLANK_TILE EQU $08

SECTION "OAMData", WRAM0, ALIGN[8]
Sprites: ; OAM Memory is for 40 sprites with 4 bytes per sprite
  ds 40 * 4
.end:

SECTION "GameState", WRAM0

currentTurn: ds 1
DEF BLACK_TURN EQU 0
DEF WHITE_TURN EQU 1

DEF BLACK_STONE EQU $02
DEF WHITE_STONE EQU $03
DEF HOSHI_TILE EQU $04

; the stone to add or remove board y, x
blackStone: ds 2
whiteStone: ds 2
deleteStone: ds 2

SECTION "vblank", ROM0[$0040]
  jp DMA_ROUTINE

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0 ; Make room for the header

EntryPoint:
  di
	; Shut down audio circuitry
	ld a, 0
	ld [rNR52], a

	; Do not turn the LCD off outside of VBlank
  call turnOffLCD

  dma_Copy2HRAM

  call ZeroOutWorkRAM

  ; init turn
  ld a, BLACK_TURN
  ld [currentTurn], a

	; Copy the tile data
	ld de, BoardTiles
	ld hl, _VRAM9000
	ld bc, BoardTilesEnd - BoardTiles
  call Memcopy

	; Copy the tilemap
	ld de, Tilemap
	ld hl, _SCRN0
	ld bc, TilemapEnd - Tilemap
  call Memcopy

  ; copy the player sprite
	ld de, PlayerSprite
	ld hl, _VRAM8000
	ld bc, PlayerSpriteEnd - PlayerSprite
  call Memcopy

  ; clear OAM sure why not
  ld a, 0
  ld b, 160
  ld hl, _OAMRAM
ClearOam:
  ld [hli], a
  dec b
  jp nz, ClearOam

  ; init player sprite
  ld hl, Sprites
  ld a, (8 * 9) + 16
  ld [hli], a
  ld a, (8 * 9) + 8
  ld [hli], a
  ld a, 0
  ld [hli], a
  ld [hli], a

  ; Initialize global variables
  ld a, 0
  ld [wCurKeys], a
  ld [wNewKeys], a

  call turnOnLCD

  ld a, IEF_VBLANK
  ld [rIE], a
  ei

Main:
  halt

  nop

  ; DRAWING STUFF

  call updatePlayerSprite
  call updateBoard

  ; DONE DRAWING

  call resetActions

  call UpdateKeys

  call playerMove
  call RecordAction
  jr z, Main ; no action it is still the same turn

  ld a, [currentTurn]
  cp a, BLACK_TURN
  jr z, .whiteTurn

.blackTurn
  ld a, BLACK_TURN
  ld [currentTurn], a

  jp Main

.whiteTurn
  ld a, WHITE_TURN
  ld [currentTurn], a

  jp Main

SECTION "Board", ROM0

resetActions:
  ld a, 0
  ld [blackStone], a
  ld [whiteStone], a
  ld [deleteStone], a
  ret

; @return z - no action
RecordAction:
  ld a, [wNewKeys]
  and a, PADF_A
  jr z, .checkB

  ; check if there is already a stone there
  ; and abort
  call getPlayerPosition
  call getAddressFromPosition
  ld a, [hl]
  cp a, BLACK_STONE
  ret z
  cp a, WHITE_STONE
  ret z

  ld a, [Sprites]
  srl a
  srl a
  srl a
  dec a
  dec a ; y offset by 16
  ld b, a

  ld a, [Sprites + 1]
  srl a
  srl a
  srl a
  dec a ; x offset by 8
  ld c, a

  ld a, [currentTurn]
  cp a, BLACK_TURN
  jr nz, .whiteTurn

.blackTurn
  ld a, b
  ld [blackStone], a

  ld a, c
  ld [blackStone + 1], a
  inc a ; to return nz
  ret

.whiteTurn
  ld a, b
  ld [whiteStone], a

  ld a, c
  ld [whiteStone + 1], a
  inc a ; to return nz

  ret

.checkB
  ld a, [wNewKeys]
  and a, PADF_B
  ret z

  ld a, [Sprites]
  srl a
  srl a
  srl a
  dec a
  dec a ; y offset by 16
  ld b, a

  ld a, [Sprites + 1]
  srl a
  srl a
  srl a
  dec a ; x offset by 8
  ld c, a

  ; record delete stone
  ; do not pass turn
  ld a, b
  ld [deleteStone], a

  ld a, c
  ld [deleteStone + 1], a
  ld a, 0
  cp a ; ret z this is a delete
  ret

; @param bc - y, x world space
; @return hl - VRAM address
getAddressFromPosition:
  ld h, 0
  ld l, b

  add hl, hl
  add hl, hl
  add hl, hl
  add hl, hl
  add hl, hl ; x32

  ld d, h
  ld e, l

  ld hl, $9800

  add hl, de

  ld a, c
  call addAToHL

  ret

; @param a - stone to place
; @param bc - y, x
placeStone:
  push af

  ; multiply b by 32
  call getAddressFromPosition

  pop af
  ld [hl], a

  ret

updateBoard:
  ld a, [whiteStone]
  cp a, 0
  jr z, .blackStone

  ld b, a
  ld a, [whiteStone + 1]
  cp a, 0
  jr z, .blackStone

  ld c, a
  ld a, WHITE_STONE

  call placeStone

.blackStone
  ld a, [blackStone]
  cp a, 0
  jr z, .deleteStone

  ld b, a
  ld a, [blackStone + 1]
  cp a, 0
  jr z, .deleteStone

  ld c, a
  ld a, BLACK_STONE

  call placeStone

.deleteStone
  ld a, [deleteStone]
  cp a, 0
  jr z, .done

  ld b, a
  ld a, [deleteStone + 1]
  cp a, 0
  jr z, .done

  ; we might need to place a hoshi
  ld c, a

  call getEmptyBoardTileForPosition

  call placeStone

.done

  ret

; @param bc
; @return a - the tile
; bc is preserved
getEmptyBoardTileForPosition:
  ld a, c
  cp a, 4
  jp z, .maybeHoshi

  cp a, 9
  jp z, .maybeHoshi

  cp a, 14
  jp z, .maybeHoshi

  ld a, 0 ; no hoshi
  ret

.maybeHoshi
  ld a, b
  cp a, 4
  jr z, .hoshi

  cp a, 9
  jr z, .hoshi

  cp a, 14
  jr z, .hoshi

  ld a, 0
  ret

.hoshi
  ld a, HOSHI_TILE

  ret

; @param a - a
; @param hl - hl
; @return hl - hl + a
addAToHL:
  add l ; a = a + l
	ld l, a ; l' = a'
	adc h ; a'' = a' + h + c ; what!?
	sub l ; l' here is a + l
	ld h, a ; so h is getting h + c yikes!

  ret

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

ZeroOutWorkRAM:
  ld hl, _RAM
  ld de, $DFFF - _RAM ; number of bytes to write
.write
  ld a, $00
  ld [hli], a
  dec de
  ld a, d
  or e
  jr nz, .write
  ret

SECTION "Input Variables", WRAM0
wCurKeys: db
wNewKeys: db

INCLUDE "helpers.asm"
INCLUDE "graphics.asm"
INCLUDE "player.asm"