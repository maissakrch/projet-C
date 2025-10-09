#include "bigbinary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===========================================================
 *  Helpers internes
 * =========================================================== */

// Normalise : supprime zéros de tête (format MSB-first) et gère le zéro canonique
static void normalizeBigBinary(BigBinary *A) {
    if (A == NULL) return;

    if (A->Tdigits == NULL || A->Taille <= 0) {
        // -> 0 canonique
        free(A->Tdigits);
        A->Tdigits = (int*)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;
        return;
    }

    // Trouver l'indice du premier bit non nul (en partant du MSB)
    int i = 0;
    while (i < A->Taille - 1 && A->Tdigits[i] == 0) i++;

    // Si tout est à 0 → 0 canonique
    if (i == A->Taille - 1 && A->Tdigits[i] == 0) {
        free(A->Tdigits);
        A->Tdigits = (int*)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;
        return;
    }

    // Si zéros de tête → compacter
    if (i > 0) {
        int newLen = A->Taille - i;
        int *nd = (int*)malloc(newLen * sizeof(int));
        memcpy(nd, A->Tdigits + i, newLen * sizeof(int));
        free(A->Tdigits);
        A->Tdigits = nd;
        A->Taille = newLen;
    }

    // Convention : si 0, signe = positif (déjà géré ci-dessus)
}

/* ===========================================================
 *  Phase 1 — Base
 * =========================================================== */

// Initialisation vide → 0 canonique
BigBinary initBigBinary() {
    BigBinary A;
    A.Tdigits = (int*)malloc(sizeof(int));
    A.Tdigits[0] = 0;
    A.Taille = 1;
    A.Signe  = 0;
    return A;
}

// Initialisation depuis une chaîne binaire (accepte espace et signe +/- en tête)
BigBinary initBigBinaryFromString(const char *str) {
    if (str == NULL || strlen(str) == 0) {
        return initBigBinary();
    }

    // Gérer espaces et signe optionnel
    int signe = 0;
    int i0 = 0;
    while (str[i0] == ' ' || str[i0] == '\t' || str[i0] == '\n') i0++;
    if (str[i0] == '-') { signe = 1; i0++; }
    else if (str[i0] == '+') { i0++; }

    // Compter uniquement '0'/'1'
    int count = 0;
    for (int i = i0; str[i] != '\0'; ++i) {
        if (str[i] == '0' || str[i] == '1') count++;
        else if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') continue;
        else {
            fprintf(stderr, "Erreur: caractère invalide '%c' dans la chaîne binaire\n", str[i]);
            return initBigBinary();
        }
    }

    if (count == 0) {
        return initBigBinary();
    }

    BigBinary A;
    A.Taille  = count;
    A.Signe   = signe;               // en Phase 1 on reste en non signé, mais on stocke si donné
    A.Tdigits = (int*)malloc(A.Taille * sizeof(int));

    int k = 0;
    for (int i = i0; str[i] != '\0'; ++i) {
        if (str[i] == '0' || str[i] == '1') {
            A.Tdigits[k++] = (str[i] == '1') ? 1 : 0;
        }
    }

    normalizeBigBinary(&A);
    return A;
}

// Affichage
void afficheBigBinary(const BigBinary A) {
    if (A.Signe) printf("-");
    for (int i = 0; i < A.Taille; ++i) printf("%d", A.Tdigits[i]);
    printf("\n");
}

// Libération mémoire
void libereBigBinary(BigBinary *A) {
    if (!A) return;
    if (A->Tdigits) free(A->Tdigits);
    A->Tdigits = NULL;
    A->Taille  = 0;
    A->Signe   = 0;
}

// Egalité (suppose normalisé par appelant — ici vos constructions/opérations normalisent)
int Egal(const BigBinary A, const BigBinary B) {
    if (A.Taille != B.Taille) return 0;
    if (A.Signe  != B.Signe)  return 0;
    for (int i = 0; i < A.Taille; ++i)
        if (A.Tdigits[i] != B.Tdigits[i]) return 0;
    return 1;
}

