#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#define DEBUG 0
#define TIME 0
#define NTHREADS  3

/*Estruturas para as threads*/
struct thread_2_params{
  // Acompanha a posição da primeira incidencia do valor que estamos analisando
  long long int pos_inicial_atual;  
  // Guarda a posição inicial do que tem a maior incidência dentre os analisados
  long long int pos_inicial_final;
  // Guarda o numero de repetições do valor atual
  long long int tam_atual;
  // Guarda o numero de repetições do valor de maior incidência
  long long int tam_final;
  // Guarda o valor atual que está sendo analisado
  int valor_atual;
  // Guarda o valor da maior repetição
  int valor_final;
};

struct thread_3_params{
  // contador com o número de 3 números repetidos contínuos
  long long int n_3repet;
  // Quantidade de repetição de cada iteração
  int n_repet;
  // valor de cada iteração
  int valor_atual;
};

struct thread_4_params{
  // Contador do número de <012345> 
  long long int n_05repet;
  int valor_atual;
};

/* Definição de funções */
struct thread_2_params get_biggest_sequence();
struct thread_3_params get_triples();
struct thread_4_params get_zero_to_five_sequence();

/* Variaveis globais */
struct thread_2_params tp2_conc, tp2_seq;
struct thread_3_params tp3_conc, tp3_seq;
struct thread_4_params tp4_conc, tp4_seq;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;
long long int M, *buffer_init, *buffer, *buffer_seq;
int g_t1=0, g_t2=0, g_t3=0, g_t4=0, N;
char *filename;

/* Funções de inicialização */
struct thread_2_params initT2( struct thread_2_params tparams){
  tparams.pos_inicial_atual = 0;
  tparams.pos_inicial_final = 0;
  tparams.tam_atual = 0;
  tparams.tam_final = 0;
  tparams.valor_atual = -1;
  tparams.valor_final = -1;
  return tparams;
}

struct thread_3_params initT3( struct thread_3_params tparams){
  tparams.n_3repet = 0;
  tparams.n_repet = 0;
  tparams.valor_atual = -1;
  return tparams;
}

struct thread_4_params initT4( struct thread_4_params tparams){
  tparams.valor_atual = -1;
  tparams.n_05repet = 0;
  return tparams;
}

/* Thread 2 - Maior sequência de valores idênticos */
void *thread2 (void *t) {
  if(DEBUG) printf("Entrando na thread 2\n");
  while(g_t2 < M){
    pthread_mutex_lock(&x_mutex);
    while(g_t1 <= g_t2){
      if(DEBUG) printf("Thread 2: vai se bloquear...\n");
      pthread_cond_wait(&x_cond, &x_mutex);
      if(DEBUG) printf("Thread 2: sinal recebido e mutex realocado\n");
    }
    pthread_mutex_unlock(&x_mutex);
    long long int local;
    local = g_t2;

    g_t2=g_t2+N;
    if(g_t2+N> M)
      g_t2=M;
    tp2_conc = get_biggest_sequence(buffer, tp2_conc, local, g_t2);
  }
  if(DEBUG) printf("Saindo da thread 2\n");
  pthread_cond_broadcast(&x_cond);
  pthread_exit(NULL);
}

/* Thread 3 - Quantidade de ocorrências de sequências contínuas de tamanho 3 do mesmo valor */
void *thread3 (void *t) {
  if(DEBUG) printf("Entrando na thread 3\n");
  while(g_t3 < M){
    pthread_mutex_lock(&x_mutex);
    while(g_t1 <= g_t3){
      if (DEBUG) printf("Thread 3: vai se bloquear...\n");
      //
      pthread_cond_wait(&x_cond, &x_mutex);

      if (DEBUG) printf("Thread 3: sinal recebido e mutex realocado\n");
      //
    }
    pthread_mutex_unlock(&x_mutex); 
    
    int local;
    local = g_t3;
    g_t3=g_t3+N;
    if(g_t3+N > M){
      g_t3=M;
    }
    tp3_conc = get_triples(buffer, tp3_conc, local, g_t3);
  }
  if(DEBUG) printf("Saindo da thread 3\n");
  pthread_cond_broadcast(&x_cond);
  pthread_exit(NULL);
}

