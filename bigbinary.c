#include "bigbinary.h"

// 🔹 Normalise le BigBinary : supprime zéros de tête, gère le cas zéro
static void normalizeBigBinary(BigBinary *A) {
    if (A->Taille == 0) {
        // Cas vide : le transformer en zéro canonique
        A->Tdigits = (int *)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;
        return;
    }

    // Trouver le premier bit non nul
    int i = 0;
    while (i < A->Taille - 1 && A->Tdigits[i] == 0) {
        i++;
    }

    // Si tous les bits sont à 0, normaliser en un seul zéro
    if (i == A->Taille - 1 && A->Tdigits[i] == 0) {
        free(A->Tdigits);
        A->Tdigits = (int *)malloc(sizeof(int));
        A->Tdigits[0] = 0;
        A->Taille = 1;
        A->Signe = 0;
        return;
    }

    // Supprimer les zéros de tête
    if (i > 0) {
        int nouvelleTaille = A->Taille - i;
        int *nouveauTableau = (int *)malloc(nouvelleTaille * sizeof(int));
        for (int j = 0; j < nouvelleTaille; j++) {
            nouveauTableau[j] = A->Tdigits[i + j];
        }
        free(A->Tdigits);
        A->Tdigits = nouveauTableau;
        A->Taille = nouvelleTaille;
    }
}

// 🔹 Initialisation vide (normalisée en zéro)
BigBinary initBigBinary() {
    BigBinary A;
    A.Tdigits = (int *)malloc(sizeof(int));
    A.Tdigits[0] = 0;
    A.Taille = 1;
    A.Signe = 0;
    return A;
}

// 🔹 Initialisation depuis une chaîne binaire
BigBinary initBigBinaryFromString(const char *str) {
    if (str == NULL || strlen(str) == 0) {
        return initBigBinary();
    }

    // Gérer le signe (optionnel pour Phase 1)
    int signe = 0;
    int debut = 0;
    if (str[0] == '-') {
        signe = 1;
        debut = 1;
    } else if (str[0] == '+') {
        debut = 1;
    }

    // Compter uniquement les caractères '0' et '1'
    int longueur = 0;
    for (int i = debut; str[i] != '\0'; i++) {
        if (str[i] == '0' || str[i] == '1') {
            longueur++;
        } else if (str[i] != ' ' && str[i] != '\t') {
            // Caractère invalide (hors espaces)
            fprintf(stderr, "Erreur: caractère invalide '%c' dans la chaîne binaire\n", str[i]);
            return initBigBinary();
        }
    }

    if (longueur == 0) {
        return initBigBinary();
    }

    BigBinary A;
    A.Taille = longueur;
    A.Signe = signe;
    A.Tdigits = (int *)malloc(A.Taille * sizeof(int));

    int index = 0;
    for (int i = debut; str[i] != '\0'; i++) {
        if (str[i] == '0' || str[i] == '1') {
            A.Tdigits[index++] = (str[i] == '1') ? 1 : 0;
        }
    }

    // Normaliser pour supprimer les zéros de tête
    normalizeBigBinary(&A);

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

// 🔹 Vérifie l'égalité (après normalisation, fonctionne même avec zéros de tête)
int Egal(const BigBinary A, const BigBinary B) {
    if (A.Taille != B.Taille) return 0;
    if (A.Signe != B.Signe) return 0;
    for (int i = 0; i < A.Taille; i++) {
        if (A.Tdigits[i] != B.Tdigits[i]) return 0;
    }
    return 1;
}

// 🔹 Vérifie si A < B (nombres positifs)
int Inferieur(const BigBinary A, const BigBinary B) {
    // Pour Phase 1 : on suppose nombres positifs
    if (A.Taille < B.Taille) return 1;
    if (A.Taille > B.Taille) return 0;

    for (int i = 0; i < A.Taille; i++) {
        if (A.Tdigits[i] < B.Tdigits[i]) return 1;
        else if (A.Tdigits[i] > B.Tdigits[i]) return 0;
    }
    return 0; // égaux
}

// 🔹 Addition binaire
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

    // Normaliser pour supprimer le zéro de tête si pas de retenue
    normalizeBigBinary(&Res);

    return Res;
}

// 🔹 Soustraction binaire (A >= B uniquement, avec vérification)
BigBinary soustractionBigBinary(const BigBinary A, const BigBinary B) {
    // Vérification : A doit être >= B
    if (Inferieur(A, B)) {
        fprintf(stderr, "ERREUR: A < B dans soustractionBigBinary (précondition non respectée)\n");
        return initBigBinary(); // Retourne 0
    }

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

    // Normaliser pour supprimer les zéros de tête
    normalizeBigBinary(&Res);

    return Res;
}