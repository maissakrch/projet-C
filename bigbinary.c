#include "bigbinary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===========================================================
 *  HELPERS INTERNES (Fonctions utilitaires privées)
 * =========================================================== */

/**
 * normalizeBigBinary - Normalise un nombre binaire
 *
 * RÔLE : Cette fonction "nettoie" un BigBinary en :
 *   1. Supprimant les zéros de tête (MSB = Most Significant Bit)
 *   2. Gérant le cas spécial du zéro (un seul bit à 0)
 *
 * EXEMPLE : "00001101" devient "1101"
 *           "00000000" devient "0"
 *
 * FORMAT : MSB-first signifie que le bit le plus significatif est en premier
 *          Exemple : [1,0,1] représente 101₂ = 5₁₀
 *
 * @param A : Pointeur vers le BigBinary à normaliser
 */
static void normalizeBigBinary(BigBinary *A) {
    // Vérification de sécurité : pointeur NULL
    if (A == NULL) return;

    // CAS 1 : Structure vide ou invalide → transformer en zéro canonique
    if (A->Tdigits == NULL || A->Taille <= 0) {
        free(A->Tdigits);
        A->Tdigits = (int*)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;  // 0 est toujours positif
        return;
    }

    // CAS 2 : Trouver le premier bit non nul (en partant du MSB/gauche)
    // On cherche où commence vraiment le nombre
    int i = 0;
    while (i < A->Taille - 1 && A->Tdigits[i] == 0) i++;

    // CAS 3 : Si tous les bits sont à 0 → zéro canonique
    if (i == A->Taille - 1 && A->Tdigits[i] == 0) {
        free(A->Tdigits);
        A->Tdigits = (int*)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;
        return;
    }

    // CAS 4 : Il y a des zéros de tête à supprimer
    // Exemple : [0,0,1,0,1] → [1,0,1]
    if (i > 0) {
        int newLen = A->Taille - i;  // Nouvelle taille
        int *nd = (int*)malloc(newLen * sizeof(int));

        // Copier uniquement la partie utile (à partir de l'indice i)
        memcpy(nd, A->Tdigits + i, newLen * sizeof(int));

        // Libérer l'ancien tableau et utiliser le nouveau
        free(A->Tdigits);
        A->Tdigits = nd;
        A->Taille = newLen;
    }

    // CONVENTION : Le zéro est toujours positif (déjà géré ci-dessus)
}

/* ===========================================================
 *  PHASE 1 — FONCTIONS DE BASE
 *  Ces fonctions permettent de créer, afficher et manipuler
 *  les BigBinary de manière élémentaire
 * =========================================================== */

/**
 * initBigBinary - Crée un BigBinary valant zéro
 *
 * RÔLE : Initialise un nombre binaire à sa valeur par défaut : 0
 *
 * STRUCTURE BigBinary :
 *   - Tdigits : tableau de bits (0 ou 1)
 *   - Taille : nombre de bits
 *   - Signe : 0 = positif, 1 = négatif
 *
 * @return : Un BigBinary représentant 0 (un seul bit à 0)
 */
BigBinary initBigBinary() {
    BigBinary A;
    A.Tdigits = (int*)malloc(sizeof(int));  // Alloue 1 int
    A.Tdigits[0] = 0;                        // Met le bit à 0
    A.Taille = 1;                            // Taille = 1 bit
    A.Signe  = 0;                            // Positif
    return A;
}

/**
 * initBigBinaryFromString - Crée un BigBinary depuis une chaîne binaire
 *
 * RÔLE : Convertit une chaîne de caractères (ex: "1011", "-101") en BigBinary
 *
 * GESTION :
 *   - Espaces/tabulations/retours à la ligne sont ignorés
 *   - Signe '+' ou '-' au début est accepté
 *   - Seuls '0' et '1' sont valides comme chiffres
 *
 * EXEMPLES :
 *   "1011"     → [1,0,1,1] = 11₁₀
 *   "  -101  " → [1,0,1] avec Signe=1 = -5₁₀
 *   "00101"    → [1,0,1] = 5₁₀ (normalisé)
 *
 * @param str : Chaîne de caractères représentant le nombre binaire
 * @return : BigBinary correspondant (ou 0 si erreur)
 */
