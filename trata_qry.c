
#include "trata_qry.h"
#include "trata_geo.h"
#include "formas.h"
#include "disparador.h"
#include "fila.h"
#include "pilha.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "text_style.h"
#include "cor_complementar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>



typedef struct {
    Forma forma;
    bool anotado;
    double disp_x, disp_y;
    double x, y; 
} ItemArena;

typedef struct {
    Fila arena;
    
    
    Disparador *disparadores;
    int n_disparadores;
    int cap_disparadores;

    Carregador *carregadores;
    int n_carregadores;
    int cap_carregadores;

    
    Pilha coletor_lixo; 
} ContextoQry;



static Disparador encontrar_disparador(ContextoQry *ctx, int id);
static Carregador encontrar_carregador(ContextoQry *ctx, int id);
static void adicionar_disparador(ContextoQry *ctx, Disparador d);
static void adicionar_carregador(ContextoQry *ctx, Carregador c);
static void registrar_lixo(ContextoQry *ctx, void *ptr);

static void comando_pd(ContextoQry *ctx, char *args);
static void comando_lc(ContextoQry *ctx, Chao chao, char *args, FILE *txt);
static void comando_atch(ContextoQry *ctx, char *args);
static void comando_shft(ContextoQry *ctx, char *args, FILE *txt);
static void comando_dsp(ContextoQry *ctx, char *args, FILE *txt);
static void comando_rjd(ContextoQry *ctx, char *args, FILE *txt);
static void comando_calc(ContextoQry *ctx, Chao chao, DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo, char *caminho_saida, FILE *txt);

static void realizar_disparo(ContextoQry *ctx, Disparador d, double dx, double dy, bool anotar, FILE *txt);
static void criar_svg_resultado(ContextoQry *ctx, Chao chao, DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo, char *caminho_saida, Fila esmagados);
static void desenhar_forma_svg(FILE *svg, Forma f);


Qry executa_qry(DadosDoArquivo dados_qry, DadosDoArquivo dados_geo, Chao chao, char *caminho_saida) {
    ContextoQry *ctx = malloc(sizeof(ContextoQry));
    ctx->arena = criaFila();
    ctx->disparadores = NULL;
    ctx->n_disparadores = 0;
    ctx->cap_disparadores = 0;
    ctx->carregadores = NULL;
    ctx->n_carregadores = 0;
    ctx->cap_carregadores = 0;
    ctx->coletor_lixo = criaPilha();

    // Preparar saída TXT
    char *nome_geo = obter_nome_arquivo(dados_geo);
    char *nome_qry = obter_nome_arquivo(dados_qry);
    
    char base_geo[256], base_qry[256];
    strcpy(base_geo, nome_geo);
    strcpy(base_qry, nome_qry);
    char *ponto = strrchr(base_geo, '.'); if(ponto) *ponto = '\0';
    ponto = strrchr(base_qry, '.'); if(ponto) *ponto = '\0';

    char caminho_txt[1024];
    sprintf(caminho_txt, "%s/%s-%s.txt", caminho_saida, base_geo, base_qry);
    FILE *txt = fopen(caminho_txt, "w");
    if (!txt) {
        perror("Falha ao abrir saída TXT");
        exit(1);
    }

    Fila linhas = obter_fila_linhas(dados_qry);
    while (!filaVazia(linhas)) {
        char *linha = dequeueFila(linhas);
        char *copia_linha = copia_string(linha);
        registrar_lixo(ctx, copia_linha);
        
        char *comando = strtok(copia_linha, " \t\n");
        if (!comando) continue;

        if (strcmp(comando, "pd") == 0) comando_pd(ctx, strtok(NULL, ""));
        else if (strcmp(comando, "lc") == 0) comando_lc(ctx, chao, strtok(NULL, ""), txt);
        else if (strcmp(comando, "atch") == 0) comando_atch(ctx, strtok(NULL, ""));
        else if (strcmp(comando, "shft") == 0) comando_shft(ctx, strtok(NULL, ""), txt);
        else if (strcmp(comando, "dsp") == 0) comando_dsp(ctx, strtok(NULL, ""), txt);
        else if (strcmp(comando, "rjd") == 0) comando_rjd(ctx, strtok(NULL, ""), txt);
        else if (strcmp(comando, "calc") == 0) comando_calc(ctx, chao, dados_qry, dados_geo, caminho_saida, txt);
    }

    fclose(txt);
    return (Qry)ctx;
}

