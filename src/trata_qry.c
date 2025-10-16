#include "trata_qry.h"
#include "fila.h"
#include "pilha.h"
#include "trata_geo.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include <stdio.h>
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
  Forma_t *shootingPosition;
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


static void executa_lc(Carregador_t **carregadores, int *contagem_carregadores, Chao chao, Pilha pilha_para_free);


static void executa_atch(Carregador_t **carregadores, int *contagem_carregadores,  Disparador_t **disparadores, int *contagem_disparadores,  Pilha pilha_para_free);


static void executa_shft(Disparador_t **disparadores, int *contagem_disparadores, Carregador_t *carregadores, int* contagem_carregadores);


static void executa_dsp(Disparador_t **disparadores, int *contagem_disparadores,  Pilha arena, Pilha pilha_para_free);


static void executa_rjd(Disparador_t **disparadores, int *contagem_disparadores,
    Pilha pilha_para_free, Pilha arena,
    Carregador_t *carregadores, int* contagem_carregadores);


static void executa_calc(Pilha arena, Chao chao);


static int getDisparadorID(Disparador_t **disparadores, int contagem_disparadores, int id);


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
static Forma_t *clona_posicao(Forma_t *forma_original, double x, double y);

static Forma_t *clone_posicao_corB(Forma_t *forma_original, const char *novaCorB, double x, double y);
                                                    
static Forma_t *clona_posicao_coresInvertidas(Forma_t *forma_original, double x, double y);
                                                      
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

  while (!filaVazia(obter_fila_linhas(dadosQry))) {
    char *linha = (char *)dequeueFila(obter_fila_linhas(dadosQry));

    char *comando = strtok(linha, " \t\r\n");

    if (comando == NULL || *comando == '\0') {

      continue;

    }

    if (strcmp(comando, "pd") == 0) {

      executa_pd(&disparadores, &contagem_disparadores, qry->pilha_para_free);

    } else if (strcmp(comando, "lc") == 0) {

      executa_lc(&carregadores, &contagem_carregadores, chao, qry->pilha_para_free);


    } else if (strcmp(comando, "atch") == 0) {

      executa_atch(&carregadores, &contagem_carregadores, &disparadores, &contagem_disparadores, qry->pilha_para_free);

    } else if (strcmp(comando, "shft") == 0) {

      executa_shft(&disparadores, &contagem_disparadores, carregadores, contagem_carregadores);

    } else if (strcmp(comando, "dsp") == 0) {

      executa_dsp(&disparadores, &contagem_disparadores, qry->arena, qry->pilha_para_free);

    } else if (strcmp(comando, "rjd") == 0) {

      executa_rjd(&disparadores, &contagem_disparadores, qry->pilha_para_free, qry->arena, carregadores, &contagem_carregadores);

    } else if (strcmp(comando, "calc") == 0) {

      executa_calc(qry->arena, chao);

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
