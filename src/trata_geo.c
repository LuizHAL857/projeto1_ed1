#include "trata_geo.h"
#include "fila.h"
#include "pilha.h"
#include "leitor_arquivos.h"
#include "circulo.h"
#include "linha.h"
#include "retangulo.h"
#include "texto.h"
#include "text_style.h"
#include <stdio.h>
#include <string.h>
#include "formas.h"



typedef struct 
{
    Fila fila_formas;
    Pilha pilha_para_free;
    Fila fila_svg;

}Chao_t;

typedef struct 
{

    TipoForma tipo;
    void* data;


}Forma_t;

static void executa_comando_retangulo(Chao_t *chao);
static void executa_comando_circulo(Chao_t *chao);
static void executa_comando_linha(Chao_t *chao);
static void executa_comando_texto(Chao_t *chao);
static void executa_comando_textstyle(Chao_t *chao);
static void cria_fila_svg(Chao_t *chao,  char* caminho_output, DadosDoArquivo fileData, const char *sufixo_comando);

Chao executa_comando_geo(DadosDoArquivo fileData,  char *caminho_output, const char *sufixo_comando){


    Chao_t *chao = malloc(sizeof(Chao_t));

    if (chao == NULL){

        printf("Erro de alocação");
        exit(1);

    } 

    chao->fila_formas = criaFila();
    chao->pilha_para_free = criaPilha();
    chao->fila_svg = criaFila();

    while(!filaVazia(obter_fila_linhas(fileData))){

        char *linha = (char*)dequeueFila(obter_fila_linhas(fileData));
        char* comando = strtok(linha, " ");

        if(strcmp(comando, "c") == 0) {

            executa_comando_circulo(chao);

        }else if(strcmp(comando, "r") == 0) {

            executa_comando_retangulo(chao);

        }else if(strcmp(comando, "l") == 0) {

            executa_comando_linha(chao);
        }else if(strcmp(comando, "t") == 0) {

            executa_comando_texto(chao);

        }else if(strcmp(comando, "ts") == 0) {

            executa_comando_textstyle(chao);

        }else {

            printf("comando inválido");

        }

    }

    cria_fila_svg(chao, caminho_output, fileData, sufixo_comando);
    return chao;

}

Fila get_fila_chao(Chao chao){

    Chao_t *chao_t = (Chao_t *)chao;

    return chao_t->fila_formas;
}



void desaloca_geo(Chao chao){

    Chao_t* chao_t = (Chao_t *)chao;
    desalocaFila(chao_t->fila_formas);
    desalocaFila(chao_t->fila_svg);

    while(!pilhaVazia(chao_t->pilha_para_free)){

        Forma_t  *forma = popPilha(chao_t->pilha_para_free);

        
        switch(forma->tipo){
       
            case CIRCLE:   desalocaCirculo(forma->data);
            break;
            
            case RECTANGLE:  desalocarRetangulo(forma->data);
            break;

            case LINE: desalocaLinha(forma->data);
            break;

            case TEXT: desalocaTexto(forma->data);
            break;

            case TEXT_STYLE: desalocaTextStyle(forma->data);
            break;

          }
          free(forma);

    }
    desalocaPilha(chao_t->pilha_para_free);
    free(chao);

}

static void executa_comando_circulo(Chao_t *chao){


    char *id = strtok(NULL, " ");
    char *X = strtok(NULL, " ");
    char *Y = strtok(NULL, " ");
    char *raio = strtok(NULL, " ");
    char *corB = strtok(NULL, " ");
    char *corP = strtok(NULL, " ");
  
    CIRCULO c = criaCirculo(atoi(id), atof(X), atof(Y),
                                  atof(raio), corP, corB);
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = CIRCLE;
    forma->data = c;
    enqueueFila(chao->fila_formas, forma);
    pushPilha(chao->pilha_para_free, forma);
    enqueueFila(chao->fila_svg, forma);
}