// A < B (non signé en Phase 1)
int Inferieur(const BigBinary A, const BigBinary B) {
    if (A.Taille < B.Taille) return 1;
    if (A.Taille > B.Taille) return 0;
    for (int i = 0; i < A.Taille; ++i) {
        if (A.Tdigits[i] < B.Tdigits[i]) return 1;
        if (A.Tdigits[i] > B.Tdigits[i]) return 0;
    }
    return 0; // égaux
}

// Addition binaire (MSB-first)
BigBinary additionBigBinary(const BigBinary A, const BigBinary B) {
    int n = (A.Taille > B.Taille) ? A.Taille : B.Taille;
    BigBinary R;
    R.Taille  = n + 1;
    R.Signe   = 0;
    R.Tdigits = (int*)calloc(R.Taille, sizeof(int));

    int carry = 0;
    for (int i = 0; i < n; ++i) {
        int abit = (A.Taille - 1 - i >= 0) ? A.Tdigits[A.Taille - 1 - i] : 0;
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;
        int sum  = abit + bbit + carry;
        R.Tdigits[R.Taille - 1 - i] = sum & 1;
        carry = (sum >> 1);
    }
    R.Tdigits[0] = carry;

    normalizeBigBinary(&R);
    return R;
}

// Soustraction binaire (précondition A >= B ; non signé en Phase 1)
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B) {
    if (Inferieur(A, B)) {
        fprintf(stderr, "ERREUR: A < B dans soustractionBigBinary (précondition non respectée)\n");
        return initBigBinary(); // retourne 0
    }

    BigBinary R;
    R.Taille  = A.Taille;
    R.Signe   = 0;
    R.Tdigits = (int*)calloc(R.Taille, sizeof(int));

    int borrow = 0;
    for (int i = 0; i < A.Taille; ++i) {
        int abit = A.Tdigits[A.Taille - 1 - i];
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;
        int diff = abit - bbit - borrow;
        if (diff < 0) { diff += 2; borrow = 1; }
        else          { borrow = 0; }
        R.Tdigits[R.Taille - 1 - i] = diff;
    }

    normalizeBigBinary(&R);
    return R;
}

/* ===========================================================
 *  Phase 2 — Helpers & opérations étendues
 * =========================================================== */

// Zéro ?
int estZero(const BigBinary A) {
    for (int i = 0; i < A.Taille; ++i) if (A.Tdigits[i] != 0) return 0;
    return 1;
}

// Pair ? (LSB = dernier bit en MSB-first)
int estPair(const BigBinary A) {
    if (A.Taille <= 0 || A.Tdigits == NULL) return 1;
    return (A.Tdigits[A.Taille - 1] == 0);
}

// Copie profonde
BigBinary copieBigBinary(const BigBinary A) {
    BigBinary C;
    C.Taille  = A.Taille;
    C.Signe   = A.Signe;
    C.Tdigits = (int*)malloc(C.Taille * sizeof(int));
    memcpy(C.Tdigits, A.Tdigits, C.Taille * sizeof(int));
    return C;
}

// Décalage gauche × 2^n (ajoute n zéros en LSB)
BigBinary decaleGauche(const BigBinary A, int n) {
    if (n <= 0 || estZero(A)) return copieBigBinary(A);

    BigBinary R;
    R.Taille  = A.Taille + n;
    R.Signe   = A.Signe;
    R.Tdigits = (int*)malloc(R.Taille * sizeof(int));

    memcpy(R.Tdigits, A.Tdigits, A.Taille * sizeof(int));
    memset(R.Tdigits + A.Taille, 0, n * sizeof(int));

    normalizeBigBinary(&R);
    return R;
}

