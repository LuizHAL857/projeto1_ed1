#include "trata_qry.h"
#include "fila.h"
#include "pilha.h"
#include "trata_geo.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "cor_complementar.h"
#include "formas.h"

typedef enum { ARRAY_DISPARADORES_FREE, ARRAY_CARREGADORES_FREE, FORMA_POSICAO_FREE, PILHA_FREE} TipoFree;

typedef struct {

  TipoForma tipo;
  void *data;

} Forma_t;


typedef struct {

  int id;
  Pilha *formas;

} Carregador_t;

typedef struct {

  int id;
  double x;
  double y;
  Forma_t *posicao_disparo;
  Carregador_t *carregadorEsquerdo;
  int idCarregadorEsquerdo;
  Carregador_t *carregadorDireito;
  int idCarregadorDireito;

} Disparador_t;

typedef struct {

  Pilha arena; // PosicaoFormaArena_t
  Pilha pilha_para_free;

} Qry_t;

typedef struct {
  
    Forma_t *forma;

  double x;
  double y;
  bool isAnnotated;
  double dispX;
  double dispY;

} PosicaoFormaArena_t;

typedef struct {


    void *pointer;
  
  
    TipoFree tipo;
  
  
  } ObjetoFree;

typedef struct {

    double minX;
    double minY;
    double maxX;
    double maxY;

} BoundingBox;
  

  

//Funções privadas
static void executa_pd(Disparador_t **disparadores, int *contagem_disparadores,  Pilha pilha_para_free);


static void executa_lc(Carregador_t **carregadores, int *contagem_carregadores, Chao chao, Pilha pilha_para_free, FILE *txt);



static void executa_atch(Carregador_t **carregadores, int *contagem_carregadores,  Disparador_t **disparadores, int *contagem_disparadores,  Pilha pilha_para_free);


static void executa_shft(Disparador_t **disparadores, int *contagem_disparadores, Carregador_t *carregadores, int* contagem_carregadores, FILE *txt);


static void executa_dsp(Disparador_t **disparadores, int *contagem_disparadores,  Pilha arena, Pilha pilha_para_free, FILE *txt);


int executa_rjd(Disparador_t **disparadores, int *contagem_disparadores,
    Pilha pilha_para_free, Pilha arena,
    Carregador_t *carregadores, int* contagem_carregadores, FILE *txt);


static void executa_calc(Pilha arena, Chao chao, FILE *txt, int qtd_comandos,int qtd_disparos);


static int encontra_disparador_por_id(Disparador_t **disparadores, int contagem_disparadores, int id);


static void realiza_shift(Disparador_t **disparadores, int contagem_disparadores,  int DisparadorID, 
       char *direcao,  int qtd, Carregador_t *carregadores, int contagem_carregadores);


static void realiza_disparo(Disparador_t **disparadores, int contagem_disparadores, int DisparadorID, double dx, double dy,  char *annotate, 
    Pilha arena, Pilha pilha_para_free);

// Funções auxiliares para comando calc
static double area_forma(TipoForma tipo, void *dados_forma);


static BoundingBox cria_boundingbox_forma( PosicaoFormaArena_t *P);

static bool sobreposicao_boundingbox(BoundingBox a, BoundingBox b);

static bool sobreposicao_formas( PosicaoFormaArena_t *a,  PosicaoFormaArena_t *b);


static Forma_t *encapsula_forma(TipoForma tipo, void *data);



// Funções para auxiliar clonagem
static Forma_t *clona_posicao(Forma_t *forma_original, double x, double y, Chao chao);

static Forma_t *clone_posicao_corB(Forma_t *forma_original,  char *novaCorB, double x, double y, Chao chao);
                                                    
static Forma_t *clona_posicao_coresInvertidas(Forma_t *forma_original, double x, double y, Chao chao);
                                                      



// Função para escrever o arquivo svg
static void cria_svg_com_resultado(DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo,  Chao chao, Pilha arena,  char *caminho_output);

Qry executa_qry(DadosDoArquivo dados_qry, DadosDoArquivo dados_geo, Chao chao,  char *output_path) {

    Qry_t *qry = malloc(sizeof(Qry_t));
    if (qry == NULL) {
    printf("Error: Failed to allocate memory for Qry\n");
    exit(1);
    }
    qry->arena = criaPilha();
    qry->pilha_para_free = criaPilha();

   

    Disparador_t *disparadores = NULL;
    int disparadorCount = 0;
    Carregador_t *carregadores = NULL;
    int carregadorCount = 0;

    int qtd_comandos = 0;
    int qtd_disparos = 0;

    size_t geo_len = strlen(obter_nome_arquivo(dados_geo));
    size_t qry_len = strlen(obter_nome_arquivo(dados_qry));
    char *geo_base = malloc(geo_len + 1);
    char *qry_base = malloc(qry_len + 1);
    if (geo_base == NULL || qry_base == NULL) {
    printf("Erro de alocação pro nome do arquivo\n");
    free(geo_base);
    free(qry_base);
    return NULL;
    }
    strcpy(geo_base, obter_nome_arquivo(dados_geo));
    strcpy(qry_base, obter_nome_arquivo(dados_qry));
    strtok(geo_base, ".");
    strtok(qry_base, ".");
    size_t path_len = strlen(output_path);
    
    size_t nome_processado_len = strlen(geo_base) + 1 + strlen(qry_base);
    size_t total_len = path_len + 1 + nome_processado_len + 4 + 1; // +4 for ".txt"
    char *output_txt_path = malloc(total_len);
    if (output_txt_path == NULL) {
    printf("Erro de alocação\n");
    free(geo_base);
    free(qry_base);
    return NULL;
    }
    int res = snprintf(output_txt_path, total_len, "%s/%s-%s.txt", output_path,
    geo_base, qry_base);
    if (res < 0 || (size_t)res >= total_len) {
    printf("Erro na construção de caminho\n");
    free(output_txt_path);
    free(geo_base);
    free(qry_base);
    return NULL;
    }
    FILE *txtFile = fopen(output_txt_path, "w");
    free(geo_base);
    free(qry_base);
    free(output_txt_path);

    while (!filaVazia(obter_fila_linhas(dados_qry))) {
    char *line = (char *)dequeueFila(obter_fila_linhas(dados_qry));
    char *command = strtok(line, " \t\r\n");

    if (command == NULL || *command == '\0') {
    continue;
    }

    if (strcmp(command, "pd") == 0) {
    executa_pd(&disparadores, &disparadorCount, qry->pilha_para_free);
    qtd_comandos++;

    } else if (strcmp(command, "lc") == 0) {
    executa_lc(&carregadores, &carregadorCount, chao, qry->pilha_para_free, txtFile);
    qtd_comandos++;

    } else if (strcmp(command, "atch") == 0) {
    executa_atch(&carregadores, &carregadorCount, &disparadores, &disparadorCount, qry->pilha_para_free);
    qtd_comandos++;

    } else if (strcmp(command, "shft") == 0) {
    executa_shft(&disparadores, &disparadorCount, carregadores, &carregadorCount, txtFile);
    qtd_comandos++;

    } else if (strcmp(command, "dsp") == 0) {
    executa_dsp(&disparadores, &disparadorCount, qry->arena, qry->pilha_para_free, txtFile);
    qtd_comandos++;
    qtd_disparos++;


    } else if (strcmp(command, "rjd") == 0) {
    qtd_disparos += executa_rjd(&disparadores, &disparadorCount, qry->pilha_para_free, qry->arena, carregadores, &carregadorCount, txtFile);
    qtd_comandos++;

    } else if (strcmp(command, "calc") == 0) {
      qtd_comandos++;
    executa_calc(qry->arena, chao, txtFile, qtd_comandos, qtd_disparos);

    } else
    printf("Unknown command: %s\n", command);
    }


    cria_svg_com_resultado(dados_qry, dados_geo, chao, qry->arena, output_path);

    fclose(txtFile);
    return (Qry)qry;
}



