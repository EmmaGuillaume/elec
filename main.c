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
    
    // Configuation des entrées sorties analogiques
    // ANA2 : passe bas
    // ANA3 : passe bande low
    // ANA4 : passe bande high
    // ANA5 : passe haut
    TRISA &= 0b00111100; // ANA2, ANA3, ANA4, ANA5 : INPUT
    ANSELA &= 0b00111100; // ANA2, ANA3, ANA4, ANA5 : ANALOGIQUE
}

void read_filters(void) {
    
}

void hsv_to_rgb_scaled(unsigned int h, char *r, char *g, char *b) {
    float s = 1.0, v = 1.0; // Full saturation and brightness
    float c = v * s;
    float x = c * (1 - abs((h / 60) % 2 - 1));
    float m = v - c;
    float r_, g_, b_;

    if(h < 60)      { r_ = c; g_ = x; b_ = 0; }
    else if(h < 120){ r_ = x; g_ = c; b_ = 0; }
    else if(h < 180){ r_ = 0; g_ = c; b_ = x; }
    else if(h < 240){ r_ = 0; g_ = x; b_ = c; }
    else if(h < 300){ r_ = x; g_ = 0; b_ = c; }
    else            { r_ = c; g_ = 0; b_ = x; }

    *r = (char)(r_ * 16);
    *g = (char)(g_ * 16);
    *b = (char)(b_ * 16);
}

void motif_rainbow_wave(void) {
    static unsigned int base_hue = 0;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int index = (y * 8 + x) * 4;
            unsigned int hue = (base_hue + x * 30 + y * 15) % 360;

            char r, g, b;
            hsv_to_rgb_scaled(hue, &r, &g, &b);

            LED_MATRIX[index + 0] = g; // G
            LED_MATRIX[index + 1] = r; // R
            LED_MATRIX[index + 2] = b; // B
            LED_MATRIX[index + 3] = 0; // W (optional)
        }
    }

    base_hue = (base_hue + 3) % 360;
}

void main(void) {
    init();
    motif_cool();
    
    while(1) {
    //motif_rainbow_wave(); // update the matrix pattern
    LATB &= ~0x10;         // Turn off LED_MASTER
    TX_64LEDS();
    __delay_ms(2);
    LATB |= 0x10;          // Turn on LED_MASTER
    __delay_ms(5);        // Control speed of animation
}
}