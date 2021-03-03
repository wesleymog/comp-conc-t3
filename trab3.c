#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#define DEBUG 1
#define NTHREADS  3

/*Estruturas*/
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
  long long int n_3repet;
  int n_repet;
  int valor_atual;
};

struct thread_4_params{
  long long int n_05repet;
  int valor_atual;
};

/* Definição de funções */
struct thread_2_params verifica_2();
struct thread_3_params verifica_3();
struct thread_4_params verifica_0_5();

/* Variaveis globais */
struct thread_2_params tparams2, tp2;
struct thread_3_params tparams3, tp3;
struct thread_4_params tparams4, tp4;

int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;
long long int M, *buffer_init, *buffer, *buffer_check;
int g_t1=0, g_t2=0, g_t3=0, g_t4=0, N;
char *filename;


/* Thread 2 */
void *thread2 (void *t) {
  long long int local;
  while(g_t2 < M){
    pthread_mutex_lock(&x_mutex);
    while(g_t1 <= g_t2){
      //printf("Thread 2: vai se bloquear...\n");
      pthread_cond_wait(&x_cond, &x_mutex);
      //printf("Thread 2: sinal recebido e mutex realocado, x = %d\n", x);
    }
    pthread_mutex_unlock(&x_mutex);
    local = g_t2;
    long long int final = g_t2+N;
    if(final> M)
      final=M;
    g_t2=final;
    tparams2 = verifica_2(buffer, tparams2, local, final);
  }
  pthread_cond_broadcast(&x_cond);
  pthread_exit(NULL);
}

/* Thread 3 */
void *thread3 (void *t) {
  //printf("Thread 3: Comecei\n");
  int local;
  while(g_t3 < M){
    pthread_mutex_lock(&x_mutex);
    while(g_t1 <= g_t3){
      //printf("Thread 3: vai se bloquear...\n");
      pthread_cond_wait(&x_cond, &x_mutex);
      //printf("Thread 3: sinal recebido e mutex realocado, x = %d\n", x);
    }
    pthread_mutex_unlock(&x_mutex); 
    
    local = g_t3;
    long long int final= g_t3+N;
    if(g_t3+N > M){
      final=M;
    }
    g_t3=final;

    tparams3 = verifica_3(buffer, tparams3, local, final);
  }
  //printf("até mais thread 3!\n");
  pthread_cond_broadcast(&x_cond);
  pthread_exit(NULL);
}

/* Thread 4 */
void *thread4 (void *t) {
  //printf("Thread 4: Comecei\n");
  int local;
  while(g_t4 < M){
    pthread_mutex_lock(&x_mutex);
    while(g_t1 <= g_t4){
      //printf("Thread 4: vai se bloquear...\n");
      pthread_cond_wait(&x_cond, &x_mutex);
      //printf("Thread 4: sinal recebido e mutex realocado, x = %d\n", x);
    }
    local = g_t4;
    long long int final=g_t4+N;
    if(final>M){
      final = M;
    }
    g_t4=final;
    pthread_mutex_unlock(&x_mutex); 
    tparams4 = verifica_0_5(buffer, tparams4, local, final);
  }
  //printf("até mais 4!\n");
  pthread_cond_broadcast(&x_cond);
  pthread_exit(NULL);
}

void write(long long int *nums){
 FILE *fptr = fopen(filename, "wb");
 fwrite(nums,sizeof(long long int),M,fptr);
 fclose(fptr);
}

void read(long long int *nums2){
 FILE *fptr = fopen(filename, "rb");
  for(long long int j=0;j<M;j++){
    fread(&nums2[j],sizeof(long long int),1,fptr);
    if(j%N == 0){
      g_t1 = j;
      pthread_cond_broadcast(&x_cond);
    }
  }
  g_t1=M;
 fclose(fptr);
}

void read_recurrence(long long int *nums2){
 FILE *fptr = fopen(filename, "rb");
 int local;
  for(long long int j=0;j<M;j++){
    fread(&nums2[j],sizeof(long long int),1,fptr);
    if(j%N == 0){
      local = g_t1;
      g_t1 = j;
      tp2 = verifica_2(nums2, tp2, local, j);
      tp3 = verifica_3(nums2, tp3, local, j);
      tp4 = verifica_0_5(nums2, tp4, local, j);
    }
  }
  g_t1=M;
 fclose(fptr);
}

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

struct thread_2_params verifica_2(long long int *nums, struct thread_2_params tparams, 
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

struct thread_3_params verifica_3(long long int *nums, struct thread_3_params tparams, 
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

struct thread_4_params verifica_0_5(long long int *nums, struct thread_4_params tparams, 
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
  buffer_init = (long long int *) malloc(sizeof( long long int) * M);
  buffer = (long long int *) malloc(sizeof( long long int) * M);
  buffer_check = (long long int *) malloc(sizeof( long long int) * M);
  if(buffer_init==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);}
  if(buffer==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);}
  if(buffer_check==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);}

  tparams2 = initT2(tparams2);
  tparams3 = initT3(tparams3);
  tparams4 = initT4(tparams4);
  tp2 = initT2(tp2);
  tp3 = initT3(tp3);
  tp4 = initT4(tp4);

  //identificadores das threads
  pthread_t threads[NTHREADS];
  long long int i=0;
  while(i < M){
    buffer_init[i] = rand()%6;   
    i++;
  }
  write(buffer_init);
  

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /* Cria as threads */
  double inicio, fim, tempo_paralel, tempo_continua;
  GET_TIME(inicio);
  pthread_create(&threads[0], NULL, thread2, NULL);
  pthread_create(&threads[1], NULL, thread3, NULL);
  pthread_create(&threads[2], NULL, thread4, NULL);
  //printf("Lendo do arquivo\n");
  
  read(buffer);
  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  GET_TIME(fim);
  tempo_paralel = fim-inicio;
  i=0;
  while(i < M){
    if(buffer[i] != buffer_init[i]){
      printf("ERRO - Diferem\n");
      printf("%lld\n",i);
      break;
    }
    i++;
  }
  //Printando resultados
  printf(
    "Maior sequência de valores idênticos: %lld %lld %d \nQuantidade de triplas: %lld \nQuantidade de ocorrências da sequência <012345>: %lld\n", 
    tparams2.pos_inicial_final, tparams2.tam_final, 
    tparams2.valor_final, tparams3.n_3repet, tparams4.n_05repet
    );
  
  GET_TIME(inicio);
  read_recurrence(buffer_check);
  GET_TIME(fim);
  tempo_continua = fim-inicio;
  /*
  printf(
    "Maior sequência de valores idênticos: %lld %lld %d \nQuantidade de triplas: %lld \nQuantidade de ocorrências da sequência <012345>: %lld\n", 
    tp2.pos_inicial_final, tp2.tam_final, 
    tp2.valor_final, tp3.n_3repet, tp4.n_05repet
    );
    */
  printf(
    "\nSequencial - Paralela - Aceleração\n%lf  -- %lf  -- %lf\n", 
    tempo_continua, tempo_paralel,tempo_continua/tempo_paralel
    );
  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  return 0;
}
