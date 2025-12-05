#include "trata_qry.h"
#include "lista.h"
#include "leitor_arquivos.h"
#include "forma.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "anteparo.h"
#include "visibilidade.h"
#include "poligono.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


typedef struct {
    Cidade cidade;
    DadosDoArquivo fileData;
    Lista comandos_executados;
    int maior_id_atual;
    char* caminho_output;
    FILE* txt_file;
    char tipo_sort;
    int threshold;
    Lista visibility_polygons; // Lista de polígonos de visibilidade para renderizar no SVG
} Qry_t;

// Declarações forward das funções estáticas
static void executa_comando_anteparo(Qry_t *qry, char *linha);
static void executa_comando_destruicao(Qry_t *qry, char *linha);
static void executa_comando_pintura(Qry_t *qry, char *linha);
static void executa_comando_clonagem(Qry_t *qry, char *linha);
static void cria_svg_qry(Qry_t *qry, DadosDoArquivo fileData);



Qry executa_comando_qry(DadosDoArquivo fileData, Cidade cidade, 
                        char *caminho_output, int maior_id_inicial, char tipo_sort, int threshold) {
    
    Qry_t *qry = malloc(sizeof(Qry_t));
    
    if (qry == NULL) {
        printf("Erro de alocação para Qry\n");
        exit(1);
    }
    
    qry->cidade = cidade;
    qry->fileData = fileData;
    qry->comandos_executados = criaLista();
    qry->maior_id_atual = maior_id_inicial;
    qry->tipo_sort = tipo_sort;
    qry->threshold = threshold;
    qry->visibility_polygons = criaLista(); // Inicializa lista de polígonos de visibilidade
    
    // Copia o caminho de output
    qry->caminho_output = malloc(strlen(caminho_output) + 1);
    if (qry->caminho_output == NULL) {
        printf("Erro de alocação para caminho_output\n");
        exit(1);
    }
    strcpy(qry->caminho_output, caminho_output);
    
    // Abre arquivo TXT de saída
    char *nome_orig = obter_nome_arquivo(fileData);
    char *nome_base = strrchr(nome_orig, '/');
    if(nome_base) nome_base++; else nome_base = nome_orig;
    
    char *nome_arq = malloc(strlen(nome_base) + 5);
    if(nome_arq) {
        strcpy(nome_arq, nome_base);
        char *dot = strrchr(nome_arq, '.');
        if(dot) *dot = '\0';
        strcat(nome_arq, ".txt");
        
        char *path = malloc(strlen(caminho_output) + strlen(nome_arq) + 2);
        if(path) {
            sprintf(path, "%s/%s", caminho_output, nome_arq);
            qry->txt_file = fopen(path, "w");
            if (qry->txt_file == NULL) {
                printf("Erro ao criar arquivo TXT: %s\n", path);
            }
            free(path);
        }
        free(nome_arq);
    } else {
        qry->txt_file = NULL;
    }
    
    // Processa cada linha do arquivo .qry
    while (!listaVazia(obter_lista_linhas(fileData))) {
        char *linha = (char*)removeInicioLista(obter_lista_linhas(fileData));
        char *linha_copia = malloc(strlen(linha) + 1);
        
        if (linha_copia == NULL) {
            printf("Erro de alocação para linha_copia\n");
            exit(1);
        }
        strcpy(linha_copia, linha);
        
        char *comando = strtok(linha, " ");
        
        if (comando == NULL) {
            free(linha_copia);
            continue;
        }
        
        printf("Processando comando: %s\n", comando);
        
        if (strcmp(comando, "a") == 0) {
            executa_comando_anteparo(qry, linha_copia);
            
        } else if (strcmp(comando, "d") == 0) {
            executa_comando_destruicao(qry, linha_copia);
            
        } else if (strcmp(comando, "p") == 0) {
            executa_comando_pintura(qry, linha_copia);
            
        } else if (strcmp(comando, "cln") == 0) {
            executa_comando_clonagem(qry, linha_copia);
            
        } else {
            printf("Comando inválido: %s\n", comando);
        }
        
        free(linha_copia);
    }
    
    
    cria_svg_qry(qry, fileData);
    
    return qry;
}