static void executa_comando_retangulo(Chao_t *chao) {
    char *id = strtok(NULL, " ");
    char *X = strtok(NULL, " ");
    char *Y = strtok(NULL, " ");
    char *largura = strtok(NULL, " ");
    char *altura = strtok(NULL, " ");
    char *corB = strtok(NULL, " ");
    char *corP = strtok(NULL, " ");
  
    RETANGULO r =
        criaRetangulo(atoi(id), atof(X), atof(Y), atof(altura),
                         atof(largura), corB, corP);
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = RECTANGLE;
    forma->data = r;
    enqueueFila(chao->fila_formas, forma);
    pushPilha(chao->pilha_para_free, forma);
    enqueueFila(chao->fila_svg, forma);
  }

  static void executa_comando_linha(Chao_t *chao) {
    char *id = strtok(NULL, " ");
    char *x1 = strtok(NULL, " ");
    char *y1 = strtok(NULL, " ");
    char *x2 = strtok(NULL, " ");
    char *y2 = strtok(NULL, " ");
    char *cor = strtok(NULL, " ");
  
    LINHA l = criaLinha(atoi(id), atof(x1), atof(y1), atof(x2),
                            atof(y2), cor);
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = LINE;
    forma->data = l;
    enqueueFila(chao->fila_formas, forma);
    pushPilha(chao->pilha_para_free, forma);
    enqueueFila(chao->fila_svg, forma);
  }

  static void executa_comando_texto(Chao_t *chao) {
    char *id = strtok(NULL, " ");
    char *X = strtok(NULL, " ");
    char *Y = strtok(NULL, " ");
    char *corB = strtok(NULL, " ");
    char *corP = strtok(NULL, " ");
    char *ancora = strtok(NULL, " ");
    char *txt = strtok(NULL, "");
  
    TEXTO t = criaTexto(atoi(id), atof(X), atof(Y),
                                corB, corP, *ancora, txt);
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = TEXT;
    forma->data = t;
    enqueueFila(chao->fila_formas, forma);
    pushPilha(chao->pilha_para_free, forma);
    enqueueFila(chao->fila_svg, forma);
  }



  static void executa_comando_textstyle(Chao_t *chao) {
    char *ff = strtok(NULL, " ");
    char *fw = strtok(NULL, " ");
    char *fs = strtok(NULL, " ");
  
    TEXTSTYLE ts =
        criaTextStyle(ff, fw, atoi(fs));
  
    Forma_t *forma = malloc(sizeof(Forma_t));
    if (forma == NULL) {
      printf("Erro de alocação\n");
      exit(1);
    }
    forma->tipo = TEXT_STYLE;
    forma->data = ts;
    enqueueFila(chao->fila_formas, forma);
    pushPilha(chao->pilha_para_free, forma);
    enqueueFila(chao->fila_svg, forma);

  }



static void cria_fila_svg(Chao_t *chao, char* caminho_output, DadosDoArquivo fileData, const char *sufixo_comando){
    const char *nome_arquivo_original = obter_nome_arquivo(fileData);
    size_t name_len = strlen(nome_arquivo_original);
    char *nome_arquivo = malloc(name_len + 1);
    
    
    if (nome_arquivo == NULL) {
        printf("Erro de alocação\n");
        return;
    }

    strcpy(nome_arquivo, nome_arquivo_original);
    strtok(nome_arquivo, ".");

    if (sufixo_comando != NULL) {
    strcat(nome_arquivo, "-");
    strcat(nome_arquivo, sufixo_comando);
    }


   
    size_t caminho_len = strlen(caminho_output);
    size_t nome_processado_len = strlen(nome_arquivo);
    size_t total_len = caminho_len + 1 + nome_processado_len + 4 +
    1; 

 
    char *caminho_output_arquivo = malloc(total_len);
    if (caminho_output_arquivo == NULL) {
    printf("Erro de alocaçõa\n");
    return;
    }

    // Use snprintf for safe string construction
    int result = snprintf(caminho_output_arquivo, total_len, "%s/%s.svg",
    caminho_output, nome_arquivo);
    if (result < 0 || (size_t)result >= total_len) {
    printf("Erro no caminho\n");
    free(caminho_output_arquivo);
    return;
    }

    FILE *file = fopen(caminho_output_arquivo, "w");
    if (file == NULL) {
    printf("Error: Failed to open file: %s\n", caminho_output_arquivo);
    free(caminho_output_arquivo);
    return;
    }
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(
    file,
    "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");
    while (!filaVazia(chao->fila_svg)) {
    Forma_t *forma = dequeueFila(chao->fila_svg);
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
    getCorBRetangulo(r),
    getCorPRetangulo(r));


    } else if (forma->tipo == LINE) {
    LINHA l = (LINHA)forma->data;
    fprintf(file,
    "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
    getX1Linha(l), getY1Linha(l), getX2Linha(l),
    getY2Linha(l), getCorLinha(l));


    } else if (forma->tipo == TEXT) {
    TEXTO t = (TEXTO)forma->data;
    char ancora = getAncoraTexto(t);
    const char *texto_ancora = "start"; // default

  
    if (ancora == 'm' || ancora == 'M') {
    texto_ancora = "middle";

    } else if (ancora == 'f' || ancora == 'F') {
    texto_ancora = "end";


    } else if (ancora == 'i' || ancora == 'I') {
    texto_ancora = "start";
    }

    fprintf(file,
    "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' "
    "text-anchor='%s'>%s</text>\n",
    getXTexto(t), getYTexto(t), getCorPTexto(t),
    getCorBTexto(t), texto_ancora, getTxtTexto(t));
    }
    }
    }
    fprintf(file, "</svg>\n");
    fclose(file);
    free(caminho_output_arquivo);
    free(nome_arquivo);
}



