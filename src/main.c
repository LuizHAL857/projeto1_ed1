#include "trata_geo.h"
#include "fila.h"
#include "pilha.h"
#include "leitor_arquivos.h"
#include "circulo.h"
#include "linha.h"
#include "retangulo.h"
#include "texto.h"
#include "text_style.h"
#include "trata_qry.h"
#include "trata_argumentos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {

    // Verifica se há argumentos demais
    if (argc > 10) { 
        printf("Erro, muitos argumentos!\n");
        exit(1);
    }

    // Obtém os caminhos e opções passadas na linha de comando
    char *caminho_output  = obter_valor_opcao(argc, argv, "o");
    char *caminho_geo     = obter_valor_opcao(argc, argv, "f");
    char *caminho_prefixo = obter_valor_opcao(argc, argv, "e");
    char *caminho_qry     = obter_valor_opcao(argc, argv, "q");
    char *comando_sufixo  = obter_sufixo_comando(argc, argv);

    // Variáveis para armazenar os caminhos completos
    char *caminho_completo_geo    = NULL;
    char *caminho_completo_qry    = NULL;

    // Caso o prefixo tenha sido especificado (-e)
    if (caminho_prefixo != NULL) {

        size_t tamanho_prefixo = strlen(caminho_prefixo);
        int falta_barra = (tamanho_prefixo > 0 && caminho_prefixo[tamanho_prefixo - 1] != '/');

        // Monta o caminho completo do arquivo GEO
        if (caminho_geo != NULL) {
            size_t geo_tamanho = strlen(caminho_geo);
            caminho_completo_geo = (char *) malloc(tamanho_prefixo + geo_tamanho + 2);

            if (falta_barra)
                sprintf(caminho_completo_geo, "%s/%s", caminho_prefixo, caminho_geo);
            else
                sprintf(caminho_completo_geo, "%s%s", caminho_prefixo, caminho_geo);

            caminho_geo = caminho_completo_geo;
        }

        

        // Monta o caminho completo do arquivo QRY
        if (caminho_qry != NULL) {
            size_t qry_tamanho = strlen(caminho_qry);
            caminho_completo_qry = (char *) malloc(tamanho_prefixo + qry_tamanho + 2);

            if (falta_barra)
                sprintf(caminho_completo_qry, "%s/%s", caminho_prefixo, caminho_qry);
            else
                sprintf(caminho_completo_qry, "%s%s", caminho_prefixo, caminho_qry);

            caminho_qry = caminho_completo_qry;
        }
    }

    // Verifica se as opções obrigatórias foram passadas (-f e -o)
    if (caminho_geo == NULL || caminho_output == NULL) {
        printf("Erro: -o e -f são obrigatórios\n");
        exit(1);
    }

    // Cria os dados do arquivo GEO
    DadosDoArquivo arqGeo = criar_dados_arquivo(caminho_geo);
    if (arqGeo == NULL) {
        printf("Erro na criação de dados do arquivo GEO\n");
        exit(1);
    }

    // Executa os comandos do arquivo GEO
    Chao chao = executa_comando_geo(arqGeo, caminho_output, comando_sufixo);

    // Caso o arquivo QRY tenha sido passado, processa-o
    if (caminho_qry != NULL) {
        DadosDoArquivo arqQry = criar_dados_arquivo(caminho_qry);

        if (arqQry == NULL) {
            printf("Erro na criação de dados do arquivo QRY\n");
            desaloca_geo(chao);
            exit(1);
        }

        Qry qry = executa_qry(arqQry, arqGeo, chao, caminho_output);

        destruir_dados_arquivo(arqQry);
        desaloca_qry(qry);
    }

    // Libera memória do arquivo GEO
    destruir_dados_arquivo(arqGeo);
    desaloca_geo(chao);

    // Libera caminhos alocados dinamicamente
    if (caminho_completo_geo != NULL)
        free(caminho_completo_geo);

    

    if (caminho_completo_qry != NULL)
        free(caminho_completo_qry);

    return 0;
}
