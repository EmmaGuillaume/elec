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

int passe_bas;
int passe_bande1;
int passe_bande2;
int passe_haut;

void motif_cool(void) {
    for (int i = 0; i < 64; i++) {
        LED_MATRIX[i*4] = 0;     // G
        LED_MATRIX[i*4+1] = 2;   // R
        LED_MATRIX[i*4+2] = 6;   // B
        LED_MATRIX[i*4+3] = 1;   // W - intensité minimale
    }
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
    
    /* Configuration des entrées analogiques */
    // AN2, AN3, AN4, AN5 en entrée analogique
    TRISA |= 0b00111100;  // bits 2 à 5 en INPUT
    ANSELA |= 0b00111100; // bits 2 à 5 en ANALOG
    
    /* Configuration du convertisseur ADC */
    ADCON0 = 0x00; // ADC OFF au début, CHS à 0000
    ADCON1 = 0x00; // Résultat justifié à droite
    ADCON2 = 0x90; // TACQ + Clock de conversion
}


unsigned int read_adc_channel(unsigned char channel) {
    unsigned int result;

    ADCON0 = (channel << 2); // Positionner CHS
    ADCON0bits.ADON = 1;     // Allumer l'ADC
    __delay_us(5);           // Temps d'acquisition
    ADCON0bits.GO = 1;       // Démarrer conversion
    while (ADCON0bits.GO);   // Attendre fin conversion
    result = (ADRESH << 8) | ADRESL;
    ADCON0bits.ADON = 0;     // Éteindre l'ADC pour éviter du bruit

    return result;
}

void read_filters(void) {
    passe_bas    = read_adc_channel(2) >> 7; // AN2 → 0..7
    passe_bande1 = read_adc_channel(3) >> 7; // AN3 → 0..7
    passe_bande2 = read_adc_channel(4) >> 7; // AN4 → 0..7
    passe_haut   = read_adc_channel(5) >> 7; // AN5 → 0..7
}

// Helper pour dessiner une colonne
void draw_column(int x, int level, char r, char g, char b) {
        for (int y = 0; y < level; y++) {
            int index = ((7 - y) * 8 + x) * 4;
            LED_MATRIX[index + 0] = g;
            LED_MATRIX[index + 1] = r;
            LED_MATRIX[index + 2] = b;
            LED_MATRIX[index + 3] = 1; // Optionnel : allumer W
        }
    }

void draw_spectrum(void) {
    // Effacer la matrice
    for (int i = 0; i < 64; i++) {
        LED_MATRIX[i*4] = 0;
        LED_MATRIX[i*4+1] = 0;
        LED_MATRIX[i*4+2] = 0;
        LED_MATRIX[i*4+3] = 0;
    }

    draw_column(0, passe_bas, 6, 0, 0);      // Rouge
    draw_column(2, passe_bande1, 6, 6, 0);  // Jaune
    draw_column(4, passe_bande2, 0, 6, 0);   // Vert
    draw_column(6, passe_haut, 0, 0, 6);     // Bleu
}

void main(void) {
    init();

    while(1) {
        for (int i=0; i<8; i++){ // -------------------------------------------------------------- DEMO CODE
            LATC = 0x01 << i;    // Commander les LEDs de test sur le PORTC ---------------------- DEMO CODE
            __delay_ms(125);     // Macro de délai ----------------------------------------------- DEMO CODE
        }
        read_filters();
        //draw_spectrum();
        //TX_64LEDS();
        
    }
}
