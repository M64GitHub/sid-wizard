; vesselsubs.inc 2021 m64

VESSELMODE .byte 0      ; 1 = vessel sync playing mode 

; main loop 

vessel  lda playadapter.playmod
        beq +           ; if playmode is 0, tune doesn't play

        lda MIDIdev
        beq goback

wait4play lda #1
        beq do_vessel   ; first run toggle

        lda #0
        sta wait4play+1 ; 1st run: wait4play: wait for incoming MIDI sync messages
        lda #1
        sta VESSELMODE  ; turn on vessel sync playing mode

        ; disable raster irq    
        lda $d01a
        and #$fe
        sta $d01a

        jsr vesseldisp

do_vessel  
        ;
        ; VESSEL MIDI READ HERE
        ;
        jsr MIDIC64.GetData.vesselReadMIDI
        cpy #MIDI.StopSeqPlay
        beq midistop
        cpy #MIDI.TimingClock
        bne do_vessel

        jsr playadapter.playsubb        
        
        jsr vesseldisp
        jsr keyhandler.keyer
        jmp vessel

+       lda VESSELMODE
        beq goback

midistop
        lda #0
        sta VESSELMODE

        lda #1
        sta wait4play+1

        ; re init vessel disp
        lda #1
        sta vesseldisp+1

        ; in case of MIDI stop
        lda #0
        sta playadapter.playmod

        ; re-enable raster irq
        lda $d01a
        ora #$1
        sta $d01a
        sei
        lda #$35
        sta banksel 
        jmp menu.retoedi 

goback  jsr display     ;considers menu-display over patterns (based on 'menumode' variable) 
        lda menu.menupoint ;check selected menupoint (value is 0 if no menupoint was launched)
        beq gotomainloop    ;if no menu was launched, main loop continues
        jmp menu.runmenup
gotomainloop jmp mainloop

; display code 

vesseldisp lda #1 ; selfmod 1st chk
        beq notfirst 
        lda #0 
        sta vesseldisp+1
        sta $d015
        
        ; clrscr
        ldy #0
-       lda #$20        
        sta $0400,y
        sta $0500,y
        sta $0600,y
        sta $0700,y
        lda #0
        sta $d800,y
        sta $d900,y
        sta $da00,y
        sta $db00,y
        iny
        bne -

        ; colors for numbers
        lda #$0b ; SEQPOS
        sta $d800+40*(3)+5+0
        sta $d800+40*(3)+5+1

        lda #$0e ; PTNPOS
        sta $d800+40*(5+9)+30+0
        sta $d800+40*(5+9)+30+1

        ; PTN 1-3
        lda #1
        sta $d800+40*(5+9)+4+0+1
        sta $d800+40*(5+9)+4+1+1

        sta $d800+40*(5+9)+7+0+1
        sta $d800+40*(5+9)+7+1+1

        sta $d800+40*(5+9)+10+0+1
        sta $d800+40*(5+9)+10+1+1

        ; INS 1-3
        lda #1
        sta $d800+40*(5+11)+4+0+1
        sta $d800+40*(5+11)+4+1+1

        sta $d800+40*(5+11)+7+0+1
        sta $d800+40*(5+11)+7+1+1

        sta $d800+40*(5+11)+10+0+1
        sta $d800+40*(5+11)+10+1+1

notfirst ldx #40
-       lda vessellogo,x
        cmp #$20
        beq +
        sta vidram+40*5-1,x
        inc $d800+40*5-1,x        
+       dex
        bne -

        ; disp positions
part3   lda player.SEQPOS
        and #$f0
        lsr
        lsr
        lsr
        lsr 
        sta vidram+40*(3)+5+0
        lda player.SEQPOS
        and #$0f
        sta vidram+40*(3)+5+1

        lda player.PTNPOS
        and #$f0
        lsr
        lsr
        lsr
        lsr
        sta vidram+40*(5+9)+30+0

        lda player.PTNPOS
        and #$0f
        sta vidram+40*(5+9)+30+1

        ; disp 3x pat nr
part4   lda player.CURPTN+0
        and #$f0
        lsr
        lsr
        lsr
        lsr 
        sta vidram+40*(5+9)+4+0+1
        lda player.CURPTN+0
        and #$0f
        sta vidram+40*(5+9)+4+1+1

        lda player.CURPTN+7
        and #$f0
        lsr
        lsr
        lsr
        lsr 
        sta vidram+40*(5+9)+7+0+1
        lda player.CURPTN+7
        and #$0f
        sta vidram+40*(5+9)+7+1+1

        lda player.CURPTN+14
        and #$f0
        lsr
        lsr
        lsr
        lsr 
        sta vidram+40*(5+9)+10+0+1
        lda player.CURPTN+14
        and #$0f
        sta vidram+40*(5+9)+10+1+1

; disp 3x ins
part5   lda player.CURINS+0
        and #$f0
        lsr
        lsr
        lsr
        lsr 
        sta vidram+40*(5+11)+4+0+1
        lda player.CURINS+0
        and #$0f
        sta vidram+40*(5+11)+4+1+1

        lda player.CURINS+7
        and #$f0
        lsr
        lsr
        lsr
        lsr 
        sta vidram+40*(5+11)+7+0+1
        lda player.CURINS+7
        and #$0f
        sta vidram+40*(5+11)+7+1+1

        lda player.CURINS+14
        and #$f0
        lsr
        lsr
        lsr
        lsr 
        sta vidram+40*(5+11)+10+0+1
        lda player.CURINS+14
        and #$0f
        sta vidram+40*(5+11)+10+1+1

; disp 3x gate
part6   lda mutesolo
        and #1
        bne +
        lda #$20
        ldx #15
        jmp stap1

+       lda player.WFGHOST+0
        and #1
        beq +
        
        lda #$20
        ldx #$a
        jmp stap1
           
+       lda #0
        ldx #0
stap1   sta vidram+40*(5+10)+4+0+1
        sta vidram+40*(5+10)+4+1+1
        txa 
        sta $d800+40*(5+10)+4+0+1
        sta $d800+40*(5+10)+4+1+1

        lda mutesolo+1
        and #1
        bne +
        lda #$20
        ldx #15
        jmp stap2

+       lda player.WFGHOST+7
        and #1
        beq +
        lda #$20
        ldx #$a
        jmp stap2
           
+       lda #0
        ldx #0
stap2   sta vidram+40*(5+10)+7+0+1
        sta vidram+40*(5+10)+7+1+1
        txa 
        sta $d800+40*(5+10)+7+0+1
        sta $d800+40*(5+10)+7+1+1

        lda mutesolo+2
        and #1
        bne +
        lda #$20
        ldx #15
        jmp stap3

+       lda player.WFGHOST+14
        and #1
        beq +
        lda #$20
        ldx #$a
        jmp stap3
           
+       lda #0
        ldx #0
stap3   sta vidram+40*(5+10)+10+0+1
        sta vidram+40*(5+10)+10+1+1
        txa 
        sta $d800+40*(5+10)+10+0+1
        sta $d800+40*(5+10)+10+1+1
        
        lda #4
        sta $d020
        rts

.section data
    .enc 'screen'
    vessellogo .text "             <-VESSEL/SYNC=>             "
.send data

.warn format(" vessel TMP WARNING IGNORE: Vesselsubs end: %x", *)