void desaloca_qry(Qry qry) {
    ContextoQry *ctx = (ContextoQry*)qry;
    
    // Liberar Itens da Arena
    while(!filaVazia(ctx->arena)) {
        ItemArena *item = dequeueFila(ctx->arena);
        
        free(item);
    }
    desalocaFila(ctx->arena);

    
    for(int i=0; i<ctx->n_disparadores; i++) {
        desaloca_disparador(ctx->disparadores[i]);
    }
    free(ctx->disparadores);

    
    for(int i=0; i<ctx->n_carregadores; i++) {
        desaloca_carregador(ctx->carregadores[i]);
    }
    free(ctx->carregadores);

   
    while(!pilhaVazia(ctx->coletor_lixo)) {
        free(popPilha(ctx->coletor_lixo));
    }
    desalocaPilha(ctx->coletor_lixo);

    free(ctx);
}


static void comando_pd(ContextoQry *ctx, char *args) {
    int id;
    double x, y;
    sscanf(args, "%d %lf %lf", &id, &x, &y);

    Disparador d = cria_disparador(id, x, y);
    adicionar_disparador(ctx, d);
}

static void comando_lc(ContextoQry *ctx, Chao chao, char *args, FILE *txt) {
    int id, n;
    sscanf(args, "%d %d", &id, &n);
    
    Carregador c = encontrar_carregador(ctx, id);
    if (!c) {
        c = cria_carregador(id);
        adicionar_carregador(ctx, c);
    }

    fprintf(txt, "lc %d %d\n", id, n);
    Fila fila_chao = get_fila_chao(chao);
    
    Pilha p_temp = criaPilha();

    for (int i = 0; i < n; i++) {
        if (filaVazia(fila_chao)) break;
        Forma f = dequeueFila(fila_chao);
        pushPilha(p_temp, f);
        
        // Reportar dados
        fprintf(txt, "  Carregou forma tipo %s ID %d\n", tipo_forma_to_string(get_tipo_forma(f)), 
            (get_tipo_forma(f) == CIRCLE) ? getIDCirculo(get_data_forma(f)) :
            (get_tipo_forma(f) == RECTANGLE) ? getIDRetangulo(get_data_forma(f)) :
            (get_tipo_forma(f) == LINE) ? getIDLinha(get_data_forma(f)) :
            (get_tipo_forma(f) == TEXT) ? getIDTexto(get_data_forma(f)) : -1);
    }

    while (!pilhaVazia(p_temp)) {
        carregador_adiciona_forma(c, popPilha(p_temp));
    }
    desalocaPilha(p_temp);
}

static void comando_atch(ContextoQry *ctx, char *args) {
    int d_id, cesq_id, cdir_id;
    sscanf(args, "%d %d %d", &d_id, &cesq_id, &cdir_id);

    Disparador d = encontrar_disparador(ctx, d_id);
    if (!d) return;

    Carregador cesq = encontrar_carregador(ctx, cesq_id);
    Carregador cdir = encontrar_carregador(ctx, cdir_id);
    
    if (!cesq) {
        cesq = cria_carregador(cesq_id);
        adicionar_carregador(ctx, cesq);
    }
    if (!cdir) {
        cdir = cria_carregador(cdir_id);
        adicionar_carregador(ctx, cdir);
    }
    
    disparador_conecta_carregadores(d, cesq, cdir);
}

static void comando_shft(ContextoQry *ctx, char *args, FILE *txt) {
    int d_id, n;
    char lado[10];
    sscanf(args, "%d %s %d", &d_id, lado, &n);

    Disparador d = encontrar_disparador(ctx, d_id);
    if (!d) return;
    
    fprintf(txt, "shft %d %s %d\n", d_id, lado, n);

    Forma f = disparador_shift(d, lado[0], n);
    
    if (f) {
        fprintf(txt, "  Forma no disparo: Tipo %s\n", tipo_forma_to_string(get_tipo_forma(f)));
    } else {
        fprintf(txt, "  Posicao de disparo vazia\n");
    }
}

static void comando_dsp(ContextoQry *ctx, char *args, FILE *txt) {
    int d_id;
    double dx, dy;
    char anotacao[10] = "i"; 
    sscanf(args, "%d %lf %lf %s", &d_id, &dx, &dy, anotacao);

    Disparador d = encontrar_disparador(ctx, d_id);
    if (!d) return;

    fprintf(txt, "dsp %d %.2f %.2f %s\n", d_id, dx, dy, anotacao);
    realizar_disparo(ctx, d, dx, dy, strcmp(anotacao, "v") == 0, txt);
}

