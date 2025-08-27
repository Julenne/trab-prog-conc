
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor
#define LOG 

int main(int argc, char*argv[]) {
   float *vetor;
   float *vetor2;
   long int n;
   float elem;
   double produto_interno=0;
   int fator=1;
   FILE * descritorArquivo;
   size_t ret;

   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: %s <dimensao> <nome arquivo saida>\n", argv[0]);
      return 1;
   }
   n = atoi(argv[1]);

   //aloca memoria para o vetor
   vetor = (float*) malloc(sizeof(float) * n);
   if(!vetor) {
      fprintf(stderr, "Erro de alocao da memoria do vetor\n");
      return 2;
   }

   //aloca memoria para o vetor2
   vetor2 = (float*) malloc(sizeof(float) * n);
   if(!vetor2) {
      fprintf(stderr, "Erro de alocao da memoria do vetor2\n");
      return 2;
   }

   //preenche o vetor com valores float aleatorios
   srand(time(NULL));
   for(long int i=0; i<n; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        vetor[i] = elem;
        fator*=-1;
   }

   fator=1;
   //preenche o vetor2 com valores float aleatorios
   for(long int i=0; i<n; i++) {
        elem = (rand() % MAX)/3.0 * fator;
        vetor2[i] = elem;
        fator*=-1;
   }

   //calcula o produto interno dos dois vetores
   for(long int i=0; i<n; i++) {
        produto_interno += vetor[i] * vetor2[i];
   }

   //imprimir na saida padrao o vetor gerado
   #ifdef LOG
   fprintf(stdout, "%ld\n", n);
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor[i]);
   }
   fprintf(stdout, "\n");

   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor2[i]);
   }
   fprintf(stdout, "\n");

   fprintf(stdout, "%f\n", produto_interno);
   #endif

   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   //escreve a dimensao
   if(fwrite(&n, sizeof(long int), 1, descritorArquivo) != 1) {
      fprintf(stderr, "Erro de escrita 1\n");
      return 4;
   }

   //escreve os elementos do vetor
   if(fwrite(vetor, sizeof(float), n, descritorArquivo) != n) {
      fprintf(stderr, "Erro de escrita 3\n");
      return 4;
   }

   //escreve os elementos do vetor2
   if(fwrite(vetor2, sizeof(float), n, descritorArquivo) != n) {
      fprintf(stderr, "Erro de escrita 3\n");
      return 4;
   }
   //escreve o produto interno
   if(fwrite(&produto_interno, sizeof(double), 1, descritorArquivo) != 1) {
      fprintf(stderr, "Erro de escrita 4\n");
      return 4;
   }

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetor);
   free(vetor2);
   return 0;
} 
