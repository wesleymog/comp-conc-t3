#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define NTHREADS  3

/* Variaveis globais */
struct thread_2_params{
  long long int pos_inicial_atual;
  long long int pos_inicial_final;
  long long int tam_atual;
  long long int tam_final;
  int valor_atual;
  int valor_final;
};
struct thread_2_params tparams2;
struct thread_3_params{
  long long int n_3repet;
  int n_repet;
  int valor_atual;
};
struct thread_3_params tparams3;
struct thread_4_params{
  long long int n_05repet;
  int valor_atual;
};
struct thread_4_params tparams4;

int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;
long long int M, *buffer_init, *buffer, *buffer_check;
int g_t1=0, g_t2=0, g_t3=0, g_t4=0, N;


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
    local = g_t2;
    long long int final = g_t2+N;
    if(final> M)
      final=M;
    g_t2=final;
    pthread_mutex_unlock(&x_mutex);
    for(local; local<final; local++){
        int valor = buffer[local];
        if(tparams2.valor_atual == valor){
          tparams2.tam_atual++;
        }
        if(tparams2.tam_atual>tparams2.tam_final){
          tparams2.tam_final=tparams2.tam_atual;
          tparams2.pos_inicial_final = tparams2.pos_inicial_atual;
          tparams2.valor_final = tparams2.valor_atual;
        }
        if(tparams2.valor_atual != valor){
          tparams2.tam_atual = 1;
          tparams2.pos_inicial_atual = local;
          tparams2.valor_atual = valor;
        }
    }
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
    local = g_t3;

    long long int final= g_t3+N;
    if(g_t3+N > M){
      final=M;
    }
    g_t3=final;

    pthread_mutex_unlock(&x_mutex); 
    for(local; local<final; local++){
        int valor = buffer[local];
        if(valor != tparams3.valor_atual){
              tparams3.valor_atual = valor;
              tparams3.n_repet = 0;
            }
        tparams3.n_repet++;
        if(tparams3.n_repet == 3){
            tparams3.n_repet = 0;
            tparams3.valor_atual = -1;
            tparams3.n_3repet++;            
        }
    }
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
    for(local; local<final; local++){
        int valor = buffer[local];
        if(valor == tparams4.valor_atual+1){
              tparams4.valor_atual++;
        }else{
          tparams4.valor_atual=-1;
        }
        if(tparams4.valor_atual == 5){
          tparams4.valor_atual = -1;
          tparams4.n_05repet++;
        }
    }
  }
  //printf("até mais 4!\n");
  pthread_cond_broadcast(&x_cond);
  pthread_exit(NULL);
}

void write(long long int *nums){
 FILE *fptr = fopen("number.bin", "wb");
 fwrite(nums,sizeof(long long int),M,fptr);
 fclose(fptr);
}

void read(long long int *nums2){
 FILE *fptr = fopen("number.bin", "rb");
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

void verifica_2(long long int *nums){
  long long int j=0;
  struct thread_2_params tp2;
  tp2 = initT2(tp2);
  while(j<M){
    if(tp2.valor_atual == nums[j]){
      tp2.tam_atual++;
    }
    if(tp2.tam_atual>tp2.tam_final){
      tp2.tam_final=tp2.tam_atual;
      tp2.pos_inicial_final = tp2.pos_inicial_atual;
      tp2.valor_final = tp2.valor_atual;
    }
    if(tp2.valor_atual != nums[j]){
      tp2.tam_atual = 1;
      tp2.pos_inicial_atual = j;
      tp2.valor_atual = nums[j];
    }
    j++;
  }
  long long int k=tp2.pos_inicial_final;
}

long long int verifica_3(long long int *nums){
  long long int j=0;
  long long int verifica3=0;

  long long int valor=-1;
  long long int repeat=0;
  while(j<M){
    if(buffer[j] != valor){
          valor = nums[j];
          repeat = 0;
        }
    repeat++;
    if(repeat == 3){
        repeat = 0;
        valor = -1;
        verifica3++;
    }
    j++;
  }
  return verifica3;
}

long long int verifica_0_5(long long int *nums){
  long long int j=0;
  long long int verifica0_5=0;
  long long int valor=-1;
  while(j<M){
    if(nums[j] == valor+1){
          valor++;
    }else{
      valor=-1;
    }
    if(valor == 5){
      valor = -1;
      verifica0_5++;
    }
    j++;
  }
  return verifica0_5;
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
  buffer_init = (long long int *) malloc(sizeof( long long int) * M);
  buffer = (long long int *) malloc(sizeof( long long int) * M);
  buffer_check = (long long int *) malloc(sizeof( long long int) * M);
  if(buffer_init==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);}
  if(buffer==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);}
  if(buffer_check==NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);}
  long long int i=0;
  while(i < M){
    buffer_init[i] = rand()%6;   
    i++;
  }
  write(buffer_init);
  
  tparams2 = initT2(tparams2);
  tparams3 = initT3(tparams3);
  tparams4 = initT4(tparams4);
  //identificadores das threads
  pthread_t threads[NTHREADS];

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
    "Maior sequência de valores idênticos: %lld %lld %d \nQuantidade de triplas: %lld \nQuantidade de ocorrências da sequência <012345>: %lld", 
    tparams2.pos_inicial_final, tparams2.tam_final, 
    tparams2.valor_final, tparams3.n_3repet, tparams4.n_05repet
    );
  
  GET_TIME(inicio);
  read(buffer_check);
  verifica_2(buffer_check);
  verifica_0_5(buffer_check);
  verifica_3(buffer_check);

  GET_TIME(fim);
  tempo_continua = fim-inicio;
  printf(
    "\n%lf %lf %lf\n", 
    tempo_continua, tempo_paralel,tempo_continua/tempo_paralel
    );
  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  return 0;
}
