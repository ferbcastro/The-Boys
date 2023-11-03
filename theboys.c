#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "conjunto.h"
#include "lef.h"
#include "fila.h"

#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS N_HABILIDADES * 5
#define N_BASES N_HEROIS / 6
#define N_MISSOES T_FIM_DO_MUNDO / 100

/*
* 1: CHEGA
* 2: MISSAO
* 3:
*
*
*
* 7: FIM
*/

struct coordenadas
{
    int x;
    int y;
};

struct heroi
{
    int id;
    int base;
    int experiencia;
    int paciencia;
    int velocidade;
    struct conjunto *habilidades;
};

struct base
{
    int id;
    int lotacaoMax;
    struct coordenadas local;
    struct conjunto *presentes;
    struct fila *filaEspera;
};

struct missao
{
    int id;
    struct coordenadas local;
    struct conjunto *habilidades;
};

struct mundo
{
    int nHerois;
    int nBases;
    int nMissoes;
    int nHabilidades;
    int relogio;
    struct coordenadas tamanhoMundo;
    struct heroi herois[N_HEROIS];
    struct base bases[N_BASES];
    struct missao missoes[N_MISSOES];
};

int aleat (int min, int max)
{
    return min + (rand() % max - min + 1); 
}

struct heroi criaHeroi (int id)
{
    struct heroi hTemp;
    int cjtComEspaco;

    hTemp.id = id;
    hTemp.experiencia = 0;
    hTemp.velocidade = aleat (50, 5000);
    hTemp.paciencia = aleat (0, 100);
    hTemp.habilidades = cria_cjt (aleat (1, 3));

    cjtComEspaco = insere_cjt (hTemp.habilidades, aleat (1, N_HABILIDADES));
    while (cjtComEspaco)
        insere_cjt (hTemp.habilidades, aleat (1, N_HABILIDADES));
    
    return hTemp;
}

struct base criaBase (int id, struct coordenadas locais[])
{
    struct base bTemp;
    int disponivel, achou = 0;

    bTemp.id = id;
    bTemp.lotacaoMax = aleat (3, 10);
    bTemp.filaEspera = fila_cria ();
    bTemp.presentes = cria_cjt (bTemp.lotacaoMax);

    while (!achou)
    { 
        bTemp.local.x = aleat (0, N_TAMANHO_MUNDO - 1);
        bTemp.local.y = aleat (0, N_TAMANHO_MUNDO - 1);
        disponivel = 1;
        while (--id)
            if (bTemp.local.x == locais[id].x && bTemp.local.y == locais[id].y)
                disponivel = 0;
        if (disponivel)
            achou = 1;
    }
    locais[id].x = bTemp.local.x;
    locais[id].y = bTemp.local.y;

    return bTemp;
}   

struct missao criaMissao (int id)
{
    struct missao mTemp;
    int cjtComEspaco;

    mTemp.id = id;
    mTemp.local.x = aleat (0, N_TAMANHO_MUNDO - 1);
    mTemp.local.y = aleat (0, N_TAMANHO_MUNDO - 1);

    mTemp.habilidades = cria_cjt (aleat (6, 10));
    cjtComEspaco = insere_cjt (mTemp.habilidades, aleat (1, N_HABILIDADES));
    while (cjtComEspaco)
        insere_cjt (mTemp.habilidades, aleat (1, 10));

    return mTemp;  
}

void inicializaMundo (struct mundo *simulacao, struct lef_t *eventos)
{
    int i;
    struct evento_t *temp;
    struct coordenadas locais [N_BASES];

    simulacao->nHerois = N_HEROIS;
    simulacao->nBases = N_BASES;
    simulacao->nMissoes = N_MISSOES;
    simulacao->nHabilidades = N_HABILIDADES;
    simulacao->tamanhoMundo.x = N_TAMANHO_MUNDO;
    simulacao->tamanhoMundo.y = N_TAMANHO_MUNDO;
    simulacao->relogio = 0;

    for (i = 0; i < N_BASES; i++)
        simulacao->bases[i] = criaBase (i, locais);

    for (i = 0; i < N_HEROIS; i++)
    {
        simulacao->herois[i] = criaHeroi (i);
        temp = cria_evento (aleat (0, 4320), 1, i, aleat (0, N_BASES - 1));
        insere_lef (eventos, temp);
    }

    for (i = 0; i < N_MISSOES; i++)
    {
        simulacao->missoes[i] = criaMissao (i);
        temp = cria_evento (aleat (0, T_FIM_DO_MUNDO), 2, i, 0);
        insere_lef (eventos, temp);
    }

    temp = cria_evento (T_FIM_DO_MUNDO, 7, 0, 0);
    insere_lef (eventos, temp);
}

void chegaHeroi (struct mundo *simulacao, struct evento_t *eventoTemp)
{
    unsigned short int espera;
    int t = simulacao->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    int prts = simulacao->bases[eventoTemp->dado2].presentes->card;
    int ltcMax = simulacao->bases[eventoTemp->dado2].lotacaoMax;
    int pcs = simulacao->herois[eventoTemp->dado1].paciencia;

    if (prts < ltcMax && fila_vazia (simulacao->bases[b].filaEspera))
        espera = 1;
    else
        espera = pcs > 10*(fila_tamanho (simulacao->bases[b].filaEspera));

    printf ("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ", t, h, b, prts, ltcMax);
    if (espera)
        printf ();
    else 
        printf ();    
}

int main ()
{
    struct evento_t *eventoTemp;
    struct lef_t *eventos;
    struct mundo simulacao;

    srand (0); 

    eventos = cria_lef ();
    inicializaMundo (&simulacao, eventos);

    eventoTemp = retira_lef (eventos);
    while (simulacao.relogio <= T_FIM_DO_MUNDO)
    {
        if (eventoTemp->tempo == simulacao.relogio)
        {
            switch (eventoTemp->tipo)
            {
            case 1:
                chegaHeroi (&simulacao);
                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            case 5:

                break;
            case 6:

                break;
            case 7:

                break;
            default:
                break;
            }

            destroi_evento (eventoTemp);
            eventoTemp = retira_lef (&eventos);
        }

        simulacao.relogio++;
    }

    return 0;
}
