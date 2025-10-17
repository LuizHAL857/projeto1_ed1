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


enum TipoForma{ CIRCLE, RECTANGLE, LINE, TEXT, TEXT_STYLE };



typedef enum TipoForma TipoForma;

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


static void executa_rjd(Disparador_t **disparadores, int *contagem_disparadores,
    Pilha pilha_para_free, Pilha arena,
    Carregador_t *carregadores, int* contagem_carregadores, FILE *txt);


static void executa_calc(Pilha arena, Chao chao, FILE *txt);


static int encontra_disparador_por_id(Disparador_t **disparadores, int contagem_disparadores, int id);


static void realiza_shift(Disparador_t **disparadores, int contagem_disparadores,  int DisparadorID, 
      const char *direcao,  int qtd, Carregador_t *carregadores, int contagem_carregadores);


static void realiza_disparo(Disparador_t **disparadores, int contagem_disparadores, int DisparadorID, double dx, double dy, const char *annotate, 
    Pilha arena, Pilha pilha_para_free);

// Funções auxiliares para comando calc
static double area_forma(TipoForma tipo, void *dados_forma);


static BoundingBox cria_boundingbox_forma(const PosicaoFormaArena_t *P);

static bool sobreposicao_boundingbox(BoundingBox a, BoundingBox b);

static bool sobreposicao_formas(const PosicaoFormaArena_t *a, const PosicaoFormaArena_t *b);


static Forma_t *encapsula_forma(TipoForma tipo, void *data);


static Forma_t *clonaForma_corB(Forma_t *forma_original,  const char *novaCorB);

static Forma_t *clonaForma_coresInvertidas(Forma_t *forma_original);

// Funções para auxiliar clonagem
static Forma_t *clona_posicao(Forma_t *forma_original, double x, double y, Chao chao);

static Forma_t *clone_posicao_corB(Forma_t *forma_original, const char *novaCorB, double x, double y, Chao chao);
                                                    
static Forma_t *clona_posicao_coresInvertidas(Forma_t *forma_original, double x, double y, Chao chao);
                                                      
static void desaloca_forma(Forma_t *forma);


// Função para escrever o arquivo svg
static void cria_svg_com_resultado(DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo,  Chao chao, Pilha arena, const char *caminho_output);

