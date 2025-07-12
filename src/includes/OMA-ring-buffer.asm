IF !DEF(OAM_RING_BUFFER_INC)
DEF OAM_RING_BUFFER_INC EQU 1

SECTION "OAMRingBufferState", WRAM0

writeOAMRingBuffer: ds 2
readOAMRingBuffer: ds 2
endOAMRingBuffer: ds 2

DEF SPRITE_SIZE EQU 4 ; bytes per sprite

SECTION "OAMRingBuffer", ROM0

/**
 * when we need to draw a sprite we request one
 * from the ring buffer, and write to it
 * then at the start of a new frame we clear
 * the sprites we used by advancing the read pointer
 */

initOAMRingBuffer:
  ld hl, _OAMRAM
  ld de, writeOAMRingBuffer
  call updatePointer

  ld hl, _OAMRAM
  ld de, readOAMRingBuffer
  call updatePointer

  ld hl, _OAMRAM + (40 * 4)
  ld de, endOAMRingBuffer
  call updatePointer

  ret

; @param hl - address of a free sprite
; @param z - OAM is full
getFreeSpriteOAMRingBuffer:
  ld hl, writeOAMRingBuffer
  call dereferencePointer

  call advancePointerOAMRingBuffer

  ret

; @param z - OAM is full
advancePointerOAMRingBuffer:
  push hl

  inc hl
  inc hl
  inc hl
  inc hl
  ld de, writeOAMRingBuffer
  call updatePointer

  pop hl

  ret

ENDC
