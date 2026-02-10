#pragma once
#ifndef CaRS_MetaH_h
#define CaRS_MetaH_h

#define MAX_CID 20
#define MAX_CAR 10
#define MAX_NOME 200

char nomeInstancia[MAX_NOME];
int num_cidades;
int num_carros;
int mat_distancia[MAX_CAR][MAX_CID][MAX_CID];
int mat_retorno[MAX_CAR][MAX_CID][MAX_CID];

typedef struct tSolucao
{
    int vet_sol[MAX_CID]; //cidade destino
	int vet_carro[MAX_CID]; //qual carro leva ate o destino
    int fo;
}Solucao;

void lerDados(const char* arq);
void testarDados(const char* arq);
void solucao_mauritania(Solucao& s);
void solucao_bolivia(Solucao& s);
void gerar_vizinha(Solucao& s, int flag);
void calcular_fo(Solucao& s);
void escrever_solucao(const char* arq_solucao, Solucao s, const char* string, double TEMPO_TOTAL, double TEMPO_MELHOR, int NUMERO_SOLUCOES);
void heu_MM(Solucao& s);
void sa(Solucao& s,	double TEMPERATURA_INICIAL, double TEMPERATURA_CONGELAMENTO, double TAXA_RESFRIAMENTO, int SA_MAXIMO, double TEMPO_MAXIMO, double &TEMPO_TOTAL, double &TEMPO_MELHOR, int &NUMERO_SOLUCOES);
void heu_cons_ale_gul(Solucao& s, const double percentual);
void heu_cons_ale(Solucao& s);
#endif 