BigBinary initBigBinaryFromString(const char *str) {
    // CAS 1 : Chaîne NULL ou vide → retourne 0
    if (str == NULL || strlen(str) == 0) {
        return initBigBinary();
    }

    // CAS 2 : Gérer les espaces et le signe optionnel
    int signe = 0;   // 0 = positif, 1 = négatif
    int i0 = 0;      // Indice de début après espaces et signe

    // Sauter les espaces/tabs/newlines au début
    while (str[i0] == ' ' || str[i0] == '\t' || str[i0] == '\n') i0++;

    // Détecter le signe
    if (str[i0] == '-') {
        signe = 1;
        i0++;
    }
    else if (str[i0] == '+') {
        i0++;
    }

    // CAS 3 : Compter les chiffres binaires valides ('0' et '1')
    int count = 0;
    for (int i = i0; str[i] != '\0'; ++i) {
        if (str[i] == '0' || str[i] == '1') {
            count++;  // Chiffre binaire valide
        }
        else if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') {
            continue;  // Espaces ignorés
        }
        else {
            // Caractère invalide détecté
            fprintf(stderr, "Erreur: caractère invalide '%c' dans la chaîne binaire\n", str[i]);
            return initBigBinary();
        }
    }

    // CAS 4 : Aucun chiffre valide → retourne 0
    if (count == 0) {
        return initBigBinary();
    }

    // CAS 5 : Créer le BigBinary avec les chiffres trouvés
    BigBinary A;
    A.Taille  = count;
    A.Signe   = signe;
    A.Tdigits = (int*)malloc(A.Taille * sizeof(int));

    // Remplir le tableau avec les bits (0 ou 1)
    int k = 0;
    for (int i = i0; str[i] != '\0'; ++i) {
        if (str[i] == '0' || str[i] == '1') {
            A.Tdigits[k++] = (str[i] == '1') ? 1 : 0;
        }
    }

    // Normaliser (supprimer les zéros de tête)
    normalizeBigBinary(&A);
    return A;
}

/**
 * afficheBigBinary - Affiche un BigBinary sur la console
 *
 * RÔLE : Imprime le nombre binaire en format lisible
 *
 * FORMAT : [signe] + chiffres binaires + newline
 * EXEMPLE : -1011\n pour -11₁₀
 *
 * @param A : Le BigBinary à afficher
 */
void afficheBigBinary(const BigBinary A) {
    // Afficher le signe si négatif
    if (A.Signe) printf("-");

    // Afficher chaque bit
    for (int i = 0; i < A.Taille; ++i) {
        printf("%d", A.Tdigits[i]);
    }
    printf("\n");
}

/**
 * libereBigBinary - Libère la mémoire d'un BigBinary
 *
 * RÔLE : Évite les fuites mémoire en libérant le tableau dynamique
 *
 * IMPORTANT : Après appel, le BigBinary ne doit plus être utilisé
 *             (le tableau est libéré et pointeur mis à NULL)
 *
 * @param A : Pointeur vers le BigBinary à libérer
 */
void libereBigBinary(BigBinary *A) {
    if (!A) return;  // Sécurité : pointeur NULL

    // Libérer le tableau de bits
    if (A->Tdigits) free(A->Tdigits);

    // Réinitialiser la structure
    A->Tdigits = NULL;
    A->Taille  = 0;
    A->Signe   = 0;
}

/**
 * Egal - Teste l'égalité de deux BigBinary
 *
 * RÔLE : Vérifie si A == B (même valeur, même signe)
 *
 * PRÉCONDITION : Les BigBinary doivent être normalisés
 *
 * LOGIQUE :
 *   1. Si tailles différentes → pas égaux
 *   2. Si signes différents → pas égaux
 *   3. Comparer bit par bit
 *
 * @param A, B : Les BigBinary à comparer
 * @return : 1 si égaux, 0 sinon
 */
int Egal(const BigBinary A, const BigBinary B) {
    // Test 1 : Tailles différentes
    if (A.Taille != B.Taille) return 0;

    // Test 2 : Signes différents
    if (A.Signe  != B.Signe)  return 0;

    // Test 3 : Comparer chaque bit
    for (int i = 0; i < A.Taille; ++i) {
        if (A.Tdigits[i] != B.Tdigits[i]) return 0;
    }

    return 1;  // Tous les tests passés → égaux
}

/**
 * Inferieur - Teste si A < B (comparaison non signée)
 *
 * RÔLE : Compare deux nombres binaires comme des entiers positifs
 *
 * ALGORITHME :
 *   1. Plus court → plus petit (101 < 1011)
 *   2. Même taille → comparer bit par bit de gauche à droite (MSB first)
 *
 * EXEMPLE :
 *   1010 < 1100 ? → compare MSB : 1==1, puis 0<1 → OUI
 *   111 < 1000 ? → tailles : 3<4 → OUI
 *
 * @param A, B : Les BigBinary à comparer
 * @return : 1 si A < B, 0 sinon
 */
