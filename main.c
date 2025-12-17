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

static int binstr_to_u64(const char *s, unsigned long long *out) {
    if (!s || !*s) return 0;
    unsigned long long v = 0ULL;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] != '0' && s[i] != '1') return 0;
        // overflow check avant shift
        if (v > (~0ULL >> 1)) return 0;
        v = (v << 1) | (unsigned long long)(s[i] - '0');
    }
    *out = v;
    return 1;
}

static void u64_to_binstr(unsigned long long x, char *buf, int bufSize) {
    if (bufSize < 2) return;
    if (x == 0ULL) { buf[0] = '0'; buf[1] = '\0'; return; }

    char tmp[128];
    int k = 0;
    while (x > 0ULL && k < (int)sizeof(tmp)-1) {
        tmp[k++] = (x & 1ULL) ? '1' : '0';
        x >>= 1ULL;
    }
    int idx = 0;
    for (int i = k-1; i >= 0 && idx < bufSize-1; i--) buf[idx++] = tmp[i];
    buf[idx] = '\0';
}

static unsigned long long gcd_u64(unsigned long long a, unsigned long long b) {
    while (b != 0ULL) {
        unsigned long long t = a % b;
        a = b;
        b = t;
    }
    return a;
}

// Inverse modulaire d = e^{-1} mod phi (Euclide étendu)
static int modInverse_u64(unsigned long long e, unsigned long long phi, unsigned long long *d_out) {
    long long t = 0, newt = 1;
    long long r = (long long)phi, newr = (long long)e;

    while (newr != 0) {
        long long q = r / newr;

        long long tmp = t - q * newt;
        t = newt;
        newt = tmp;

        tmp = r - q * newr;
        r = newr;
        newr = tmp;
    }

    if (r != 1) return 0;      // pas d’inverse si gcd != 1
    if (t < 0) t += (long long)phi;

    *d_out = (unsigned long long)t;
    return 1;
}

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


    // Décalage à gauche de 3 positions (multiplication par 2^3 = 8)
    BigBinary X_L3 = decaleGauche(A, 3);
    
    // Décalage à droite de 3 positions (division entière par 2^3 = 8)
    BigBinary X_R3 = decaleDroite(A, 3);
    
    // Affichage des résultats
    show("A", A);
    show("A<<3", X_L3);
    show("A>>3", X_R3);

    // TEST 2.1.2 : Soustraction absolue |A - B|
    // -----------------------------------------
    // Cette fonction calcule la valeur absolue de la différence,
    // quel que soit l'ordre des opérandes

    BigBinary ABS = soustractionAbsolue(A, B);
    show("|A - B|", ABS);

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
    BigBinary G1 = pgcdBinaire(A, B);
    show("gcd(A,B)", G1);



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

    BigBinary R1 = BigBinary_mod(A, B);
    show("A mod B", R1);


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
    libereBigBinary(&X_L3);
    libereBigBinary(&X_R3);
    libereBigBinary(&ABS);
    
    // Phase 2 - Partie 2 : PGCD
    libereBigBinary(&G1); 

    
    // Phase 2 - Partie 3 : Modulo et Exponentiation

    libereBigBinary(&base1); 
    libereBigBinary(&exp1); 
    libereBigBinary(&mod1); 
    libereBigBinary(&expRes1);

    printf("\n===== PHASE 3 – RSA simplifie =====\n");

    char inputP[1024], inputQ[1024], inputE[1024], inputM[1024];

    printf("Entrez p en binaire (petit premier) : ");
    scanf("%1023s", inputP);

    printf("Entrez q en binaire (petit premier) : ");
    scanf("%1023s", inputQ);

    printf("Entrez e en binaire (copremier avec phi(n)) : ");
    scanf("%1023s", inputE);

    printf("Entrez M (message) en binaire : ");
    scanf("%1023s", inputM);

    // BigBinary versions (pour expMod)
    BigBinary pBB = initBigBinaryFromString(inputP);
    BigBinary qBB = initBigBinaryFromString(inputQ);
    BigBinary eBB = initBigBinaryFromString(inputE);
    BigBinary MBB = initBigBinaryFromString(inputM);

    // On calcule n = p*q et phi(n) en uint64 (RSA simplifié)
    unsigned long long p, q, e, M;
    if (!binstr_to_u64(inputP, &p) || !binstr_to_u64(inputQ, &q) ||
        !binstr_to_u64(inputE, &e) || !binstr_to_u64(inputM, &M)) {
        printf("Erreur: entrees invalides (0/1 uniquement, <= 64 bits)\n");
        // libère ce qu'on a déjà
        libereBigBinary(&pBB); libereBigBinary(&qBB);
        libereBigBinary(&eBB); libereBigBinary(&MBB);
        return 1;
    }

    unsigned long long n = p * q;
    unsigned long long phi = (p - 1ULL) * (q - 1ULL);

    printf("\n--- Infos RSA ---\n");
    printf("n = p*q (dec) = %llu\n", n);
    printf("phi(n) (dec) = %llu\n", phi);

    // vérifier gcd(e,phi)=1
    if (gcd_u64(e, phi) != 1ULL) {
        printf("Erreur: e n'est pas copremier avec phi(n). Choisis un autre e.\n");
        libereBigBinary(&pBB); libereBigBinary(&qBB);
        libereBigBinary(&eBB); libereBigBinary(&MBB);
        return 1;
    }

    // calcul de d
    unsigned long long d;
    if (!modInverse_u64(e, phi, &d)) {
        printf("Erreur: impossible de calculer d (inverse modulaire)\n");
        libereBigBinary(&pBB); libereBigBinary(&qBB);
        libereBigBinary(&eBB); libereBigBinary(&MBB);
        return 1;
    }

    char nStr[128], dStr[128];
    u64_to_binstr(n, nStr, sizeof(nStr));
    u64_to_binstr(d, dStr, sizeof(dStr));

    BigBinary nBB = initBigBinaryFromString(nStr);
    BigBinary dBB = initBigBinaryFromString(dStr);

    printf("d (dec) = %llu\n", d);
    printf("n (bin) = %s\n", nStr);
    printf("d (bin) = %s\n", dStr);

    // Si M >= n, on réduit : M = M mod n
    if (!Inferieur(MBB, nBB) || Egal(MBB, nBB)) {
        BigBinary Mr = BigBinary_mod(MBB, nBB);
        libereBigBinary(&MBB);
        MBB = Mr;
        printf("Note: M >= n, donc on a reduit M = M mod n\n");
    }

    printf("\n--- Chiffrement ---\n");
    BigBinary CBB = BigBinary_RSA_encrypt(MBB, eBB, nBB);
    printf("C = "); afficheBigBinary(CBB);

    printf("\n--- Dechiffrement ---\n");
    BigBinary M2BB = BigBinary_RSA_decrypt(CBB, dBB, nBB);
    printf("M' = "); afficheBigBinary(M2BB);

    // Libérations phase 3
    libereBigBinary(&pBB);
    libereBigBinary(&qBB);
    libereBigBinary(&eBB);
    libereBigBinary(&MBB);
    libereBigBinary(&nBB);
    libereBigBinary(&dBB);
    libereBigBinary(&CBB);
    libereBigBinary(&M2BB);



    /*
     * ========================================================================
     * FIN DU PROGRAMME
     * ========================================================================
     *
     */
    return 0;
}