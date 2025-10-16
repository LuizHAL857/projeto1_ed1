#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H
#include "leitor_arquivos.h"
#include "trata_geo.h"

typedef void *Qry;

Qry executa_comando_qry(DadosDoArquivo dados_qry, DadosDoArquivo dados_geo,
                          Chao chao, const char *caminho_output);

void desaloca_qry(Qry qry);

#endif 