#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS N_HABILIDADES * 5
#define N_BASES N_HEROIS / 6
#define N_MISSOES T_FIM_DO_MUNDO / 100

struct local
{
    int x;
    int y;
}

struct heroi
{
    int id;
    int experiencia;
    int paciencia;
    int velocidade;
    struct conjunto *habilidades;
}

struct base
{
    int id;
    struct local;
    int lotacaoMax;
    struct conjunto *presentes;
    struct fila *filaEspera;
}

struct missao
{
    int id;
    struct local;
    struct conjunto *habilidades;
}

struct mundo
{

}

int aleat (int min, int max)
{

}

struct heroi criaHeroi ()
{

}

struct base criaBase ()
{

}

struct missao criaMissao ()
{

}

int main (){
    /* declaracoes de variaveis aqui */

    srand (0); /* use zero, nao faca com time (0) */

    /* coloque seu codigo aqui */

    return 0;
}
