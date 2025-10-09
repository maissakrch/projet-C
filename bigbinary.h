#ifndef BIGBINARY_H
#define BIGBINARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int *Tdigits;  // tableau de bits (0 ou 1)
    int Taille;    // taille du tableau
    int Signe;     // 0 = positif, 1 = négatif
} BigBinary;

// 🔹 Fonctions demandées
BigBinary initBigBinary();                               // initialisation vide
BigBinary initBigBinaryFromString(const char *str);      // depuis une chaîne binaire
void afficheBigBinary(const BigBinary A);                // affichage
void libereBigBinary(BigBinary *A);                      // libération mémoire

int Egal(const BigBinary A, const BigBinary B);          // égalité
int Inferieur(const BigBinary A, const BigBinary B);     // comparaison

BigBinary additionBigBinary(const BigBinary A, const BigBinary B); // addition
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B); // soustraction (A ≥ B)

#endif
