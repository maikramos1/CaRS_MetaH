#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <memory.h>
#include "CaRS_MetaH.h"

#define MAX(X,Y)((X > Y) ? (X) : (Y))
#define MIN(X,Y)((X < Y) ? (X) : (Y))

const double TEMPERATURA_INICIAL = 1000.0;
const double TEMPERATURA_CONGELAMENTO = 0.01;
const double TAXA_RESFRIAMENTO = 0.98;
const int SA_MAXIMO = 100;
const int TEMPO_MAXIMO = 100;

int main(void) {
	Solucao s;
	int n = 0;
	const char* entrada[] = {
		//Instâncias Não-Euclideanas:
		//Pequenas
		"Pequenas/Mauritania10n.car", // 0
		"Pequenas/Bolivia10n.car", // 1
		//Médias
		"Medias/AfricaSul11n.car", // 2
		"Medias/Peru13n.car", // 3
		//Grandes
		"Grandes/Brasil16n.car", // 4
		"Grandes/Russia17n.car" // 5
	};
	
	int TEMPO_TOTAL = 0;
	int TEMPO_MELHOR = 0;
	int NUMERO_SOLUCOES = 0;

	lerDados(entrada[n]);
	//testarDados(" ");
	
	/*
	solucao_mauritania(s);
	printf("mauritania");
	escrever_solucao("saida.txt", s);
	
	//printf("bolivia");
	//solucao_bolivia(s);
	gerar_vizinha(s,2);
	calcular_fo(s);
	printf("vizinha");
	escrever_solucao("saida.txt", s);

	heu_cons_ale_gul(s, 0.3);
	printf("gulosa");
	escrever_solucao("saida.txt", s);

	heu_cons_ale(s);
	printf("aleatoria");
	escrever_solucao("saida.txt", s);	
	
	heu_cons_ale_gul(s, 0.3);
	heu_MM(s);
	printf("busca local");
	escrever_solucao("saida.txt", s);
		
	heu_cons_ale_gul(s, 0.3);
	sa(s);
	printf("SA sem busca local");
	escrever_solucao("saida.txt", s);
	*/
	heu_cons_ale_gul(s, 0.3);
	sa(s,TEMPO_TOTAL,TEMPO_MELHOR,NUMERO_SOLUCOES);
	printf("SA com anabolizantes");
	escrever_solucao("saida.txt", s, entrada[n]);
	



	
	//sa(s);
	//escrever_solucao("saida.txt", s);


	return 0;
}

void lerDados(const char* arq) {
	FILE* f = fopen(arq, "r");

	char aux[200];

	if (f == NULL) {
		printf("Erro ao abrir o arquivo: %s\n", arq);
		return;
	}

	// Leitura do nome
	fscanf(f, "%s %s %s", aux, aux, &nomeInstancia);

	// joga fora tipo e comentario
	fscanf(f, " %[^\n] ", aux);
	fscanf(f, " %[^\n] ", aux);


	// Leitura do numero de cidades
	fscanf(f, " %s %s %d", aux, aux, &num_cidades);

	// Leitura do numero de carros
	fscanf(f, "%s %s %d", aux, aux, &num_carros);

	// joga fora tipo, formato e seção das arestas
	fscanf(f, " %[^\n]", aux);
	fscanf(f, " %[^\n]", aux);
	fscanf(f, " %[^\n]", aux);


	for (int i = 0; i < num_carros; i++) {
		fscanf(f, "%d", &mat_distancia[i][0][0]);
		for (int j = 0; j < num_cidades; j++) {
			for (int k = 0; k < num_cidades; k++) {
				fscanf(f, "%d", &mat_distancia[i][j][k]);
			}
		}
	}

	//joga fora seção de retorno
	fscanf(f, " %s", aux);

	for (int i = 0; i < num_carros; i++) {
		fscanf(f, "%d", &mat_retorno[i][0][0]);
		for (int j = 0; j < num_cidades; j++) {
			for (int k = 0; k < num_cidades; k++) {
				fscanf(f, "%d", &mat_retorno[i][j][k]);
			}
		}
	}
	fclose(f);
}