int Inferieur(const BigBinary A, const BigBinary B) {
    // Cas 1 : A a moins de bits → A < B
    if (A.Taille < B.Taille) return 1;

    // Cas 2 : A a plus de bits → A > B
    if (A.Taille > B.Taille) return 0;

    // Cas 3 : Même taille → comparer bit par bit (MSB first)
    for (int i = 0; i < A.Taille; ++i) {
        if (A.Tdigits[i] < B.Tdigits[i]) return 1;  // A < B
        if (A.Tdigits[i] > B.Tdigits[i]) return 0;  // A > B
    }

    return 0;  // Égaux → A n'est pas < B
}

/**
 * additionBigBinary - Addition de deux BigBinary
 *
 * RÔLE : Calcule A + B en binaire (comme addition de nombres positifs)
 *
 * ALGORITHME : Addition classique avec retenue (carry)
 *   - Parcours de droite à gauche (LSB → MSB)
 *   - À chaque position : bit_A + bit_B + retenue
 *   - Si résultat ≥ 2 : on garde le bit de poids faible et propage la retenue
 *
 * EXEMPLE :
 *     1011 (11)
 *   + 0111 (7)
 *   -------
 *    10010 (18)
 *
 * DÉTAIL :
 *   Position 0 (LSB) : 1+1+0 = 2 → bit=0, carry=1
 *   Position 1       : 1+1+1 = 3 → bit=1, carry=1
 *   Position 2       : 0+1+1 = 2 → bit=0, carry=1
 *   Position 3       : 1+0+1 = 2 → bit=0, carry=1
 *   Position 4 (MSB) : 0+0+1 = 1 → bit=1, carry=0
 *
 * @param A, B : Les BigBinary à additionner
 * @return : Résultat A + B (normalisé)
 */
BigBinary additionBigBinary(const BigBinary A, const BigBinary B) {
    // Déterminer la taille maximale
    int n = (A.Taille > B.Taille) ? A.Taille : B.Taille;

    // Créer le résultat (taille max + 1 pour la retenue finale)
    BigBinary R;
    R.Taille  = n + 1;
    R.Signe   = 0;  // Non signé en Phase 1
    R.Tdigits = (int*)calloc(R.Taille, sizeof(int));  // Initialise à 0

    int carry = 0;  // Retenue initiale

    // Boucle de droite à gauche (LSB → MSB)
    for (int i = 0; i < n; ++i) {
        // Récupérer le bit de A (ou 0 si hors bornes)
        int abit = (A.Taille - 1 - i >= 0) ? A.Tdigits[A.Taille - 1 - i] : 0;

        // Récupérer le bit de B (ou 0 si hors bornes)
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;

        // Addition : bit_A + bit_B + retenue
        int sum = abit + bbit + carry;

        // Le bit résultat est le bit de poids faible de sum
        R.Tdigits[R.Taille - 1 - i] = sum & 1;  // sum % 2

        // La retenue est le bit de poids fort de sum
        carry = (sum >> 1);  // sum / 2
    }

    // Placer la retenue finale (MSB)
    R.Tdigits[0] = carry;

    // Normaliser (supprimer les zéros de tête)
    normalizeBigBinary(&R);
    return R;
}

/**
 * soustractionBigBinary - Soustraction A - B
 *
 * RÔLE : Calcule A - B en binaire
 *
 * PRÉCONDITION CRITIQUE : A >= B (sinon erreur)
 *
 * ALGORITHME : Soustraction classique avec emprunt (borrow)
 *   - Parcours de droite à gauche (LSB → MSB)
 *   - À chaque position : bit_A - bit_B - emprunt
 *   - Si résultat < 0 : on emprunte 2 et propage l'emprunt
 *
 * EXEMPLE :
 *     1011 (11)
 *   - 0101 (5)
 *   -------
 *     0110 (6)
 *
 * DÉTAIL :
 *   Position 0 (LSB) : 1-1-0 = 0 → bit=0, borrow=0
 *   Position 1       : 1-0-0 = 1 → bit=1, borrow=0
 *   Position 2       : 0-1-0 = -1 → bit=1 (0-1+2), borrow=1
 *   Position 3       : 1-0-1 = 0 → bit=0, borrow=0
 *
 * @param A, B : Les BigBinary (A doit être >= B)
 * @return : Résultat A - B (normalisé)
 */
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B) {
    // VÉRIFICATION : A doit être >= B
    if (Inferieur(A, B)) {
        fprintf(stderr, "ERREUR: A < B dans soustractionBigBinary (précondition non respectée)\n");
        return initBigBinary();  // Retourne 0 par défaut
    }

    // Créer le résultat (même taille que A)
    BigBinary R;
    R.Taille  = A.Taille;
    R.Signe   = 0;  // Non signé en Phase 1
    R.Tdigits = (int*)calloc(R.Taille, sizeof(int));  // Initialise à 0

    int borrow = 0;  // Emprunt initial

    // Boucle de droite à gauche (LSB → MSB)
    for (int i = 0; i < A.Taille; ++i) {
        // Bit de A
        int abit = A.Tdigits[A.Taille - 1 - i];

        // Bit de B (ou 0 si hors bornes)
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;

        // Soustraction : bit_A - bit_B - emprunt
        int diff = abit - bbit - borrow;

        // Si diff < 0, on emprunte 2 (en binaire)
        if (diff < 0) {
            diff += 2;      // diff devient 0 ou 1
            borrow = 1;     // Propagation de l'emprunt
        }
        else {
            borrow = 0;     // Pas d'emprunt
        }

        // Placer le bit résultat
        R.Tdigits[R.Taille - 1 - i] = diff;
    }

    // Normaliser (supprimer les zéros de tête)
    normalizeBigBinary(&R);
    return R;
}