Qry executa_qry(DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo, Chao chao, const char *caminho_output) {

  Qry_t *qry = malloc(sizeof(Qry_t));
  if (qry == NULL) {
    printf("Erro na alocação do qry\n");
    exit(1);
  }
  qry->arena = criaPilha();
  qry->pilha_para_free = criaPilha();

  


  Disparador_t *disparadores = NULL;

  int contagem_disparadores = 0;

  Carregador_t *carregadores = NULL;

  int contagem_carregadores = 0;
   // Abrir arquivo .txt com o mesmo nome-base do SVG de saída, mas extensão .txt


   size_t geo_len = strlen(obter_nome_arquivo(dadosGeo));


   size_t qry_len = strlen(obter_nome_arquivo(dadosQry));
 
 
   char *geo_base = malloc(geo_len + 1);
 
 
   char *qry_base = malloc(qry_len + 1);
 
 
   if (geo_base == NULL || qry_base == NULL) {
 
    printf("Erro na alocação do nome do arquivo\n");
 
    free(geo_base);
 
    free(qry_base);
    return NULL;
 
 
   }
 
 
   strcpy(geo_base, obter_nome_arquivo(dadosGeo));
 
 
   strcpy(qry_base, obter_nome_arquivo(dadosQry));
 
 
   strtok(geo_base, ".");
 
 
   strtok(qry_base, ".");
 
 
   size_t caminho_len = strlen(caminho_output);
 
 
  
 
 
   size_t nome_processsado_len = strlen(geo_base) + 1 + strlen(qry_base);
 
 
   size_t total_len = caminho_len + 1 + nome_processsado_len + 4 + 1;  
 
   char *caminho_saida_txt = malloc(total_len);
 
 
   if (caminho_saida_txt == NULL) {
 
 
     printf("Erro de alocação\n");
 
 
     free(geo_base);
 
 
     free(qry_base);
 
 
     return NULL;
 
 
   }
 
 
   int res = snprintf(caminho_saida_txt, total_len, "%s/%s-%s.txt", caminho_output, geo_base, qry_base);
 
 
   if (res < 0 || (size_t)res >= total_len) {
 
 
     printf("Erro:construção de caminho falhou\n");
 
 
     free(caminho_saida_txt);
 
 
     free(geo_base);
 
 
     free(qry_base);
 
 
     return NULL;
 
 
   }
 
 
   FILE *ArqTxt = fopen(caminho_saida_txt, "w");
 
 
   free(geo_base);
 
 
   free(qry_base);
 
 
   free(caminho_saida_txt);

  while (!filaVazia(obter_fila_linhas(dadosQry))) {
    char *linha = (char *)dequeueFila(obter_fila_linhas(dadosQry));

    char *comando = strtok(linha, " \t\r\n");

    if (comando == NULL || *comando == '\0') {

      continue;

    }

    if (strcmp(comando, "pd") == 0) {

      executa_pd(&disparadores, &contagem_disparadores, qry->pilha_para_free);

    } else if (strcmp(comando, "lc") == 0) {

      executa_lc(&carregadores, &contagem_carregadores, chao, qry->pilha_para_free, ArqTxt);


    } else if (strcmp(comando, "atch") == 0) {

      executa_atch(&carregadores, &contagem_carregadores, &disparadores, &contagem_disparadores, qry->pilha_para_free);

    } else if (strcmp(comando, "shft") == 0) {

      executa_shft(&disparadores, &contagem_disparadores, carregadores, contagem_carregadores, ArqTxt);

    } else if (strcmp(comando, "dsp") == 0) {

      executa_dsp(&disparadores, &contagem_disparadores, qry->arena, qry->pilha_para_free, ArqTxt);

    } else if (strcmp(comando, "rjd") == 0) {

      executa_rjd(&disparadores, &contagem_disparadores, qry->pilha_para_free, qry->arena, carregadores, &contagem_carregadores, ArqTxt);

    } else if (strcmp(comando, "calc") == 0) {

      executa_calc(qry->arena, chao, ArqTxt);

    } else
      printf("Comando desconhecido: %s\n", comando);
  }

 
  cria_svg_com_resultado(dadosQry, dadosGeo, chao, qry->arena,
                       caminho_output);

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
          free(o->pointer);

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

static void executa_lc(Carregador_t **carregadores, int *contagem_carregadores, Chao chao, Pilha pilha_para_free,  FILE *txtFile) {
    char *id = strtok(NULL, " ");
    char *primeirasFormas = strtok(NULL, " ");

    int CarregadorId = atoi(id);
    int nova_qtd_formas = atoi(primeirasFormas);

    fprintf(txtFile, "[lc]\n");
    fprintf(txtFile, "\tCarregador ID: %d\n", CarregadorId);
    fprintf(txtFile, "\tNova quantidade de formas: %d\n", nova_qtd_formas);

    
    int id_carregador_existente = -1;
    for (int i = 0; i < *contagem_carregadores; i++) {

        if ((*carregadores)[i].id == CarregadorId) {

            id_carregador_existente = i;
            break;

        }
    }

    if (id_carregador_existente == -1) {
    
    *contagem_carregadores += 1;
    *carregadores = realloc(*carregadores, *contagem_carregadores * sizeof(Carregador_t));
    if (*carregadores == NULL) {
            printf("Erro na alocação dos carregadores\n");
            exit(1);
    }

   
    bool carregadores_ja_marcados = false;
    int tamanhoPilha = tamanho_pilha(pilha_para_free);
    for (int i = 0; i < tamanhoPilha; i++) {

        ObjetoFree *objeto = (ObjetoFree *)pilhaElemento(pilha_para_free, i);

        if (objeto != NULL && objeto->tipo == ARRAY_CARREGADORES_FREE) {
           
            objeto->pointer = *carregadores;
            carregadores_ja_marcados = true;
            break;
        }

    }

    if (!carregadores_ja_marcados) {

        ObjetoFree *carregador_objeto = malloc(sizeof(ObjetoFree));

        if (carregador_objeto != NULL) {

            carregador_objeto->pointer = *carregadores;
            carregador_objeto->tipo = ARRAY_CARREGADORES_FREE;
            pushPilha(pilha_para_free, carregador_objeto);

        }
    }
    (*carregadores)[*contagem_carregadores - 1] = (Carregador_t){.id = CarregadorId, .formas = NULL};

    id_carregador_existente = *contagem_carregadores - 1;

    }

   
    if ((*carregadores)[id_carregador_existente].formas == NULL) {

        (*carregadores)[id_carregador_existente].formas = malloc(sizeof(Pilha));
        if ((*carregadores)[id_carregador_existente].formas == NULL) {

            printf("Erro na alocação de memória na pilha do carregador\n");
            exit(1);
        }

        
        ObjetoFree *pilha_item = malloc(sizeof(ObjetoFree));

        if (pilha_item != NULL) {

            pilha_item->pointer = (*carregadores)[id_carregador_existente].formas;
            pilha_item->tipo = PILHA_FREE;
            pushPilha(pilha_para_free, pilha_item);

            }
            
            *(*carregadores)[id_carregador_existente].formas = criaPilha();
            if (*(*carregadores)[id_carregador_existente].formas == NULL) {

            printf("Erro na criação da pilha dos carregadores\n");

            exit(1);
        }
    }

   
    for (int i = 0; i < nova_qtd_formas; i++) {

        Forma_t *forma = dequeueFila(obtem_pilha_para_desalocar(chao));

        if (forma != NULL) {

            if (!pushPilha(*(*carregadores)[id_carregador_existente].formas, forma)) {

                printf("Erro em colocar forma no carregador\n");
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
        const char *direcao,  int qtd, Carregador_t *carregadores, int contagem_carregadores){

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

    fprintf(txt, "[shft]");

    int disparadorIDInt = atoi(disparadorID);
    int qtdPressionadaInt = atoi(qtdPressionada);

    fprintf(txt, "\tDisparador ID: %d", disparadorIDInt);
    fprintf(txt, "\tBotão: %s", botao_esquerdo_ou_direito);
    fprintf(txt, "\tQuantidade de vezes pressionado: %d", qtdPressionadaInt);
    fprintf(txt, "\n");


    realiza_shift(disparadores, *contagem_disparadores, disparadorIDInt, botao_esquerdo_ou_direito, qtdPressionadaInt, carregadores, *contagem_carregadores);
}

static void realiza_disparo(Disparador_t **disparadores, int contagem_disparadores, int DisparadorID, double dx, double dy, const char *annotate, 
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

    TipoForma tipo_forma = forma->tipo;

    
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

    fprintf(txt, "[dsp]\n");
    fprintf(txt, "\tDisparador ID: %d\n", disparadorIDInt);
    fprintf(txt, "\tDX: %f\n", dxDouble);
    fprintf(txt, "\tDY: %f\n", dyDouble);
    fprintf(txt, "\tAnotação das dimensões: %s\n", dimensoes_annotate);

    realiza_disparo(disparadores, *contagem_disparadores, disparadorIDInt, dxDouble,
    dyDouble, dimensoes_annotate, arena, pilha_para_free);

}
static void executa_rjd(Disparador_t **disparadores, int *contagem_disparadores,
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
            return;
        }

        Disparador_t *disparador = &(*disparadores)[indice_disparador];

        Carregador_t *carregador = NULL;
        
        if (strcmp(botao_esquerdo_ou_direiro, "e") == 0) {

        
        int id_alvo = (*disparadores)[indice_disparador].idCarregadorDireito;
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
            return;
        }

        if (carregador == NULL || carregador->formas == NULL) {
            printf("Error: Loader not found or shapes stack is NULL\n");
            return;
        }

        int qtd = 1;

        fprintf(txt, "[rjd]\n");
        fprintf(txt, "\tDisparador ID: %d\n", disparadorIdInt);
        fprintf(txt, "\tBotão: %s\n", botao_esquerdo_ou_direiro);
        fprintf(txt, "\tDX: %f\n", dxDouble);
        fprintf(txt, "\tDY: %f\n", dyDouble);
        fprintf(txt, "\tIX: %f\n", iXDouble);
        fprintf(txt, "\tIY: %f\n", iYDouble);
        fprintf(txt, "\n");

        
        while (!pilhaVazia(*(carregador->formas))) {
            realiza_shift(disparadores, *contagem_disparadores, disparadorIdInt, botao_esquerdo_ou_direiro, 1, carregadores, *contagem_carregadores);
            realiza_disparo(disparadores, *contagem_disparadores, disparadorIdInt,
                            qtd * iXDouble + dxDouble,  qtd * iYDouble + dyDouble, "i", arena, pilha_para_free);
            qtd++;
        }
}






static void executa_calc(Pilha arena, Chao chao, FILE *txt){
    // deve ser processado em ordem de lançamento

    Pilha temp = criaPilha();
    while (!pilhaVazia(arena)) {

        pushPilha(temp, popPilha(arena));
    }

    // Calcula total esmagado

    double area_esmagada_total = 0.0;

    for (int i = 0; i < tamanhoPilha(temp); i++) {
        PosicaoFormaArena_t *item = (PosicaoFormaArena_t *)pilhaElemento(temp, i);

        if (item != NULL && item->isAnnotated) {
             area_esmagada_total += area_forma(item->forma->tipo, item->forma->data);
        }
    }

    
    while (!pilhaVazia(temp)) {

        PosicaoFormaArena_t *I = (PosicaoFormaArena_t *)popPilha(temp);
        
        if (pilhaVazia(temp)) {
        
            Forma_t *PosI = clona_posicao(I->forma, I->x, I->y, chao);

            if (PosI != NULL) {

                enqueueFila(get_fila_chao(chao), PosI);
            }
            continue;
        }
        PosicaoFormaArena_t *J = (PosicaoFormaArena_t *)popPilha(temp);


        bool sobreposicao = sobreposicao_formas(I, J);

        if (sobreposicao) {

            double areaI = area_forma(I->forma->tipo, I->forma->data);
            double areaJ = area_forma(J->forma->tipo, J->forma->data);

            if (areaI < areaJ) {
            
            Forma_t *PosJ = clona_posicao(J->forma, J->x, J->y, chao);

            if (PosJ != NULL) {
                enqueueFila(get_fila_chao(chao), PosJ);

            }
            } else  {
            
            const char *CorPI = NULL;
            switch (I->forma->tipo) {
            case CIRCLE:
                CorPI = getCorPCirculo((CIRCULO)I->forma->data);
                break;
                case RECTANGLE:
                CorPI = getCorPRetangulo((RETANGULO)I->forma->data);
                break;
                case TEXT:
                CorPI = getCorPTexto((TEXTO)I->forma->data);
                break;
                case LINE:
                case TEXT_STYLE:
                CorPI = NULL;
                break;
            }

            
            Forma_t *POSJini = NULL;
            if (CorPI != NULL) {

            POSJini = clone_posicao_corB(J->forma, CorPI,  J->x, J->y, chao);
            } else {

            POSJini = clona_posicao(J->forma, J->x, J->y, chao);

            }

           
            Forma_t *PosI = clona_posicao(I->forma, I->x, I->y, chao);

            if (PosI != NULL) {
                enqueueFila(get_fila_chao(chao), PosI);

            }
            if (POSJini != NULL) {
                enqueueFila(get_fila_chao(chao), POSJini);

            }

           
            Forma_t *PosIClone = clona_posicao_coresInvertidas(I->forma, I->x, I->y, chao);

            if (PosIClone != NULL) {

                enqueueFila(get_fila_chao(chao), PosIClone);

            }
            } 
        } else {
        
        Forma_t *PosI = clona_posicao(I->forma, I->x, I->y, chao);
        Forma_t *PosJ = clona_posicao(J->forma, J->x, J->y, chao);

        if (PosI != NULL) {
            enqueueFila(get_fila_chao(chao), PosI);

        }
        if (PosJ != NULL) {
            enqueueFila(get_fila_chao(chao), PosJ);

        }
        }
    }

    
    fprintf(txt, "[calc]\n");
    fprintf(txt, "\tResultado: %.2lf\n", area_esmagada_total);
    fprintf(txt, "\n");
    
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