void testarDados(const char* arq) {

	FILE* f;
	if (!strcmp(arq, " ")) f = stdout;
	else f = fopen(arq, "w");

	fprintf(f, "Nome da Instancia: %s\n", nomeInstancia);
	fprintf(f, "Numero de Cidades: %d\n", num_cidades);
	fprintf(f, "Numero de Carros: %d\n", num_carros);
	fprintf(f, "Matriz de Distancias:\n");

	for (int i = 0; i < num_carros; i++) {
		fprintf(f, "Carro %d:\n", i);
		for (int j = 0; j < num_cidades; j++) {
			for (int k = 0; k < num_cidades; k++) {
				fprintf(f, "%3d ", mat_distancia[i][j][k]);
			}
			fprintf(f, "\n");
		}
		fprintf(f, "\n");
	}

	fprintf(f, "Matriz de Retornos:\n");
	for (int i = 0; i < num_carros; i++) {
		fprintf(f, "Carro %d:\n", i);
		for (int j = 0; j < num_cidades; j++) {
			for (int k = 0; k < num_cidades; k++) {
				fprintf(f, "%3d ", mat_retorno[i][j][k]);
			}
			fprintf(f, "\n");
		}
		fprintf(f, "\n");
	}

	if (strcmp(arq, " "))
		fclose(f);
}

void gerar_vizinha(Solucao& s, int flag) {
	if (flag == 1 || flag == 3) {
		int cidade = rand() % num_cidades;

		if (num_carros > 1) { 
			int carro_atual = s.vet_carro[cidade];
			int novo_carro = rand() % num_carros;
			while (novo_carro == carro_atual) {
				novo_carro = rand() % num_carros;
			}
			s.vet_carro[cidade] = novo_carro;
		}
	}

	else if (flag == 2 || flag == 3) {

		int u = rand() % num_cidades;
		int prev_u = -1;
		for (int k = 0; k < num_cidades; k++) {
			if (s.vet_sol[k] == u) {
				prev_u = k;
				break;
			}
		}

		int v = rand() % num_cidades;
		while (v == u || v == prev_u) {
			v = rand() % num_cidades;
		}

		int next_u = s.vet_sol[u]; 
		int next_v = s.vet_sol[v]; 

		s.vet_sol[prev_u] = next_u;
		s.vet_sol[v] = u;
		s.vet_sol[u] = next_v;
	}
}

void calcular_fo(Solucao& s) {
	s.fo = 0;

	//distancia
	for (int j = 0; j < num_cidades; j++) {
		s.fo += mat_distancia[s.vet_carro[j]][j][s.vet_sol[j]];
	}

	int primeiro_no_ciclo = 0;
	int troca_encontrada = 0;
	
	int atual_temp = 0;
	for (int k = 0; k < num_cidades; k++) {
		int proximo = s.vet_sol[atual_temp];
		if (s.vet_carro[atual_temp] != s.vet_carro[proximo]) {
			primeiro_no_ciclo = proximo; 
			troca_encontrada = 1;
			break;
		}
		atual_temp = proximo;
	}

	int cidade_atual = primeiro_no_ciclo;
	int cidade_onde_pegou = primeiro_no_ciclo; // Onde o carro atual foi alugado
	int conta = 0;

	while (conta < num_cidades) {
		int proxima_cidade = s.vet_sol[cidade_atual];
		int carro_atual = s.vet_carro[cidade_atual];
		int carro_proximo = s.vet_carro[proxima_cidade];
		
		if (carro_atual != carro_proximo) {

			//retorno quando tiver troca de carro
			s.fo += mat_retorno[carro_atual][proxima_cidade][cidade_onde_pegou];

			// RESTRIÇÃO
			if (proxima_cidade % 2 != 0) {
				s.fo += 10000; // Penalidade alta para inviabilizar a solução
			}

			cidade_onde_pegou = proxima_cidade;
		}

		cidade_atual = proxima_cidade;
		conta++;
	}
	if (conta < num_cidades) {
		s.fo += 1000000; // Penalidade alta para inviabilizar a solução
	}
};