static void executa_comando_anteparo(Qry_t *qry, char *linha) {
    strtok(linha, " ");
    char *id_inicio = strtok(NULL, " ");
    char *id_fim = strtok(NULL, " ");
    char *orientacao = strtok(NULL, " \n");
    
    if (id_inicio == NULL || id_fim == NULL || orientacao == NULL) {
        printf("Erro: comando 'a' com parâmetros inválidos\n");
        return;
    }
    
    int i = atoi(id_inicio);
    int j = atoi(id_fim);
    char h_ou_v = orientacao[0];
    
    
    
    Lista lista_formas = get_lista_cidade(qry->cidade);
    Lista lista_svg = get_lista_svg_cidade(qry->cidade);
    Lista lista_free = obtem_lista_para_desalocar(qry->cidade);
    
    Lista to_remove = criaLista();
    Lista to_add = criaLista();
    
    Celula aux = getInicioLista(lista_formas);
    while(aux != NULL) {
        Forma f = getConteudoCelula(aux);
        tipo_forma tipo = getTipoForma(f);
        void* data = getDataForma(f);
        int id = -1;
        
        switch(tipo) {
            case CIRCLE: id = getIDCirculo(data); break;
            case RECTANGLE: id = getIDRetangulo(data); break;
            case LINE: id = getIDLinha(data); break;
            case TEXT: id = getIDTexto(data); break;
            default: break;
        }
        
        if (id >= i && id <= j) {
          
            if (tipo == RECTANGLE) {
                Anteparo anteparos[4];
                transforma_retangulo_em_anteparos(f, &qry->maior_id_atual, anteparos);
                
                if (qry->txt_file) fprintf(qry->txt_file, "Retangulo ID %d transformado em anteparos:\n", id);
                
                for(int k=0; k<4; k++) {
                    if (anteparos[k] != NULL) {
                        Forma novo = criaForma(ANTEPARO, anteparos[k]);
                        insereFinalLista(to_add, novo);
                        
                        if (qry->txt_file) {
                            fprintf(qry->txt_file, "  Anteparo ID %d: (%.2f, %.2f) -> (%.2f, %.2f)\n", 
                                getIDAnteparo(anteparos[k]), 
                                getX1Anteparo(anteparos[k]), getY1Anteparo(anteparos[k]),
                                getX2Anteparo(anteparos[k]), getY2Anteparo(anteparos[k]));
                        }
                    }
                }
            } else {
                Anteparo a = transforma_em_anteparo(f, h_ou_v, ++qry->maior_id_atual);
                if (a != NULL) {
                    Forma novo = criaForma(ANTEPARO, a);
                    insereFinalLista(to_add, novo);
                    
                    if (qry->txt_file) {
                        char *tipo_str = (tipo==CIRCLE?"Circulo":(tipo==LINE?"Linha":"Texto"));
                        fprintf(qry->txt_file, "%s ID %d transformado em anteparo ID %d: (%.2f, %.2f) -> (%.2f, %.2f)\n", 
                            tipo_str, id,
                            getIDAnteparo(a), getX1Anteparo(a), getY1Anteparo(a), 
                            getX2Anteparo(a), getY2Anteparo(a));
                    }
                }
            }
            insereFinalLista(to_remove, f);
        }
        aux = getProxCelula(aux);
    }
    
    
    while(!listaVazia(to_remove)) {
        Forma f = removeInicioLista(to_remove);
        removeElementoLista(lista_formas, f);
        removeElementoLista(lista_svg, f);
        removeElementoLista(lista_free, f);
        desalocaForma(f);
    }
    liberaLista(to_remove);
    
   
    while(!listaVazia(to_add)) {
        Forma f = removeInicioLista(to_add);
        insereFinalLista(lista_formas, f);
        insereFinalLista(lista_svg, f);
        insereFinalLista(lista_free, f);
    }
    liberaLista(to_add);
}


static BoundingBox getBBForma(Forma f) {
    BoundingBox bb = {INFINITY, INFINITY, -INFINITY, -INFINITY};
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    
    switch(tipo) {
        case CIRCLE: {
            float x = getXCirculo(data);
            float y = getYCirculo(data);
            float r = getRaioCirculo(data);
            bb.min_x = x - r; bb.max_x = x + r;
            bb.min_y = y - r; bb.max_y = y + r;
            break;
        }
        case RECTANGLE: {
            float x = getXRetangulo(data);
            float y = getYRetangulo(data);
            float w = getLarguraRetangulo(data);
            float h = getAlturaRetangulo(data);
            bb.min_x = x; bb.max_x = x + w;
            bb.min_y = y; bb.max_y = y + h;
            break;
        }
        case LINE: {
            float x1 = getX1Linha(data);
            float y1 = getY1Linha(data);
            float x2 = getX2Linha(data);
            float y2 = getY2Linha(data);
            bb.min_x = fmin(x1, x2); bb.max_x = fmax(x1, x2);
            bb.min_y = fmin(y1, y2); bb.max_y = fmax(y1, y2);
            break;
        }
        case TEXT: {
            float x = getXTexto(data);
            float y = getYTexto(data);
            bb.min_x = x; bb.max_x = x + 10; 
            bb.min_y = y; bb.max_y = y + 10;
            break;
        }
        case ANTEPARO: {
            float x1 = getX1Anteparo(data);
            float y1 = getY1Anteparo(data);
            float x2 = getX2Anteparo(data);
            float y2 = getY2Anteparo(data);
            bb.min_x = fmin(x1, x2); bb.max_x = fmax(x1, x2);
            bb.min_y = fmin(y1, y2); bb.max_y = fmax(y1, y2);
            break;
        }
        default: break;
    }
    return bb;
}