static void comando_rjd(ContextoQry *ctx, char *args, FILE *txt) {
    int d_id;
    char lado[10];
    double dx, dy, ix, iy;
    sscanf(args, "%d %s %lf %lf %lf %lf", &d_id, lado, &dx, &dy, &ix, &iy);

    Disparador d = encontrar_disparador(ctx, d_id);
    if (!d) return;
    
    fprintf(txt, "rjd %d %s ...\n", d_id, lado);

    int k = 0;
    
    while (true) {
        disparador_shift(d, lado[0], 1);
        Forma f = disparador_obter_forma_disparo(d);
        if (!f) break; 
        
        double dx_atual = dx + k * ix;
        double dy_atual = dy + k * iy;
        realizar_disparo(ctx, d, dx_atual, dy_atual, false, txt);
        
        k++;
    }
}

static void comando_calc(ContextoQry *ctx, Chao chao, DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo, char *caminho_saida, FILE *txt) {
    fprintf(txt, "calc\n");
    double area_total_esmagada = 0;
    Fila esmagados = criaFila(); 

    Fila fila_chao = get_fila_chao(chao);
    Pilha pilha_free = obtem_pilha_para_desalocar(chao);

    while (!filaVazia(ctx->arena)) {
        ItemArena *itemI = dequeueFila(ctx->arena);
        if (filaVazia(ctx->arena)) {
            enqueueFila(fila_chao, itemI->forma);
            free(itemI);
            break;
        }
        ItemArena *itemJ = dequeueFila(ctx->arena);

        if (verifica_sobreposicao(itemI->forma, itemJ->forma)) {
            double areaI = calcula_area_forma(itemI->forma);
            double areaJ = calcula_area_forma(itemJ->forma);
            
            if (areaI < areaJ) {
                area_total_esmagada += areaI;
                fprintf(txt, "  Esmagamento: I (area %.2f) < J (area %.2f). I destruido.\n", areaI, areaJ);
                
                double cx, cy;
                obter_posicao_forma(itemI->forma, &cx, &cy);
                double *posicao = malloc(2*sizeof(double));
                posicao[0] = cx; posicao[1] = cy;
                enqueueFila(esmagados, posicao);
                registrar_lixo(ctx, posicao);

                enqueueFila(fila_chao, itemJ->forma);
                
               
                
            } else {
                char *preenchimentoI = obter_cor_preenchimento(itemI->forma);
                definir_cor_borda(itemJ->forma, preenchimentoI);
                free(preenchimentoI);

                enqueueFila(fila_chao, itemI->forma);
                enqueueFila(fila_chao, itemJ->forma);
                
                Forma cloneI = clona_e_troca_cores(itemI->forma);
                if (cloneI) {
                    enqueueFila(fila_chao, cloneI);
                    
                    pushPilha(pilha_free, cloneI);
                }
                
                fprintf(txt, "  Colisao: I (area %.2f) >= J (area %.2f). J modificado, I clonado.\n", areaI, areaJ);
            }
        } else {
            enqueueFila(fila_chao, itemI->forma);
            enqueueFila(fila_chao, itemJ->forma);
        }
        free(itemI);
        free(itemJ);
    }

    fprintf(txt, "  Total area esmagada: %.2f\n", area_total_esmagada);

    criar_svg_resultado(ctx, chao, dadosQry, dadosGeo, caminho_saida, esmagados);
    
    desalocaFila(esmagados);
}


static void realizar_disparo(ContextoQry *ctx, Disparador d, double dx, double dy, bool anotar, FILE *txt) {
    Forma f = disparador_remover_forma_disparo(d);
    if (!f) return;

    double x_disp, y_disp;
    obter_posicao_disparador(d, &x_disp, &y_disp);

    double x_final = x_disp + dx;
    double y_final = y_disp + dy;
    definir_posicao_forma(f, x_final, y_final);

    ItemArena *item = malloc(sizeof(ItemArena));
    item->forma = f;
    item->anotado = anotar;
    item->disp_x = x_disp;
    item->disp_y = y_disp;
    item->x = x_final;
    item->y = y_final;
    
    enqueueFila(ctx->arena, item);

    fprintf(txt, "  Disparo: Pos Final (%.2f, %.2f)\n", x_final, y_final);
}