void escrever_solucao(const char* arq_solucao, Solucao s, const char* string) {
		FILE* f;
		FILE* g;
		f = stdout;
		g = fopen(arq_solucao, "w");

		fprintf(f, "\n//------------------------------\n");
		fprintf(f, "  SOLUCAO DO PROBLEMA CaRS %s\n", string);
		fprintf(f, "//------------------------------\n\n");
		fprintf(g, "//------------------------------\n");
		fprintf(g, "  SOLUCAO DO PROBLEMA CaRS %s\n", string);
		fprintf(g, "//------------------------------\n\n");
		fprintf(f, "Valor da Funcao Objetivo: %d\n\n", s.fo);
		fprintf(g, "Valor da Funcao Objetivo: %d\n\n", s.fo);
		fprintf(f, "CaRS: ");
		fprintf(g, "CaRS: ");

		int count = 0;
		int cidade_atual = 0;
		while (count < num_cidades) {

			int idx = -1;
			for (int i = 0; i < num_cidades; i++) {          
				if (i == cidade_atual) {
					idx = i;
					break;
				}
			}

			if (idx == -1) {
				fprintf(f, "[ERRO] Nao achei aresta saindo da cidade %d\n", cidade_atual);
				fprintf(g, "[ERRO] Nao achei aresta saindo da cidade %d\n", cidade_atual);
				break;
			}

			int destino = s.vet_sol[idx];
			int carro = s.vet_carro[idx];

			if (destino != 0) {
				fprintf(f, "%d -[%d]-> ", cidade_atual, carro);
				fprintf(g, "%d -[%d]-> ", cidade_atual, carro);
			}
			else {
				fprintf(f, "%d -[%d]-> %d ", cidade_atual, carro, destino);
				fprintf(g, "%d -[%d]-> %d ", cidade_atual, carro, destino);
			}

			cidade_atual = destino;
			count++;

			if (cidade_atual == 0) break;
		}

		fprintf(f, "\n\n\n");
		

		fclose(g);
};

void solucao_mauritania(Solucao& s) {
	//mauritania
	//fo:622
	memset(&s, 0, sizeof(Solucao));
	s.fo = 0;
	//mauritania
	s.vet_sol[0] = 3;
	s.vet_carro[0] = 0;
	
	s.vet_sol[3] = 4;
	s.vet_carro[3] = 0;

	s.vet_sol[4] = 1;
	s.vet_carro[4] = 0;

	s.vet_sol[1] = 5;
	s.vet_carro[1] = 0;

	s.vet_sol[5] = 2;
	s.vet_carro[5] = 0;

	s.vet_sol[2] = 6;
	s.vet_carro[2] = 0;

	s.vet_sol[6] = 9;
	s.vet_carro[6] = 1;

	s.vet_sol[9] = 7;
	s.vet_carro[9] = 1;

	s.vet_sol[7] = 8;
	s.vet_carro[7] = 1;

	s.vet_sol[8] = 0;
	s.vet_carro[8] = 1;

	calcular_fo(s);
};

void solucao_bolivia(Solucao& s) {
	memset(&s, 0, sizeof(Solucao));
	s.fo = 0;

	//bolivia 825
	s.vet_sol[0] = 1;
	s.vet_carro[0] = 0;

	s.vet_sol[1] = 3;
	s.vet_carro[1] = 0;

	s.vet_sol[3] = 2;
	s.vet_carro[3] = 0;

	s.vet_sol[2] = 8;
	s.vet_carro[2] = 1;

	s.vet_sol[8] = 9;
	s.vet_carro[8] = 1;

	s.vet_sol[9] = 4;
	s.vet_carro[9] = 1;

	s.vet_sol[4] = 6;
	s.vet_carro[4] = 2;

	s.vet_sol[6] = 5;
	s.vet_carro[6] = 2;

	s.vet_sol[5] = 7;
	s.vet_carro[5] = 2;

	s.vet_sol[7] = 0;
	s.vet_carro[7] = 2;
	calcular_fo(s);
};

