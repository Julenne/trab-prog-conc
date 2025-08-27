#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"
#include <math.h>

float *vetor, *vetor2;
double produto_interno;
double start, finish, elapsed;

#define LOG 

// estrutura de dados para passar argumentos para a thread
typedef struct
{
    // tamanho do vetor
    long int n;
    // numero de threads
    short int nthreads;
    // identificador da thread
    short int id;
} t_args;

void *ProdutoInterno(void *args)
{
    t_args *arg = (t_args *)args;
    int ini, fim, fatia;         
    float soma_local = 0, *ret;  

    fatia = arg->n / arg->nthreads;
    ini = arg->id * fatia;         
    fim = ini + fatia;             
    if (arg->id == (arg->nthreads - 1))
        fim = arg->n;

    for (long int i = ini; i < fim; i++)
    {
        soma_local += vetor[i] * vetor2[i];
    }

    ret = (float *)malloc(sizeof(float));
    if (ret != NULL)
        *ret = soma_local;
    else
        printf("--ERRO: malloc() thread\n");
    pthread_exit((void *)ret);
}

// funcao principal do programa
int main(int argc, char *argv[])
{
    GET_TIME(start);
    long int n;                
    short int nthreads;         
    FILE *arq;                  
    size_t ret;                 
    float soma_par_global;      
    float *soma_retorno_threads;

    pthread_t *tid_sistema;

    // recebe os valores de entrada
    if (argc < 3)
    {
        printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]);
        exit(-1);
    }

    // abre o arquivo de entrada
    arq = fopen(argv[1], "rb");
    if (arq == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        exit(-1);
    }
    // le dimensao
    if (fread(&n, sizeof(long int), 1, arq) != 1)
    {
        fprintf(stderr, "Erro ao ler dimensão\n");
        fclose(arq);
        exit(-1);
    }

    vetor = (float *)malloc(sizeof(float) * n);
    vetor2 = (float *)malloc(sizeof(float) * n);

    if (!vetor || !vetor2)
    {
        fprintf(stderr, "Erro de alocação\n");
        fclose(arq);
        free(vetor);
        free(vetor2);
        exit(-1);
    }

    // le vetor
    if (fread(vetor, sizeof(float), n, arq) != n)
    {
        fprintf(stderr, "Erro ao ler vetor\n");
        exit(-1);
    }
    // le vetor2
    if (fread(vetor2, sizeof(float), n, arq) != n)
    {
        fprintf(stderr, "Erro ao ler vetor2\n");
        exit(-1);
    }
    // le produto interno
    if (fread(&produto_interno, sizeof(double), 1, arq) != 1)
    {
        fprintf(stderr, "Erro ao ler produto interno\n");
        exit(-1);
    }

    #ifdef LOG
    // verifica se está tudo certo
    printf("Dimensão: %ld\n", n);

    printf("Vetor:\n");
    for (long int i = 0; i < n; i++)
    {
        printf("%f ", vetor[i]);
    }
    printf("\n");

    printf("Vetor2:\n");
    for (long int i = 0; i < n; i++)
    {
        printf("%f ", vetor2[i]);
    }
    printf("\n");

    printf("Produto interno: %lf\n", produto_interno);
    // fim da verificacao
    #endif

    // le o numero de threads da entrada do usuario
    nthreads = atoi(argv[2]);

    // limita o numero de threads ao tamanho do vetor
    if (nthreads > n)
        nthreads = n;

    // aloca espaco para o vetor de identificadores das threads no sistema
    tid_sistema = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid_sistema == NULL)
    {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }

    // cria as threads
    for (long int i = 0; i < nthreads; i++)
    {
        t_args *args;
        args = (t_args *)malloc(sizeof(t_args));
        if (args == NULL)
        {
            printf("--ERRO: malloc argumentos\n");
            exit(-1);
        }
        args->n = n;
        args->nthreads = nthreads;
        args->id = i;
        if (pthread_create(&tid_sistema[i], NULL, ProdutoInterno, (void *)args))
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    // espera todas as threads terminarem e calcula a soma total das threads
    soma_par_global = 0;
    for (int i = 0; i < nthreads; i++)
    {
        if (pthread_join(tid_sistema[i], (void *)&soma_retorno_threads))
        {
            printf("--ERRO: pthread_join()\n");
            exit(-1);
        }
        soma_par_global += *soma_retorno_threads;
        free(soma_retorno_threads);
    }

    float erro = fabs((produto_interno - soma_par_global) / produto_interno);

    // calcula tempo de execucao
    GET_TIME(finish);
    elapsed = finish - start;

    printf("\n");
    printf("soma_concorrente             = %.26f\n", soma_par_global);
    printf("Erro relativo = %.26f\n", erro);
    printf("Tempo de execucao: %lf\n", elapsed);

    free(vetor);
    free(vetor2);
    free(tid_sistema);
    fclose(arq);

    return 0;
}
