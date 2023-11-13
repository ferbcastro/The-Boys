/*
* Projeto 'The Boys' 
* Disciplina CI1001 - Programacao I
* Autor: Fernando de Barros Castro
* Data: 12/11/2023
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "conjunto.h"
#include "lef.h"
#include "fila.h"

#define N_HABILIDADES 10
#define N_HEROIS N_HABILIDADES * 5
#define N_BASES N_HEROIS / 6
#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define T_MIN_CHEGADA_H 0
#define T_MAX_CHEGADA_H 4320
#define N_MISSOES T_FIM_DO_MUNDO / 100
#define N_TAMANHO_MUNDO 20000
#define MIN_X 0
#define MIN_Y 0
#define MAX_X N_TAMANHO_MUNDO - 1
#define MAX_Y N_TAMANHO_MUNDO - 1
#define MAIOR_DIST 29000
#define MIN_HABILIDADES_H 1
#define MAX_HABILIDADES_H 3
#define MIN_VELOCIDADE_H 50
#define MAX_VELOCIDADE_H 5000
#define MIN_PACIENCIA_H 0
#define MAX_PACIENCIA_H 100
#define MIN_HABILIDADES_M 6
#define MAX_HABILIDADES_M 10
#define MIN_HEROIS_B 3
#define MAX_HEROIS_B 10

/*
* CODIGO PARA CADA EVENTO: 
* 
* 1: CHEGA, 2: MISSAO, 3: ESPERA, 4: DESISTE,
* 5: AVISA, 6: ENTRA, 7: SAI, 8: VIAJA, 9: FIM
*/

/*
* 1 UNIDADE DE TEMPO = 1 MINUTO
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
    int resolvida;
    int nAgendamentos;
    struct coordenadas local;
    struct conjunto *habilidades;
};

struct mundo
{
    int nHerois;
    int nBases;
    int nMissoes;
    int nMissoesResolvidas;
    int nHabilidades;
    int relogio;
    struct coordenadas tamanhoMundo;
    struct heroi herois[N_HEROIS];
    struct base bases[N_BASES];
    struct missao missoes[N_MISSOES];
};

/* inicio funcoes auxiliares */

/* retorna um pseudo aleatorio do conjunto fechado [min, max] */
int aleat (int min, int max)
{
    return min + rand() % (max - min + 1);
}

/* calcula distancia cartesiana entre dois pontos (x, y) */
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

/* sorteia 'y' da base e retorna 0 se alguma 
 * base ja tem o mesmo 'y' e 1 caso contrario */
int ajustaY (struct mundo *s, struct base *b, int num)
{
    int i = 0, verifica = 1;

    b->local.y = aleat (MIN_Y, MAX_Y);
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
    h.velocidade = aleat (MIN_VELOCIDADE_H, MAX_VELOCIDADE_H);
    h.paciencia = aleat (MIN_PACIENCIA_H, MAX_PACIENCIA_H);
    h.habilidades = cria_cjt (aleat (MIN_HABILIDADES_H, MAX_HABILIDADES_H));
    while (insere_cjt (h.habilidades, aleat (0, N_HABILIDADES - 1)));
    
    return h;
}

struct base criaBase (int id, struct mundo *s)
{
    struct base b;

    b.id = id;
    b.lotacaoMax = aleat (MIN_HEROIS_B, MAX_HEROIS_B);
    b.filaEspera = fila_cria ();
    b.presentes = cria_cjt (b.lotacaoMax);
    b.local.x = aleat (MIN_X, MAX_X);
    /* o loop garante que todas as bases
     * tenham coordenadas diferentes */
    while (!ajustaY (s, &b, id));

    return b;
}   

struct missao criaMissao (int id)
{
    struct missao m;

    m.id = id;
    m.nAgendamentos = 1;
    m.resolvida = 0;
    m.local.x = aleat (MIN_X, MAX_X);
    m.local.y = aleat (MIN_Y, MAX_Y);
    m.habilidades = cria_cjt (aleat (MIN_HABILIDADES_M, MAX_HABILIDADES_M));
    while (insere_cjt (m.habilidades, aleat (0, N_HABILIDADES - 1)));

    return m;  
}

