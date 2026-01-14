#include "bigbinary.h"
#include <stdio.h>

/* =========================================================
 * AFFICHAGE UTILE
 * ========================================================= */

void show(const char *label, BigBinary X) {
    printf("%s = ", label);
    afficheBigBinary(X);
}

/* =========================================================
 * MAIN
 * ========================================================= */

int main(void) {

    /* =====================================================
     * PHASE 1 : OPERATIONS DE BASE
     * ===================================================== */

    printf("========== PHASE 1 : OPERATIONS DE BASE ==========\n\n");

    char Astr[256], Bstr[256];

    printf("Entrez A (binaire) : ");
    scanf("%255s", Astr);

    printf("Entrez B (binaire) : ");
    scanf("%255s", Bstr);

    BigBinary A = initBigBinaryFromString(Astr);
    BigBinary B = initBigBinaryFromString(Bstr);

    show("\nA", A);
    show("B", B);

    // Addition
    BigBinary S = additionBigBinary(A, B);
    printf("\n--- Addition ---\n");
    show("A + B", S);

    // Soustraction sécurisée
    printf("\n--- Soustraction ---\n");
    if (!Inferieur(A, B)) {
        BigBinary D = soustractionBigBinary(A, B);
        show("A - B", D);
        libereBigBinary(&D);
    } else {
        printf("A < B : soustraction non autorisee\n");
    }

    // Comparaisons
    printf("\n--- Comparaisons ---\n");
    printf("A == B ? %s\n", Egal(A, B) ? "OUI" : "NON");
    printf("A < B  ? %s\n", Inferieur(A, B) ? "OUI" : "NON");

    libereBigBinary(&S);

    /* =====================================================
     * PHASE 2 : ALGORITHMES AVANCES
     * ===================================================== */

    printf("\n========== PHASE 2 : ALGORITHMES AVANCES ==========\n");

    BigBinary G = pgcdBinaire(A, B);
    show("PGCD(A, B)", G);

    BigBinary R = BigBinary_mod(A, B);
    show("A mod B", R);

    printf("\n--- Multiplication egyptienne ---\n");
    BigBinary P = multiplicationEgyptienne(A, B);
    show("A × B", P);
    libereBigBinary(&P);


    libereBigBinary(&G);
    libereBigBinary(&R);

    /* =====================================================
     * PHASE 3 : RSA SIMPLIFIE (DEMO PEDAGOGIQUE)
     * ===================================================== */

    printf("\n========== PHASE 3 : RSA SIMPLIFIE ==========\n");
    printf("Parametres fixes pour demonstration correcte\n\n");

    // RSA classique de démo
    BigBinary p = initBigBinaryFromString("1011");   // 11
    BigBinary q = initBigBinaryFromString("1101");   // 13
    BigBinary n = initBigBinaryFromString("10001111"); // 143
    BigBinary e = initBigBinaryFromString("111");    // 7
    BigBinary d = initBigBinaryFromString("1100111"); // 103

    show("p", p);
    show("q", q);
    show("n", n);
    show("e", e);
    show("d", d);

    char Mstr[256];
    printf("\nEntrez le message M (binaire, M < 143) : ");
    scanf("%255s", Mstr);

    BigBinary M = initBigBinaryFromString(Mstr);

    // Vérification M < n
    if (!Inferieur(M, n)) {
        printf("Erreur : M doit etre strictement inferieur a n\n");
        return 1;
    }

    // Chiffrement / Déchiffrement
    BigBinary C = BigBinary_RSA_encrypt(M, e, n);
    BigBinary M2 = BigBinary_RSA_decrypt(C, d, n);

    printf("\n--- Chiffrement ---\n");
    show("C", C);

    printf("\n--- Dechiffrement ---\n");
    show("M'", M2);

    // Nettoyage
    libereBigBinary(&A);
    libereBigBinary(&B);
    libereBigBinary(&M);
    libereBigBinary(&C);
    libereBigBinary(&M2);
    libereBigBinary(&p);
    libereBigBinary(&q);
    libereBigBinary(&n);
    libereBigBinary(&e);
    libereBigBinary(&d);

    return 0;
}