void desaloca_qry(Qry qry) {
    Qry_t *qry_t = (Qry_t *)qry;
    while (!pilhaVazia(qry_t->pilha_para_free)) {
      ObjetoFree *o = (ObjetoFree *)popPilha(qry_t->pilha_para_free);
  
      if (o != NULL && o->pointer != NULL) {

        switch (o->tipo) {
        case ARRAY_DISPARADORES_FREE:


        case ARRAY_CARREGADORES_FREE:

        
        case FORMA_POSICAO_FREE:

          free(o->pointer);

          break;
        case PILHA_FREE: {

          Pilha *p = (Pilha *)o->pointer;

          if (p != NULL && *p != NULL) {
            
            desalocaPilha(*p);

          }
       

          break;
        }
        }
        free(o);
      }
    }
    desalocaPilha(qry_t->arena);
    desalocaPilha(qry_t->pilha_para_free);
    free(qry_t);

  }


  /*
==========================
FUNÇÕES PRIVADAS
==========================
*/

static void executa_pd(Disparador_t **disparadores, int *contagem_disparadores, Pilha pilha_para_liberar) {
    char *id = strtok(NULL, " ");
    char *X = strtok(NULL, " ");
    char *Y = strtok(NULL, " ");

    *contagem_disparadores += 1;

    *disparadores = realloc(*disparadores, *contagem_disparadores * sizeof(Disparador_t));
    if (*disparadores == NULL) {

        printf("Erro na alocação dos disparadores\n");
        exit(1);

    }

    
    bool disparador_ja_marcado = false;
    int contagem_tamanho_pilha = tamanhoPilha(pilha_para_liberar);

    for (int i = 0; i < contagem_tamanho_pilha; i++) {
    ObjetoFree *objeto = (ObjetoFree *)pilhaElemento(pilha_para_liberar, i);

        if (objeto != NULL && objeto->tipo == ARRAY_DISPARADORES_FREE) {
        
            objeto->pointer = *disparadores;
            disparador_ja_marcado = true;
            break;
        }
    }

    if (!disparador_ja_marcado) {

        ObjetoFree *disparador_objeto = malloc(sizeof(ObjetoFree));

        if (disparador_objeto != NULL) 
        {
            disparador_objeto->pointer = *disparadores;
            disparador_objeto->tipo = ARRAY_DISPARADORES_FREE;
            pushPilha(pilha_para_liberar, disparador_objeto);

        }
    }
    (*disparadores)[*contagem_disparadores - 1] = (Disparador_t){.id = atoi(id),
                        .x = atof(X),
                        .y = atof(Y),
                        .posicao_disparo = NULL,
                        .carregadorDireito = NULL,
                        .carregadorEsquerdo = NULL,
                        .idCarregadorDireito = -1,
                        .idCarregadorEsquerdo = -1};


}

static void executa_lc(Carregador_t **loaders, int *loadersCount, Chao ground, Pilha pilha_para_free, FILE *txtFile) {
    char *identifier = strtok(NULL, " ");
    char *primeirasXFormas = strtok(NULL, " ");

    int loaderId = atoi(identifier);
    int nova_quantia_formas= atoi(primeirasXFormas);

    fprintf(txtFile, "--lc\n");
    fprintf(txtFile, "\t·Carregador ID: %d\n", loaderId);
    fprintf(txtFile, "\t·Nova quantia de formas: %d\n", nova_quantia_formas);

    
    int existingLoaderIndex = -1;
    for (int i = 0; i < *loadersCount; i++) {
    if ((*loaders)[i].id == loaderId) {
    existingLoaderIndex = i;
    break;
    }
    }

    if (existingLoaderIndex == -1) {
    
    *loadersCount += 1;
    *loaders = realloc(*loaders, *loadersCount * sizeof(Carregador_t));
    if (*loaders == NULL) {
    printf("Erro na alocação dos carregadores\n");
    exit(1);
    }

    
    bool carregadores_ja_marcados = false;
    int pilha_size_count = tamanhoPilha(pilha_para_free);
    for (int i = 0; i < pilha_size_count; i++) {
    ObjetoFree *existing_item = (ObjetoFree *)pilhaElemento(pilha_para_free, i);
    if (existing_item != NULL && existing_item->tipo == ARRAY_CARREGADORES_FREE) {
    
    existing_item->pointer = *loaders;
    carregadores_ja_marcados = true;
    break;
    }
    }

    if (!carregadores_ja_marcados) {
    ObjetoFree *loaders_item = malloc(sizeof(ObjetoFree));
    if (loaders_item != NULL) {
    loaders_item->pointer = *loaders;
    loaders_item->tipo = ARRAY_CARREGADORES_FREE;
    pushPilha(pilha_para_free, loaders_item);
    }
    }
    (*loaders)[*loadersCount - 1] = (Carregador_t){.id = loaderId, .formas = NULL};
    existingLoaderIndex = *loadersCount - 1;
    }

   
    if ((*loaders)[existingLoaderIndex].formas == NULL) {
    (*loaders)[existingLoaderIndex].formas = malloc(sizeof(Pilha));
    if ((*loaders)[existingLoaderIndex].formas == NULL) {
    printf("Erro de alocação nos carregadores\n");
    exit(1);
    }

    
    ObjetoFree *stack_item = malloc(sizeof(ObjetoFree));
    if (stack_item != NULL) {
    stack_item->pointer = (*loaders)[existingLoaderIndex].formas;
    stack_item->tipo = PILHA_FREE;
    pushPilha(pilha_para_free, stack_item);
    }
    *(*loaders)[existingLoaderIndex].formas = criaPilha();
    if (*(*loaders)[existingLoaderIndex].formas == NULL) {
    printf("Erro na alocação de carregadores\n");
    exit(1);
    }
    }

    
    for (int i = 0; i < nova_quantia_formas; i++) {
    Forma_t *forma = dequeueFila(get_fila_chao(ground));
    if (forma != NULL) {
    if (!pushPilha(*(*loaders)[existingLoaderIndex].formas, forma)) {
    printf("Error na alocação de carregadores\n");
    exit(1);
    }
    }
    }
}