/* ===========================================================
 *  PHASE 2 — HELPERS & OPÉRATIONS ÉTENDUES
 *  Fonctions avancées pour manipulation et algorithmes complexes
 * =========================================================== */

/**
 * estZero - Teste si un BigBinary vaut zéro
 *
 * RÔLE : Vérifie si tous les bits sont à 0
 *
 * @param A : Le BigBinary à tester
 * @return : 1 si A == 0, 0 sinon
 */
int estZero(const BigBinary A) {
    // Parcourir tous les bits
    for (int i = 0; i < A.Taille; ++i) {
        if (A.Tdigits[i] != 0) return 0;  // Un bit non nul trouvé
    }
    return 1;  // Tous les bits sont à 0
}

/**
 * estPair - Teste si un BigBinary est pair
 *
 * RÔLE : Un nombre est pair si son dernier bit (LSB) est 0
 *
 * FORMAT MSB-FIRST : Le LSB est le dernier élément du tableau
 *
 * EXEMPLE :
 *   1010 → LSB = 0 → pair
 *   1011 → LSB = 1 → impair
 *
 * @param A : Le BigBinary à tester
 * @return : 1 si pair, 0 si impair
 */
int estPair(const BigBinary A) {
    // Sécurité
    if (A.Taille <= 0 || A.Tdigits == NULL) return 1;

    // Le LSB est le dernier élément
    return (A.Tdigits[A.Taille - 1] == 0);
}

/**
 * copieBigBinary - Crée une copie profonde d'un BigBinary
 *
 * RÔLE : Clone complètement un BigBinary (nouveau tableau alloué)
 *
 * IMPORTANT : L'appelant doit libérer la copie avec libereBigBinary()
 *
 * @param A : Le BigBinary à copier
 * @return : Une copie indépendante de A
 */
BigBinary copieBigBinary(const BigBinary A) {
    BigBinary C;
    C.Taille  = A.Taille;
    C.Signe   = A.Signe;

    // Allouer un nouveau tableau
    C.Tdigits = (int*)malloc(C.Taille * sizeof(int));

    // Copier tous les bits
    memcpy(C.Tdigits, A.Tdigits, C.Taille * sizeof(int));

    return C;
}

/**
 * decaleGauche - Décalage à gauche de n positions (multiplication par 2^n)
 *
 * RÔLE : Équivalent à multiplier par 2^n
 *
 * OPÉRATION : Ajoute n zéros à droite (côté LSB)
 *
 * EXEMPLE :
 *   decaleGauche(101, 2) = 10100
 *   101₂ × 2² = 5 × 4 = 20 = 10100₂
 *
 * DANS LE TABLEAU MSB-FIRST :
 *   [1,0,1] devient [1,0,1,0,0]
 *
 * @param A : Le BigBinary à décaler
 * @param n : Nombre de positions (bits) de décalage
 * @return : Résultat A << n (normalisé)
 */
BigBinary decaleGauche(const BigBinary A, int n) {
    // Cas triviaux : pas de décalage ou A = 0
    if (n <= 0 || estZero(A)) return copieBigBinary(A);

    // Créer le résultat (taille augmentée de n)
    BigBinary R;
    R.Taille  = A.Taille + n;
    R.Signe   = A.Signe;
    R.Tdigits = (int*)malloc(R.Taille * sizeof(int));

    // Copier A au début (MSB)
    memcpy(R.Tdigits, A.Tdigits, A.Taille * sizeof(int));

    // Ajouter n zéros à la fin (LSB)
    memset(R.Tdigits + A.Taille, 0, n * sizeof(int));

    normalizeBigBinary(&R);
    return R;
}

