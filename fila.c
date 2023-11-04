#include <stdlib.h>
#include <stdio.h>
#include "fila.h"

struct fila *fila_cria ()
{
    struct fila *novaF;
    
    /* retorna NULL caso alocacao nao foi bem sucedida */    
    if (!(novaF = malloc (sizeof (struct fila))))
        return NULL;
    novaF->tamanho = 0;
    novaF->ini = NULL;
    novaF->fim = NULL;

    return novaF;
}

void fila_destroi (struct fila **fila)
{
    struct nodo *ptrAux;
    
    /* retorna caso fila nao foi criada */
    if (!fila)
        return;
    /* retira elementos enquanto fila nao estiver vazia */
    while ((*fila)->tamanho)
    {
        ptrAux = (*fila)->ini->prox;
        free ((*fila)->ini);
        (*fila)->ini = ptrAux;
        --(*fila)->tamanho;
    }
    free (*fila);
    *fila = NULL;
}

int enqueue (struct fila *fila, int dado)
{
    struct nodo *novoN;
    
    /* retorna caso fila nao foi criada ou
     * houve falha na alocacao de novoN */
    if (!fila || !(novoN = malloc (sizeof(struct nodo))))
        return 0;

    novoN->prox = NULL;
    novoN->chave = dado;
    /* se fila estiver vazia, ao adicionar um elemento,
     * o inicio e fim devem apontar para ele */
    if (!fila->tamanho)
    {
        fila->ini = novoN;
        fila->fim = novoN;
    }
    else
    {
        fila->fim->prox = novoN;
        fila->fim = novoN;
    }
    ++fila->tamanho;

    return 1;
}

int dequeue (struct fila *fila, int *dado)
{
    struct nodo *ptrAux;

    /* nada a fazer se fila estiver vazia */
    if (fila_vazia (fila))
        return 0;

    ptrAux = fila->ini->prox;
    *dado = fila->ini->chave;

    free (fila->ini);
    fila->ini = ptrAux;
    --fila->tamanho;

    return 1;
}

int fila_tamanho (struct fila *fila)
{
    /* tamanho da fila eh 0 caso nao foi criada */
    if (!fila)
        return 0;
    
    return fila->tamanho;
}

int fila_vazia (struct fila *fila)
{
    /* fila esta vazia caso nao foi criada */
    if (!fila)
        return 1;
    
    return !fila->tamanho; 
}

void fila_imprime (struct fila *fila)
{
    int i;
    struct nodo *ptrAux;

    printf ("[ ");
    ptrAux = fila->ini;
    while (ptrAux)
    {
        printf ("%2d ", ptrAux->chave);
        ptrAux = ptrAux->prox;
    }
    printf ("]\n");
}