static void executa_atch(Carregador_t **carregadores, int *contagem_carregadores,
      Disparador_t **disparadores, int *contagem_disparadores,
      Pilha pilha_para_liberar) {


    char *disparadorID = strtok(NULL, " ");
    char *carregadorEsquerdoID = strtok(NULL, " ");
    char *carregadorDireitoID = strtok(NULL, " ");

    int disparadorIDInt = atoi(disparadorID);
    int carregadorEsquerdoIDInt = atoi(carregadorEsquerdoID);
    int carregadorDireitoIDInt = atoi(carregadorDireitoID);

    int  indice_disparador = encontra_disparador_por_id(disparadores, *contagem_disparadores, disparadorIDInt);

    if (indice_disparador != -1) {
    
        Carregador_t *carregadorEsquerdoPtr = NULL;
        Carregador_t *carregadorDireitoPtr = NULL;

        for (int j = 0; j < *contagem_carregadores; j++) {
            if ((*carregadores)[j].id == carregadorEsquerdoIDInt) {
            carregadorEsquerdoPtr = &(*carregadores)[j];
            }
            if ((*carregadores)[j].id == carregadorDireitoIDInt) {
            carregadorDireitoPtr = &(*carregadores)[j];
            }
        }

        
        if (carregadorEsquerdoPtr == NULL) {
            *contagem_carregadores += 1;
            *carregadores = realloc(*carregadores, *contagem_carregadores * sizeof(Carregador_t));
            
            if (*carregadores == NULL) {
            printf("Erro de alocação dos carregadores\n");
            exit(1);
            }
            
            bool carregador_ja_marcado = false;
            int pilha_tamanho_contagem = tamanhoPilha(pilha_para_liberar);

            for (int i = 0; i < pilha_tamanho_contagem; i++) {
                ObjetoFree *objeto = (ObjetoFree *)pilhaElemento(pilha_para_liberar, i);
                if (objeto != NULL && objeto->tipo == ARRAY_CARREGADORES_FREE) {
                
                    objeto->pointer = *carregadores;
                    carregador_ja_marcado = true;
                    break;
                }
        }

        if (!carregador_ja_marcado) {
            ObjetoFree *carregador_objeto = malloc(sizeof(ObjetoFree));

            if (carregador_objeto != NULL) {

                carregador_objeto->pointer = *carregadores;
                carregador_objeto->tipo = ARRAY_CARREGADORES_FREE;
                pushPilha(pilha_para_liberar, carregador_objeto);
            }
        }

        (*carregadores)[*contagem_carregadores - 1] = (Carregador_t){.id = carregadorEsquerdoIDInt, .formas = NULL};
        
        (*carregadores)[*contagem_carregadores - 1].formas = malloc(sizeof(Pilha));

        if ((*carregadores)[*contagem_carregadores - 1].formas == NULL) {

            printf("Erro de alocação para pilhas de carregadores\n");

            exit(1);
        }

        ObjetoFree *pilha_item = malloc(sizeof(ObjetoFree));

        if (pilha_item != NULL) 
        {
            pilha_item->pointer = (*carregadores)[*contagem_carregadores - 1].formas;
            pilha_item->tipo = PILHA_FREE;
            pushPilha(pilha_para_liberar, pilha_item);

        }
        *(*carregadores)[*contagem_carregadores - 1].formas = criaPilha();
        if (*(*carregadores)[*contagem_carregadores - 1].formas == NULL) {
            
            printf("Erro na criação de pilha para carregador\n");
            exit(1);
        }
        carregadorEsquerdoPtr = &(*carregadores)[*contagem_carregadores - 1];
    }

    
    if (carregadorDireitoPtr == NULL) {
        *contagem_carregadores += 1;
        *carregadores = realloc(*carregadores, *contagem_carregadores * sizeof(Carregador_t));

        if (*carregadores == NULL) {
            printf("Erro de alocação para os carregadores\n");
            exit(1);
        }
        
        bool carregador_ja_marcado = false;

        int pilha_tamanho_contagem = tamanhoPilha(pilha_para_liberar);

        for (int i = 0; i < pilha_tamanho_contagem; i++) {
            
            ObjetoFree *objeto = (ObjetoFree *)pilhaElemento(pilha_para_liberar, i);

            if (objeto != NULL && objeto->tipo == ARRAY_CARREGADORES_FREE) {

            
            objeto->pointer = *carregadores;
            carregador_ja_marcado = true;
            break;
            }
        }

        if (!carregador_ja_marcado) {

        ObjetoFree *carregador_objeto = malloc(sizeof(ObjetoFree));

        if (carregador_objeto != NULL) {

        carregador_objeto->pointer = *carregadores;
        carregador_objeto->tipo = ARRAY_CARREGADORES_FREE;

        pushPilha(pilha_para_liberar, carregador_objeto);

        }
        }
        (*carregadores)[*contagem_carregadores - 1] =  (Carregador_t){.id = carregadorDireitoIDInt, .formas = NULL};
        
        (*carregadores)[*contagem_carregadores - 1].formas = malloc(sizeof(Pilha));

        if ((*carregadores)[*contagem_carregadores - 1].formas == NULL) {


            printf("Erro na alocação de pilha dos carregadores\n");
            exit(1);
        }
        ObjetoFree *pilha_item = malloc(sizeof(ObjetoFree));

        if (pilha_item != NULL) {

            pilha_item->pointer = (*carregadores)[*contagem_carregadores - 1].formas;
            pilha_item->tipo = PILHA_FREE;
            pushPilha(pilha_para_liberar, pilha_item);

        }
        *(*carregadores)[*contagem_carregadores - 1].formas = criaPilha();

        if (*(*carregadores)[*contagem_carregadores - 1].formas == NULL) {

            printf("Erro de alocação de pilha no carregador\n");
            exit(1);
        }
        
        carregadorDireitoPtr = &(*carregadores)[*contagem_carregadores - 1];
        
    }

    (*disparadores)[indice_disparador].carregadorEsquerdo = carregadorEsquerdoPtr;
    (*disparadores)[indice_disparador].carregadorDireito = carregadorDireitoPtr;
    (*disparadores)[indice_disparador].idCarregadorEsquerdo = carregadorEsquerdoIDInt;
    (*disparadores)[indice_disparador].idCarregadorDireito = carregadorDireitoIDInt;
    
    } else {
        printf("Erro: disparador de indice %d não encontrado\n", disparadorIDInt);
    }
    }