static void desenhar_forma_svg(FILE *svg, Forma f) {
    TipoForma tipo = get_tipo_forma(f);
    void *dados = get_data_forma(f);
    if (tipo == CIRCLE) {
        CIRCULO c = (CIRCULO)dados;
        fprintf(svg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"%s\" stroke=\"%s\" />\n",
            getXCirculo(c), getYCirculo(c), getRaioCirculo(c), getCorPCirculo(c), getCorBCirculo(c));
    } else if (tipo == RECTANGLE) {
        RETANGULO r = (RETANGULO)dados;
        fprintf(svg, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" fill=\"%s\" stroke=\"%s\" />\n",
            getXRetangulo(r), getYRetangulo(r), getLarguraRetangulo(r), getAlturaRetangulo(r), getCorPRetangulo(r), getCorBRetangulo(r));
    } else if (tipo == LINE) {
        LINHA l = (LINHA)dados;
        fprintf(svg, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" stroke=\"%s\" />\n",
            getX1Linha(l), getY1Linha(l), getX2Linha(l), getY2Linha(l), getCorLinha(l));
    } else if (tipo == TEXT) {
        TEXTO t = (TEXTO)dados;
        fprintf(svg, "<text x=\"%.2f\" y=\"%.2f\" fill=\"%s\" stroke=\"%s\">%s</text>\n",
            getXTexto(t), getYTexto(t), getCorPTexto(t), getCorBTexto(t), getTxtTexto(t));
    }
}

static void criar_svg_resultado(ContextoQry *ctx, Chao chao, DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo, char *caminho_saida, Fila esmagados) {
    char *nome_geo = obter_nome_arquivo(dadosGeo);
    char *nome_qry = obter_nome_arquivo(dadosQry);
    char base_geo[256], base_qry[256];
    strcpy(base_geo, nome_geo);
    strcpy(base_qry, nome_qry);
    char *ponto = strrchr(base_geo, '.'); if(ponto) *ponto = '\0';
    ponto = strrchr(base_qry, '.'); if(ponto) *ponto = '\0';

    char caminho_svg[1024];
    sprintf(caminho_svg, "%s/%s-%s.svg", caminho_saida, base_geo, base_qry);
    FILE *svg = fopen(caminho_svg, "w");
    fprintf(svg, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");

    
    Fila fila_chao = get_fila_chao(chao);
    Fila temp = criaFila();
    while(!filaVazia(fila_chao)) {
        Forma f = dequeueFila(fila_chao);
        desenhar_forma_svg(svg, f);
        enqueueFila(temp, f);
    }
    while(!filaVazia(temp)) enqueueFila(fila_chao, dequeueFila(temp));
    desalocaFila(temp);

    
    for(int i=0; i<ctx->n_disparadores; i++) {
        Disparador d = ctx->disparadores[i];
        Forma f_disp = disparador_obter_forma_disparo(d);
        if (f_disp) desenhar_forma_svg(svg, f_disp);
    }
    
    for(int i=0; i<ctx->n_carregadores; i++) {
        Carregador c = ctx->carregadores[i];
        Pilha p_temp = criaPilha();
        while(!carregador_vazio(c)) {
            Forma f = carregador_remover_forma(c);
            desenhar_forma_svg(svg, f);
            pushPilha(p_temp, f);
        }
        while(!pilhaVazia(p_temp)) {
            carregador_adiciona_forma(c, popPilha(p_temp));
        }
        desalocaPilha(p_temp);
    }

  
    temp = criaFila();
    while(!filaVazia(esmagados)) {
        double *posicao = dequeueFila(esmagados);
        fprintf(svg, "<text x=\"%.2f\" y=\"%.2f\" fill=\"red\" font-size=\"20\">*</text>\n", posicao[0], posicao[1]);
        enqueueFila(temp, posicao);
    }
    while(!filaVazia(temp)) enqueueFila(esmagados, dequeueFila(temp));
    desalocaFila(temp);

    fprintf(svg, "</svg>\n");
    fclose(svg);
}

static Disparador encontrar_disparador(ContextoQry *ctx, int id) {
    for(int i=0; i<ctx->n_disparadores; i++) {
        if(obter_id_disparador(ctx->disparadores[i]) == id) return ctx->disparadores[i];
    }
    return NULL;
}

static Carregador encontrar_carregador(ContextoQry *ctx, int id) {
    for(int i=0; i<ctx->n_carregadores; i++) {
        if(obter_id_carregador(ctx->carregadores[i]) == id) return ctx->carregadores[i];
    }
    return NULL;
}

static void adicionar_disparador(ContextoQry *ctx, Disparador d) {
    ctx->n_disparadores++;
    ctx->disparadores = realloc(ctx->disparadores, ctx->n_disparadores * sizeof(Disparador));
    ctx->disparadores[ctx->n_disparadores-1] = d;
}

static void adicionar_carregador(ContextoQry *ctx, Carregador c) {
    ctx->n_carregadores++;
    ctx->carregadores = realloc(ctx->carregadores, ctx->n_carregadores * sizeof(Carregador));
    ctx->carregadores[ctx->n_carregadores-1] = c;
}

static void registrar_lixo(ContextoQry *ctx, void *ptr) {
    pushPilha(ctx->coletor_lixo, ptr);
}