/* Thread 4 - Quantidade de ocorrências da sequência <012345>*/
void *thread4 (void *t) {
  if(DEBUG) printf("Entrei na thread 4\n");
  int local;
  while(g_t4 < M){
    pthread_mutex_lock(&x_mutex);
    while(g_t1 <= g_t4){
      if(DEBUG) printf("Thread 4: vai se bloquear...\n");
      pthread_cond_wait(&x_cond, &x_mutex);
      if(DEBUG) printf("Thread 4: sinal recebido e mutex realocado\n");
    }
    pthread_mutex_unlock(&x_mutex); 
    local = g_t4;
    g_t4=g_t4+N;

    if(g_t4>M){
      g_t4 = M;
    }
    tp4_conc = get_zero_to_five_sequence(buffer, tp4_conc, local, g_t4);
  }
  if(DEBUG) printf("Saindo da thread 4\n");
  pthread_cond_broadcast(&x_cond);
  pthread_exit(NULL);
}

struct thread_2_params get_biggest_sequence(long long int *nums, struct thread_2_params tparams, 
  long long int j, long long int max){
  while(j<max){
    if(tparams.valor_atual == nums[j]){
      tparams.tam_atual++;
    }
    if(tparams.tam_atual>tparams.tam_final){
      tparams.tam_final=tparams.tam_atual;
      tparams.pos_inicial_final = tparams.pos_inicial_atual;
      tparams.valor_final = tparams.valor_atual;
    }
    if(tparams.valor_atual != nums[j]){
      tparams.tam_atual = 1;
      tparams.pos_inicial_atual = j;
      tparams.valor_atual = nums[j];
    }
    j++;
  }
  return tparams;
}

struct thread_3_params get_triples(long long int *nums, struct thread_3_params tparams, 
  long long int j, long long int max){
  while(j<max){
    if(nums[j] != tparams.valor_atual){
          tparams.valor_atual = nums[j];
          tparams.n_repet = 0;
        }
    tparams.n_repet++;
    if(tparams.n_repet == 3){
        tparams.n_repet = 0;
        tparams.valor_atual = -1;
        tparams.n_3repet++;
    }
    j++;
  }
  return tparams;
}

struct thread_4_params get_zero_to_five_sequence(long long int *nums, struct thread_4_params tparams, 
  long long int j, long long int max
  ){
  long long int valor=-1;
  while(j<max){
    if(nums[j] == tparams.valor_atual+1){
      tparams.valor_atual++;
    }else{
      tparams.valor_atual=-1;
    }
    if(tparams.valor_atual == 5){
      tparams.valor_atual = -1;
      tparams.n_05repet++;
    }
    j++;
  }
  return tparams;
}

/* Função que escreve a partir de um array  */
void write(long long int *nums){
 FILE *fptr = fopen(filename, "wb");
 fwrite(nums,sizeof(long long int),M,fptr);
 fclose(fptr);
}