// Décalage droite ÷ 2^n (supprime n bits de LSB)
BigBinary decaleDroite(const BigBinary A, int n) {
    if (n <= 0) return copieBigBinary(A);
    if (n >= A.Taille) {
        BigBinary Z = initBigBinary();
        return Z;
    }

    BigBinary R;
    R.Taille  = A.Taille - n;
    R.Signe   = A.Signe;
    R.Tdigits = (int*)malloc(R.Taille * sizeof(int));
    memcpy(R.Tdigits, A.Tdigits, R.Taille * sizeof(int));

    normalizeBigBinary(&R);
    return R;
}

// |A - B| (non signé)
BigBinary soustractionAbsolue(const BigBinary A, const BigBinary B) {
    if (Inferieur(A, B)) {
        return soustractionBigBinary(B, A);
    } else {
        return soustractionBigBinary(A, B);
    }
}

// Compte les zéros de fin (trailing zeros) côté LSB
static int countTrailingZeros(const BigBinary A) {
    int c = 0;
    for (int i = A.Taille - 1; i >= 0; --i) {
        if (A.Tdigits[i] == 0) c++;
        else break;
    }
    return c;
}

// Décalages 1 et k (helpers)
static BigBinary rshift1(const BigBinary A) { return decaleDroite(A, 1); }
static BigBinary lshiftK(const BigBinary A, int k) { return decaleGauche(A, k); }

// PGCD binaire (algorithme de Stein)
BigBinary pgcdBinaire(const BigBinary A, const BigBinary B) {
    // Copies de travail
    BigBinary X = copieBigBinary(A); normalizeBigBinary(&X);
    BigBinary Y = copieBigBinary(B); normalizeBigBinary(&Y);

    if (estZero(X)) {
        // retour Y, libère X
        // (Y déjà normalisé)
        return Y; // appelant devra libérer le retour
    }
    if (estZero(Y)) {
        return X;
    }

    // Facteurs de 2 communs
    int kx = countTrailingZeros(X);
    int ky = countTrailingZeros(Y);
    int k  = (kx < ky) ? kx : ky;

    // Enlever 2^kx et 2^ky
    BigBinary t;
    t = decaleDroite(X, kx); libereBigBinary(&X); X = t;
    t = decaleDroite(Y, ky); libereBigBinary(&Y); Y = t;

    // Rendre X impair
    while (estPair(X)) { t = rshift1(X); libereBigBinary(&X); X = t; }

    // Boucle principale
    while (!estZero(Y)) {
        // Rendre Y impair
        while (estPair(Y)) { t = rshift1(Y); libereBigBinary(&Y); Y = t; }

        // s'assurer X <= Y (en valeur)
        if (Inferieur(Y, X) == 0 && Egal(Y, X) == 0) {
            // Y > X ? si non, alors X > Y -> swap
            if (Inferieur(X, Y) == 0) { BigBinary tmp = X; X = Y; Y = tmp; }
        } else if (Inferieur(X, Y) == 0) {
            // X >= Y → swap pour avoir X <= Y
            BigBinary tmp = X; X = Y; Y = tmp;
        }

        // Y = Y - X (deviendra pair)
        t = soustractionAbsolue(Y, X);
        libereBigBinary(&Y);
        Y = t;
    }

    // Réappliquer 2^k
    BigBinary G = lshiftK(X, k);
    libereBigBinary(&X);
    libereBigBinary(&Y); // Y == 0 à ce stade

    normalizeBigBinary(&G);
    return G;
}

// ===== Modulo (A mod B), B > 0, algorithme par alignement & soustractions =====
BigBinary BigBinary_mod(const BigBinary A, const BigBinary B) {
    // Gestion des cas limites
    if (estZero(B)) {
        fprintf(stderr, "Erreur: modulo par zero\n");
        return initBigBinary();
    }
    BigBinary R = copieBigBinary(A);   // reste courant
    // Normalise au cas où
    // (normalizeBigBinary est static ici, mais déjà appelée par vos constructeurs/ops)
    // On va soustraire B<<k quand possible, de k = (|A|-|B|) à 0
    if (Inferieur(R, B)) return R;     // déjà plus petit que B

    int maxShift = R.Taille - B.Taille;
    for (int k = maxShift; k >= 0; --k) {
        BigBinary Bk = decaleGauche(B, k);
        if (!Inferieur(R, Bk)) {                 // si R >= B<<k
            BigBinary tmp = soustractionBigBinary(R, Bk);
            libereBigBinary(&R);
            R = tmp;
        }
        libereBigBinary(&Bk);
        if (estZero(R)) break;
    }
    return R; // Normalisé par soustractionBigBinary
}