/**
 * decaleDroite - Décalage à droite de n positions (division par 2^n)
 *
 * RÔLE : Équivalent à diviser par 2^n (division entière)
 *
 * OPÉRATION : Supprime n bits à droite (côté LSB)
 *
 * EXEMPLE :
 *   decaleDroite(10110, 2) = 101
 *   10110₂ ÷ 2² = 22 ÷ 4 = 5 = 101₂
 *
 * DANS LE TABLEAU MSB-FIRST :
 *   [1,0,1,1,0] devient [1,0,1]
 *
 * @param A : Le BigBinary à décaler
 * @param n : Nombre de positions (bits) de décalage
 * @return : Résultat A >> n (normalisé)
 */
BigBinary decaleDroite(const BigBinary A, int n) {
    // Cas trivial : pas de décalage
    if (n <= 0) return copieBigBinary(A);

    // Si décalage >= taille → résultat = 0
    if (n >= A.Taille) {
        return initBigBinary();
    }

    // Créer le résultat (taille réduite de n)
    BigBinary R;
    R.Taille  = A.Taille - n;
    R.Signe   = A.Signe;
    R.Tdigits = (int*)malloc(R.Taille * sizeof(int));

    // Copier uniquement la partie MSB (on supprime les n derniers bits)
    memcpy(R.Tdigits, A.Tdigits, R.Taille * sizeof(int));

    normalizeBigBinary(&R);
    return R;
}

/**
 * soustractionAbsolue - Calcule |A - B| (valeur absolue)
 *
 * RÔLE : Soustraction sans se soucier de l'ordre
 *
 * LOGIQUE :
 *   - Si A >= B → retourne A - B
 *   - Si A < B → retourne B - A
 *
 * @param A, B : Les BigBinary
 * @return : |A - B| (toujours positif)
 */
BigBinary soustractionAbsolue(const BigBinary A, const BigBinary B) {
    if (Inferieur(A, B)) {
        return soustractionBigBinary(B, A);  // B - A
    } else {
        return soustractionBigBinary(A, B);  // A - B
    }
}

/**
 * countTrailingZeros - Compte les zéros de fin (trailing zeros)
 *
 * RÔLE : Compte combien de zéros consécutifs il y a à droite (LSB)
 *
 * USAGE : Pour l'algorithme de Stein (PGCD binaire)
 *
 * EXEMPLE :
 *   1011000 → 3 trailing zeros
 *   1010101 → 0 trailing zeros
 *
 * @param A : Le BigBinary
 * @return : Nombre de zéros de fin
 */
static int countTrailingZeros(const BigBinary A) {
    int c = 0;
    // Parcourir de droite à gauche (LSB → MSB)
    for (int i = A.Taille - 1; i >= 0; --i) {
        if (A.Tdigits[i] == 0) {
            c++;  // Incrémenter le compteur
        }
        else {
            break;  // Dès qu'on trouve un 1, on s'arrête
        }
    }
    return c;
}

/**
 * rshift1 - Décalage à droite d'une position (divise par 2)
 *
 * RÔLE : Helper pour simplifier le code (équivalent à A >> 1)
 */
static BigBinary rshift1(const BigBinary A) {
    return decaleDroite(A, 1);
}

/**
 * lshiftK - Décalage à gauche de k positions (multiplie par 2^k)
 *
 * RÔLE : Helper pour simplifier le code (équivalent à A << k)
 */
static BigBinary lshiftK(const BigBinary A, int k) {
    return decaleGauche(A, k);
}

/**
 * pgcdBinaire - PGCD (Plus Grand Commun Diviseur) par l'algorithme de Stein
 *
 * RÔLE : Calcule le PGCD de deux nombres binaires efficacement
 *
 * ALGORITHME DE STEIN (algorithme binaire) :
 *   C'est une alternative à l'algorithme d'Euclide, optimisée pour les ordinateurs
 *   car elle n'utilise que des décalages, comparaisons et soustractions
 *
 * PRINCIPE :
 *   1. Si X = 0 → PGCD = Y
 *   2. Si Y = 0 → PGCD = X
 *   3. Extraire les facteurs de 2 communs (trailing zeros)
 *   4. Rendre X et Y impairs (diviser par 2 jusqu'à ce qu'impairs)
 *   5. Boucle : soustraire le plus petit du plus grand, diviser par 2 si pair
 *   6. Réappliquer les facteurs de 2 extraits au début
 *
 * EXEMPLE : PGCD(48, 18)
 *   48 = 110000₂, 18 = 10010₂
 *   Trailing zeros : 4 pour 48, 1 pour 18 → k=1 (minimum)
 *   Après division : 1100 et 1001
 *   ... itérations ...
 *   Résultat × 2¹ = 6
 *
 * @param A, B : Les BigBinary
 * @return : PGCD(A, B) (l'appelant doit libérer)
 */