static bool formaEstaDentro(Poligono p, Forma f) {
    Ponto pt;
    tipo_forma tipo = getTipoForma(f);
    void* data = getDataForma(f);
    
    switch(tipo) {
        case CIRCLE: {
            // Para círculos, testa o centro e pontos ao redor da circunferência
            float cx = getXCirculo(data);
            float cy = getYCirculo(data);
            float r = getRaioCirculo(data);
            
            // Testa o centro
            pt.x = cx;
            pt.y = cy;
            if (isInside(p, pt)) return true;
            
            // Testa 8 pontos ao redor da circunferência
            for (int i = 0; i < 8; i++) {
                float angulo = (2.0 * M_PI * i) / 8.0;
                pt.x = cx + r * cos(angulo);
                pt.y = cy + r * sin(angulo);
                if (isInside(p, pt)) return true;
            }
            return false;
        }
            
        case RECTANGLE: {
            // Para retângulos, testa os 4 cantos e o centro
            float x = getXRetangulo(data);
            float y = getYRetangulo(data);
            float w = getLarguraRetangulo(data);
            float h = getAlturaRetangulo(data);
            
            // Testa os 4 cantos
            Ponto cantos[4] = {
                {x, y},
                {x + w, y},
                {x, y + h},
                {x + w, y + h}
            };
            
            for (int i = 0; i < 4; i++) {
                if (isInside(p, cantos[i])) return true;
            }
            
            // Testa o centro
            pt.x = x + w/2;
            pt.y = y + h/2;
            return isInside(p, pt);
        }
            
        case LINE: {
            // Para linhas, testa vários pontos ao longo do segmento
            float x1 = getX1Linha(data);
            float y1 = getY1Linha(data);
            float x2 = getX2Linha(data);
            float y2 = getY2Linha(data);
            
            // Testa 5 pontos ao longo da linha
            for (int i = 0; i <= 4; i++) {
                float t = i / 4.0f;
                pt.x = x1 + t * (x2 - x1);
                pt.y = y1 + t * (y2 - y1);
                if (isInside(p, pt)) return true;
            }
            return false;
        }
            
        case TEXT: {
            // Para texto, testa o ponto de âncora
            pt.x = getXTexto(data);
            pt.y = getYTexto(data);
            return isInside(p, pt);
        }
            
        case ANTEPARO: {
            // Para anteparos, verifica múltiplos pontos ao longo do segmento
            // Isso garante que anteparos na borda do polígono sejam detectados
            Ponto p1 = {getX1Anteparo(data), getY1Anteparo(data)};
            Ponto p2 = {getX2Anteparo(data), getY2Anteparo(data)};
            
            // Testa vários pontos ao longo do segmento
            for (int i = 0; i <= 4; i++) {
                float t = i / 4.0f;
                Ponto pt = {
                    p1.x + t * (p2.x - p1.x),
                    p1.y + t * (p2.y - p1.y)
                };
                
                if (isInside(p, pt)) {
                    return true;
                }
                
                // Testa pontos deslocados para ambos os lados do segmento
                // (útil para anteparos exatamente na borda)
                float dx = p2.y - p1.y;  // perpendicular ao segmento
                float dy = -(p2.x - p1.x);
                float len = sqrt(dx*dx + dy*dy);
                if (len > 0.001f) {
                    dx /= len;
                    dy /= len;
                    
                    // Testa deslocamento de 1.0 unidade para um lado
                    Ponto pt_offset1 = {pt.x + dx * 1.0f, pt.y + dy * 1.0f};
                    if (isInside(p, pt_offset1)) {
                        return true;
                    }
                    
                    // Testa deslocamento para o outro lado
                    Ponto pt_offset2 = {pt.x - dx * 1.0f, pt.y - dy * 1.0f};
                    if (isInside(p, pt_offset2)) {
                        return true;
                    }
                }
            }
            
            return false;
        }
            
        default:
            return false;
    }
}


static Poligono calculaPoligonoVisibilidade(ContextoVisibilidade ctx, float x, float y) {
    int num_raios = 72; 
    Poligono regiao_visibilidade = criaPoligono();
    
    if (!regiao_visibilidade) return NULL;

    for (int i = 0; i < num_raios; i++) {
        float angulo = -M_PI + (2.0 * M_PI * i / num_raios);
        float dx = cos(angulo);
        float dy = sin(angulo);
        
        float min_dist = 0.0f;
        float max_dist = 10000.0f;
        float dist = max_dist;
        
        float px_far = x + dx * max_dist;
        float py_far = y + dy * max_dist;
        
        if (!pontoVisivel(ctx, px_far, py_far)) {
            
            for (int iter = 0; iter < 32; iter++) { 
                float mid_dist = (min_dist + max_dist) / 2.0f;
                float px_mid = x + dx * mid_dist;
                float py_mid = y + dy * mid_dist;
                
                if (pontoVisivel(ctx, px_mid, py_mid)) {
                    min_dist = mid_dist;
                } else {
                    max_dist = mid_dist;
                }
            }
            dist = min_dist;
        }
        
        Ponto p = {x + dx * dist, y + dy * dist};
        insereVertice(regiao_visibilidade, p);
    }
    
    return regiao_visibilidade;
}