//esse aqui fiz no gepeteco, nada fazia funcionar
void heu_MM(Solucao& s) {
	int melhorou = 1;
	Solucao melhor_vizinho;

	int ant[MAX_CID];

	calcular_fo(s);

	while (melhorou) {
		melhorou = 0;
		int melhor_fo_iteracao = s.fo;
		memcpy(&melhor_vizinho, &s, sizeof(Solucao));

		for (int k = 0; k < num_cidades; k++) {
			ant[s.vet_sol[k]] = k;
		}

		for (int i = 0; i < num_cidades - 1; i++) {
			for (int j = i + 1; j < num_cidades; j++) {
				int u = i;
				int v = j;

				int prev_u = ant[u];
				int next_u = s.vet_sol[u];
				int prev_v = ant[v];
				int next_v = s.vet_sol[v];

				if (next_u == v) {
					s.vet_sol[prev_u] = v;
					s.vet_sol[v] = u;
					s.vet_sol[u] = next_v;
					
				}
				else if (next_v == u) {
					s.vet_sol[prev_v] = u;
					s.vet_sol[u] = v;
					s.vet_sol[v] = next_u;
				}
				else {
					s.vet_sol[prev_u] = v;
					s.vet_sol[v] = next_u;
					s.vet_sol[prev_v] = u;
					s.vet_sol[u] = next_v;
				}

				calcular_fo(s);
				if (s.fo < melhor_fo_iteracao) {
					melhor_fo_iteracao = s.fo;
					memcpy(&melhor_vizinho, &s, sizeof(Solucao));
					melhorou = 1;
				}
				if (next_u == v) {
					s.vet_sol[prev_u] = u;
					s.vet_sol[u] = v;
					s.vet_sol[v] = next_v;
				}
				else if (next_v == u) {
					s.vet_sol[prev_v] = v;
					s.vet_sol[v] = u;
					s.vet_sol[u] = next_u;
				}
				else {
					s.vet_sol[prev_u] = u;
					s.vet_sol[u] = next_u;
					s.vet_sol[prev_v] = v;
					s.vet_sol[v] = next_v;
				}
			}
		}

		// Restaura o valor de FO correto da solução atual antes de tentar carros
		s.fo = melhor_fo_iteracao;
		// Se não houve melhora na troca de cidades, garante que a struct 's' está íntegra
		if (!melhorou) calcular_fo(s);

		// --- VIZINHANÇA 2: TROCA DE CARROS ---
		for (int j = 0; j < num_cidades; j++) {

			if (j % 2 != 0) continue; // Pula os pares (não pode trocar neles)

			int carro_original = s.vet_carro[j];

			for (int k = 0; k < num_carros; k++) {
				if (k == carro_original) continue;

				s.vet_carro[j] = k;

				calcular_fo(s);

				if (s.fo < melhor_fo_iteracao) {
					melhor_fo_iteracao = s.fo;
					memcpy(&melhor_vizinho, &s, sizeof(Solucao));
					melhorou = 1;
				}
				// Undo
				s.vet_carro[j] = carro_original;
			}
		}

		if (melhorou) {
			memcpy(&s, &melhor_vizinho, sizeof(Solucao));
		}
	}
	calcular_fo(s);
}

void sa(Solucao& s, int TEMPO_TOTAL, int TEMPO_MELHOR, int NUMERO_SOLUCOES) {
	// inicia
	Solucao s_melhor;
	Solucao s_vizinha;
	memset(&s_melhor, 0, sizeof(Solucao));
	memset(&s_vizinha, 0, sizeof(Solucao));
	memcpy(&s_melhor, &s, sizeof(Solucao));

	double T = TEMPERATURA_INICIAL;

	while (T > TEMPERATURA_CONGELAMENTO) {

		for (int i = 0; i < SA_MAXIMO; i++) {
			memset(&s_vizinha, 0, sizeof(Solucao));
			memcpy(&s_vizinha, &s, sizeof(Solucao));

			// pedra, papel ou tesoura
			gerar_vizinha(s_vizinha, rand() % 3);
			//====================================================================================================
			// Injeção de anabolizante para o SA
			//====================================================================================================
			heu_MM(s_vizinha);
			//====================================================================================================
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			//====================================================================================================
			calcular_fo(s_vizinha);
			double delta = s_vizinha.fo - s.fo;

			if (delta < 0) {
				// aceita
				memcpy(&s, &s_vizinha, sizeof(Solucao));

				if (s.fo < s_melhor.fo) {
					memcpy(&s_melhor, &s, sizeof(Solucao));
					//nova melhor solucao
					//printf("FO: %d\n", s_melhor.fo);
					//escrever_solucao("saida.txt", s_melhor);
				}
			}
			else {
				// Aceita piora, probabilidade e^(-delta/T)
				double prob = exp(-delta / T);
				double r = ((double)rand() / (RAND_MAX));

				if (r < prob) {
					memcpy(&s, &s_vizinha, sizeof(Solucao));
				}
			}
		}

		// fica frio ai
		T *= TAXA_RESFRIAMENTO;
	}

	// escreve a melhor que achou
	memcpy(&s, &s_melhor, sizeof(Solucao));
}