// ===== Multiplication modulaire par décalages/ajouts (sans multiplier général) =====
static BigBinary add_mod(const BigBinary X, const BigBinary Y, const BigBinary mod) {
    BigBinary s = additionBigBinary(X, Y);
    BigBinary r = BigBinary_mod(s, mod);
    libereBigBinary(&s);
    return r;
}
static BigBinary lshift1_mod(const BigBinary X, const BigBinary mod) {
    BigBinary d = decaleGauche(X, 1);
    BigBinary r = BigBinary_mod(d, mod);
    libereBigBinary(&d);
    return r;
}

// Z = (X * Y) mod mod  (schoolbook via décalages, Y traité bit-à-bit)
static BigBinary BigBinary_mul_mod(const BigBinary X, const BigBinary Y, const BigBinary mod) {
    BigBinary a = BigBinary_mod(X, mod);
    BigBinary b = copieBigBinary(Y);
    BigBinary res = initBigBinary(); // 0

    while (!estZero(b)) {
        // si b est impair → res = (res + a) mod mod
        if (!estPair(b)) {
            BigBinary tmp = add_mod(res, a, mod);
            libereBigBinary(&res);
            res = tmp;
        }
        // a = (a << 1) mod mod
        BigBinary a2 = lshift1_mod(a, mod);
        libereBigBinary(&a);
        a = a2;

        // b >>= 1
        BigBinary b2 = decaleDroite(b, 1);
        libereBigBinary(&b);
        b = b2;
    }
    libereBigBinary(&a);
    libereBigBinary(&b);
    return res;
}

// Convertit un BigBinary (≤64 bits) vers uint64_t ; renvoie 1 si ok, 0 sinon
static int to_u64(const BigBinary E, unsigned long long *out) {
    if (E.Taille > 64) return 0;
    unsigned long long v = 0ULL;
    for (int i = 0; i < E.Taille; ++i) {
        v = (v << 1) | (unsigned long long)(E.Tdigits[i] ? 1 : 0);
    }
    *out = v;
    return 1;
}

// ===== Exponentiation modulaire rapide (square-and-multiply) =====
BigBinary BigBinary_expMod(const BigBinary M, const BigBinary exp, const BigBinary mod) {
    if (estZero(mod)) {
        fprintf(stderr, "Erreur: mod nul dans expMod\n");
        return initBigBinary();
    }
    // Si mod == 1 → résultat = 0
    BigBinary one = initBigBinaryFromString("1");
    BigBinary mod_eq_1 = BigBinary_mod(one, mod); // juste pour tester mod==1 proprement
    if (estZero(mod_eq_1)) { // mod == 1
        libereBigBinary(&one);
        return initBigBinary();
    }
    libereBigBinary(&mod_eq_1);

    unsigned long long e = 0ULL;
    if (!to_u64(exp, &e)) {
        fprintf(stderr, "Erreur: exposant > 64 bits\n");
        libereBigBinary(&one);
        return initBigBinary();
    }

    BigBinary base = BigBinary_mod(M, mod);
    BigBinary result = initBigBinaryFromString("1");

    while (e > 0ULL) {
        if (e & 1ULL) {
            BigBinary tmp = BigBinary_mul_mod(result, base, mod);
            libereBigBinary(&result);
            result = tmp;
        }
        e >>= 1ULL;
        if (e > 0ULL) { // inutile de faire le dernier carré si on sort
            BigBinary sq = BigBinary_mul_mod(base, base, mod);
            libereBigBinary(&base);
            base = sq;
        }
    }

    libereBigBinary(&one);
    libereBigBinary(&base);
    return result;
}

