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
#include "trata_qry.h"
#include "trata_argumentos.h"
#include <stdlib.h>


int main(int argc, char *argv[]) {

 
  if (argc > 10) { 
    printf("Erro: muitos argumentos\n");
    exit(1);
  }

  

   char *caminho_output = obter_valor_opcao(argc, argv, "o");
   char *caminho_geo = obter_valor_opcao(argc, argv, "f");
   char *caminho_qry = obter_valor_opcao(argc, argv, "q");
   char *sufixo_comando = obter_sufixo_comando(argc, argv);

  
  if (caminho_geo == NULL || caminho_output == NULL) {
    printf("Erro: -f e -o são necessários\n");
    exit(1);
  }

  DadosDoArquivo arq_geo = criar_dados_arquivo(caminho_geo);
  if (arq_geo == NULL) {
    printf("Erro na criação dos dados do arquivo geo\n");
    exit(1);
  }


  Chao chao = executa_comando_geo(arq_geo, caminho_output, sufixo_comando);



  if (caminho_qry != NULL) {
    DadosDoArquivo arq_qry = criar_dados_arquivo(caminho_qry);
    if (arq_qry == NULL) {
     
      desaloca_geo(chao);
      exit(1);
    }
   
    Qry qry = executa_qry(arq_qry, arq_geo, chao, caminho_output);
    
    destruir_dados_arquivo(arq_qry);
    desaloca_qry(qry);
   
  }
 
  destruir_dados_arquivo(arq_geo);
  desaloca_geo(chao);

  return 0;
}