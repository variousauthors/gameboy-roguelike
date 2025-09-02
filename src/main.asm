INCLUDE "src/includes/hardware.asm"

DEF BRICK_LEFT EQU $05
DEF BRICK_RIGHT EQU $06
DEF BLANK_TILE EQU $08

SECTION "GameState", WRAM0

; the stone to add or remove board y, x
blackStone: ds 2
whiteStone: ds 2
blackCapture: ds 2
whiteCapture: ds 2

SECTION "vblank_interrupt", ROM0[$0040]
  reti

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0 ; Make room for the header

EntryPoint:
	; Shut down audio circuitry
	ld a, 0
	ld [rNR52], a

	; Do not turn the LCD off outside of VBlank
  call turnOffLCD

  call ZeroOutWorkRAM

	; Copy the tile data
	ld de, BoardTiles
	ld hl, $9000
	ld bc, BoardTilesEnd - BoardTiles
  call Memcopy

	; Copy the tilemap
	ld de, Tilemap
	ld hl, $9800
	ld bc, TilemapEnd - Tilemap
  call Memcopy

  ; copy the player sprite
	ld de, PlayerSprite
	ld hl, $8000
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
  ld hl, _OAMRAM
  ld a, 8 + 16
  ld [hli], a
  ld a, 8 + 8
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

  call updateBoard
  call resetActions

  call UpdateKeys

  call RecordAction
  call playerMove

  jp Main

SECTION "Board", ROM0

resetActions:
  ld a, 0
  ld [blackStone], a
  ld [blackCapture], a
  ld [whiteStone], a
  ld [whiteCapture], a
  ret

RecordAction:
  ld a, [wNewKeys]
  and a, PADF_A
  ret z

  ld a, [_OAMRAM]
  srl a
  srl a
  srl a
  dec a
  dec a ; y offset by 16
  ld [whiteStone], a

  ld a, [_OAMRAM + 1]
  srl a
  srl a
  srl a
  dec a ; x offset by 8
  ld [whiteStone + 1], a

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


  ; multiply b by 32
  ld h, 0
  ld l, b

  add hl, hl
  add hl, hl
  add hl, hl
  add hl, hl
  add hl, hl ; x32

  ld d, h
  ld e, l

  ; place white stone
  ld hl, $9800

  add hl, de

  ld a, c
  call addAToHL

  ld [hl], $03

.blackStone

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

INCLUDE "src/includes/helpers.asm"
INCLUDE "src/includes/graphics.asm"
INCLUDE "src/includes/player.asm"