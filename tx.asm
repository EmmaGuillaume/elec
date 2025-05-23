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
    ; Cette fonction envoie sur CMD_MATRIX l'intégralité de la matrice LED_MATRIX,
    ; Chaque bit de chaque octet encodé en largeur d'impulsion

    ; Place un pointeur au début de la matrice LED_MATRIX
    ; Voir section 10.8.12 (p. 150) de la datasheet PIC18F25K40
    MOVFF _pC + 0, WREG ; Charge le LSB du pointeur de LED_MATRIX dans WREG
    MOVWF FSR0L, 0      ; Définit le LSB du registre d'adressage indirect
    MOVFF _pC + 1, WREG ; Charge le MSB du pointeur de LED_MATRIX dans WREG
    MOVWF FSR0H, 0      ; Définit le MSB du registre d'adressage indirect
    
    ;sortie
    ;sortine numérique
    ;1 ou 0

    ; Désormais, dÃ¨s l'exécution de l'instruction suivante, la valeur pointée par <FSR0H-FSR0L> est chargée dans WREG, et <FSR0H-FSR0L> est incrémenté :
    ; MOVF POSTINC0, 0, 0

    ; Envoie la commande pour piloter chacune des 64 LEDs
    ; TODO

    RETURN

TX_1b_0:
    ; Turn on LED_CMD (RB5) and RB4
    bsf PORTB, 5       ; Set RB5
    nop
    nop
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
    nop
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
    nop
    nop

    ; Turn off LED_CMD (RB5) and RB4 again
    bcf PORTB, 5       ; Clear RB5
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    RETURN
    

    

_TRANSMISSION:
    CALL TX_1b_0
    CALL TX_1b_1
    RETURN
    