static void realiza_shift(Disparador_t **disparadores, int contagem_disparadores,  int DisparadorID, 
         char *direcao,  int qtd, Carregador_t *carregadores, int contagem_carregadores){

    int indice_disparador = encontra_disparador_por_id(disparadores, contagem_disparadores, DisparadorID);

    if (indice_disparador == -1) {
        printf("Erro: disparador com a ID %d não encontrado\n", DisparadorID);
        return;
    }

    Disparador_t *disparador = &(*disparadores)[indice_disparador];
    

    
    Carregador_t *esquerdo_resolvido = NULL;
    Carregador_t *direito_resolvido = NULL;

    if (disparador->idCarregadorEsquerdo != -1) {

        for (int i = 0; i < contagem_carregadores; i++) {

            if (carregadores[i].id == disparador->idCarregadorEsquerdo) {
            esquerdo_resolvido = &carregadores[i];
            break;
            }
        }
    }

    if (disparador->idCarregadorDireito != -1) {

        for (int i = 0; i < contagem_carregadores; i++) 
        {
            if (carregadores[i].id == disparador->idCarregadorDireito) {
            direito_resolvido = &carregadores[i];
            break;
            }
        }
    }

    disparador->carregadorEsquerdo = esquerdo_resolvido;
    disparador->carregadorDireito = direito_resolvido;


    for (int i = 0; i < qtd; i++) {

        if (strcmp(direcao, "e") == 0) {
            
            if (disparador->carregadorEsquerdo == NULL || pilhaVazia(*(disparador->carregadorEsquerdo->formas))) {

                 continue; 
            }

         
            if (disparador->posicao_disparo != NULL && disparador->carregadorDireito != NULL) {


                pushPilha(*(disparador->carregadorDireito->formas), disparador->posicao_disparo);
            }

            disparador->posicao_disparo = popPilha(*(disparador->carregadorEsquerdo->formas));

        }
        if (strcmp(direcao, "d") == 0) {

           
            if (disparador->carregadorDireito == NULL || pilhaVazia(*(disparador->carregadorDireito->formas))) {

            continue;
            }

         
            if (disparador->posicao_disparo != NULL && disparador->carregadorEsquerdo != NULL) {

            pushPilha(*(disparador->carregadorEsquerdo->formas), disparador->posicao_disparo);

            }

            disparador->posicao_disparo = popPilha(*(disparador->carregadorDireito->formas));

        }
    }
}



static void executa_shft(Disparador_t **disparadores, int *contagem_disparadores, Carregador_t *carregadores, int* contagem_carregadores, FILE *txt) {

    char *disparadorID = strtok(NULL, " ");
    char *botao_esquerdo_ou_direito = strtok(NULL, " ");
    char *qtdPressionada = strtok(NULL, " ");

    fprintf(txt, "--shft:\n");

    int disparadorIDInt = atoi(disparadorID);
    int qtdPressionadaInt = atoi(qtdPressionada);

    fprintf(txt, "\t·Disparador ID: %d\n", disparadorIDInt);
    fprintf(txt, "\t·Botão: %s", botao_esquerdo_ou_direito);
    fprintf(txt, "\t·Quantidade de vezes pressionado: %d", qtdPressionadaInt);
    fprintf(txt, "\n");


    realiza_shift(disparadores, *contagem_disparadores, disparadorIDInt, botao_esquerdo_ou_direito, qtdPressionadaInt, carregadores, *contagem_carregadores);
}

static void realiza_disparo(Disparador_t **disparadores, int contagem_disparadores, int DisparadorID, double dx, double dy,  char *annotate, 
                            Pilha arena, Pilha pilha_para_free){

    int indice_disparador = encontra_disparador_por_id(disparadores, contagem_disparadores, DisparadorID);

    if (indice_disparador == -1) {

        printf("Erro disparador com id %d não encontrado\n", DisparadorID);
        return;
    }

    Disparador_t *disparador = &(*disparadores)[indice_disparador];

    
    if (disparador->posicao_disparo == NULL) {

        return;
    }

    double formaX_na_arena = disparador->x + dx;
    double formaY_na_arena = disparador->y + dy;

    Forma_t *forma = (Forma_t *)disparador->posicao_disparo;

 

    
    PosicaoFormaArena_t *posicao_forma_arena = malloc(sizeof(PosicaoFormaArena_t));

    if (posicao_forma_arena == NULL) {

        printf("Erro de alocação \n");
        exit(1);
    }
    posicao_forma_arena->forma = forma;
    posicao_forma_arena->x = formaX_na_arena;
    posicao_forma_arena->y = formaY_na_arena;
    posicao_forma_arena->isAnnotated = strcmp(annotate, "v") == 0;
    posicao_forma_arena->dispX = disparador->x;
    posicao_forma_arena->dispY = disparador->y;

    
    disparador->posicao_disparo = NULL;

    pushPilha(arena, (void *)posicao_forma_arena);
    ObjetoFree *forma_item = malloc(sizeof(ObjetoFree));

    if (forma_item != NULL) {
        forma_item->pointer = posicao_forma_arena;
        forma_item->tipo = FORMA_POSICAO_FREE;
        pushPilha(pilha_para_free, forma_item);
    }
}


static void executa_dsp(Disparador_t **disparadores, int *contagem_disparadores,  Pilha arena, Pilha pilha_para_free, FILE *txt) {

    char *disparadorID = strtok(NULL, " ");
    char *dx = strtok(NULL, " ");
    char *dy = strtok(NULL, " ");
    char *dimensoes_annotate = strtok(NULL, " "); // v ou i

    int disparadorIDInt = atoi(disparadorID);
    double dxDouble = atof(dx);
    double dyDouble = atof(dy);

    fprintf(txt, "--dsp:\n");
    fprintf(txt, "\t·Disparador ID: %d\n", disparadorIDInt);
    fprintf(txt, "\t·DX: %f\n", dxDouble);
    fprintf(txt, "\t·DY: %f\n", dyDouble);
    fprintf(txt, "\t·Anotação das dimensões: %s\n\n", dimensoes_annotate);

    realiza_disparo(disparadores, *contagem_disparadores, disparadorIDInt, dxDouble,
    dyDouble, dimensoes_annotate, arena, pilha_para_free);

}

