#include <stdlib.h>
#include <stdio.h>
#include "lef.h"

struct evento_t *cria_evento (int tempo, int tipo, int dado1, int dado2)
{
    struct evento_t *auxE;

    /* retorna NULL se a alocacao nao foi bem sucedida */
    auxE = malloc (sizeof (struct evento_t));
    if (!auxE)
        return NULL;
    
    auxE->tempo = tempo;
    auxE->tipo = tipo;
    auxE->dado1 = dado1;
    auxE->dado2 = dado2;

    return auxE;
}
 
struct evento_t *destroi_evento (struct evento_t *e)
{
    /* evento so pode ser liberado se tiver sido alocado */
    if (e)
        free (e);

    return NULL;
}

struct lef_t *cria_lef ()
{
    struct lef_t *novaL;

    /* retorna NULL se alocacao nao foi bem sucedida */
    novaL = malloc (sizeof (struct lef_t));
    if (!novaL)
        return NULL;
    novaL->primeiro = NULL;

    return novaL;
}

struct lef_t *destroi_lef (struct lef_t *l)
{
    struct nodo_lef_t *ptrAux;

    /* retorna NULL se a lef nao foi criada */
    if (!l)
        return NULL;
    
    /* o loop libera os nodos e eventos enquanto ptrAux nao for NULL */
    while (l->primeiro)
    {
        ptrAux = l->primeiro->prox;
        free (l->primeiro->evento);
        free (l->primeiro);
        l->primeiro = ptrAux;
    }
    free (l);

    return NULL;
}

int insere_lef (struct lef_t *l, struct evento_t *e)
{
    struct nodo_lef_t *novoN, *ptrAux;

    /* retorna 0 caso a fila nao exista ou o evento foi criado */
    if (!l || !e)
        return 0;
    /* retorna 0 se a alocacao nao foi bem sucedida */
    novoN = malloc (sizeof(struct nodo_lef_t));
    if (!novoN)
        return 0;

    novoN->evento = e;
    /* trata o caso em que a lista esteja vazia ou que
     * o tempo do evento for menor que todos da fila */
    if (!l->primeiro || e->tempo < l->primeiro->evento->tempo)
    {
        novoN->prox = l->primeiro;
        l->primeiro = novoN;
        return 1;
    }
    /* insere ordenado na lef */
    ptrAux = l->primeiro;
    while (ptrAux->prox && ptrAux->prox->evento->tempo <= e->tempo)
        ptrAux = ptrAux->prox;
    novoN->prox = ptrAux->prox;
    ptrAux->prox = novoN;

    return 1;
}

struct evento_t *retira_lef (struct lef_t *l)
{
    struct evento_t *auxE;
    struct nodo_lef_t *ptrAux;

    /* retorna NULL se a lef nao foi criada */
    if (!l)
        return NULL;
    
    auxE = l->primeiro->evento;
    ptrAux = l->primeiro->prox;
    free (l->primeiro);
    l->primeiro = ptrAux;

    return auxE;
}

int vazia_lef (struct lef_t *l)
{
    /* lef esta vazia se nao foi criada ou se nao contem elementos */
    if (!l || !l->primeiro)
        return 1;
    
    return 0;
}

void imprime_lef (struct lef_t *l)
{
    struct nodo_lef_t *ptrAux;
    int cont = 0;

    /* retorna caso lef nao foi criada */
    if (!l)
        return;
    
    printf ("LEF:\n");
    ptrAux = l->primeiro;
    while (ptrAux)
    {
        printf ("  tempo %d ", ptrAux->evento->tempo);
        printf ("tipo %d ", ptrAux->evento->tipo);
        printf ("d1 %d ", ptrAux->evento->dado1);
        printf ("d2 %d\n", ptrAux->evento->dado2);
        ptrAux = ptrAux->prox;
        cont++;
    }
    printf ("  Total %d eventos\n", cont);

    return;
}