BigBinary pgcdBinaire(const BigBinary A, const BigBinary B) {
    // ÉTAPE 1 : Créer des copies de travail (modifiables)
    BigBinary X = copieBigBinary(A);
    normalizeBigBinary(&X);
    BigBinary Y = copieBigBinary(B);
    normalizeBigBinary(&Y);

    // ÉTAPE 2 : Cas de base
    if (estZero(X)) {
        libereBigBinary(&X);
        return Y;  // PGCD(0, Y) = Y
    }
    if (estZero(Y)) {
        libereBigBinary(&Y);
        return X;  // PGCD(X, 0) = X
    }

    // ÉTAPE 3 : Extraire les facteurs de 2 communs
    int kx = countTrailingZeros(X);  // X = X' × 2^kx
    int ky = countTrailingZeros(Y);  // Y = Y' × 2^ky
    int k  = (kx < ky) ? kx : ky;    // k = min(kx, ky)

    // ÉTAPE 4 : Diviser X et Y par 2^kx et 2^ky (rendre impairs)
    BigBinary t;
    t = decaleDroite(X, kx);
    libereBigBinary(&X);
    X = t;

    t = decaleDroite(Y, ky);
    libereBigBinary(&Y);
    Y = t;

    // ÉTAPE 5 : Rendre X impair (sécurité, normalement déjà fait)
    while (estPair(X)) {
        t = rshift1(X);
        libereBigBinary(&X);
        X = t;
    }

    // ÉTAPE 6 : Boucle principale de l'algorithme de Stein
    while (!estZero(Y)) {
        // 6a. Rendre Y impair
        while (estPair(Y)) {
            t = rshift1(Y);
            libereBigBinary(&Y);
            Y = t;
        }

        // 6b. S'assurer que X <= Y (échanger si nécessaire)
        // Cette logique complexe vérifie et échange X/Y pour avoir X <= Y
        if (Inferieur(Y, X) == 0 && Egal(Y, X) == 0) {
            // Y >= X et Y != X → Y > X
            if (Inferieur(X, Y) == 0) {
                // X >= Y → swap
                BigBinary tmp = X;
                X = Y;
                Y = tmp;
            }
        } else if (Inferieur(X, Y) == 0) {
            // X >= Y → swap
            BigBinary tmp = X;
            X = Y;
            Y = tmp;
        }

        // 6c. Y = Y - X (deviendra pair, sera divisé par 2 au prochain tour)
        t = soustractionAbsolue(Y, X);
        libereBigBinary(&Y);
        Y = t;
    }

    // ÉTAPE 7 : Réappliquer les facteurs de 2 extraits (multiplier par 2^k)
    BigBinary G = lshiftK(X, k);
    libereBigBinary(&X);
    libereBigBinary(&Y);  // Y == 0 à ce stade

    normalizeBigBinary(&G);
    return G;
}

/**
 * BigBinary_mod - Calcule A modulo B (A mod B)
 *
 * RÔLE : Reste de la division de A par B
 *
 * ALGORITHME : Soustraction répétée avec alignement
 *   On soustrait B décalé à gauche (B×2^k) tant que possible
 *   pour se rapprocher rapidement de 0
 *
 * PRINCIPE :
 *   1. Si A < B → résultat = A
 *   2. Sinon, trouver le plus grand k tel que B×2^k <= A
 *   3. Soustraire B×2^k de A
 *   4. Répéter jusqu'à A < B
 *
 * EXEMPLE : 19 mod 5
 *   19 = 10011₂, 5 = 101₂
 *   k=2 : 5×4=10100₂ > 19 → non
 *   k=1 : 5×2=1010₂ <= 19 → 19-10=1001₂ (9)
 *   k=1 : 5×2=1010₂ > 9 → non
 *   k=0 : 5×1=101₂ <= 9 → 9-5=100₂ (4)
 *   Résultat : 4
 *
 * @param A : Le dividende
 * @param B : Le diviseur (doit être > 0)
 * @return : A mod B (normalisé)
 */
