#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "conjunto.h"
#include "lef.h"
#include "fila.h"

#define N_HABILIDADES 10
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define MAIOR_DIST 29000

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
    int nMissoesAgendadas;
    int nMissoesResolvidas;
    int nHabilidades;
    int relogio;
    struct coordenadas tamanhoMundo;
    struct heroi herois[N_HABILIDADES * 5];
    struct base bases[(N_HABILIDADES * 5) / 6];
    struct missao missoes[T_FIM_DO_MUNDO / 100];
};

/* inicio funcoes auxiliares */

int aleat (int min, int max)
{
    return min + rand() % (max - min + 1);
}

int calculaDist (struct coordenadas a, struct coordenadas b)
{
    return (int)sqrt ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

int achaMenorPosVet (int vet[], int tam)
{
    int i, menorPos;

    menorPos = 0;
    for (i = 1; i < tam; i++)
        if (vet[menorPos] > vet[i])
            menorPos = i;

    return menorPos;
}

int ajustaY (struct mundo *s, struct base *b, int num)
{
    int i = 0, verifica = 1;

    b->local.y = aleat (0, N_TAMANHO_MUNDO - 1);
    while (verifica && i < num)
    {
        verifica = (s->bases[i].local.y != b->local.y);
        i++;
    }

    return verifica;
}

/* fim funcoes auxiliares */

struct heroi criaHeroi (int id)
{
    struct heroi h;

    h.id = id;
    h.experiencia = 0;
    h.velocidade = aleat (50, 5000);
    h.paciencia = aleat (0, 100);
    h.habilidades = cria_cjt (aleat (1, 3));
    while (insere_cjt (h.habilidades, aleat (0, N_HABILIDADES - 1)));
    
    return h;
}

struct base criaBase (int id, struct mundo *s)
{
    struct base b;

    b.id = id;
    b.lotacaoMax = aleat (3, 10);
    b.filaEspera = fila_cria ();
    b.presentes = cria_cjt (b.lotacaoMax);
    b.local.x = aleat (0, N_TAMANHO_MUNDO - 1);
    while (!ajustaY (s, &b, id));

    return b;
}   

struct missao criaMissao (int id)
{
    struct missao m;

    m.id = id;
    m.local.x = aleat (0, N_TAMANHO_MUNDO - 1);
    m.local.y = aleat (0, N_TAMANHO_MUNDO - 1);
    m.habilidades = cria_cjt (aleat (6, 10));
    while (insere_cjt (m.habilidades, aleat (0, N_HABILIDADES - 1)));

    return m;  
}

void inicializaMundo (struct mundo *simulacao, struct lef_t *eventos)
{
    int i;

    simulacao->nHabilidades = N_HABILIDADES;
    simulacao->nHerois = N_HABILIDADES * 5;
    simulacao->nBases = simulacao->nHerois / 6;
    simulacao->nMissoes = simulacao->nMissoesAgendadas = T_FIM_DO_MUNDO / 100;
    simulacao->tamanhoMundo.x = simulacao->tamanhoMundo.y = N_TAMANHO_MUNDO;
    simulacao->nMissoesResolvidas = simulacao->relogio = 0;

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

    insere_lef (eventos, cria_evento (T_FIM_DO_MUNDO, 9, 0, 0));
}

void heroiChega (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    int c = cardinalidade_cjt (s->bases[b].presentes);
    int p = s->herois[h].paciencia;
    int lMax = s->bases[b].lotacaoMax;
    struct fila *f = s->bases[b].filaEspera;

    s->herois[h].base = b;

    printf ("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ", s->relogio, h, b, c, lMax);
    if ((c < lMax && fila_vazia (f)) || p > 10 * fila_tamanho (f))
    {
        printf ("ESPERA\n");
        insere_lef (e, cria_evento (s->relogio, 3, h, b));
    }
    else
    {
        printf ("DESISTE\n");
        insere_lef (e, cria_evento (s->relogio, 4, h, 0)); 
    }   
}

void heroiEspera (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = s->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    struct fila *f = s->bases[b].filaEspera;

    printf ("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", t, h, b, fila_tamanho(f));
    enqueue (s->bases[b].filaEspera, h);
    insere_lef (e, cria_evento (t, 5, 0, b));
}

void heroiDesiste (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = s->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;

    printf ("%6d: DESISTE HEROI %2d BASE %d\n", t, h, b);
    insere_lef (e, cria_evento (t, 8, h, aleat (0, s->nBases - 1)));
}

void avisaPorteiro (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int h;
    int t = s->relogio;
    int bId = eventoTemp->dado2;
    struct base b = s->bases[bId];
    int c = cardinalidade_cjt (b.presentes);
    int lMax = b.lotacaoMax;
    
    printf ("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA ", t, bId, c, lMax);
    fila_imprime (b.filaEspera);
    while (c < lMax && dequeue (b.filaEspera, &h))
    {
        insere_cjt (b.presentes, h);
        insere_lef (e, cria_evento (t, 6, h, bId));
        printf ("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", t, bId, h);
        c++;
    } 
}

void heroiEntra (struct mundo *simulacao, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = simulacao->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    int lMax = simulacao->bases[b].lotacaoMax;
    int c = cardinalidade_cjt (simulacao->bases[b].presentes);
    int temp;

    temp = t + 15 + simulacao->herois[h].paciencia * aleat (1, 20);
    insere_lef (e, cria_evento (temp, 7, h, b));
    printf ("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", t, h, b, c, lMax, temp);
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
    printf ("%6d: SAI    HEROI %2d BASE %d (%2d/%2d)\n", t, h, b, presentes, ltcMax);
}

void heroiViaja (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int h = eventoTemp->dado1, t;
    struct coordenadas bDest = s->bases[eventoTemp->dado2].local;
    struct coordenadas bAtual = s->bases[s->herois[h].base].local;
    int vel = s->herois[h].velocidade;

    vel = s->herois[h].velocidade;
    t = s->relogio + calculaDist (bAtual, bDest) / vel;

    insere_lef (e, cria_evento (t, 1, h, eventoTemp->dado2));
    printf ("%6d: VIAJA  HEROI %2d ", s->relogio, h);
    printf ("BASE %d BASE %d ", s->herois[h].base, eventoTemp->dado2);
    printf ("DIST %d VEL %d CHEGA %d\n", calculaDist (bAtual, bDest), vel, t);
}

void missao (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = s->relogio;
    int mId = eventoTemp->dado1;
    int dists[s->nBases];
    int j, menorPos, hId, achou; 
    struct conjunto *aux, *habilidadesB;

    printf ("%6d: MISSAO %d HAB REQ: ", s->relogio, mId);
    imprime_cjt (s->missoes[mId].habilidades);

    for (j = 0; j < s->nBases; j++)
        dists[j] = calculaDist (s->bases[j].local, s->missoes[mId].local);

    achou = j = 0;
    while (!achou && j < s->nBases)
    {
        habilidadesB = cria_cjt (N_HABILIDADES);
        menorPos = achaMenorPosVet (dists, s->nBases);
        dists[menorPos] = MAIOR_DIST;

        inicia_iterador_cjt (s->bases[menorPos].presentes);
        while (incrementa_iterador_cjt (s->bases[menorPos].presentes, &hId))
        {
            aux = habilidadesB;
            habilidadesB = uniao_cjt (habilidadesB, s->herois[hId].habilidades);
            destroi_cjt (aux);
        }

        printf ("%6d: MISSAO %d HAB BASE %d: ", t, mId, menorPos);
        imprime_cjt (habilidadesB);
        achou = contido_cjt (s->missoes[mId].habilidades, habilidadesB);
        destroi_cjt (habilidadesB);
        j++;
    }

    if (achou)
    {
        printf ("%6d: MISSAO %d CUMPRIDA BASE %d HEROIS: ", t, mId, menorPos);
        imprime_cjt (s->bases[menorPos].presentes);

        inicia_iterador_cjt (s->bases[menorPos].presentes);
        while (incrementa_iterador_cjt (s->bases[menorPos].presentes, &hId))
            ++s->herois[hId].experiencia;

        ++s->nMissoesResolvidas;
        return;
    }
    
    printf ("%6d: MISSAO %d IMPOSSIVEL\n", t, mId);
    if (t + 1440 < T_FIM_DO_MUNDO)
    {
        insere_lef (e, cria_evento (t + 1440, 2, mId, 0));
        ++s->nMissoesAgendadas;
    }
}

void fimSimulacao (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int i;
    struct heroi h;

    printf ("%6d: FIM\n", s->relogio);

    for (i = 0; i < s->nHerois; i++)
    {
        h = s->herois[i];
        printf ("HEROI %2d PAC %3d VEL %4d EXP %4d HABS ", i, h.paciencia, h.velocidade, h.experiencia);
        imprime_cjt (h.habilidades);
        h.habilidades = destroi_cjt (h.habilidades);
    }

    printf ("%d/%d MISSOES CUMPRIDAS ", s->nMissoesResolvidas, s->nMissoes);
    printf ("(%.2f%%), MEDIA ", (float)(100*s->nMissoesResolvidas / s->nMissoes));
    printf ("%.2f TENTATIVAS/MISSAO\n", (float)(s->nMissoesAgendadas / s->nMissoes));

    for (i = 0; i < s->nBases; i++)
    {
        s->bases[i].presentes = destroi_cjt (s->bases[i].presentes);
        fila_destroi (&s->bases[i].filaEspera);
    }

    for (i = 0; i < s->nMissoes; i++)
        s->missoes[i].habilidades = destroi_cjt (s->missoes[i].habilidades);

    destroi_evento (eventoTemp);
    destroi_lef (e);
}

int main ()
{
    struct evento_t *eventoTemp;
    struct lef_t *eventos;
    struct mundo simulacao;
    void (*vetFuncoes[])(struct mundo *, struct evento_t *, struct lef_t *) = 
    {
        &heroiChega, 
        &missao, 
        &heroiEspera, 
        &heroiDesiste, 
        &avisaPorteiro, 
        &heroiEntra, 
        &heroiSai, 
        &heroiViaja,
    };

    srand (0); 

    eventos = cria_lef ();
    inicializaMundo (&simulacao, eventos);

    eventoTemp = retira_lef (eventos);
    while (simulacao.relogio < T_FIM_DO_MUNDO)
    {
        while (eventoTemp->tempo == simulacao.relogio)
        {
            (*vetFuncoes[eventoTemp->tipo - 1])(&simulacao, eventoTemp, eventos);
            destroi_evento (eventoTemp);
            eventoTemp = retira_lef (eventos);
        }

        ++simulacao.relogio;
    }

    /* evento fim destroi tudo e imprime estatisticas */
    fimSimulacao (&simulacao, eventoTemp, eventos);

    return 0;
}
