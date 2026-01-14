#ifndef BIGBINARY_H
#define BIGBINARY_H

#include <stdio.h>   // Pour printf, scanf, etc.
#include <stdlib.h>  // Pour malloc, free, etc.
#include <string.h>  // Pour strlen, strcpy, etc.

/* ===========================================================
 *  STRUCTURE PRINCIPALE
 * =========================================================== */

/**
 * Structure BigBinary : représente un grand nombre en binaire
 *
 * Cette structure permet de manipuler des nombres binaires de taille arbitraire,
 * bien plus grands que les types natifs (int, long, etc.)
 */
typedef struct {
    int *Tdigits;  // 📌 Tableau dynamique de bits (0 ou 1)
                   //    Exemple : 1011 sera stocké comme [1, 0, 1, 1]
                   //    MSB-first = Most Significant Bit en premier (poids fort à gauche)

    int Taille;    // 📌 Nombre de bits dans le tableau
                   //    Exemple : pour 1011, Taille = 4

    int Signe;     // 📌 Signe du nombre :
                   //    - 0 = positif
                   //    - 1 = négatif
                   //    (En Phase 1, on travaille uniquement avec des nombres non signés)
} BigBinary;

/* ===========================================================
 *  PHASE 1 — FONCTIONS DE BASE
 * =========================================================== */

// 🔹 INITIALISATION

/**
 * initBigBinary() : Crée un BigBinary initialisé à 0
 *
 * Utilisation : BigBinary nombre = initBigBinary();
 * Résultat : Un nombre binaire valant 0
 */
BigBinary initBigBinary();

/**
 * initBigBinaryFromString() : Crée un BigBinary depuis une chaîne binaire
 *
 * Paramètre : str = chaîne de caractères contenant uniquement '0' et '1'
 *             Exemple : "1011" représente le nombre binaire 1011 (11 en décimal)
 *
 * Utilisation : BigBinary nombre = initBigBinaryFromString("1011");
 * Résultat : Un BigBinary représentant le nombre binaire donné
 */
BigBinary initBigBinaryFromString(const char *str);

// 🔹 AFFICHAGE ET LIBÉRATION MÉMOIRE

/**
 * afficheBigBinary() : Affiche le BigBinary à l'écran
 *
 * Paramètre : A = le BigBinary à afficher
 * Affichage : Les bits du nombre, du plus significatif au moins significatif
 *             Exemple : pour 1011, affiche "1011"
 */
void afficheBigBinary(const BigBinary A);

/**
 * libereBigBinary() : Libère la mémoire allouée pour un BigBinary
 *
 * ⚠️ IMPORTANT : TOUJOURS appeler cette fonction quand vous n'avez plus besoin
 *    d'un BigBinary, sinon vous aurez des fuites mémoire (memory leaks)
 *
 * Paramètre : A = pointeur vers le BigBinary à libérer
 * Utilisation : libereBigBinary(&nombre);
 */
void libereBigBinary(BigBinary *A);

// 🔹 COMPARAISONS

/**
 * Egal() : Teste si deux BigBinary sont égaux
 *
 * Paramètres : A et B = les deux nombres à comparer
 * Retour : 1 si A == B, 0 sinon
 *
 * Exemple :
 *   A = 1011, B = 1011 → retourne 1
 *   A = 1011, B = 1010 → retourne 0
 */
int Egal(const BigBinary A, const BigBinary B);

/**
 * Inferieur() : Teste si A est strictement inférieur à B
 *
 * Paramètres : A et B = les deux nombres à comparer (non signés)
 * Retour : 1 si A < B, 0 sinon
 *
 * Exemple :
 *   A = 1010 (10), B = 1011 (11) → retourne 1
 *   A = 1011 (11), B = 1010 (10) → retourne 0
 */
int Inferieur(const BigBinary A, const BigBinary B);

// 🔹 OPÉRATIONS ARITHMÉTIQUES DE BASE

/**
 * additionBigBinary() : Addition binaire de deux BigBinary
 *
 * Paramètres : A et B = les deux nombres à additionner
 * Retour : Un nouveau BigBinary = A + B
 *
 * Algorithme : Addition bit à bit avec gestion des retenues
 * Exemple : 1011 + 0110 = 10001
 */
BigBinary additionBigBinary(const BigBinary A, const BigBinary B);

/**
 * soustractionBigBinary() : Soustraction binaire (A - B)
 *
 * ⚠️ PRÉCONDITION : A doit être supérieur ou égal à B (A ≥ B)
 *
 * Paramètres : A et B = les deux nombres (A ≥ B)
 * Retour : Un nouveau BigBinary = A - B
 *
 * Algorithme : Soustraction bit à bit avec gestion des emprunts
 * Exemple : 1011 - 0110 = 0101
 */
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B);

/* ===========================================================
 *  PHASE 2 — EXTENSIONS : Helpers, décalages, PGCD
 * =========================================================== */

// === FONCTIONS UTILITAIRES (HELPERS) ===

/**
 * estZero() : Vérifie si un BigBinary vaut 0
 *
 * Paramètre : A = le nombre à tester
 * Retour : 1 si A == 0, 0 sinon
 *
 * Utilité : Optimisation des algorithmes (éviter divisions par zéro, etc.)
 */
int estZero(const BigBinary A);