BigBinary BigBinary_mod(const BigBinary A, const BigBinary B) {
    // CAS 1 : Division par zéro
    if (estZero(B)) {
        fprintf(stderr, "Erreur: modulo par zero\n");
        return initBigBinary();
    }

    // CAS 2 : Copier A comme reste initial
    BigBinary R = copieBigBinary(A);

    // CAS 3 : Si A < B, le reste est déjà A
    if (Inferieur(R, B)) return R;

    // CAS 4 : Soustraction répétée avec alignement
    int maxShift = R.Taille - B.Taille;  // Décalage maximum possible

    for (int k = maxShift; k >= 0; --k) {
        // Calculer B × 2^k
        BigBinary Bk = decaleGauche(B, k);

        // Si R >= B×2^k, soustraire
        if (!Inferieur(R, Bk)) {
            BigBinary tmp = soustractionBigBinary(R, Bk);
            libereBigBinary(&R);
            R = tmp;
        }

        libereBigBinary(&Bk);

        // Optimisation : si R = 0, on peut arrêter
        if (estZero(R)) break;
    }

    return R;  // Normalisé par soustractionBigBinary
}

/**
 * add_mod - Addition modulaire : (X + Y) mod mod
 *
 * RÔLE : Additionne puis prend le modulo (évite les débordements)
 *
 * @param X, Y : Opérandes
 * @param mod : Le modulo
 * @return : (X + Y) mod mod
 */
static BigBinary add_mod(const BigBinary X, const BigBinary Y, const BigBinary mod) {
    BigBinary s = additionBigBinary(X, Y);  // X + Y
    BigBinary r = BigBinary_mod(s, mod);    // (X + Y) mod mod
    libereBigBinary(&s);
    return r;
}

/**
 * lshift1_mod - Décalage gauche modulaire : (X × 2) mod mod
 *
 * RÔLE : Multiplie par 2 puis prend le modulo
 *
 * @param X : Opérande
 * @param mod : Le modulo
 * @return : (X × 2) mod mod
 */
static BigBinary lshift1_mod(const BigBinary X, const BigBinary mod) {
    BigBinary d = decaleGauche(X, 1);       // X × 2
    BigBinary r = BigBinary_mod(d, mod);    // (X × 2) mod mod
    libereBigBinary(&d);
    return r;
}

/**
 * BigBinary_mul_mod - Multiplication modulaire : (X × Y) mod mod
 *
 * RÔLE : Multiplie deux BigBinary modulo un troisième (sans débordement)
 *
 * ALGORITHME : Multiplication "schoolbook" par décalages et additions
 *   C'est équivalent à la multiplication que l'on apprend à l'école,
 *   mais en binaire et avec modulo à chaque étape
 *
 * PRINCIPE :
 *   X × Y = X × (somme des bits de Y × leur poids en puissance de 2)
 *
 *   On traite Y bit par bit :
 *   - Si bit = 1 : ajouter X décalé de la position du bit
 *   - Puis décaler X d'une position (multiplier par 2)
 *
 * EXEMPLE : 5 × 3 mod 7
 *   5 = 101₂, 3 = 11₂
 *   res = 0, a = 5
 *   bit 0 de 3 = 1 : res = (0 + 5) mod 7 = 5, a = (5×2) mod 7 = 3
 *   bit 1 de 3 = 1 : res = (5 + 3) mod 7 = 1
 *   Résultat : 1 (car 5×3 = 15 = 2×7 + 1)
 *
 * @param X, Y : Les opérandes
 * @param mod : Le modulo
 * @return : (X × Y) mod mod
 */
static BigBinary BigBinary_mul_mod(const BigBinary X, const BigBinary Y, const BigBinary mod) {
    // Initialisation
    BigBinary a = BigBinary_mod(X, mod);    // a = X mod mod
    BigBinary b = copieBigBinary(Y);        // b = Y (sera divisé par 2 à chaque tour)
    BigBinary res = initBigBinary();        // res = 0 (accumulateur)

    // Boucle : tant que b != 0
    while (!estZero(b)) {
        // Si b est impair (bit de poids faible = 1)
        if (!estPair(b)) {
            // res = (res + a) mod mod
            BigBinary tmp = add_mod(res, a, mod);
            libereBigBinary(&res);
            res = tmp;
        }

        // a = (a × 2) mod mod (décalage gauche)
        BigBinary a2 = lshift1_mod(a, mod);
        libereBigBinary(&a);
        a = a2;

        // b = b >> 1 (diviser par 2)
        BigBinary b2 = decaleDroite(b, 1);
        libereBigBinary(&b);
        b = b2;
    }

    libereBigBinary(&a);
    libereBigBinary(&b);
    return res;
}