void heu_cons_ale_gul(Solucao& s, const double percentual) {
	int visitado[MAX_CID];
	int cand_id[MAX_CID];
	int cand_custo[MAX_CID];
	int i, j, k, cont_lista, limite, pos_escolhida;
	int cidade_atual, prox_cidade, carro_atual, aux_custo, aux_id;
	int cidade_inicio;

	memset(visitado, 0, sizeof(visitado));

	cidade_inicio = rand() % num_cidades;
	cidade_atual = cidade_inicio;
	carro_atual = rand() % num_carros;
	visitado[cidade_inicio] = 1;

	for (i = 0; i < num_cidades - 1; i++) {
		s.vet_carro[cidade_atual] = carro_atual;

		cont_lista = 0;
		for (j = 0; j < num_cidades; j++) {
			if (visitado[j] == 0) {
				cand_id[cont_lista] = j;
				cand_custo[cont_lista] = mat_distancia[carro_atual][cidade_atual][j];
				cont_lista++;
			}
		}

		for (j = 0; j < cont_lista - 1; j++) {
			for (k = 0; k < cont_lista - j - 1; k++) {
				if (cand_custo[k] > cand_custo[k + 1]) {
					// custo
					aux_custo = cand_custo[k];
					cand_custo[k] = cand_custo[k + 1];
					cand_custo[k + 1] = aux_custo;

					// id
					aux_id = cand_id[k];
					cand_id[k] = cand_id[k + 1];
					cand_id[k + 1] = aux_id;
				}
			}
		}

		limite = (int)((percentual / 100.0) * cont_lista);
		if (limite < 1) limite = 1;
		pos_escolhida = rand() % limite;
		prox_cidade = cand_id[pos_escolhida];
		s.vet_sol[cidade_atual] = prox_cidade;
		visitado[prox_cidade] = 1;
 
		if (prox_cidade % 2 == 0) {
			if ((rand() % 100) < 50) { 
				int novo_carro = rand() % num_carros;
				if (num_carros > 1) {
					while (novo_carro == carro_atual) {
						novo_carro = rand() % num_carros;
					}
				}
				carro_atual = novo_carro;
			}
		}
		cidade_atual = prox_cidade;
	}

	s.vet_sol[cidade_atual] = cidade_inicio;
	s.vet_carro[cidade_atual] = carro_atual;

	calcular_fo(s);
}

void heu_cons_ale(Solucao& s) {
	int perm[MAX_CID];
	int i, r, temp;

	memset(&s, 0, sizeof(s));

	for (i = 0; i < num_cidades; i++) {
		perm[i] = i;
	}

	for (i = 0; i < num_cidades; i++) {
		r = rand() % num_cidades;
		temp = perm[i];
		perm[i] = perm[r];
		perm[r] = temp;
	}

	for (i = 0; i < num_cidades - 1; i++) {
		int origem = perm[i];
		int destino = perm[i + 1];
		s.vet_sol[origem] = destino;
		s.vet_carro[origem] = rand() % num_carros;
	}

	int ultima = perm[num_cidades - 1];
	int primeira = perm[0];

	s.vet_sol[ultima] = primeira;
	s.vet_carro[ultima] = rand() % num_carros;

	calcular_fo(s);
}