#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "conjunto.h"
#include "lef.h"
#include "fila.h"

#define N_HABILIDADES 10
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000

/*
* 1: CHEGA
* 2: MISSAO
* 3: ESPERA
* 4: DESISTE
* 5: AVISA
* 6: ENTRA 
* 7: SAI
* 8: VIAJA
* 9: FIM
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
    struct heroi herois[N_HABILIDADES * 5];
    struct base bases[(N_HABILIDADES * 5) / 6];
    struct missao missoes[T_FIM_DO_MUNDO / 100];
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

struct base criaBase (int id, struct mundo *simulacao)
{
    struct base bTemp;
    int i, disponivel, achou = 0;

    bTemp.id = id;
    bTemp.lotacaoMax = aleat (3, 10);
    bTemp.filaEspera = fila_cria ();
    bTemp.presentes = cria_cjt (bTemp.lotacaoMax);

    while (!achou)
    { 
        bTemp.local.x = aleat (0, N_TAMANHO_MUNDO - 1);
        bTemp.local.y = aleat (0, N_TAMANHO_MUNDO - 1);
        disponivel = 1;
        for (i = id - 1; i >= 0; i--)
            if (bTemp.local.x == simulacao->bases[id].local.x && bTemp.local.y == simulacao->bases[id].local.y)
                disponivel = 0;
        if (disponivel)
            achou = 1;
    }

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

    simulacao->nHabilidades = N_HABILIDADES;
    simulacao->nHerois = N_HABILIDADES * 5;
    simulacao->nBases = simulacao->nHerois / 6;
    simulacao->nMissoes = T_FIM_DO_MUNDO / 100;
    simulacao->tamanhoMundo.x = N_TAMANHO_MUNDO;
    simulacao->tamanhoMundo.y = N_TAMANHO_MUNDO;
    simulacao->relogio = 0;

    for (i = 0; i < simulacao->nBases; i++)
        simulacao->bases[i] = criaBase (i, simulacao);

    for (i = 0; i < simulacao->nHerois; i++)
    {
        simulacao->herois[i] = criaHeroi (i);
        insere_lef (eventos, cria_evento (aleat (0, 4320), 1, i, aleat (0, simulacao->nBases - 1)));
    }

    for (i = 0; i < simulacao->nMissoes; i++)
    {
        simulacao->missoes[i] = criaMissao (i);
        insere_lef (eventos, cria_evento (aleat (0, T_FIM_DO_MUNDO), 2, i, 0));
    }

    insere_lef (eventos, cria_evento (T_FIM_DO_MUNDO, 7, 0, 0));
}

void heroiChega (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{
    unsigned short int espera;
    int t = simulacao->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    int presentes = cardinalidade_cjt (simulacao->bases[b].presentes);
    int ltcMax = simulacao->bases[b].lotacaoMax;
    int pcs = simulacao->herois[h].paciencia;

    simulacao->herois[h].base = b;

    if (presentes < ltcMax && fila_vazia (simulacao->bases[b].filaEspera))
        espera = 1;
    else
        espera = pcs > 10*(fila_tamanho (simulacao->bases[b].filaEspera));

    printf ("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ", t, h, b, presentes, ltcMax);
    if (espera)
    {
        printf ("ESPERA\n");
        insere_lef (e, cria_evento (t, 3, h, b)); 
    }
    else
    { 
        printf ("DESISTE\n");
        insere_lef (e, cria_evento (t, 4, h, 0)); 
    }    
}

void heroiEspera (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = simulacao->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;

    printf ("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", t, h, b, fila_tamanho(simulacao->bases[b].filaEspera));
    enqueue (simulacao->bases[b].filaEspera, h);
    insere_lef (e, cria_evento (t, 5, 0, b));
}

void heroiDesiste (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = simulacao->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;

    printf ("%6d: DESISTE HEROI %2d BASE %d\n", t, h, b);
    insere_lef (e, cria_evento (t, 8, h, aleat (0, simulacao->nBases)));
}

void avisaPorteiro (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = simulacao->relogio;
    int b = eventoTemp->dado2;
    int presentes = cardinalidade_cjt (simulacao->bases[b].presentes);
    int ltcMax = simulacao->bases[b].lotacaoMax;
    int i, h;

    printf ("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA", t, b, presentes, ltcMax);
    fila_imprime (simulacao->bases[b].filaEspera);
    while (presentes < ltcMax && !fila_vazia (simulacao->bases[b].filaEspera))
    {
        dequeue (simulacao->bases[b].filaEspera, &h);
        insere_cjt (simulacao->bases[b].presentes, h);
        insere_lef (e, cria_evento (t, 6, h, b));
        printf ("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d", t, b, h);
        presentes++;
    } 
    printf ("]\n"); 
}

void heroiEntra (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = simulacao->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    int ltcMax = simulacao->bases[b].lotacaoMax;
    int pcs = simulacao->herois[h].paciencia;
    int presentes = cardinalidade_cjt (simulacao->bases[b].presentes);
    int tpb;

    tpb = 15 + pcs * aleat (1, 20);
    insere_lef (e, cria_evento (t + tpb, 7, h, b));
    printf ("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d", t, h, b, presentes, ltcMax);
}

void heroiSai (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = simulacao->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    int ltcMax = simulacao->bases[b].lotacaoMax;
    int presentes;

    retira_cjt (simulacao->bases[b].presentes, h);
    presentes = cardinalidade_cjt (simulacao->bases[b].presentes);

    insere_lef (e, cria_evento (t, 8, h, aleat (0, simulacao->nBases - 1)));
    insere_lef (e, cria_evento (t, 5, 0, b));
    printf ("%6d: SAI    HEROI %2d BASE %d (%2d/%2d)", t, h, b, presentes, ltcMax);
}

void heroiViaja (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{

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
    while (simulacao.relogio < T_FIM_DO_MUNDO)
    {
        if (eventoTemp->tempo == simulacao.relogio)
        {
            switch (eventoTemp->tipo)
            {
                case 1:
                    heroiChega (&simulacao, eventoTemp, eventos);
                    break;
                case 2:

                    break;
                case 3:
                    heroiEspera (&simulacao, eventoTemp, eventos);
                    break;
                case 4:
                    heroiDesiste (&simulacao, eventoTemp, eventos);
                    break;
                case 5:
                    avisaPorteiro (&simulacao, eventoTemp, eventos);
                    break;
                case 6:
                    heroiEntra (&simulacao, eventoTemp, eventos);
                    break;
                case 7:
                    heroiSai (&simulacao, eventoTemp, eventos);
                    break;
                case 8:
                    heroiViaja (&simulacao, eventoTemp, eventos);
                    break;
                default:
                    break;
            }

            destroi_evento (eventoTemp);
            eventoTemp = retira_lef (&eventos);
        }

        ++simulacao.relogio;
    }

    return 0;
}
