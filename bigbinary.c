#include "bigbinary.h"

// 🔹 Initialisation vide
BigBinary initBigBinary() {
    BigBinary A;
    A.Tdigits = NULL;
    A.Taille = 0;
    A.Signe = 0;
    return A;
}

// 🔹 Initialisation depuis une chaîne binaire ex: "101101"
BigBinary initBigBinaryFromString(const char *str) {
    BigBinary A;
    A.Taille = strlen(str);
    A.Signe = 0;
    A.Tdigits = (int *)malloc(A.Taille * sizeof(int));

    for (int i = 0; i < A.Taille; i++) {
        A.Tdigits[i] = (str[i] == '1') ? 1 : 0;
    }
    return A;
}
// 🔹 Affiche le contenu du BigBinary
void afficheBigBinary(const BigBinary A) {
    if (A.Signe) printf("-");
    for (int i = 0; i < A.Taille; i++) {
        printf("%d", A.Tdigits[i]);
    }
    printf("\n");
}
// 🔹 Libère la mémoire allouée
void libereBigBinary(BigBinary *A) {
    if (A->Tdigits != NULL) {
        free(A->Tdigits);
        A->Tdigits = NULL;
    }
    A->Taille = 0;
    A->Signe = 0;
}
// 🔹 Vérifie l'égalité : renvoie 1 si A == B, sinon 0
int Egal(const BigBinary A, const BigBinary B) {
    if (A.Taille != B.Taille) return 0;
    for (int i = 0; i < A.Taille; i++) {
        if (A.Tdigits[i] != B.Tdigits[i]) return 0;
    }
    return 1;
}

// 🔹 Vérifie si A < B (en binaire, sans signe)
int Inferieur(const BigBinary A, const BigBinary B) {
    if (A.Taille < B.Taille) return 1;
    if (A.Taille > B.Taille) return 0;

    for (int i = 0; i < A.Taille; i++) {
        if (A.Tdigits[i] < B.Tdigits[i]) return 1;
        else if (A.Tdigits[i] > B.Tdigits[i]) return 0;
    }
    return 0; // égaux
}
BigBinary additionBigBinary(const BigBinary A, const BigBinary B) {
    int n = (A.Taille > B.Taille) ? A.Taille : B.Taille;
    BigBinary Res;
    Res.Taille = n + 1;
    Res.Signe = 0;
    Res.Tdigits = (int *)calloc(Res.Taille, sizeof(int));

    int carry = 0;

    for (int i = 0; i < n; i++) {
        int abit = (A.Taille - 1 - i >= 0) ? A.Tdigits[A.Taille - 1 - i] : 0;
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;

        int sum = abit + bbit + carry;
        Res.Tdigits[Res.Taille - 1 - i] = sum % 2;
        carry = sum / 2;
    }

    Res.Tdigits[0] = carry;
    return Res;
}
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B) {
    BigBinary Res;
    Res.Taille = A.Taille;
    Res.Signe = 0;
    Res.Tdigits = (int *)calloc(Res.Taille, sizeof(int));

    int borrow = 0;

    for (int i = 0; i < A.Taille; i++) {
        int abit = A.Tdigits[A.Taille - 1 - i];
        int bbit = (B.Taille - 1 - i >= 0) ? B.Tdigits[B.Taille - 1 - i] : 0;

        int diff = abit - bbit - borrow;
        if (diff < 0) {
            diff += 2;
            borrow = 1;
        } else {
            borrow = 0;
        }

        Res.Tdigits[Res.Taille - 1 - i] = diff;
    }
    return Res;
}
