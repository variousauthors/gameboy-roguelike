IF !DEF(OAM_RING_BUFFER_INC)
DEF OAM_RING_BUFFER_INC EQU 1

SECTION "OAMRingBufferState", WRAM0

writeOAMRingBuffer: ds 2
readOAMRingBuffer: ds 2

DEF _OAMRAM_END EQU $FEA0

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
getFreeSpriteOAMRingBuffer:
  ld hl, writeOAMRingBuffer
  call dereferencePointer

  ; advance the pointer without affecting hl
  call advancePointerOAMRingBuffer

  ret

/** at the start of each frame we want to
 * clear the sprites we drew last frame 
 * so we will advance the read pointer 
 * to the write pointer, clearing as we go */
clearSpriteOAMRingBuffer:
  ld hl, readOAMRingBuffer
  call dereferencePointer

  ld a, 0
.loop
  ld [hli], a
  ld [hli], a


.done

  ret

; @param hl - address in OAM
; @param z - OAM is full
advancePointerOAMRingBuffer:
  push hl

  ; first advance to the next entry
  inc hl
  inc hl
  inc hl
  inc hl
  
  ; check if we are at the end
  ld de, _OAMRAM_END
  call isEqualAddress
  jr nz, .skipWrap

  ; set hl to the start of OAM
  ld hl, _OAMRAM
.skipWrap

  ld de, writeOAMRingBuffer
  call updatePointer

  pop hl

  ret

ENDC