/* Função de escrever em um buffer, a partir de um arquivo binário */
void read(long long int *nums2){
 FILE *fptr = fopen(filename, "rb");
  for(long long int j=0;j<M;j++){
    fread(&nums2[j],sizeof(long long int),1,fptr);
    if(j%N == 0){
      if(DEBUG) printf("Saindo de read com g_t1 = %d\n", g_t1);
      g_t1 = j;
      pthread_cond_broadcast(&x_cond);
    }
  }
  g_t1=M;
 fclose(fptr);
}
/* 
  Função que executa a leitura do buffer ao mesmo tempo 
  que analisa os dados de maneira sequencial
*/
void read_sequential(long long int *nums2){
 FILE *fptr = fopen(filename, "rb");
 int local;
  for(long long int j=0;j<M;j++){
    fread(&nums2[j],sizeof(long long int),1,fptr);
    if(j%N == 0){
      local = g_t1;
      g_t1 = j;
      tp2_seq = get_biggest_sequence(nums2, tp2_seq, local, j);
      tp3_seq = get_triples(nums2, tp3_seq, local, j);
      tp4_seq = get_zero_to_five_sequence(nums2, tp4_seq, local, j);
    }
  }
  g_t1=M;
 fclose(fptr);
}
/* Funcao principal */
int main(int argc, char *argv[]) {
  if(argc < 3){
      fprintf(stderr, "Digite: %s <tamanho de N> <tamanho de M>\n", argv[0]);
      return 1;
  }
  // inicialização das variáveis
  N = atoi(argv[1]);
  M = atoll(argv[2]);
  filename = "number1.bin";

  // Inicialização de Buffers

  // para popular o binário
  buffer_init = (long long int *) malloc(sizeof( long long int) * M); 
  if(buffer_init==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);
  }

  // para a aplicação princial
  buffer = (long long int *) malloc(sizeof( long long int) * M); 
  if(buffer==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);
  }

  // para a sequencial
  buffer_seq = (long long int *) malloc(sizeof( long long int) * M); 
  if(buffer_seq==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);
  }


  // Inicialização das structs 
  tp2_conc = initT2(tp2_conc);
  tp3_conc = initT3(tp3_conc);
  tp4_conc = initT4(tp4_conc);
  tp2_seq = initT2(tp2_seq);
  tp3_seq = initT3(tp3_seq);
  tp4_seq = initT4(tp4_seq);

  // Inicialização das várias do GET_TIME
  double inicio, fim, tempo_paralel, tempo_sequencial;

  //identificadores das threads
  pthread_t threads[NTHREADS];

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /*
    Populando buffer inicial e o escrevendo no buffer
  */
  long long int i=0;
  while(i < M){
    buffer_init[i] = rand()%6;   
    i++;
  }
  write(buffer_init);

  /* Cria as threads */
  GET_TIME(inicio);
  pthread_create(&threads[0], NULL, thread2, NULL);
  pthread_create(&threads[1], NULL, thread3, NULL);
  pthread_create(&threads[2], NULL, thread4, NULL);
  
  // Executando a leitura
  read(buffer);

  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  GET_TIME(fim);
  tempo_paralel = fim-inicio;

  // Verificação se o buffer atual e o inicial são iguais
  i=0;
  while(i < M){
    if(buffer[i] != buffer_init[i]){
      printf("ERRO - Buffer inicial e final diferem\n");
      printf("%lld\n",i);
      return 1;
    }
    i++;
  }

  //Printando resultados
  printf(
    "Maior sequência de valores idênticos: %lld %lld %d \nQuantidade de triplas: %lld \nQuantidade de ocorrências da sequência <012345>: %lld\n", 
    tp2_conc.pos_inicial_final, tp2_conc.tam_final, 
    tp2_conc.valor_final, tp3_conc.n_3repet, tp4_conc.n_05repet
    );
  
  GET_TIME(inicio);
  read_sequential(buffer_seq);
  GET_TIME(fim);
  tempo_sequencial = fim-inicio;
  if(DEBUG){
    printf(
      "Maior sequência de valores idênticos: %lld %lld %d \nQuantidade de triplas: %lld \nQuantidade de ocorrências da sequência <012345>: %lld\n", 
      tp2_seq.pos_inicial_final, tp2_seq.tam_final, 
      tp2_seq.valor_final, tp3_seq.n_3repet, tp4_seq.n_05repet
      );
  }if(TIME){
    printf(
      "\nSequencial - Paralela - Aceleração\n%lf  -- %lf  -- %lf\n", 
      tempo_sequencial, tempo_paralel,tempo_sequencial/tempo_paralel
      );
    }

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  free(buffer);
  free(buffer_init);
  free(buffer_seq);
  return 0;
}
