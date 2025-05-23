/* --------------------------------------------------------------
 * Fichier     :   main.c
 * Auteur(s)   :
 * Description :   Affichage statique sur matrice LED 8x8
 * -------------------------------------------------------------- */
#include <xc.h>
// Configuration materielle du PIC :
#pragma config FEXTOSC = OFF           // Pas de source d'horloge externe
#pragma config RSTOSC = HFINTOSC_64MHZ // Horloge interne de 64 MHz
#pragma config WDTE = OFF              // Désactiver le watchdog
#define _XTAL_FREQ 16000000 // Frequence d'horloge - necessaire aux macros de delay

// Déclaration de fonctions et variables globales
extern void TX_64LEDS(void); // Fonction définie dans tx.asm
extern void TRANSMISSION(void);
volatile char LED_MATRIX[256]; // Définition d'une matrice de 64 x 4 octets (R/G/B/W)
volatile const char * pC = LED_MATRIX; // Pointeur vers LED_MATRIX

// Fonction pour initialiser un motif statique
void init_pattern(void) {
    // Effacer d'abord toutes les LEDs
    for (int i = 0; i < 256; i++) {
        LED_MATRIX[i] = 0;
    }

    // Ligne du haut
    for (int led = 0; led < 8; led++) {
        LED_MATRIX[led*4] = 16;      // R =
        LED_MATRIX[led*4+1] = 0;      // G
        LED_MATRIX[led*4+2] = 0;      // B
        LED_MATRIX[led*4+3] = 0;      // W
    }
    
    // Ligne du bas
    for (int led = 56; led < 64; led++) {
        LED_MATRIX[led*4] = 16;      // R
        LED_MATRIX[led*4+1] = 0;      // G
        LED_MATRIX[led*4+2] = 0;      // B
        LED_MATRIX[led*4+3] = 0;      // W
    }
    
    // Colonne de gauche
    for (int led = 0; led < 56; led += 8) {
        LED_MATRIX[led*4] = 16;      // R
        LED_MATRIX[led*4+1] = 0;      // G
        LED_MATRIX[led*4+2] = 0;      // B
        LED_MATRIX[led*4+3] = 0;      // W
    }
    
    // Colonne de droite
    for (int led = 7; led < 64; led += 8) {
        LED_MATRIX[led*4] = 16;      // R
        LED_MATRIX[led*4+1] = 0;      // G
        LED_MATRIX[led*4+2] = 0;      // B
        LED_MATRIX[led*4+3] = 0;      // W
    }

    int innerSquare[] = {18, 19, 20, 21, 26, 27, 28, 29, 34, 35, 36, 37, 42, 43, 44, 45};
    for (int i = 0; i < 16; i++) {
        int led = innerSquare[i];
        LED_MATRIX[led*4] = 0;        // R
        LED_MATRIX[led*4+1] = 16;     // G
        LED_MATRIX[led*4+2] = 0;      // B
        LED_MATRIX[led*4+3] = 0;      // W
    }
}

void transmition(void) {
    LATB |= 0b00100000;  // Allumer LED_CMD
    LATB |= 0x10;
    __delay_ms(200);

    LATB &= 0b11011111;  // Éteindre LED_CMD
    LATB &= 0xEF;
    __delay_ms(200);

    LATB |= 0b00100000;  // Allumer LED_CMD
    LATB |= 0x10;
    __delay_ms(200);

    LATB &= 0b11011111;  // Éteindre LED_CMD
    LATB &= 0xEF;
    __delay_ms(200);
}

void init(void) {
    /* Configuration des entrées / sorties */
    TRISB &= 0xEF; // LED_MASTER : OUTPUT
    TRISC &= 0x00; // LED0-7     : OUTPUT
    ANSELB &= 0b11011111; // LED_CMD : PAS_ANALOGIQUE
    TRISB &= 0b11011111; // LED_CMD : OUTPUT
    
    /* Initialisation */
    LATB &= 0xEF; // Éteindre LEDM
    LATC = 0x00;  // Éteindre LED0-7
    LATB &= 0b11011111;  // Éteindre LED_CMD
}

void main(void) {
    //init_pattern(); // Initialiser le motif de LEDs
    
    init();
    //init_pattern();
    
    /* Boucle principale */
    while(1) {
        // Envoyer les données pour allumer les LEDs
        //TX_64LEDS();
        //transmition();
        TRANSMISSION();
        
        // Allumer la LED maître pour indiquer que l'affichage est actif
        LATB |= 0x10;
    }
    
    return;
}