static void destroiFormasEmColisao(Lista lista_formas, Poligono regiao_visibilidade, Qry_t *qry) {
    Lista lista_svg = get_lista_svg_cidade(qry->cidade);
    Lista lista_free = obtem_lista_para_desalocar(qry->cidade);
    BoundingBox bb_poly = getBoundingBox(regiao_visibilidade);
    
    // Expande BB do polígono com uma margem de tolerância para garantir 
    // que as paredes que delimitam a visibilidade sejam capturadas
    float margem = 1.0f;
    bb_poly.min_x -= margem;
    bb_poly.max_x += margem;
    bb_poly.min_y -= margem;
    bb_poly.max_y += margem;
    
    Lista formas_para_destruir = criaLista();
    
    // Coleta formas a destruir
    for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
        Forma f = getConteudoCelula(c);
        BoundingBox bb_forma = getBBForma(f);
        
        // Teste rápido: Bounding Box
        if (haInterseccaoBB(bb_poly, bb_forma)) {
            // Teste preciso: verifica se o centro da forma está dentro do polígono
            if (formaEstaDentro(regiao_visibilidade, f)) {
                insereFinalLista(formas_para_destruir, f);
            }
        }
    }
    
    // Destrói as formas coletadas
    int count = 0;
    while (!listaVazia(formas_para_destruir)) {
        Forma f = removeInicioLista(formas_para_destruir);
        tipo_forma tipo = getTipoForma(f);
        void* data = getDataForma(f);
        
        int id = -1;
        char *tipo_str = "Desconhecido";
        
        switch(tipo) {
            case CIRCLE: id = getIDCirculo(data); tipo_str = "Circulo"; break;
            case RECTANGLE: id = getIDRetangulo(data); tipo_str = "Retangulo"; break;
            case LINE: id = getIDLinha(data); tipo_str = "Linha"; break;
            case TEXT: id = getIDTexto(data); tipo_str = "Texto"; break;
            case ANTEPARO: id = getIDAnteparo(data); tipo_str = "Anteparo"; break;
            default: break;
        }
        
        if (qry->txt_file && id != -1) {
            fprintf(qry->txt_file, "  Destruído: %s ID %d\n", tipo_str, id);
        }
        
        removeElementoLista(lista_formas, f);
        removeElementoLista(lista_svg, f);
        removeElementoLista(lista_free, f);
        desalocaForma(f);
        count++;
    }
    
    liberaLista(formas_para_destruir);
    
    if (qry->txt_file) {
        fprintf(qry->txt_file, "Total de formas destruídas: %d\n", count);
    }
}