void inicializaMundo (struct mundo *s, struct lef_t *eventos)
{
    int i;

    s->nHabilidades = N_HABILIDADES;
    s->nHerois = N_HEROIS;
    s->nBases = N_BASES;
    s->nMissoes = N_MISSOES;
    s->tamanhoMundo.x = s->tamanhoMundo.y = N_TAMANHO_MUNDO;
    s->nMissoesResolvidas = s->relogio = T_INICIO;

    /* insere na lef no tempo T_FIM_DO_MUNDO 
     * o evento que termina a simulacao */
    insere_lef (eventos, cria_evento (T_FIM_DO_MUNDO, 9, 0, 0));

    for (i = 0; i < s->nBases; i++)
        s->bases[i] = criaBase (i, s);

    /* cria os herois e sorteia o tempo de chegada 
     * de cada um em uma base tambem sorteada */
    for (i = 0; i < s->nHerois; i++)
    {
        s->herois[i] = criaHeroi (i);
        insere_lef (eventos, cria_evento (aleat (T_MIN_CHEGADA_H, T_MAX_CHEGADA_H), 
                    1, i, aleat (0, s->nBases - 1)));
    }

    /* cria as missoes e sorteia para cada 
     * uma quando devera ser realizada */
    for (i = 0; i < s->nMissoes; i++)
    {
        s->missoes[i] = criaMissao (i);
        insere_lef (eventos, cria_evento (aleat (T_INICIO, T_FIM_DO_MUNDO), 2, i, 0));
    }
}

void heroiChega (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    int c = cardinalidade_cjt (s->bases[b].presentes);
    int p = s->herois[h].paciencia;
    int lMax = s->bases[b].lotacaoMax;
    struct fila *f = s->bases[b].filaEspera;

    /* atualiza a base do heroi ao chegar na base */
    s->herois[h].base = b;

    /* heroi espera na fila se essa estiver vazia e houver vagas na base 
     * ou caso sua paciencia seja dez vezes maior que o tamanho da fila */
    printf ("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ", s->relogio, h, b, c, lMax);
    if ((c < lMax && fila_vazia (f)) || p > 10 * fila_tamanho (f))
    {
        printf ("ESPERA\n");
        insere_lef (e, cria_evento (s->relogio, 3, h, b));
        return;
    }
    
    printf ("DESISTE\n");
    insere_lef (e, cria_evento (s->relogio, 4, h, 0));    
}

void heroiEspera (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = s->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;
    struct fila *f = s->bases[b].filaEspera;

    /* o heroi eh adicionada a fila de espera 
     * e o porteiro avisado de sua chegada */
    printf ("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", t, h, b, fila_tamanho(f));
    enqueue (s->bases[b].filaEspera, h);
    insere_lef (e, cria_evento (t, 5, 0, b));
}

void heroiDesiste (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int t = s->relogio;
    int h = eventoTemp->dado1;
    int b = eventoTemp->dado2;

    /* uma base qualquer eh sorteada como novo destino do heroi */
    printf ("%6d: DESIST HEROI %2d BASE %d\n", t, h, b);
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

    /* porteiro libera entrada se houver vaga
     * na base e heroi(s) esperando na fila */
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

    /* abaixo o evento heroiSai eh agendado com base na 
     * duracao calculada da permanencia do heroi na base */
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

    /* heroi eh retirado do conjunto de presentes da base */
    retira_cjt (simulacao->bases[b].presentes, h);
    presentes = cardinalidade_cjt (simulacao->bases[b].presentes);

    /* uma nova base destino eh sorteada para heroi e o 
     * porteiro avisado de que ha mais uma vaga na base */
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

    /* abaixo o evento heroiChega eh agendado com base na
     * duracao calculada da viagem ate a base destino */
    t = s->relogio + calculaDist (bAtual, bDest) / vel;
    insere_lef (e, cria_evento (t, 1, h, eventoTemp->dado2));
    
    printf ("%6d: VIAJA  HEROI %2d ", s->relogio, h);
    printf ("BASE %d BASE %d ", s->herois[h].base, eventoTemp->dado2);
    printf ("DIST %d VEL %d CHEGA %d\n", calculaDist (bAtual, bDest), vel, t);
}