int executa_rjd(Disparador_t **disparadores, int *contagem_disparadores,
    Pilha pilha_para_free, Pilha arena,
    Carregador_t *carregadores, int* contagem_carregadores, FILE *txt) {


        char *disparadorId = strtok(NULL, " ");
        char *botao_esquerdo_ou_direiro = strtok(NULL, " ");
        char *dx = strtok(NULL, " ");
        char *dy = strtok(NULL, " ");
        char *iX = strtok(NULL, " ");
        char *iY = strtok(NULL, " ");

        int disparadorIdInt = atoi(disparadorId);
        double dxDouble = atof(dx);
        double dyDouble = atof(dy);
        double iXDouble = atof(iX);
        double iYDouble = atof(iY);

        int indice_disparador = encontra_disparador_por_id(disparadores, *contagem_disparadores, disparadorIdInt);
        
        if (indice_disparador == -1) {

            printf("Erro disparador com ID %d não encontrado\n", disparadorIdInt);
            return 0;
        }

        Disparador_t *disparador = &(*disparadores)[indice_disparador];

        Carregador_t *carregador = NULL;
        
        if (strcmp(botao_esquerdo_ou_direiro, "e") == 0) {

        
        int id_alvo = (*disparadores)[indice_disparador].idCarregadorEsquerdo;
            if (id_alvo != -1) {

                for (int i = 0; i < *contagem_carregadores; i++) {

                    if (carregadores[i].id == id_alvo) {
                        
                        (*disparadores)[indice_disparador].carregadorEsquerdo = &carregadores[i];
                        break;
                    }
                }
            }

        } else if (strcmp(botao_esquerdo_ou_direiro, "d") == 0) {

            int id_alvo = (*disparadores)[indice_disparador].idCarregadorDireito;
            if (id_alvo != -1) {

                for (int i = 0; i < *contagem_carregadores; i++) {

                    if (carregadores[i].id == id_alvo) {

                        (*disparadores)[indice_disparador].carregadorDireito = &carregadores[i];
                        break;
                    }
                }
            }
        }
      
        if (strcmp(botao_esquerdo_ou_direiro, "e") == 0) {

        carregador = disparador->carregadorEsquerdo;

        } else if (strcmp(botao_esquerdo_ou_direiro, "d") == 0) {

        carregador = disparador->carregadorDireito;

        } else {
            printf("Erro: botão deve ser 'e' ou'd' \n");
            return 0;
        }

        if (carregador == NULL || carregador->formas == NULL) {
            printf("Error: Loader not found or shapes stack is NULL\n");
            return 0; 
        }

        int qtd = 1;

        fprintf(txt, "--rjd:\n");
        fprintf(txt, "\t·Disparador ID: %d\n", disparadorIdInt);
        fprintf(txt, "\t·Botão: %s\n", botao_esquerdo_ou_direiro);
        fprintf(txt, "\t·DX: %f\n", dxDouble);
        fprintf(txt, "\t·DY: %f\n", dyDouble);
        fprintf(txt, "\t·IX: %f\n", iXDouble);
        fprintf(txt, "\t·IY: %f\n", iYDouble);
        fprintf(txt, "\n\n");

        int qtd_disparos = 0;
        while (!pilhaVazia(*(carregador->formas))) {
            realiza_shift(disparadores, *contagem_disparadores, disparadorIdInt, botao_esquerdo_ou_direiro, 1, carregadores, *contagem_carregadores);
            realiza_disparo(disparadores, *contagem_disparadores, disparadorIdInt,
                            qtd * iXDouble + dxDouble,  qtd * iYDouble + dyDouble, "i", arena, pilha_para_free);
            qtd++;
            qtd_disparos++;
        }
        return qtd_disparos;
}


void executa_calc(Pilha arena, Chao chao, FILE *txtFile, int qtd_comandos, int qtd_disparos) {
  
  Pilha temp = criaPilha();
  while (!pilhaVazia(arena)) {
    pushPilha(temp, popPilha(arena));
  }

  
  double area_total_esmagada = 0.0;

  int total_formas_esmagadas = 0;
  int total_formas_clonadas = 0;


  while (!pilhaVazia(temp)) {
    PosicaoFormaArena_t *I = (PosicaoFormaArena_t *)popPilha(temp);
    if (pilhaVazia(temp)) {
    
      Forma_t *Ipos = clona_posicao(I->forma, I->x, I->y, chao);
      if (Ipos != NULL) {
        enqueueFila(get_fila_chao(chao), Ipos);
      }
      continue;
    }
    PosicaoFormaArena_t *J = (PosicaoFormaArena_t *)popPilha(temp);

    bool overlap = sobreposicao_formas(I, J);
    if (overlap) {
      double areaI = area_forma(I->forma->tipo, I->forma->data);
      double areaJ = area_forma(J->forma->tipo, J->forma->data);
      
      area_total_esmagada += (areaI < areaJ) ? areaI : areaJ;

      if (areaI < areaJ) {
     
        Forma_t *Jpos = clona_posicao(J->forma, J->x, J->y, chao);
        if (Jpos != NULL) {
          enqueueFila(get_fila_chao(chao), Jpos);
        }

        total_formas_esmagadas++;
      } else if (areaI >= areaJ) {
        
        total_formas_clonadas++;

         char *fillColorI = NULL;
        switch (I->forma->tipo) {
        case CIRCLE:
          fillColorI = getCorPCirculo((CIRCULO)I->forma->data);
          break;
        case RECTANGLE:
          fillColorI =getCorPRetangulo((RETANGULO)I->forma->data);
          break;
        case TEXT:
          fillColorI = getCorPTexto((TEXTO)I->forma->data);
          break;
        case LINE:
        case TEXT_STYLE:
          fillColorI = NULL;
          break;
        }

        
        Forma_t *JprimePos = NULL;
        if (fillColorI != NULL) {
          JprimePos = clone_posicao_corB(J->forma, fillColorI, J->x, J->y, chao);
        } else {
          JprimePos = clona_posicao(J->forma, J->x, J->y, chao);
        }

    
        Forma_t *Ipos = clona_posicao(I->forma, I->x, I->y, chao);
        if (Ipos != NULL) {
          enqueueFila(get_fila_chao(chao), Ipos);
        }
        if (JprimePos != NULL) {
          enqueueFila(get_fila_chao(chao), JprimePos);
        }

       
        Forma_t *IclonePos = clona_posicao_coresInvertidas(I->forma, I->x, I->y, chao);
        if (IclonePos != NULL) {
          enqueueFila(get_fila_chao(chao), IclonePos);
        }
      } else {
     
        Forma_t *Ipos = clona_posicao(I->forma, I->x, I->y, chao);
        Forma_t *Jpos = clona_posicao(J->forma, J->x, J->y, chao);
        if (Ipos != NULL) {
          enqueueFila(get_fila_chao(chao), Ipos);
        }
        if (Jpos != NULL) {
          enqueueFila(get_fila_chao(chao), Jpos);
        }
      }
    } else {
      
      Forma_t *Ipos = clona_posicao(I->forma, I->x, I->y, chao);
      Forma_t *Jpos = clona_posicao(J->forma, J->x, J->y, chao);
      if (Ipos != NULL) {
        enqueueFila(get_fila_chao(chao), Ipos);
      }
      if (Jpos != NULL) {
        enqueueFila(get_fila_chao(chao), Jpos);
      }
    }
  }

  
  fprintf(txtFile, "--calc:\n");
  fprintf(txtFile, "\t·Resultado final: %.2lf\n", area_total_esmagada);
  fprintf(txtFile, "\t·Quantidade de Comandos:%d\n", qtd_comandos);
  fprintf(txtFile, "\t·Quantidade de Disparos:%d\n", qtd_disparos);
  fprintf(txtFile, "\t·Total de formas esmagadas:%d\n", total_formas_esmagadas);
  fprintf(txtFile, "\t·Total de formas clonadas:%d\n", total_formas_clonadas);
  fprintf(txtFile, "\n");
  
  desalocaPilha(temp);
}

    