static void geraSVGVisibilidade(Poligono regiao_visibilidade, float x, float y, char* sufixo, Qry_t *qry) {
    if (strcmp(sufixo, "-") == 0) {
        // Adiciona o polígono de visibilidade à lista para renderizar no SVG principal
        // Cria uma estrutura para armazenar o polígono e a posição da bomba
        typedef struct {
            Poligono poligono;
            float bomb_x;
            float bomb_y;
        } VisibilityData;
        
        VisibilityData* vis_data = malloc(sizeof(VisibilityData));
        if (vis_data) {
            vis_data->poligono = regiao_visibilidade;
            vis_data->bomb_x = x;
            vis_data->bomb_y = y;
            insereFinalLista(qry->visibility_polygons, vis_data);
        }
        return;
    }

    char *nome_orig = obter_nome_arquivo(qry->fileData);
    char *nome_base = strrchr(nome_orig, '/');
    if(nome_base) nome_base++; else nome_base = nome_orig;
    
    char *nome_arq = malloc(strlen(nome_base) + strlen(sufixo) + 10);
    if (!nome_arq) return;

    strcpy(nome_arq, nome_base);
    char *dot = strrchr(nome_arq, '.');
    if(dot) *dot = '\0';
    
    strcat(nome_arq, "-");
    strcat(nome_arq, sufixo);
    strcat(nome_arq, "-vis.svg");
    
    char *path = malloc(strlen(qry->caminho_output) + strlen(nome_arq) + 2);
    if (path) {
        sprintf(path, "%s/%s", qry->caminho_output, nome_arq);
        
        FILE *svg_file = fopen(path, "w");
        if (svg_file) {
            // Calcula o Bounding Box do polígono para definir o viewBox
            BoundingBox bb = getBoundingBox(regiao_visibilidade);
            
            // Adiciona margem ao viewBox
            float margem = 50.0f;
            float vb_x = bb.min_x - margem;
            float vb_y = bb.min_y - margem;
            float vb_w = (bb.max_x - bb.min_x) + 2*margem;
            float vb_h = (bb.max_y - bb.min_y) + 2*margem;
            
            fprintf(svg_file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            fprintf(svg_file, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%.2f %.2f %.2f %.2f\">\n",
                    vb_x, vb_y, vb_w, vb_h);
            
            fprintf(svg_file, "<polygon points=\"");
            Lista vertices = getVertices(regiao_visibilidade);
            for (Celula c = getInicioLista(vertices); c; c = getProxCelula(c)) {
                Ponto* p = (Ponto*)getConteudoCelula(c);
                fprintf(svg_file, "%.2f,%.2f ", p->x, p->y);
            }
            fprintf(svg_file, "\" fill=\"rgba(255,200,0,0.3)\" stroke=\"orange\" stroke-width=\"2\"/>\n");
            fprintf(svg_file, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"5\" fill=\"red\"/>\n", x, y);
            fprintf(svg_file, "</svg>\n");
            fclose(svg_file);
        }
        free(path);
    }
    free(nome_arq);
}

static void executa_comando_destruicao(Qry_t *qry, char *linha) {
    strtok(linha, " ");
    char *x_str = strtok(NULL, " ");
    char *y_str = strtok(NULL, " ");
    char *sufixo = strtok(NULL, " \n");
    
    if (x_str == NULL || y_str == NULL || sufixo == NULL) {
        printf("Erro: comando 'd' com parâmetros inválidos\n");
        return;
    }
    
    float x = atof(x_str);
    float y = atof(y_str);
    
    printf("  Comando DESTRUIÇÃO: x=%.2f, y=%.2f, sufixo=%s\n", x, y, sufixo);
    
    // 1. Criar contexto de visibilidade
    Lista lista_formas = get_lista_cidade(qry->cidade);
    ContextoVisibilidade ctx = criaContextoVisibilidade(x, y, lista_formas, 
                                                        qry->tipo_sort, qry->threshold);
    
    if (!ctx) {
        printf("Erro ao criar contexto de visibilidade\n");
        return;
    }
    
    // 2. Calcular polígono de visibilidade
    Poligono regiao_visibilidade = calculaPoligonoVisibilidade(ctx, x, y);
    
    if (regiao_visibilidade) {
        // 3. Identificar e destruir formas dentro do polígono
        if (qry->txt_file) {
            fprintf(qry->txt_file, "\nBomba de destruição em (%.2f, %.2f):\n", x, y);
        }
        
        destroiFormasEmColisao(lista_formas, regiao_visibilidade, qry);
        
        // 4. Gerar SVG da região de visibilidade
        geraSVGVisibilidade(regiao_visibilidade, x, y, sufixo, qry);
        
        // Só libera o polígono se não foi armazenado para renderização posterior
        if (strcmp(sufixo, "-") != 0) {
            liberaPoligono(regiao_visibilidade);
        }
    }
    
    // Libera contexto de visibilidade
    liberaContextoVisibilidade(ctx);
}



static void executa_comando_pintura(Qry_t *qry, char *linha) {
    strtok(linha, " ");
    char *x_str = strtok(NULL, " ");
    char *y_str = strtok(NULL, " ");
    char *cor = strtok(NULL, " ");
    char *sufixo = strtok(NULL, " \n");
    
    if (x_str == NULL || y_str == NULL || cor == NULL || sufixo == NULL) {
        printf("Erro: comando 'p' com parâmetros inválidos\n");
        return;
    }
    
    float x = atof(x_str);
    float y = atof(y_str);
    
    printf("  Comando PINTURA: x=%.2f, y=%.2f, cor=%s, sufixo=%s\n", x, y, cor, sufixo);
    
    // 1. Criar contexto de visibilidade
    Lista lista_formas = get_lista_cidade(qry->cidade);
    ContextoVisibilidade ctx = criaContextoVisibilidade(x, y, lista_formas, 
                                                        qry->tipo_sort, qry->threshold);
    
    if (!ctx) {
        printf("Erro ao criar contexto de visibilidade\n");
        return;
    }
    
    // 2. Calcular polígono de visibilidade
    Poligono regiao_visibilidade = calculaPoligonoVisibilidade(ctx, x, y);
    
    if (regiao_visibilidade) {
        // 3. Identificar e pintar formas dentro do polígono
        if (qry->txt_file) {
            fprintf(qry->txt_file, "\nBomba de pintura em (%.2f, %.2f) com cor %s:\n", x, y, cor);
        }
        
        BoundingBox bb_poly = getBoundingBox(regiao_visibilidade);
        
        // Expande BB com margem de tolerância
        float margem = 1.0f;
        bb_poly.min_x -= margem;
        bb_poly.max_x += margem;
        bb_poly.min_y -= margem;
        bb_poly.max_y += margem;
        
        int count = 0;
        for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
            Forma f = getConteudoCelula(c);
            BoundingBox bb_forma = getBBForma(f);
            
            // Teste rápido: Bounding Box
            if (haInterseccaoBB(bb_poly, bb_forma)) {
                // Teste preciso: verifica se a forma está dentro do polígono
                if (formaEstaDentro(regiao_visibilidade, f)) {
                    tipo_forma tipo = getTipoForma(f);
                    void* data = getDataForma(f);
                    int id = -1;
                    char *tipo_str = "Desconhecido";
                    
                    // Altera as cores da forma
                    switch(tipo) {
                        case CIRCLE:
                            id = getIDCirculo(data);
                            tipo_str = "Circulo";
                            setCorPCirculo(data, cor);
                            setCorBCirculo(data, cor);
                            break;
                        case RECTANGLE:
                            id = getIDRetangulo(data);
                            tipo_str = "Retangulo";
                            setCorPRetangulo(data, cor);
                            setCorBRetangulo(data, cor);
                            break;
                        case LINE:
                            id = getIDLinha(data);
                            tipo_str = "Linha";
                            setCorLinha(data, cor);
                            break;
                        case TEXT:
                            id = getIDTexto(data);
                            tipo_str = "Texto";
                            setCorPTexto(data, cor);
                            setCorBTexto(data, cor);
                            break;
                        case ANTEPARO:
                            id = getIDAnteparo(data);
                            tipo_str = "Anteparo";
                            setCorAnteparo(data, cor);
                            break;
                        default:
                            break;
                    }
                    
                    if (qry->txt_file && id != -1) {
                        fprintf(qry->txt_file, "  Pintado: %s ID %d\n", tipo_str, id);
                    }
                    count++;
                }
            }
        }
        
        if (qry->txt_file) {
            fprintf(qry->txt_file, "Total de formas pintadas: %d\n", count);
        }
        
        // 4. Gerar SVG da região de visibilidade
        geraSVGVisibilidade(regiao_visibilidade, x, y, sufixo, qry);
        
        // Só libera o polígono se não foi armazenado para renderização posterior
        if (strcmp(sufixo, "-") != 0) {
            liberaPoligono(regiao_visibilidade);
        }
    }
    
    // Libera contexto de visibilidade
    liberaContextoVisibilidade(ctx);
}


