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

int pause = 1;
int mode = 0;

unsigned int read_adc_channel(unsigned char channel) {
    int result;

    ADPCH = channel;            // Sélectionner le canal)
    ADCON0bits.ADON = 1;        // Allumer l'ADC
    __delay_us(5);              // Temps d'acquisition
    ADCON0bits.GO = 1;          // Démarrer la conversion
    while (ADCON0bits.GO);      // Attendre la fin

    result = (ADRESH << 8) | ADRESL; // concaténation des deux registres
    int min = 300;
    int max = 1023;
    result = (result - min) * (1023 / (max - min)); // Petite formule de scale pour lisser les résultats sur une meilleure intervale

    ADCON0bits.ADON = 0;          // Éteindre ADC pour éviter le bruit
    return result;
}

void read_filters(void) {
    passe_bas    = read_adc_channel(5) >> 6;
    passe_bande1 = read_adc_channel(3) >> 6;
    passe_bande2 = read_adc_channel(2) >> 6;
    passe_haut   = read_adc_channel(4) >> 6;
}

// Helper pour dessiner une colonne
void draw_column(int x, int level, char r, char g, char b) {
    for (int y = 0; y < level; y++) {
        int index = ((7 - y) * 8 + x) * 4;
        LED_MATRIX[index + 0] = g;
        LED_MATRIX[index + 1] = r;
        LED_MATRIX[index + 2] = b;
        LED_MATRIX[index + 3] = 0; // pas de blanc parce que c'est trop douloureux pour les rétines de Emma
    }
}

void draw_spectrum_rbw(void) {
    // Effacer la matrice
    for (int i = 0; i < 64; i++) {
        LED_MATRIX[i*4] = 0;
        LED_MATRIX[i*4+1] = 0;
        LED_MATRIX[i*4+2] = 0;
        LED_MATRIX[i*4+3] = 0;
    }

    draw_column(0, passe_bas, 6, 0, 0);      // Rouge
    draw_column(1, passe_bas, 6, 0, 0);      // Rouge
    draw_column(2, passe_bande1, 6, 6, 0);  // Jaune
    draw_column(3, passe_bande1, 6, 6, 0);  // Jaune
    draw_column(4, passe_bande2, 0, 6, 0);   // Vert
    draw_column(5, passe_bande2, 0, 6, 0);   // Vert
    draw_column(6, passe_haut, 0, 0, 6);     // Bleu
    draw_column(7, passe_haut, 0, 0, 6);     // Bleu
}

void draw_spectrum_lebanese(void) {
    // Effacer la matrice
    for (int i = 0; i < 64; i++) {
        LED_MATRIX[i*4] = 0;
        LED_MATRIX[i*4+1] = 0;
        LED_MATRIX[i*4+2] = 0;
        LED_MATRIX[i*4+3] = 0;
    }

    draw_column(0, passe_bas,    6, 2, 0);
    draw_column(1, passe_bas,    6, 2, 0);
    draw_column(2, passe_bande1, 6, 4, 1);
    draw_column(3, passe_bande1, 6, 4, 1);
    draw_column(4, passe_bande2, 6, 6, 6);
    draw_column(5, passe_bande2, 6, 6, 6);
    draw_column(6, passe_haut,   6, 0, 4);
    draw_column(7, passe_haut,   6, 0, 4);
}

void draw_spectrum_aro(void) {
    // Effacer la matrice
    for (int i = 0; i < 64; i++) {
        LED_MATRIX[i*4] = 0;
        LED_MATRIX[i*4+1] = 0;
        LED_MATRIX[i*4+2] = 0;
        LED_MATRIX[i*4+3] = 0;
    }

    draw_column(0, passe_bas,    0, 6, 0);
    draw_column(1, passe_bas,    0, 6, 0);
    draw_column(2, passe_bande1, 2, 6, 2);
    draw_column(3, passe_bande1, 2, 6, 2);
    draw_column(4, passe_bande2, 6, 6, 6);
    draw_column(5, passe_bande2, 6, 6, 6);
    draw_column(6, passe_haut,   3, 3, 3);
    draw_column(7, passe_haut,   3, 3, 3);
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
    ADCON0bits.ADFM = 1; // Right justifiy
    ADCON0bits.ADCS = 1; // Clock truc 
    
    /* Configuration des boutons */
    // Bouton 0 et Bouton 1
    TRISB |= 0b11000000;  // bits 1 et 2 en INPUT
    ANSELB &= 0b00111111; // bits 1 et 2 : PAS_ANALOGIQUE
    
    read_filters();
    draw_spectrum_lebanese();
}

void main(void) {
    init();

    while(1) {
        if (pause){
            read_filters();
            switch (mode) {
                case 0:
                    draw_spectrum_lebanese();
                    break;
                case 1:
                    draw_spectrum_rbw();
                    break;
                case 2:
                    draw_spectrum_aro();
                    break;
            }
        }
        TX_64LEDS();
        
    }
}