void missao (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int dists[s->nBases];
    int j, menorPos, hId, achou;
    int t = s->relogio, mId = eventoTemp->dado1;
    struct conjunto *aux, *habilidadesB;

    printf ("%6d: MISSAO %d HAB REQ: ", s->relogio, mId);
    imprime_cjt (s->missoes[mId].habilidades);

    /* a distancia da base 'i' ate a missao eh guardada em dists[i] */
    for (j = 0; j < s->nBases; j++)
        dists[j] = calculaDist (s->bases[j].local, s->missoes[mId].local);

    /* loop procura pela base mais proxima que possa executar a missao */
    achou = j = 0;
    while (!achou && j < s->nBases)
    {
        /* apos achar a base 'i' mais perto, ela eh descartada 
         * como possibilidade para a proxima iteracao */
        habilidadesB = cria_cjt (s->nHabilidades);
        menorPos = achaMenorPosVet (dists, s->nBases);
        dists[menorPos] = MAIOR_DIST;

        /* as diferentes habilidades dos herois da base 
         * 'i' sao reunidas no conjunto habilidadesB */
        inicia_iterador_cjt (s->bases[menorPos].presentes);
        while (incrementa_iterador_cjt (s->bases[menorPos].presentes, &hId))
        {
            aux = habilidadesB;
            habilidadesB = uniao_cjt (habilidadesB, s->herois[hId].habilidades);
            destroi_cjt (aux);
        }

        printf ("%6d: MISSAO %d HAB BASE %d: ", t, mId, menorPos);
        imprime_cjt (habilidadesB);
        /* verifica se a base 'i' eh compativel com a missao */
        achou = contido_cjt (s->missoes[mId].habilidades, habilidadesB);
        destroi_cjt (habilidadesB);
        j++;
    }

    if (achou)
    {
        printf ("%6d: MISSAO %d CUMPRIDA BASE %d HEROIS: ", t, mId, menorPos);
        imprime_cjt (s->bases[menorPos].presentes);

        /* incrementa em 1 a experiencia dos herois que executaram a missao */
        inicia_iterador_cjt (s->bases[menorPos].presentes);
        while (incrementa_iterador_cjt (s->bases[menorPos].presentes, &hId))
            ++s->herois[hId].experiencia;

        ++s->nMissoesResolvidas;
        s->missoes[mId].resolvida = 1;
        return;
    }
    
    /* se missao seja impossivel, essa eh reagendada em 24h */
    printf ("%6d: MISSAO %d IMPOSSIVEL\n", t, mId);
    insere_lef (e, cria_evento (t + 1440, 2, mId, 0));
    ++s->missoes[mId].nAgendamentos;
}

void fimSimulacao (struct mundo *s, struct evento_t *eventoTemp, struct lef_t *e)
{
    int i, somaAgendamentos = 0;

    printf ("%6d: FIM\n", s->relogio);

    /* abaixo toda a memoria alocada eh liberada */
    for (i = 0; i < s->nHerois; i++)
    {
        printf ("HEROI %2d PAC %3d ", i, s->herois[i].paciencia);
        printf ("VEL %4d ", s->herois[i].velocidade);
        printf ("EXP %4d HABS ", s->herois[i].experiencia);
        imprime_cjt (s->herois[i].habilidades);
        s->herois[i].habilidades = destroi_cjt (s->herois[i].habilidades);
    }

    for (i = 0; i < s->nBases; i++)
    {
        s->bases[i].presentes = destroi_cjt (s->bases[i].presentes);
        fila_destroi (&s->bases[i].filaEspera);
    }

    for (i = 0; i < s->nMissoes; i++)
        s->missoes[i].habilidades = destroi_cjt (s->missoes[i].habilidades);

    destroi_evento (eventoTemp);
    destroi_lef (e);

    /* abaixo sao impressas as estatisticas da simulacao */
    printf ("%d/%d MISSOES CUMPRIDAS ", s->nMissoesResolvidas, s->nMissoes);
    printf ("(%.2f%%), MEDIA ", (float)(100*s->nMissoesResolvidas / s->nMissoes));
    /* loop soma todos os agendamentos que levaram a missoes cumpridas */
    for (i = 0; i < s->nMissoes; i++)
        if (s->missoes[i].resolvida)
            somaAgendamentos += s->missoes[i].nAgendamentos;
    printf ("%.2f TENTATIVAS/MISSAO\n", (float)(somaAgendamentos / s->nMissoesResolvidas));
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

    /* inicializa semente randomica */
    srand (0); 

    eventos = cria_lef ();
    inicializaMundo (&simulacao, eventos);

    /* retira primeiro evento da lef verificando 
     * se houve falha na alocacao dessa */
    if (!(eventoTemp = retira_lef (eventos)))
    {
        printf ("FALHA NA ALOCACAO DA LEF\n");
        return -1;
    }
    
    while (simulacao.relogio < T_FIM_DO_MUNDO)
    {
        while (eventoTemp->tempo == simulacao.relogio)
        {
            /* verifica se o tipo do evento eh valido */
            if (eventoTemp->tipo < 1 || eventoTemp->tipo > 9)
            {
                printf ("EVENTO INVALIDO\n");
                return -1;
            }

            (*vetFuncoes[eventoTemp->tipo - 1])(&simulacao, eventoTemp, eventos);
            destroi_evento (eventoTemp);

            if (!(eventoTemp = retira_lef (eventos)))
            {
                printf ("FALHA NA ALOCACAO DA LEF\n");
                return -1;
            }
        }

        ++simulacao.relogio;
    }

    fimSimulacao (&simulacao, eventoTemp, eventos);

    return 0;
}
