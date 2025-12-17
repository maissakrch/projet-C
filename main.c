/*
 * ============================================================================
 * FICHIER DE TEST POUR LA BIBLIOTHÈQUE BIGBINARY
 * ============================================================================
 * 
 * Ce programme teste toutes les fonctionnalités de la bibliothèque BigBinary
 * en deux phases :
 * 
 * PHASE 1 : Opérations de base
 *   - Initialisation depuis chaînes binaires
 *   - Comparaisons (égalité, infériorité)
 *   - Addition et soustraction binaires
 * 
 * PHASE 2 : Opérations avancées
 *   - Partie 1 : Décalages (gauche/droite) et valeur absolue
 *   - Partie 2 : PGCD binaire (algorithme de Stein)
 *   - Partie 3 : Modulo et exponentiation modulaire
 * 
 * ============================================================================
 */

#include "bigbinary.h"
#include <stdio.h>

/**
 * show - Fonction utilitaire pour afficher un BigBinary avec son label
 * 
 * RÔLE : Simplifie l'affichage des résultats dans les tests
 * 
 * FORMAT DE SORTIE : "label = bits\n"
 * EXEMPLE : "A = 10110\n"
 * 
 * @param label : Texte descriptif du nombre
 * @param X : Le BigBinary à afficher
 */
static void show(const char *label, BigBinary X) {
    printf("%s = ", label);
    afficheBigBinary(X);
}