static void executa_comando_clonagem(Qry_t *qry, char *linha) {
    strtok(linha, " ");
    char *x_str = strtok(NULL, " ");
    char *y_str = strtok(NULL, " ");
    char *dx_str = strtok(NULL, " ");
    char *dy_str = strtok(NULL, " ");
    char *sufixo = strtok(NULL, " \n");
    
    if (x_str == NULL || y_str == NULL || dx_str == NULL || 
        dy_str == NULL || sufixo == NULL) {
        printf("Erro: comando 'cln' com parâmetros inválidos\n");
        return;
    }
    
    float x = atof(x_str);
    float y = atof(y_str);
    float dx = atof(dx_str);
    float dy = atof(dy_str);
    
    printf("  Comando CLONAGEM: x=%.2f, y=%.2f, dx=%.2f, dy=%.2f, sufixo=%s\n", 
           x, y, dx, dy, sufixo);
    
    // 1. Criar contexto de visibilidade
    Lista lista_formas = get_lista_cidade(qry->cidade);
    Lista lista_svg = get_lista_svg_cidade(qry->cidade);
    Lista lista_free = obtem_lista_para_desalocar(qry->cidade);
    
    ContextoVisibilidade ctx = criaContextoVisibilidade(x, y, lista_formas, 
                                                        qry->tipo_sort, qry->threshold);
    
    if (!ctx) {
        printf("Erro ao criar contexto de visibilidade\n");
        return;
    }
    
    // 2. Calcular polígono de visibilidade
    Poligono regiao_visibilidade = calculaPoligonoVisibilidade(ctx, x, y);
    
    if (regiao_visibilidade) {
        // 3. Identificar formas visíveis e cloná-las
        if (qry->txt_file) {
            fprintf(qry->txt_file, "\nBomba de clonagem em (%.2f, %.2f) com deslocamento (%.2f, %.2f):\n", 
                    x, y, dx, dy);
        }
        
        BoundingBox bb_poly = getBoundingBox(regiao_visibilidade);
        
        // Expande BB com margem de tolerância
        float margem = 1.0f;
        bb_poly.min_x -= margem;
        bb_poly.max_x += margem;
        bb_poly.min_y -= margem;
        bb_poly.max_y += margem;
        
        Lista clones = criaLista();
        int count = 0;
        
        for (Celula c = getInicioLista(lista_formas); c; c = getProxCelula(c)) {
            Forma f = getConteudoCelula(c);
            BoundingBox bb_forma = getBBForma(f);
            
            // Teste rápido: Bounding Box
            if (haInterseccaoBB(bb_poly, bb_forma)) {
                // Teste preciso: verifica se a forma está dentro do polígono
                if (formaEstaDentro(regiao_visibilidade, f)) {
                    tipo_forma tipo = getTipoForma(f);
                    void* data = getDataForma(f);
                    int id_original = -1;
                    int id_clone = ++qry->maior_id_atual;
                    char *tipo_str = "Desconhecido";
                    void* clone_data = NULL;
                    
                    // Cria clone usando as funções de clonagem
                    switch(tipo) {
                        case CIRCLE:
                            id_original = getIDCirculo(data);
                            tipo_str = "Circulo";
                            clone_data = clonaCirculo(data, id_clone, dx, dy);
                            break;
                        case RECTANGLE:
                            id_original = getIDRetangulo(data);
                            tipo_str = "Retangulo";
                            clone_data = clonaRetangulo(data, id_clone, dx, dy);
                            break;
                        case LINE:
                            id_original = getIDLinha(data);
                            tipo_str = "Linha";
                            clone_data = clonaLinha(data, id_clone, dx, dy);
                            break;
                        case TEXT:
                            id_original = getIDTexto(data);
                            tipo_str = "Texto";
                            clone_data = clonaTexto(data, id_clone, dx, dy);
                            break;
                        case ANTEPARO:
                            id_original = getIDAnteparo(data);
                            tipo_str = "Anteparo";
                            clone_data = clonaAnteparo(data, id_clone, dx, dy);
                            break;
                        default:
                            break;
                    }
                    
                    if (clone_data != NULL) {
                        Forma clone_forma = criaForma(tipo, clone_data);
                        insereFinalLista(clones, clone_forma);
                        
                        if (qry->txt_file && id_original != -1) {
                            fprintf(qry->txt_file, "  Clonado: %s ID %d -> Clone ID %d\n", 
                                    tipo_str, id_original, id_clone);
                        }
                        count++;
                    }
                }
            }
        }
        
        // Adiciona os clones às listas da cidade
        while (!listaVazia(clones)) {
            Forma clone = removeInicioLista(clones);
            insereFinalLista(lista_formas, clone);
            insereFinalLista(lista_svg, clone);
            insereFinalLista(lista_free, clone);
        }
        liberaLista(clones);
        
        if (qry->txt_file) {
            fprintf(qry->txt_file, "Total de formas clonadas: %d\n", count);
        }
        
        // 4. Gerar SVG da região de visibilidade
        geraSVGVisibilidade(regiao_visibilidade, x, y, sufixo, qry);
        
        // Só libera o polígono se não foi armazenado para renderização posterior
        if (strcmp(sufixo, "-") != 0) {
            liberaPoligono(regiao_visibilidade);
        }
    }
    
    // Libera contexto de visibilidade
    liberaContextoVisibilidade(ctx);
}