/**
 * estPair() : Vérifie si un BigBinary est pair
 *
 * Paramètre : A = le nombre à tester
 * Retour : 1 si A est pair (dernier bit = 0), 0 sinon
 *
 * Astuce : Un nombre est pair si son bit de poids faible (LSB) = 0
 * Exemple : 1010 (pair) → retourne 1, 1011 (impair) → retourne 0
 */
int estPair(const BigBinary A);

/**
 * copieBigBinary() : Crée une copie profonde d'un BigBinary
 *
 * Paramètre : A = le nombre à copier
 * Retour : Un nouveau BigBinary identique à A (mais indépendant en mémoire)
 *
 * ⚠️ IMPORTANT : La copie doit être libérée séparément avec libereBigBinary()
 */
BigBinary copieBigBinary(const BigBinary A);

// === OPÉRATIONS DE DÉCALAGE ===

/**
 * decaleGauche() : Décalage à gauche (équivalent à multiplier par 2^n)
 *
 * Paramètres :
 *   - A = le nombre à décaler
 *   - n = nombre de positions à décaler
 *
 * Retour : Un nouveau BigBinary = A × 2^n
 *
 * Exemple :
 *   decaleGauche(1011, 2) = 101100 (1011 × 4 = 44 en décimal)
 */
BigBinary decaleGauche(const BigBinary A, int n);

/**
 * decaleDroite() : Décalage à droite (équivalent à diviser par 2^n)
 *
 * Paramètres :
 *   - A = le nombre à décaler
 *   - n = nombre de positions à décaler
 *
 * Retour : Un nouveau BigBinary = A ÷ 2^n (division entière)
 *
 * Exemple :
 *   decaleDroite(1011, 2) = 10 (1011 ÷ 4 = 2 en décimal, reste ignoré)
 */
BigBinary decaleDroite(const BigBinary A, int n);

// === OPÉRATIONS ÉTENDUES ===

/**
 * soustractionAbsolue() : Calcule |A - B| (valeur absolue de la différence)
 *
 * Paramètres : A et B (pas besoin que A ≥ B)
 * Retour : Un nouveau BigBinary = |A - B|
 *
 * Utilité : Utile quand on ne connaît pas l'ordre de A et B
 * Exemple :
 *   soustractionAbsolue(1011, 1101) = |11 - 13| = 2 = 0010
 */
BigBinary soustractionAbsolue(const BigBinary A, const BigBinary B);

// === ALGORITHME DE PGCD BINAIRE ===

/**
 * pgcdBinaire() : Calcule le PGCD (Plus Grand Commun Diviseur) de A et B
 *
 * Algorithme : Algorithme de Stein (ou algorithme binaire du PGCD)
 *   → Plus efficace que l'algorithme d'Euclide en binaire
 *   → Utilise uniquement des décalages et des soustractions
 *
 * Paramètres : A et B = les deux nombres (non signés)
 * Retour : PGCD(A, B)
 *
 * Exemple :
 *   pgcdBinaire(12, 18) = 6
 *   En binaire : pgcdBinaire(1100, 10010) = 0110
 */
BigBinary pgcdBinaire(const BigBinary A, const BigBinary B);

// === OPÉRATIONS MODULAIRES ===

/**
 * BigBinary_mod() : Calcule A modulo B (reste de la division)
 *
 * ⚠️ PRÉCONDITION : B > 0 (B ne peut pas être nul)
 *
 * Paramètres : A et B
 * Retour : A mod B (reste de A ÷ B)
 *
 * Exemple :
 *   BigBinary_mod(1011, 0011) = 10 (11 mod 3 = 2)
 */
BigBinary BigBinary_mod(const BigBinary A, const BigBinary B);

/**
 * BigBinary_expMod() : Exponentiation modulaire (M^exp mod mod)
 *
 * Algorithme : Exponentiation rapide (square-and-multiply)
 *   → Calcule efficacement de grandes puissances modulo un nombre
 *   → Essentiel pour la cryptographie (RSA, etc.)
 *
 * Paramètres :
 *   - M = la base
 *   - exp = l'exposant (doit tenir sur 64 bits maximum)
 *   - mod = le modulo
 *
 * Retour : (M^exp) mod mod
 *
 * Exemple :
 *   BigBinary_expMod(5, 3, 13) = (5³) mod 13 = 125 mod 13 = 8
 *
 * ⚠️ LIMITATION : exp doit être ≤ 64 bits (suffisant pour la plupart des applications)
 */
BigBinary BigBinary_expMod(const BigBinary M, const BigBinary exp, const BigBinary mod);

// ================= PHASE 3 : RSA simplifié =================

// Chiffrement RSA : C = M^e mod N
BigBinary BigBinary_RSA_encrypt(BigBinary message, BigBinary e, BigBinary n);

// Déchiffrement RSA : M = C^d mod N
BigBinary BigBinary_RSA_decrypt(BigBinary cipher, BigBinary d, BigBinary n);

/**
 * multiplicationEgyptienne() : Multiplie deux BigBinary
 *
 * Algorithme ancien basé sur décalages et additions,
 * parfaitement adapté aux grands entiers binaires.
 *
 * Paramètres : A et B
 * Retour : A × B
 */
BigBinary multiplicationEgyptienne(const BigBinary A, const BigBinary B);



#endif // BIGBINARY_H