int main(void) {
    /*
     * ========================================================================
     * PHASE 1 – VÉRIFICATION DES OPÉRATIONS DE BASE
     * ========================================================================
     * 
     * On teste les fonctionnalités fondamentales :
     *   - Création de BigBinary depuis des chaînes binaires
     *   - Comparaisons entre nombres
     *   - Addition et soustraction
     */
    printf("===== PHASE 1 – Vérification de base =====\n");
    
    // TEST 1.1 : Initialisation depuis des chaînes binaires
    // ----------------------------------------------------
    // Création de deux nombres :

    char inputA[1024];
    char inputB[1024];

    printf("Entrez A en binaire : ");
    scanf("%1023s", inputA);

    printf("Entrez B en binaire : ");
    scanf("%1023s", inputB);

    BigBinary A = initBigBinaryFromString(inputA);
    BigBinary B = initBigBinaryFromString(inputB);

    // Affichage des valeurs
    show("A", A);  // Affichera : A = 10110
    show("B", B);  // Affichera : B = 1101

    // TEST 1.2 : Test d'égalité
    // -------------------------
    // Egal(A,B) devrait retourner 0 car 22 ≠ 13
    printf("Egal(A,B) = %d\n", Egal(A, B));

    // TEST 1.3 : Test de comparaison (inférieur)
    // ------------------------------------------
    // A < B ? devrait retourner 0 car 22 > 13
    printf("A < B ?   = %d\n", Inferieur(A, B));

    // TEST 1.4 : Addition binaire
    // ---------------------------
    // A + B = 22 + 13 = 35
    // En binaire : 35 = 32 + 2 + 1 = "100011"
    BigBinary S = additionBigBinary(A, B);
    show("A + B", S); // Affichera : A + B = 100011 (35)

    // TEST 1.5 : Soustraction binaire
    // -------------------------------
    // A - B = 22 - 13 = 9
    // En binaire : 9 = 8 + 1 = "1001"
    BigBinary D = soustractionBigBinary(A, B);
    show("A - B", D); // Affichera : A - B = 1001 (9)

    // Libération de la mémoire temporaire
    libereBigBinary(&S);
    libereBigBinary(&D);

    /*
     * ========================================================================
     * PHASE 2 – PARTIE 1 : DÉCALAGES ET SOUSTRACTION ABSOLUE
     * ========================================================================
     * 
     * Les décalages binaires sont des opérations très efficaces :
     *   - Décalage à gauche (<<) : multiplication par 2^n
     *   - Décalage à droite (>>) : division entière par 2^n
     */
    printf("\n===== PHASE 2 – Décalages et |A-B| =====\n");
    
    // TEST 2.1.1 : Décalages binaires
    // -------------------------------
    // Création d'un nombre de test :
    char inputX[1024];

    printf("Entrez X en binaire : ");
    scanf("%1023s", inputX);

    BigBinary X = initBigBinaryFromString(inputX);

    // Décalage à gauche de 3 positions (multiplication par 2^3 = 8)
    BigBinary X_L3 = decaleGauche(X, 3);
    
    // Décalage à droite de 3 positions (division entière par 2^3 = 8)
    BigBinary X_R3 = decaleDroite(X, 3);
    
    // Affichage des résultats
    show("X", X);         // Affichera : X = 101101000
    show("X<<3", X_L3);   // Affichera : X<<3 = 101101000000 (2880)
    show("X>>3", X_R3);   // Affichera : X>>3 = 101101 (45)

    // TEST 2.1.2 : Soustraction absolue |U - V|
    // -----------------------------------------
    // Cette fonction calcule la valeur absolue de la différence,
    // quel que soit l'ordre des opérandes
    
    char inputU[1024];
    char inputV[1024];

    printf("Entrez U en binaire : ");
    scanf("%1023s", inputU);

    printf("Entrez V en binaire : ");
    scanf("%1023s", inputV);

    BigBinary U = initBigBinaryFromString(inputU);
    BigBinary V = initBigBinaryFromString(inputV);

    BigBinary ABS = soustractionAbsolue(U, V);
    show("|U - V|", ABS);

    /*
     * ========================================================================
     * PHASE 2 – PARTIE 2 : PGCD BINAIRE (ALGORITHME DE STEIN)
     * ========================================================================
     * 
     * Le PGCD (Plus Grand Commun Diviseur) est calculé avec l'algorithme
     * de Stein, une méthode optimisée pour les ordinateurs car elle
     * n'utilise que des décalages, comparaisons et soustractions
     * (pas de divisions coûteuses).
     * 
     * RAPPEL : Le PGCD est le plus grand nombre qui divise à la fois
     *          les deux nombres donnés.
     */
    printf("\n===== PHASE 2 – PGCD binaire (Stein) =====\n");
    
    // TEST 2.2.1 :
    // --------------------------
    // CALCUL MANUEL :
    BigBinary G1 = pgcdBinaire(U, V);
    show("gcd(U,V)", G1);

    // TEST 2.2.2 : PGCD
    // -------------------------
    char inputC1[1024];
    char inputD1[1024];

    printf("Entrez C1 en binaire : ");
    scanf("%1023s", inputC1);

    printf("Entrez D1 en binaire : ");
    scanf("%1023s", inputD1);

    BigBinary C1 = initBigBinaryFromString(inputC1);
    BigBinary D1 = initBigBinaryFromString(inputD1);
    BigBinary G2 = pgcdBinaire(C1, D1);
    show("gcd(C1,D1)", G2);

    // TEST 2.2.3 :
    // ------------------------
    char inputC2[1024];
    char inputD2[1024];

    printf("Entrez C2 en binaire : ");
    scanf("%1023s", inputC2);

    printf("Entrez D2 en binaire : ");
    scanf("%1023s", inputD2);

    BigBinary C2 = initBigBinaryFromString(inputC2);
    BigBinary D2 = initBigBinaryFromString(inputD2);
    BigBinary G3 = pgcdBinaire(C2, D2);
    show("gcd(C2,D2)", G3);

    /*
     * ========================================================================
     * PHASE 2 – PARTIE 3 : MODULO ET EXPONENTIATION MODULAIRE
     * ========================================================================
     * 
     * Ces opérations sont fondamentales en cryptographie :
     * 
     * MODULO : Reste de la division euclidienne
     *   a mod n = reste de a ÷ n
     *   Exemple : 17 mod 5 = 2 (car 17 = 5 × 3 + 2)
     * 
     * EXPONENTIATION MODULAIRE : (base^exposant) mod modulo
     *   Calcul efficace des grandes puissances modulo un nombre
     *   Utilisé dans RSA, Diffie-Hellman, etc.
     *   Algorithme : "square-and-multiply"
     */
    printf("\n===== PHASE 2 – Modulo et Exponentiation modulaire =====\n");

    // ========================================================================
    // SECTION 2.3.1 : TESTS DU MODULO
    // ========================================================================
    
    // TEST 2.3.1.1 :
    // -------------------------
    char inputM1[1024];
    char inputN1[1024];

    printf("Entrez M1 en binaire : ");
    scanf("%1023s", inputM1);

    printf("Entrez N1 en binaire : ");
    scanf("%1023s", inputN1);

    BigBinary M1 = initBigBinaryFromString(inputM1);
    BigBinary N1 = initBigBinaryFromString(inputN1);
    BigBinary R1 = BigBinary_mod(M1, N1);
    show("M1 mod N1", R1);

    // TEST 2.3.1.2 : 48 mod 18
    // ------------------------

    char inputM2[1024];
    char inputN2[1024];

    printf("Entrez M2 en binaire : ");
    scanf("%1023s", inputM2);

    printf("Entrez N2 en binaire : ");
    scanf("%1023s", inputN2);

    BigBinary M2 = initBigBinaryFromString(inputM2);
    BigBinary N2 = initBigBinaryFromString(inputN2);
    BigBinary R2 = BigBinary_mod(M2, N2);
    show("M2 mod N2", R2);

    // ========================================================================
    // SECTION 2.3.2 : TESTS DE L'EXPONENTIATION MODULAIRE
    // ========================================================================
    
    // TEST 2.3.2.1 :
    // --------------------------

    char inputbase1[1024];
    char inputexp1[1024];

    printf("Entrez base1 en binaire : ");
    scanf("%1023s", inputbase1);

    printf("Entrez exp1 en binaire : ");
    scanf("%1023s", inputexp1);

    BigBinary base1 = initBigBinaryFromString(inputbase1);
    BigBinary exp1 = initBigBinaryFromString(inputexp1);

    char inputmod1[1024];

    printf("Entrez mod1 en binaire : ");
    scanf("%1023s", inputmod1);

    BigBinary mod1 = initBigBinaryFromString(inputmod1);
    BigBinary expRes1 = BigBinary_expMod(base1, exp1, mod1);
    show("base1^exp1 mod mod1", expRes1);

    // TEST 2.3.2.2 :

    char inputbase2[1024];
    char inputexp2[1024];
    char inputmod2[1024];

    printf("Entrez base2 en binaire : ");
    scanf("%1023s", inputbase2);

    printf("Entrez exp2 en binaire : ");
    scanf("%1023s", inputexp2);

    printf("Entrez mod2 en binaire : ");
    scanf("%1023s", inputmod2);

    BigBinary base2 = initBigBinaryFromString(inputbase2);
    BigBinary exp2 = initBigBinaryFromString(inputexp2);
    BigBinary mod2 = initBigBinaryFromString(inputmod2);

    BigBinary expRes2 = BigBinary_expMod(base2, exp2, mod2);
    show("base2^exp2 mod mod2", expRes2);

    /*
     * ========================================================================
     * LIBÉRATION DE MÉMOIRE
     * ========================================================================
     * 
     * IMPORTANT : Chaque BigBinary alloue dynamiquement de la mémoire
     * (via malloc dans le tableau Tdigits).
     * 
     * Pour éviter les fuites mémoire, on doit libérer TOUS les BigBinary
     * créés dans ce programme.
     * 
     * ORDRE DE LIBÉRATION :
     *   1. Variables de la Phase 1
     *   2. Variables de la Phase 2 - Partie 1
     *   3. Variables de la Phase 2 - Partie 2
     *   4. Variables de la Phase 2 - Partie 3
     */
    
    // Phase 1
    libereBigBinary(&A);  
    libereBigBinary(&B);
    
    // Phase 2 - Partie 1 : Décalages
    libereBigBinary(&X);  
    libereBigBinary(&X_L3); 
    libereBigBinary(&X_R3);
    libereBigBinary(&U);  
    libereBigBinary(&V);    
    libereBigBinary(&ABS);
    
    // Phase 2 - Partie 2 : PGCD
    libereBigBinary(&G1); 
    libereBigBinary(&C1);   
    libereBigBinary(&D1); 
    libereBigBinary(&G2);
    libereBigBinary(&C2); 
    libereBigBinary(&D2);   
    libereBigBinary(&G3);
    
    // Phase 2 - Partie 3 : Modulo et Exponentiation
    libereBigBinary(&M1); 
    libereBigBinary(&N1);   
    libereBigBinary(&R1);
    libereBigBinary(&M2); 
    libereBigBinary(&N2);   
    libereBigBinary(&R2);
    libereBigBinary(&base1); 
    libereBigBinary(&exp1); 
    libereBigBinary(&mod1); 
    libereBigBinary(&expRes1);
    libereBigBinary(&base2); 
    libereBigBinary(&exp2); 
    libereBigBinary(&mod2); 
    libereBigBinary(&expRes2);



    /*
     * ========================================================================
     * FIN DU PROGRAMME
     * ========================================================================
     *
     */
    return 0;
}