/**
 * @brief Cria o arquivo SVG com o estado final da cidade após processamento do .qry
 */
static void cria_svg_qry(Qry_t *qry, DadosDoArquivo fileData) {
    // Extrai o nome base do arquivo
    char *nome_orig = obter_nome_arquivo(fileData);
    char *nome_base = strrchr(nome_orig, '/');
    if(nome_base) nome_base++; else nome_base = nome_orig;
    
    // Cria nome do arquivo SVG
    size_t name_len = strlen(nome_base);
    char *nome_arquivo = malloc(name_len + 1);
    if (nome_arquivo == NULL) {
        printf("Erro de alocação\n");
        return;
    }
    
    strcpy(nome_arquivo, nome_base);
    char *dot = strrchr(nome_arquivo, '.');
    if (dot) *dot = '\0';
    
    // Monta o caminho completo do arquivo SVG
    size_t caminho_len = strlen(qry->caminho_output);
    size_t nome_len_final = strlen(nome_arquivo);
    size_t total_len = caminho_len + 1 + nome_len_final + 5; 
    
    char *caminho_output_arquivo = malloc(total_len);
    if (caminho_output_arquivo == NULL) {
        printf("Erro de alocação\n");
        free(nome_arquivo);
        return;
    }
    
    snprintf(caminho_output_arquivo, total_len, "%s/%s.svg", qry->caminho_output, nome_arquivo);
    
    
    FILE *file = fopen(caminho_output_arquivo, "w");
    if (file == NULL) {
        printf("Erro ao criar arquivo SVG: %s\n", caminho_output_arquivo);
        free(caminho_output_arquivo);
        free(nome_arquivo);
        return;
    }
    
   
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");
    
   
    Lista lista_svg_temp = criaLista();
    
   
    Lista lista_formas = get_lista_cidade(qry->cidade);
    Celula aux = getInicioLista(lista_formas);
    while (aux != NULL) {
        Forma f = getConteudoCelula(aux);
        insereFinalLista(lista_svg_temp, f);
        aux = getProxCelula(aux);
    }
    
    
    while (!listaVazia(lista_svg_temp)) {
        Forma forma = removeFinalLista(lista_svg_temp);
        if (forma != NULL) {
            tipo_forma tipo = getTipoForma(forma);
            void* data = getDataForma(forma);
            
            if (tipo == CIRCLE) {
                CIRCULO c = (CIRCULO)data;
                fprintf(file, "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
                    getXCirculo(c), getYCirculo(c), getRaioCirculo(c), 
                    getCorPCirculo(c), getCorBCirculo(c));
                    
            } else if (tipo == RECTANGLE) {
                RETANGULO r = (RETANGULO)data;
                fprintf(file, "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' stroke='%s'/>\n",
                    getXRetangulo(r), getYRetangulo(r), getLarguraRetangulo(r), 
                    getAlturaRetangulo(r), getCorPRetangulo(r), getCorBRetangulo(r));
                    
            } else if (tipo == LINE) {
                LINHA l = (LINHA)data;
                fprintf(file, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                    getX1Linha(l), getY1Linha(l), getX2Linha(l), getY2Linha(l), getCorLinha(l));
                    
            } else if (tipo == TEXT) {
                TEXTO t = (TEXTO)data;
                char ancora = getAncoraTexto(t);
                char *texto_ancora = "start";
                
                if (ancora == 'm' || ancora == 'M') {
                    texto_ancora = "middle";
                } else if (ancora == 'f' || ancora == 'F') {
                    texto_ancora = "end";
                } else if (ancora == 'i' || ancora == 'I') {
                    texto_ancora = "start";
                }
                
                fprintf(file, "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' text-anchor='%s'>%s</text>\n",
                    getXTexto(t), getYTexto(t), getCorPTexto(t), 
                    getCorBTexto(t), texto_ancora, getTxtTexto(t));
                    
            } else if (tipo == ANTEPARO) {
                Anteparo a = (Anteparo)data;
                fprintf(file, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                    getX1Anteparo(a), getY1Anteparo(a), getX2Anteparo(a), 
                    getY2Anteparo(a), getCorAnteparo(a));
            }
        }
    }
    
    // Renderiza polígonos de visibilidade (se houver)
    if (!listaVazia(qry->visibility_polygons)) {
        typedef struct {
            Poligono poligono;
            float bomb_x;
            float bomb_y;
        } VisibilityData;
        
        for (Celula c = getInicioLista(qry->visibility_polygons); c; c = getProxCelula(c)) {
            VisibilityData* vis_data = (VisibilityData*)getConteudoCelula(c);
            
            fprintf(file, "<!-- Região de visibilidade da bomba em (%.2f, %.2f) -->\n", 
                    vis_data->bomb_x, vis_data->bomb_y);
            fprintf(file, "<g id=\"visibility-region\" opacity=\"0.5\">\n");
            fprintf(file, "  <polygon points=\"");
            
            Lista vertices = getVertices(vis_data->poligono);
            for (Celula v = getInicioLista(vertices); v; v = getProxCelula(v)) {
                Ponto* p = (Ponto*)getConteudoCelula(v);
                fprintf(file, "%.2f,%.2f ", p->x, p->y);
            }
            
            fprintf(file, "\" fill=\"rgba(255,200,0,0.3)\" stroke=\"orange\" stroke-width=\"2\"/>\n");
            fprintf(file, "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"5\" fill=\"red\"/>\n", 
                    vis_data->bomb_x, vis_data->bomb_y);
            fprintf(file, "</g>\n");
        }
    }
    
    // Fecha o SVG
    fprintf(file, "</svg>\n");
    fclose(file);
    
    // Libera memória
    liberaLista(lista_svg_temp);
    free(caminho_output_arquivo);
    free(nome_arquivo);
}


void desaloca_qry(Qry qry) {
    if (qry == NULL) {
        return;
    }
    
    Qry_t *qry_t = (Qry_t *)qry;
    
    if (qry_t->comandos_executados != NULL) {
        liberaLista(qry_t->comandos_executados);
    }
    
    // Libera lista de polígonos de visibilidade e seus conteúdos
    if (qry_t->visibility_polygons != NULL) {
        // Libera cada polígono armazenado
        while (!listaVazia(qry_t->visibility_polygons)) {
            typedef struct {
                Poligono poligono;
                float bomb_x;
                float bomb_y;
            } VisibilityData;
            
            VisibilityData* vis_data = (VisibilityData*)removeInicioLista(qry_t->visibility_polygons);
            if (vis_data != NULL) {
                if (vis_data->poligono != NULL) {
                    liberaPoligono(vis_data->poligono);
                }
                free(vis_data);
            }
        }
        liberaLista(qry_t->visibility_polygons);
    }
    
    if (qry_t->caminho_output != NULL) {
        free(qry_t->caminho_output);
    }
    
    if (qry_t->txt_file != NULL) {
        fclose(qry_t->txt_file);
    }
    
    free(qry_t);
}
