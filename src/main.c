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
#include "trata_argumentos.h"



int main(int argc, char *argv[]) {

    if (argc > 10) { 
      printf("Erro: Muitos argumentos\n");
      exit(1);
    }
  
   
     char *caminho_output = obter_valor_opcao(argc, argv, "o");
    const char *caminho_input_geo = obter_valor_opcao(argc, argv, "f");
    //const char *caminho_input_qry = obter_valor_opcao(argc, argv, "q");
    const char *comando_sufixo = obter_sufixo_comando(argc, argv);
  
    
    if (caminho_input_geo == NULL || caminho_output == NULL) {
      printf("Erro: -f e -o são necessários\n");
      exit(1);
    }typedef void *Chao;
  
   
    Chao executa_comando_geo(DadosDoArquivo fileData,  char *caminho_output,
                                const char *comando_sufixo);
    
    
    void desaloca_geo(Chao chao);
  
    DadosDoArquivo arquivo_geo = criar_dados_arquivo(caminho_input_geo);
    if (arquivo_geo == NULL) {
      printf("Erro na criação dos dados do arquivo\n");
      exit(1);
    }
  
    Chao chao = executa_comando_geo(arquivo_geo, caminho_output, comando_sufixo);
    
    
  
    destruir_dados_arquivo(arquivo_geo);
    desaloca_geo(chao);
  
    return 0;
  }