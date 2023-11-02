#include <stdio.h>
#include <stdlib.h>
#include "conjunto.h"

#define min(a, b) ((a > b) ? b : a)

struct conjunto *cria_cjt (int max)
{
    struct conjunto *novoCjt;
    int *vet;
    
    /* retorna NULL caso alocacao de novoCjt 
     * ou de vet nao foi bem sucedida */
    novoCjt = malloc (sizeof(struct conjunto));
    vet = malloc (max*sizeof(int));
    if (!novoCjt || !vet)
        return NULL;

    novoCjt->v = vet;
    novoCjt->max = max;
    novoCjt->card = 0;

    return novoCjt;
}

struct conjunto *destroi_cjt (struct conjunto *c)
{
    /* retorna NULL se conjunto nao foi criado */
    if (!c)
        return NULL;
    
    free (c->v);
    free (c);
     
    return NULL;
}

int vazio_cjt (struct conjunto *c)
{
    /* conjunto esta vazio se nao foi criado */
    if (!c)
        return 1;
    
    /* retorna 1 se c->card for 0 e 0 nos demais casos */
    return (c->card == 0);
}

int cardinalidade_cjt (struct conjunto *c)
{
    /* conjunto tem cardinalidade 0 se nao foi criado */
    if (!c)
        return 0;
    
    return c->card;
}

int insere_cjt (struct conjunto *c, int elemento)
{
    int ini, fim, meio, j;
    
    /* nao ha espaco para inserir se conjunto nao 
     * existe ou cardinalidade for igual a max */
    if (!c || c->card == c->max)
        return 0;

    ini = 0;
    fim = c->card - 1;

    /* o conjunto eh mantido ordenado, logo busca binaria
     * eh feita para achar posicao correta do elemento */ 
    while (ini <= fim)
    {
        meio = (ini + fim) / 2;
        if (elemento >= c->v[meio])
            ini = meio + 1;
        else
            fim = meio - 1;
    }
    /* nao insere se elemento ja esta no conjunto */
    if (ini && c->v[ini - 1] == elemento)
        return 1;
    /* desloca os elementos para inserir ordenado */
    for (j = c->card - 1; j >= ini; j--)
        c->v[j + 1] = c->v[j];

    c->v[ini] = elemento;
    ++c->card;

    return 1;
}

int retira_cjt (struct conjunto *c, int elemento)
{
    int ini, fim, meio, j;
    
    /* nada a fazer se conjunto nao foi criado ou nao tem elementos */
    if (!c || !c->card)
        return 0;

    ini = 0;
    fim = c->card - 1; 
    while (ini <= fim)
    {
        meio = (ini + fim) / 2;
        if (elemento >= c->v[meio])
            ini = meio + 1;
        else
            fim = meio - 1;
    }

    /* nada a fazer se elemento nao esta no conjunto */
    if (!(ini && c->v[ini - 1] == elemento))
        return 0;

    /* retira 'elemento' mantendo o vetor ordenado */    
    --c->card;
    for (j = ini - 1; j < c->card; j++)
        c->v[j] = c->v[j + 1];

    return 1;
}

int pertence_cjt (struct conjunto *c, int elemento)
{
    int ini, fim, meio;

    /* 'elemento' nao pertence ao conjunto se esse 
     * nao foi criado ou tem cardinalidade 0 */
    if (!c || !c->card)
        return 0;
    
    ini = 0;
    fim = c->card - 1;
    while (ini < fim)
    {
        meio = (ini + fim) / 2;
        if (elemento >= c->v[meio])
            ini = meio + 1;
        else
            fim = meio - 1;
    }
    /* verifica se 'elemento' nao esta no conjunto */
    if (!(ini && c->v[ini - 1] == elemento))
        return 0;
    
    return 1;
}

int contido_cjt (struct conjunto *c1, struct conjunto *c2)
{
    int i, j;
    
    /* conjunto vazio esta contido em todos os conjuntos */
    if (!c1)
        return 1;
    /* nenhum conjunto alem do vazio esta contido no vazio */
    if (!c2)
        return 0;

    i = j = 0;
    while (i < c2->card && j != c1->card)
    {
        if (c2->v[i] == c1->v[j])
            j++;
        i++;
    }
    /* nao esta contido se nao percorreu todos os elementos de c1 */
    if (j != c1->card)
        return 0;
    
    return 1;
}

int sao_iguais_cjt (struct conjunto *c1, struct conjunto *c2)
{
    /* dois conjuntos vazios sao iguais */
    if (!c1 && !c2)
        return 1;
    /* se apenas um for vazio ou a cardinalidade
     * deles for diferente, nao podem ser iguais */
    if (!c1 || !c2 || c1->card != c2->card)
        return 0;
    
    /* se c1 tem mesma cardinalidade de c2, soh 
     * pode estar contido em c2 se forem iguais */
    return contido_cjt (c1, c2);
}

struct conjunto *diferenca_cjt (struct conjunto *c1, struct conjunto *c2)
{
    int i, j, k;
    struct conjunto *cjtD;
   
    /* se c1 ou c2 nao foi criado, entao a 
     * diferencao eh o conjunto vazio c1 */
    if (!c1 || !c2)
        return c1;
    /* retorna NULL se houve falha na alocacao de cjtD */
    if (!(cjtD = cria_cjt (c1->card)))
        return NULL;
    
