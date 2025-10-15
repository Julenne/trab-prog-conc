
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

int N, M;
int nthreads = 50; // nthreads consumidoras
int *buffer;
int prod_i = 0;
int consu_i = 0;
int total_primos = 0;
int *primos_por_thread;

sem_t sem_buffer_vazio;
sem_t sem_buffer_cheio;
sem_t mutex;

void *produtora()
{
    int valor = 1;

    while (valor <= N)
    {
        sem_wait(&sem_buffer_vazio);
        pthread_mutex_lock(&mutex);

        buffer[prod_i % M] = valor;
        prod_i++;
        valor++;

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_buffer_cheio); 
    }

    pthread_exit(NULL);
}
int ehPrimo(long long int n)
{
    int i;
    if (n <= 1)
        return 0;
    if (n == 2)
        return 1;
    if (n % 2 == 0)
        return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0)
            return 0;
    return 1;
}

void *consumidora(void *arg)
{
    int id = *(int *)arg;
    int primos = 0;

    while (1)
    {
        sem_wait(&sem_buffer_cheio); 
        pthread_mutex_lock(&mutex);

        if (consu_i >= N)
        {
            pthread_mutex_unlock(&mutex);
            sem_post(&sem_buffer_cheio);
            break; 
        }

        int numero = buffer[consu_i % M];
        consu_i++;

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_buffer_vazio); 

        if (ehPrimo(numero))
            primos++;
    }
    primos_por_thread[id] = primos;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t *thread_prod;
    pthread_t *thread_consu;

    printf("Digite N e M");
    scanf("%d %d", &N, &M);

    if (M > N)
    {
        printf("M deve ser menor que N\n");
        return 1;
    }

    buffer = malloc(sizeof(int) * M);
    if (buffer == NULL)
    {
        puts("ERRO--malloc");
        return 2;
    }

    thread_prod = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
    if (thread_prod == NULL)
    {
        puts("ERRO--malloc");
        return 3;
    }

    thread_consu = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
    if (thread_consu == NULL)
    {
        puts("ERRO--malloc");
        return 4;
    }

    primos_por_thread = (int *)malloc(sizeof(int) * nthreads);
    if (primos_por_thread == NULL)
    {
        puts("ERRO--malloc");
        return 5;
    }

    for (int i = 0; i < nthreads; i++)
        primos_por_thread[i] = 0;

    // semaforos e mutex
    sem_init(&sem_buffer_cheio, 0, 0);
    sem_init(&sem_buffer_vazio, 0, 1);
    sem_init(&mutex, 0, 1);

    // apenas uma thread produtora
    pthread_create(&thread_prod, NULL, produtora, NULL);

    //varias threads consumidoras
    int *ids = malloc(sizeof(int) * nthreads);
    for (int t = 0; t < nthreads; t++)
    {
        ids[t] = t;
        pthread_create(&thread_consu[t], NULL, consumidora, &ids[t]);
    }

    pthread_join(thread_prod, NULL);
    for (int t = 0; t < nthreads; t++)
    {
        if (pthread_join(thread_consu[t], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    int max_primos = 0, vencedora = 0;
    total_primos = 0;
    for (int i = 0; i < nthreads; i++)
    {
        total_primos += primos_por_thread[i];
        if (primos_por_thread[i] > max_primos)
        {
            max_primos = primos_por_thread[i];
            vencedora = i;
        }
    }

    printf("Total de numeros primos: %d\n", total_primos);
    printf("Thread vencedora: %d (primos=%d)\n", vencedora, max_primos);

    sem_destroy(&sem_buffer_vazio);
    sem_destroy(&sem_buffer_cheio);
    sem_destroy(&mutex);
    free(ids);
    free(buffer);
    free(primos_por_thread);

    return 0;
}
