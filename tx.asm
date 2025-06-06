#include <xc.inc>

; When assembly code is placed in a psect, it can be manipulated as a
; whole by the linker and placed in memory.
psect   txfunc,local,class=CODE,reloc=2 ; PIC18's should have a reloc (alignment) flag of 2 for any psect which contains executable code.

; -----------------------------------------------------------------
; GLOBALS
;
; DÃ©claration de fonctions et variables globales permettant au code C et Ã  l'asm de les partager
; Une mÃªme fonction ou variable cÃ´tÃ© asm est prÃ©fixÃ©e par un underscore, et ne l'est pas cÃ´tÃ© C
; Avec ce formalisme, elles sont utilisables de faÃ§on intercangeable et transparente :
; | ---- asm ----- | ------------- C ----------------- |
; | _TX_64LEDS  <--|--> void TX_64LEDS(void)           |
; | _pC         <--|--> volatile const char * pC       |
; | _LED_MATRIX <--|--> volatile char LED_MATRIX [256] |
    
; Fonction globales
global _TX_64LEDS ; Fonction dÃ©finie dans tx.asm ; Fonction permettant d'envoyer la commande pour piloter les 64 LEDs, telle que dÃ©crite dans LED_MATRIX
global _TRANSMISSION
; Constantes/variables globales
global _pC         ; Constante dÃ©finie dans main.c ; Pointeur vers LED_MATRIX
global _LED_MATRIX ; Variable  dÃ©finie dans main.c ; Tableau (256 octets = 64 x 4) des composantes RGBW de la matrice LED (1 octet/couleur/LED)

_TX_64LEDS:
    ; Initialiser le pointeur FSR0 vers LED_MATRIX
    MOVFF _pC + 0, WREG
    MOVWF FSR0L, 0
    MOVFF _pC + 1, WREG
    MOVWF FSR0H, 0

    REPT 256
	CALL TX_1octet
    ENDM
    
    RETURN
    
TX_1octet:
    movf POSTINC0, W     ; charger *FSR0 dans WREG et incrémenter FSR0

    ; Bit 7 à 0
    btfsc WREG, 7
    goto send_1
    call TX_1b_0
    goto next6
send_1:
    call TX_1b_1
next6:

    btfsc WREG, 6
    goto send_2
    call TX_1b_0
    goto next5
send_2:
    call TX_1b_1
next5:

    btfsc WREG, 5
    goto send_3
    call TX_1b_0
    goto next4
send_3:
    call TX_1b_1
next4:

    btfsc WREG, 4
    goto send_4
    call TX_1b_0
    goto next3
send_4:
    call TX_1b_1
next3:

    btfsc WREG, 3
    goto send_5
    call TX_1b_0
    goto next2
send_5:
    call TX_1b_1
next2:

    btfsc WREG, 2
    goto send_6
    call TX_1b_0
    goto next1
send_6:
    call TX_1b_1
next1:

    btfsc WREG, 1
    goto send_7
    call TX_1b_0
    goto next0
send_7:
    call TX_1b_1
next0:

    btfsc WREG, 0
    goto send_8
    call TX_1b_0
    goto end_tx
send_8:
    call TX_1b_1
end_tx:

    return

TX_1b_0:
    ; Turn on LED_CMD (RB5) and RB4
    bsf PORTB, 5       ; Set RB5
    nop
    nop
    nop

    ; Turn off LED_CMD (RB5) and RB4
    bcf PORTB, 5       ; Clear RB5
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    
    RETURN
    
TX_1b_1:
    ; Turn on LED_CMD (RB5) and RB4 again
    bsf PORTB, 5       ; Set RB5
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    ; Turn off LED_CMD (RB5) and RB4 again
    bcf PORTB, 5       ; Clear RB5

    RETURN

_TRANSMISSION:
    CALL TX_1b_0
    CALL TX_1b_1
    RETURN
    