    i = j = k = 0;
    while (i < c1->card && j < c2->card)
    {
        /* se os elementos forem iguais, c1->v[i] nao esta em cjtD */
        if (c1->v[i] == c2->v[j])
        {
            i++;
            j++;
        }
        else
        {
            if (c1->v[i] > c2->v[j])
                j++;
            /* como estao ordenados, se c1->v[i] for menor que c2->v[j],
             * garantidamente esse elemento de c1 nao esta em c2 */
            else
            {
                cjtD->v[k] = c1->v[i];
                k++;
                i++;
            }
        }
    }
    /* se c1 for maior que c2, coloca em cjtD o resto de seus elementos */
    for (j = i; j < c1->card; j++)
    {
        cjtD->v[k] = c1->v[j];
        k++;
    }
    cjtD->card = k;
    
    return cjtD;
}

struct conjunto *interseccao_cjt (struct conjunto *c1, struct conjunto *c2)
{
    int i, j, k;
    struct conjunto *cjtI;
    
    /* se c1 ou c2 nao foi criado, a interseccao eh vazia */
    if (!c1 || !c2)
        return c1;
    /* retorna NULL se houve falha na alocacao de cjtI */
    if (!(cjtI = cria_cjt (min (c1->card, c2->card))))
        return NULL;
    
    i = j = k = 0;
    while (i < c1->card && j < c2->card)
    {
        /* se os elementos forem iguais, um deles esta em cjtI */
        if (c1->v[i] == c2->v[j])
        {
            cjtI->v[k] = c1->v[i];
            k++;
            i++;
            j++;
        }
        else
        {
            /* se c2->v[j] for menor, entao esse nao pode estar na interseccao */
            if (c1->v[i] > c2->v[j])
                j++;
            /* o oposto tambem vale para c1->v[i] */
            else
                i++;
        }
    }
    cjtI->card = k;
    
    return cjtI;
}

struct conjunto *uniao_cjt (struct conjunto *c1, struct conjunto *c2)
{
    int i, j, k;
    struct conjunto *cjtU;

    /* se c1 nao foi criado, a uniao eh c2 */
    if (!c1)
        return c2;
    /* o mesmo vale para c2 */
    if (!c2)
        return c1;
    /* retorna NULL se houve falha na alocacao de cjtU */
    if (!(cjtU = cria_cjt (c1->card + c2->card)))
        return NULL;
    
    i = j = k = 0;
    while (i < c1->card && j < c2->card)
    {
        /* se os elementos forem iguais, em deles esta em cjtU */
        if (c1->v[i] == c2->v[j])
        {
            cjtU->v[k] = c1->v[i];
            i++;
            j++;
        }
        /* caso não, mantem cjt. uniao ordenado colocando o menor elemento */
        else
        {
            if (c1->v[i] > c2->v[j])
            {
                cjtU->v[k] = c2->v[j];
                j++;
            }
            else   
            {
                cjtU->v[k] = c1->v[i];
                i++;
            }
        }
        k++;
    }
    /* coloca os elementos restantes caso a 
     * cardinalidade dos cjts. for diferente */
    while (j < c2->card)
    {
        cjtU->v[k] = c2->v[j];
        j++;
        k++;
    }
    while (i < c1->card)
    {
        cjtU->v[k] = c1->v[i];
        i++;
        k++;
    }
    cjtU->card = k;
    
    return cjtU;   
}

struct conjunto *copia_cjt (struct conjunto *c)
{
    int i;
    struct conjunto *cjtTemp;
    
    /* retorna NULL caso c ou cjtTemp nao foi criado */
    if (!c || !(cjtTemp = cria_cjt (c->card)))
        return NULL;
    
    cjtTemp->card = c->card;
    for (i = 0; i < c->card; i++)
        cjtTemp->v[i] = c->v[i];

    return cjtTemp;
}

struct conjunto *cria_subcjt_cjt (struct conjunto *c, int n)
{
    int temp = 0;
    struct conjunto *cjtAleat;

    /* retorna NULL se o conjunto c nao foi criado */
    if (!c)
        return NULL;
    /* devolve uma copia de c caso n seja maior
     * ou igual que a cardinalidade de c */
    if (n >= c->card)
    {
        cjtAleat = copia_cjt (c);
        return cjtAleat;
    }
    /* retorna NULL se houve falha na alocacao do cjtAleat */
    if (!(cjtAleat = cria_cjt (n)))
        return NULL;
    
    while (cjtAleat->card < n)
    {
        /* temp recebe um iterador pseudoaleatorio valido de c */
        temp = rand () % c->card;
        insere_cjt (cjtAleat, c->v[temp]);
    }

    return cjtAleat;
}

void imprime_cjt (struct conjunto *c)
{
    int j;
    /* c esta vazio se nao foi criado ou tem cardinalidade 0 */
    if (!c || !c->card)
    {   
        printf ("conjunto vazio\n"); 
        return;
    }
    /* ja que os elementos foram inseridos em ordem, c esta ordenado */
    for (j = 0; j < c->card - 1; j++)
        printf ("%d ", c->v[j]);
    printf ("%d\n", c->v[j]);

    return;
}

void inicia_iterador_cjt (struct conjunto *c)
{
    if (c)
        c->ptr = 0;
    
    return;
}

int incrementa_iterador_cjt (struct conjunto *c, int *ret_iterador)
{
    /* c->ptr nao aponta para um elemento valido caso c 
     * nao foi criado ou eh maior ou igual a c->card */
    if (!c || c->ptr >= c->card)
        return 0;
    
    *ret_iterador = c->v[c->ptr];
    ++c->ptr;
    
    return 1;
}

int retira_um_elemento_cjt (struct conjunto *c)
{
    int elemento;

    /* retira o ultimo elemento do conjunto c */
    elemento = c->v[c->card - 1];
    retira_cjt (c, elemento);

    return elemento;
}
