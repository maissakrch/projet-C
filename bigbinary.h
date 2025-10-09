#ifndef BIGBINARY_H
#define BIGBINARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===========================================================
 *  Structure principale
 * =========================================================== */

typedef struct {
    int *Tdigits;  // Tableau de bits (0 ou 1), MSB-first
    int Taille;    // Taille du tableau
    int Signe;     // 0 = positif, 1 = négatif (Phase 1 = non signé)
} BigBinary;

/* ===========================================================
 *  Phase 1 — Fonctions de base
 * =========================================================== */

// 🔹 Initialisation
BigBinary initBigBinary();                               // Initialise un BigBinary vide (0)
BigBinary initBigBinaryFromString(const char *str);      // Initialise depuis une chaîne binaire

// 🔹 Affichage et libération
void afficheBigBinary(const BigBinary A);                // Affiche le BigBinary sur stdout
void libereBigBinary(BigBinary *A);                      // Libère la mémoire allouée

// 🔹 Comparaisons
int Egal(const BigBinary A, const BigBinary B);          // Test d’égalité
int Inferieur(const BigBinary A, const BigBinary B);     // Test si A < B (non signé)

// 🔹 Opérations de base
BigBinary additionBigBinary(const BigBinary A, const BigBinary B);        // Addition binaire
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B);    // Soustraction (A ≥ B)

/* ===========================================================
 *  Phase 2 — Extensions : Helpers, décalages, PGCD
 * =========================================================== */

// === Helpers ===
int estZero(const BigBinary A);                          // Retourne 1 si A == 0
int estPair(const BigBinary A);                          // Retourne 1 si A est pair (dernier bit = 0)
BigBinary copieBigBinary(const BigBinary A);             // Copie profonde d’un BigBinary

// === Décalages ===
BigBinary decaleGauche(const BigBinary A, int n);        // Décalage gauche (×2^n)
BigBinary decaleDroite(const BigBinary A, int n);        // Décalage droite (÷2^n)

// === Opérations étendues ===
BigBinary soustractionAbsolue(const BigBinary A, const BigBinary B);  // Valeur absolue de (A - B)

// === PGCD binaire (algorithme de Stein) ===
BigBinary pgcdBinaire(const BigBinary A, const BigBinary B);           // Calcule PGCD(A,B) en binaire

// === Phase 2 (suite) : modulo + exp modulaire ===
BigBinary BigBinary_mod(const BigBinary A, const BigBinary B);                 // A mod B (B>0)
BigBinary BigBinary_expMod(const BigBinary M, const BigBinary exp, const BigBinary mod); // exp <= 64 bits


#endif // BIGBINARY_H