static int encontra_disparador_por_id(Disparador_t **disparadores, int contagem_disparadores, int id){

    for (int i = 0; i < contagem_disparadores; i++) {
    if ((*disparadores)[i].id == id) {
    return i;
    }
    }
    return -1;
}

// =====================
// Funções auxiliares
// =====================

static Forma_t *encapsula_forma(TipoForma tipo, void *data) {
    Forma_t *f = (Forma_t *)malloc(sizeof(Forma_t));
    if (f == NULL) {
        printf("Erro na alocação de forma\n");
        exit(1);
    }
    f->tipo = tipo;
    f->data = data;

    return f;
}

static double area_forma(TipoForma tipo, void *dados_forma) {

    switch (tipo) {
    case CIRCLE: {
    double a = calculoAreaCirculo((CIRCULO)dados_forma);
    return a;

    }
    case RECTANGLE: {

        double a = calculoAreaRetangulo((RETANGULO)dados_forma);
        return a;
    }
    case LINE: {
   
        double a = calculoAreaLinha((LINHA)dados_forma);
        return a;

    }
    case TEXT: {
    
        double a = calculoAreaTexto((TEXTO)dados_forma);
        return a;
    }
    case TEXT_STYLE:
    return 0.0;
    }
    return 0.0;
}



static BoundingBox cria_boundingbox_forma( PosicaoFormaArena_t *P){

    BoundingBox box;

    switch (P->forma->tipo) {

    case CIRCLE: {
      double r = getRaioCirculo((CIRCULO)P->forma->data);

      box.minX = P->x - r;
      box.maxX = P->x + r;
      box.minY = P->y - r;
      box.maxY = P->y + r;
      break;

    }
    case RECTANGLE: {
      double l = getLarguraRetangulo((RETANGULO)P->forma->data);
      double a = getAlturaRetangulo((RETANGULO)P->forma->data);
      box.minX = P->x;
      box.minY = P->y;
      box.maxX = P->x + l;
      box.maxY = P->y + a;
      break;
    }

    case TEXT: {
    
      TEXTO t = (TEXTO)P->forma->data;

       char *txt = getTxtTexto(t);
      int len = (int)strlen(txt);
      double segLen = 10.0 * (double)len;

      char ancora= getAncoraTexto(t);

      double x1 = P->x;
      double y1 = P->y;
      double x2 = P->x;
      double y2 = P->y;


      if (ancora == 'i' || ancora == 'I') {
      
        x2 = P->x + segLen;
        y2 = P->y;
      } else if (ancora == 'f' || ancora == 'F' || ancora == 'e' ||  ancora == 'E') {
       
        x1 = P->x - segLen;
        y1 = P->y;

      } else if (ancora == 'm' || ancora == 'M') {

        
        x1 = P->x - segLen * 0.5;
        y1 = P->y;
        x2 = P->x + segLen * 0.5;
        y2 = P->y;
      } else {

        
        x2 = P->x + segLen;
        y2 = P->y;

      }
  
      double dx = x2 - x1;
      double dy = y2 - y1; 
      double minLocalX = (dx < 0.0) ? dx : 0.0;
      double maxLocalX = (dx > 0.0) ? dx : 0.0;
      double minLocalY = (dy < 0.0) ? dy : 0.0;
      double maxLocalY = (dy > 0.0) ? dy : 0.0;

      
      box.minX = x1 + minLocalX - 1.0;
      box.maxX = x1 + maxLocalX + 1.0;
      box.minY = y1 + minLocalY - 1.0;
      box.maxY = y1 + maxLocalY + 1.0;
      break;


    }
    case LINE: {


      double x1 = getX1Linha((LINHA)P->forma->data);
      double y1 = getY1Linha((LINHA)P->forma->data);
      double x2 = getX2Linha((LINHA)P->forma->data);
      double y2 = getY2Linha((LINHA)P->forma->data);
      double dx = x2 - x1;
      double dy = y2 - y1;
      double minLocalX = (dx < 0.0) ? dx : 0.0;
      double maxLocalX = (dx > 0.0) ? dx : 0.0;
      double minLocalY = (dy < 0.0) ? dy : 0.0;
      double maxLocalY = (dy > 0.0) ? dy : 0.0;
    
      box.minX = P->x + minLocalX - 1.0;
      box.maxX = P->x + maxLocalX + 1.0;
      box.minY = P->y + minLocalY - 1.0;
      box.maxY = P->y + maxLocalY + 1.0;
      break;


    }
    case TEXT_STYLE: {
      
      box.minX = box.maxX = P->x;
      box.minY = box.maxY = P->y;
      break;
    }
    }

    return box;
  }
  
  static bool sobreposicao_boundingbox(BoundingBox a, BoundingBox b) {   

        if (a.maxX < b.minX)
        return false;
        if (b.maxX < a.minX)
        return false;
        if (a.maxY < b.minY)
        return false;
        if (b.maxY < a.minY)
        return false;
        return true;

  }
  
  static bool sobreposicao_formas( PosicaoFormaArena_t *a,  PosicaoFormaArena_t *b) {


        BoundingBox aa = cria_boundingbox_forma(a);
        BoundingBox bb = cria_boundingbox_forma(b);

        return sobreposicao_boundingbox(aa, bb);


  }
  

  
  
 
  
  static Forma_t *clona_posicao(Forma_t *forma_original, double x, double y, Chao chao) {

    if (forma_original == NULL)
      return NULL;

    Forma_t *clonado = NULL;
    switch (forma_original->tipo) {

    case CIRCLE: {

      CIRCULO c = (CIRCULO)forma_original->data;
      int id = getIDCirculo(c);
      double r = getRaioCirculo(c);
       char *borda = getCorBCirculo(c);
       char *preenchimento = getCorPCirculo(c);
      CIRCULO novo_circulo = criaCirculo(id, x, y, r, borda, preenchimento);
      clonado = encapsula_forma(CIRCLE, novo_circulo);

      break;
    }
    case RECTANGLE: {

      RETANGULO r = (RETANGULO)forma_original->data;
      int id = getIDRetangulo(r);
      double l = getLarguraRetangulo(r);
      double a = getAlturaRetangulo(r);
       char *borda = getCorBRetangulo(r);
       char *preenchimento = getCorPRetangulo(r);
      RETANGULO novo_retangulo = criaRetangulo(id, x, y, a, l, borda, preenchimento);
      clonado = encapsula_forma(RECTANGLE, novo_retangulo);
      break;

    }
    case TEXT: {

      TEXTO t = (TEXTO)forma_original->data;
      int id = getIDTexto(t);
       char *borda = getCorBTexto(t);
       char *preenchimento = getCorPTexto(t);
      char ancora = getAncoraTexto(t);
       char *txt = getTxtTexto(t);
      TEXTO novo_texto = criaTexto(id, x, y, borda, preenchimento, ancora, txt);
      clonado = encapsula_forma(TEXT, novo_texto);
      break;

    }
    case LINE: {

      LINHA l = (LINHA)forma_original->data;
      int id = getIDLinha(l);
      double dx = getX2Linha(l) - getX1Linha(l);
      double dy = getY2Linha(l) - getY1Linha(l);
      LINHA nova_linha = criaLinha(id, x, y, x + dx, y + dy, getCorLinha(l));
      clonado = encapsula_forma(LINE, nova_linha);

      break;
    }
    case TEXT_STYLE:
      return NULL;
    }
  
   
    if (clonado != NULL && chao != NULL) {
      pushPilha(obtem_pilha_para_desalocar(chao), clonado);

    }
    return clonado;
  }
  
  static Forma_t *clone_posicao_corB(Forma_t *forma_original,  char *novaCorB, double x, double y, Chao chao) {

    if (forma_original == NULL)
      return NULL;
    Forma_t *clonado = NULL;
    switch (forma_original->tipo) {
    case CIRCLE: {

      CIRCULO c = (CIRCULO)forma_original->data;
      int id = getIDCirculo(c);
      double r = getRaioCirculo(c);
       char *preenchimento = getCorPCirculo(c);
      CIRCULO novo_circulo = criaCirculo(id, x, y, r, novaCorB, preenchimento);
      clonado = encapsula_forma(CIRCLE, novo_circulo);
      break;
    }
    case RECTANGLE: {
      RETANGULO r = (RETANGULO)forma_original->data;
      int id = getIDRetangulo(r);
      double l = getLarguraRetangulo(r);
      double a = getAlturaRetangulo(r);
       char *preenchimento = getCorPRetangulo(r);
      RETANGULO novo_retangulo = criaRetangulo(id, x, y, a, l, novaCorB, preenchimento);
      clonado = encapsula_forma(RECTANGLE, novo_retangulo);
      break;
    }
    case TEXT: {
      TEXTO t = (TEXTO)forma_original->data;
      int id = getIDTexto(t);
       char *preenchimento = getCorPTexto(t);
      char ancora = getAncoraTexto(t);
       char *txt = getTxtTexto(t);
      TEXTO novo_texto = criaTexto(id, x, y, novaCorB, preenchimento, ancora, txt);
      clonado = encapsula_forma(TEXT, novo_texto);
      break;
    }
    case LINE: {
      LINHA l = (LINHA)forma_original->data;
      int id = getIDLinha(l);
      double dx = getX2Linha(l) - getX1Linha(l);
      double dy = getY2Linha(l) - getY1Linha(l);
      LINHA nova_linha = criaLinha(id, x, y, x + dx, y + dy, novaCorB);
      clonado = encapsula_forma(LINE, nova_linha);
      break;
    }
    case TEXT_STYLE:
      return NULL;
    }
  
    
    if (clonado != NULL && chao != NULL) {
      pushPilha(obtem_pilha_para_desalocar(chao), clonado);
    }
  
    return clonado;
  }
  
  static Forma_t *clona_posicao_coresInvertidas(Forma_t *forma_original, double x, double y, Chao chao) {

    if (forma_original == NULL)
      return NULL;
    Forma_t *clonado = NULL;
    switch (forma_original->tipo) {

    case CIRCLE: {
      CIRCULO c = (CIRCULO)forma_original->data;
      int id = getIDCirculo(c);
      double r = getRaioCirculo(c);
       char *borda = getCorBCirculo(c);
       char *preenchimento = getCorPCirculo(c);
      CIRCULO novo_circulo = criaCirculo(id, x, y, r, borda, preenchimento);
      clonado = encapsula_forma(CIRCLE, novo_circulo);
      break;
    }
    case RECTANGLE: {
      RETANGULO r = (RETANGULO)forma_original->data;
      int id = getIDRetangulo(r);
      double l = getLarguraRetangulo(r);
      double a = getAlturaRetangulo(r);
       char *borda = getCorBRetangulo(r);
       char *preenchimento = getCorPRetangulo(r);
      RETANGULO novo_retangulo = criaRetangulo(id, x, y, a, l, preenchimento, borda);
      clonado = encapsula_forma(RECTANGLE, novo_retangulo);
      break;
    }
    case TEXT: {
      TEXTO t = (TEXTO)forma_original->data;
      int id = getIDTexto(t);
       char *borda = getCorBTexto(t);
       char *preenchimento = getCorPTexto(t);
      char ancora = getAncoraTexto(t);
       char *txt = getTxtTexto(t);
      TEXTO novo_texto = criaTexto(id, x, y, preenchimento, borda, ancora, txt);
      clonado = encapsula_forma(TEXT, novo_texto);
      break;
    }
    case LINE: {
      LINHA l = (LINHA)forma_original->data;
      int id = getIDLinha(l);
      double dx = getX2Linha(l) - getX1Linha(l);
      double dy = getY2Linha(l) - getX1Linha(l);
       char *c = getCorLinha(l);
      char *complemento = cor_complementar(c);
      if (complemento == NULL)
        return NULL;
      LINHA nova_linha = criaLinha(id, x, y, x + dx, y + dy, complemento);
      free(complemento);
      clonado = encapsula_forma(LINE, nova_linha);
      break;
    }
    case TEXT_STYLE:
      return NULL;
    }
  
    
    if (clonado != NULL && chao != NULL) {
      pushPilha(obtem_pilha_para_desalocar(chao), clonado);
    }
  
    return clonado;
  }
  
  // =====================
  //Escritor do svg
  // =====================
  static void cria_svg_com_resultado(DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo,  Chao chao, Pilha arena,  char *caminho_output) {

     char *nome_geo_src = obter_nome_arquivo(dadosGeo);
     char *nome_qry_src = obter_nome_arquivo(dadosQry);
    size_t geo_len = strlen(nome_geo_src);
    size_t qry_len = strlen(nome_qry_src);
  
    char *geo_base = malloc(geo_len + 1);
    char *qry_base = malloc(qry_len + 1);
    if (geo_base == NULL || qry_base == NULL) {
      printf("Erro de alocação\n");
      free(geo_base);
      free(qry_base);
      return;
    }
    strcpy(geo_base, nome_geo_src);
    strcpy(qry_base, nome_qry_src);
    strtok(geo_base, ".");
    strtok(qry_base, ".");
  
    // geoBase-qryBase.svg
    size_t caminho_len = strlen(caminho_output);
    size_t nome_processado_len = strlen(geo_base) + 1 + strlen(qry_base);
    size_t total_len = caminho_len + 1 + nome_processado_len + 4 + 1;
    char *caminho_output_arquivo = malloc(total_len);
    if (caminho_output_arquivo == NULL) {
      printf("Error na alocação de memoria pro caminho do arquivo\n");
      free(geo_base);
      free(qry_base);
      return;
    }
  
    int result = snprintf(caminho_output_arquivo, total_len, "%s/%s-%s.svg", caminho_output, geo_base, qry_base);
    if (result < 0 || (size_t)result >= total_len) {
      printf("Error: construção do caminho falhou\n");
      free(caminho_output_arquivo);
      free(geo_base);
      free(qry_base);
      return;
    }
  
    FILE *file = fopen(caminho_output_arquivo, "w");
    if (file == NULL) {
      printf("Error falha em abrir arquivo: %s\n", caminho_output_arquivo);
      free(caminho_output_arquivo);
      free(geo_base);
      free(qry_base);
      return;
    }
  
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 "
                  "1000\">\n");
  
    
    Fila fila_chao = get_fila_chao(chao);
    Fila fila_temp = criaFila();
    while (!filaVazia(fila_chao)) {
      Forma_t *forma = (Forma_t *)dequeueFila(fila_chao);
      if (forma != NULL) {
        if (forma->tipo == CIRCLE) {
          CIRCULO c = (CIRCULO)forma->data;
          fprintf(
              file,
              "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
              getXCirculo(c), getYCirculo(c),
              getRaioCirculo(c), getCorPCirculo(c),
              getCorBCirculo(c));

        } else if (forma->tipo == RECTANGLE) {
          RETANGULO r = (RETANGULO)forma->data;
          fprintf(file,
                  "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' "
                  "stroke='%s'/>\n",
                  getXRetangulo(r), getYRetangulo(r),
                  getLarguraRetangulo(r), getAlturaRetangulo(r),
                  getCorPRetangulo(r),
                  getCorBRetangulo(r));
        } else if (forma->tipo == LINE) {

            LINHA l = (LINHA)forma->data;

          fprintf(file,
                  "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                  getX1Linha(l), getY1Linha(l), getX2Linha(l),
                  getY2Linha(l), getCorLinha(l));
        } else if (forma->tipo == TEXT) {

          TEXTO t = (TEXTO)forma->data;
          char ancora = getAncoraTexto(t);
           char *ancora_texto = "start";
          if (ancora == 'm' || ancora == 'M') {
            ancora_texto = "middle";
          } else if (ancora == 'e' || ancora == 'E' || ancora == 'f' || ancora == 'F') {
             ancora_texto = "end";
          } else if (ancora == 's' || ancora == 'S' || ancora == 'i' || ancora == 'I') {
            ancora_texto = "start";
          }
          fprintf(file,
                  "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' "
                  "text-anchor='%s'>%s</text>\n",
                  getXTexto(t), getYTexto(t), getCorPTexto(t),
                  getCorBTexto(t), ancora_texto, getTxtTexto(t));
        }
      }
      enqueueFila(fila_temp, forma);
    }
    
    while (!filaVazia(fila_temp)) {
      enqueueFila(fila_chao, dequeueFila(fila_temp));
    }
    desalocaFila(fila_temp);
  
    
    Pilha pilha_temp = criaPilha();
    while (!pilhaVazia(arena)) {
      PosicaoFormaArena_t *f = (PosicaoFormaArena_t *)popPilha(arena);
      if (f != NULL && f->isAnnotated) {
        
        fprintf(file,
                "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='red' "
                "stroke-dasharray='4,2' stroke-width='1'/>\n",
                f->dispX, f->dispY, f->x, f->y);
    
        fprintf(file,
                "<circle cx='%.2f' cy='%.2f' r='3' fill='none' stroke='red' "
                "stroke-width='1'/>\n",
                f->x, f->y);

        double dx = f->x - f->dispX;
        double dy = f->y - f->dispY;
        double midHx = f->dispX + dx * 0.5;
        double midHy = f->dispY;
        double midVx = f->x;
        double midVy = f->dispY + dy * 0.5;
  
        fprintf(file,
                "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='purple' "
                "stroke-dasharray='2,2' stroke-width='0.8'/>\n",
                f->dispX, f->dispY, f->x, f->dispY);
        
        fprintf(file,
                "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='purple' "
                "stroke-dasharray='2,2' stroke-width='0.8'/>\n",
                f->x, f->dispY, f->x, f->y);
  
        fprintf(file,
                "<text x='%.2f' y='%.2f' fill='purple' font-size='12' "
                "text-anchor='middle'>%.2f</text>\n",
                midHx, midHy - 5.0, dx);
  
        fprintf(file,
                "<text x='%.2f' y='%.2f' fill='purple' font-size='12' "
                "text-anchor='middle' transform='rotate(-90 %.2f "
                "%.2f)'>%.2f</text>\n",
                midVx + 10.0, midVy, midVx + 10.0, midVy, dy);
      }
      pushPilha(pilha_temp, f);
    }
    
    while (!pilhaVazia(pilha_temp)) {
      pushPilha(arena, popPilha(pilha_temp));
    }
    desalocaPilha(pilha_temp);
  
    fprintf(file, "</svg>\n");
    fclose(file);
    free(caminho_output_arquivo);
    free(geo_base);
    free(qry_base);
  }