#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <memory.h>
#include "CaRS_MetaH.h"

#define MAX(X,Y)((X > Y) ? (X) : (Y))
#define MIN(X,Y)((X < Y) ? (X) : (Y))

const double TEMPERATURA_INICIAL = 1000.0;
const double TEMPERATURA_FINAL = 0.01;
const double ALPHA = 0.98; // Taxa de resfriamento
const int ITER_POR_TEMP = 100;

int main(void) {

	Solucao s;

	const char* entrada[] = {
		//Instâncias Não-Euclideanas:
		//Pequenas
		"Pequenas/Mauritania10n.car",
		"Pequenas/Bolivia10n.car",
		//Médias
		"Medias/AfricaSul11n.car",
		"Medias/Peru13n.car",
		//Grandes
		"Grandes/Brasil16n.car",
		"Grandes/Russia17n.car"
	};

	lerDados(entrada[1]);

	//testarDados(" ");
	
	//solucao_mauritania(s);
	//printf("mauritania");
	//escrever_solucao("saida.txt", s);

	printf("bolivia");
	solucao_bolivia(s);
	//gerar_vizinha(s,rand()%3);
	calcular_fo(s);
	//original
	escrever_solucao("saida.txt", s);
	
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
	if (flag == 1 || 3) {
		//troca carro
		int cidade = rand() % num_cidades;
		int carro = rand() % num_carros;
		while (carro == s.vet_carro[cidade]) {
			carro = rand() % num_carros;
		}
		s.vet_carro[cidade] = carro;
	}
	else if (flag == 2 || flag == 3){
		
		//troca cidade
		//fui burro e agora vou ter que empurrar o vetor inteiro
		int origem = rand() % num_cidades;
		int destino = rand() % num_cidades;

		while (origem == destino) {
			destino = rand() % num_cidades;
		}

		int cidade_movida = s.vet_sol[origem];

		// Desloca para a esquerda <<<<<
		if (origem < destino) for (int i = origem; i < destino; i++) s.vet_sol[i] = s.vet_sol[i + 1];

		// Desloca para a direita >>>>>
		else for (int i = origem; i > destino; i--) s.vet_sol[i] = s.vet_sol[i - 1];
		
		s.vet_sol[destino] = cidade_movida;
		
	}
};

void calcular_fo(Solucao& s) {
	
	s.fo = 0;

	//distancia
	for (int j = 0; j < num_cidades; j++) {
		s.fo += mat_distancia[s.vet_carro[j]][j][s.vet_sol[j]];
	}

	int primeiro_no_ciclo = 0;
	bool troca_encontrada = false;

	
	int atual_temp = 0;
	for (int k = 0; k < num_cidades; k++) {
		int proximo = s.vet_sol[atual_temp];
		if (s.vet_carro[atual_temp] != s.vet_carro[proximo]) {
			primeiro_no_ciclo = proximo; 
			troca_encontrada = true;
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
				s.fo += 1000000; // Penalidade alta para inviabilizar a solução
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

void escrever_solucao(const char* arq_solucao, Solucao s) {
		FILE* f;
		FILE* g;
		f = stdout;
		g = fopen(arq_solucao, "w");

		fprintf(f, "\n//------------------------------\n");
		fprintf(f, "  SOLUCAO DO PROBLEMA CaRS\n");
		fprintf(f, "//------------------------------\n\n");
		fprintf(g, "//------------------------------\n");
		fprintf(g, "  SOLUCAO DO PROBLEMA CaRS\n");
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
	/*
	for (int i = 0; i < num_cidades-1; i++) {
		s.vet_sol[i] = i+1;
		s.vet_carro[i] = 0;
	}
	s.vet_sol[num_cidades-1] = 0;
	s.vet_carro[num_cidades - 1] = 0;
	*/

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
	/*
	for (int i = 0; i < num_cidades-1; i++) {
		s.vet_sol[i] = i+1;
		s.vet_carro[i] = 0;
	}
	s.vet_sol[num_cidades-1] = 0;
	s.vet_carro[num_cidades - 1] = 0;
	*/
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

void heu_MM(Solucao& s) {
	int melhorou = 1;
	Solucao melhor_vizinho;
	Solucao vizinho;
	memcpy(&melhor_vizinho, &s, sizeof(Solucao));
	memcpy(&vizinho, &s, sizeof(Solucao));

	while (melhorou) {
		melhorou = 0;
		int melhor_fo_local = s.fo;

		// troca todas as cidades
		for (int i = 0; i < num_cidades - 1; i++) {
			for (int j = i + 1; j < num_cidades; j++) {
				int aux = vizinho.vet_sol[i];
				vizinho.vet_sol[i] = vizinho.vet_sol[j];
				vizinho.vet_sol[j] = aux;
				calcular_fo(vizinho);
				if (vizinho.fo < melhor_fo_local) {
					melhor_fo_local = vizinho.fo;
					memcpy(&melhor_vizinho, &vizinho, sizeof(Solucao));
					melhorou = 1;
				}
			}
		}

		if (melhorou) {
			memcpy(&s, &melhor_vizinho, sizeof(Solucao));
			continue; 
		}
				
		for (int j = 0; j < num_cidades; j++) {
			int carro_original = s.vet_carro[j];

			for (int i = 0; i < num_carros; i++) {
				if (i == carro_original) continue;

				Solucao vizinho = s;
				vizinho.vet_carro[j] = i;

				calcular_fo(vizinho);

				if (vizinho.fo < melhor_fo_local) {
					melhor_fo_local = vizinho.fo;
					memcpy(&melhor_vizinho, &vizinho, sizeof(Solucao));
					melhorou = 1;
				}
			}
		}

		if (melhorou) {
			memcpy(&s, &melhor_vizinho, sizeof(Solucao));
		}
	}
}

void sa(Solucao& s) {

	// inicia
	Solucao s_melhor;
	Solucao s_vizinha;
	memset(&s_melhor, 0, sizeof(Solucao));
	memset(&s_vizinha, 0, sizeof(Solucao));
	memcpy(&s_melhor, &s, sizeof(Solucao));

	double T = TEMPERATURA_INICIAL;

	while (T > TEMPERATURA_FINAL) {

		for (int i = 0; i < ITER_POR_TEMP; i++) {
			memset(&s_vizinha, 0, sizeof(Solucao));
			memcpy(&s_vizinha, &s, sizeof(Solucao));

			// pedra, papel ou tesoura
			gerar_vizinha(s_vizinha, rand() % 3);
			calcular_fo(s_vizinha);
			double delta = s_vizinha.fo - s.fo;

			if (delta < 0) {
				// aceita
				memcpy(&s, &s_vizinha, sizeof(Solucao));

				if (s.fo < s_melhor.fo) {
					memcpy(&s_melhor, &s, sizeof(Solucao));
					//nova melhor solucao
					//printf("FO: %d\n", s_melhor.fo);
					escrever_solucao("saida.txt", s_melhor);
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
		T *= ALPHA;
	}

	// escreve a melhor que achou
	memcpy(&s, &s_melhor, sizeof(Solucao));
}