/**
 * to_u64 - Convertit un BigBinary en uint64_t
 *
 * RÔLE : Pour les opérations nécessitant un entier natif (< 64 bits)
 *
 * LIMITE : Ne fonctionne que si le BigBinary a au plus 64 bits
 *
 * @param E : Le BigBinary à convertir
 * @param out : Pointeur vers la variable recevant le résultat
 * @return : 1 si succès, 0 si E > 64 bits
 */
static int to_u64(const BigBinary E, unsigned long long *out) {
    // Vérification : pas plus de 64 bits
    if (E.Taille > 64) return 0;

    unsigned long long v = 0ULL;

    // Construire le nombre bit par bit
    for (int i = 0; i < E.Taille; ++i) {
        v = (v << 1) | (unsigned long long)(E.Tdigits[i] ? 1 : 0);
    }

    *out = v;
    return 1;  // Succès
}

/**
 * BigBinary_expMod - Exponentiation modulaire : (M^exp) mod mod
 *
 * RÔLE : Calcule M puissance exp modulo mod (TRÈS efficace)
 *
 * ALGORITHME : "Square-and-multiply" (carré et multiplie)
 *   C'est l'algorithme standard pour les grandes exponentiations
 *   (utilisé en cryptographie : RSA, Diffie-Hellman, etc.)
 *
 * PRINCIPE :
 *   On décompose l'exposant en binaire :
 *   exp = sum(bit_i × 2^i) pour i de 0 à n
 *
 *   Donc M^exp = M^(sum(bit_i × 2^i)) = produit(M^(2^i))^bit_i
 *
 *   À chaque étape :
 *   - Si bit_i = 1 : multiplier le résultat par base^(2^i)
 *   - base^(2^(i+1)) = (base^(2^i))² (d'où "square")
 *
 * EXEMPLE : 3^13 mod 7
 *   13 = 1101₂
 *   base = 3, result = 1
 *
 *   bit 0 = 1 : result = 1×3 = 3, base = 3² = 9 mod 7 = 2
 *   bit 1 = 0 : base = 2² = 4
 *   bit 2 = 1 : result = 3×4 = 12 mod 7 = 5, base = 4² = 16 mod 7 = 2
 *   bit 3 = 1 : result = 5×2 = 10 mod 7 = 3
 *
 *   Résultat : 3 (vérif : 3^13 = 1594323 = 227760×7 + 3)
 *
 * COMPLEXITÉ : O(log(exp)) multiplications au lieu de O(exp)
 *   Pour exp = 1000000, seulement ~20 opérations au lieu de 1000000 !
 *
 * @param M : La base
 * @param exp : L'exposant (doit tenir sur 64 bits)
 * @param mod : Le modulo
 * @return : (M^exp) mod mod
 */
BigBinary BigBinary_expMod(const BigBinary M, const BigBinary exp, const BigBinary mod) {
    // CAS 1 : Modulo nul
    if (estZero(mod)) {
        fprintf(stderr, "Erreur: mod nul dans expMod\n");
        return initBigBinary();
    }

    // CAS 2 : Si mod == 1 → résultat toujours 0
    BigBinary one = initBigBinaryFromString("1");
    BigBinary mod_eq_1 = BigBinary_mod(one, mod);
    if (estZero(mod_eq_1)) {
        libereBigBinary(&mod_eq_1);
        libereBigBinary(&one);
        return initBigBinary();
    }
    libereBigBinary(&mod_eq_1);

    // CAS 3 : Convertir l'exposant en uint64_t
    unsigned long long e = 0ULL;
    if (!to_u64(exp, &e)) {
        fprintf(stderr, "Erreur: exposant > 64 bits\n");
        libereBigBinary(&one);
        return initBigBinary();
    }

    // ÉTAPE 1 : Initialisation
    BigBinary base = BigBinary_mod(M, mod);      // base = M mod mod
    BigBinary result = initBigBinaryFromString("1");  // result = 1

    // ÉTAPE 2 : Boucle square-and-multiply
    while (e > 0ULL) {
        // Si le bit de poids faible de e est 1
        if (e & 1ULL) {
            // result = (result × base) mod mod
            BigBinary tmp = BigBinary_mul_mod(result, base, mod);
            libereBigBinary(&result);
            result = tmp;
        }

        // Passer au bit suivant
        e >>= 1ULL;

        // Si on n'a pas fini, calculer le carré de base
        if (e > 0ULL) {
            // base = (base × base) mod mod
            BigBinary sq = BigBinary_mul_mod(base, base, mod);
            libereBigBinary(&base);
            base = sq;
        }
    }

    libereBigBinary(&one);
    libereBigBinary(&base);
    return result;
}