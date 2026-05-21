# Subprojeto de Iniciação Científica

| Campo | Conteúdo |
|---|---|
| **Edital:** | Edital PIIC 2026/2027 |
| **Área do Conhecimento (CNPq):** | 3.08.00.00-5 — Engenharia de Produção |
| **Subárea do Conhecimento (CNPq):** | 3.08.02.00-8 — Pesquisa Operacional |
| **Título do Projeto:** | Modelagem e otimização de problemas de corte e processamento de bobinas de aço |
| **Título do Subprojeto:** | Metaheurística híbrida *Clustering Search* e *Simulated Annealing* para uma variante do Problema do Caixeiro Alugador |
| **Orientador(a):** | Geraldo Regis Mauri |
| **Estudante:** | Maik |

| Estudante apresenta alguma necessidade de acessibilidade? | Sim ( ) | Não ( x ) |
|---|---|---|
| Se sim, informar o recurso ou suporte de acessibilidade que necessita. | | |

---

## Resumo

A Pesquisa Operacional oferece ferramentas matemáticas e computacionais para apoiar a tomada de decisão em problemas complexos de otimização. Nesse contexto, o Problema do Caixeiro Alugador (*Traveling Car Renter Problem* — CaRS), introduzido por Goldbarg et al. (2012), é uma generalização NP-difícil do Problema do Caixeiro Viajante na qual o percurso pode ser realizado utilizando diferentes carros alugados, cada um com custos próprios de deslocamento e taxas de retorno. Recentemente, Lima et al. (2025), em trabalho publicado no SBPO 2025, propuseram uma nova variante do CaRS que relaxa três pressupostos clássicos: não há cidade base pré-definida, o mesmo tipo de carro pode ser alugado múltiplas vezes ao longo do trajeto, e a devolução dos veículos é restrita a um subconjunto pré-definido de cidades (cidades com índice ímpar). Os autores apresentam uma formulação matemática implementada no solver GLPK e um algoritmo memético, com resultados reportados para 58 instâncias. Este subprojeto investigará uma metaheurística híbrida ainda não aplicada a essa variante: *Clustering Search* combinado com *Simulated Annealing* (CS+SA). O foco será sobre as 29 instâncias não-Euclideanas, nas quais o GLPK falhou em encontrar a solução ótima em 17 delas dentro do limite de 10.000 s segundo a Tabela 2 de Lima et al. (2025). A implementação será em C/C++ e os resultados serão comparados ao algoritmo memético e ao solver exato da literatura.

**Palavras-chave:** Caixeiro Alugador. *Clustering Search*. *Simulated Annealing*. Metaheurísticas. Pesquisa Operacional.

---

## 1 Introdução

A área de Pesquisa Operacional (PO) se dedica a formular problemas reais por meio da linguagem matemática e se utiliza de algoritmos e da estatística para auxiliar na tomada de decisão. A PO tem como objetivo a obtenção da melhor solução (ou uma aproximação) — isto é, busca-se a maximização ou a minimização de uma função objetivo respeitando as restrições do problema. Essa área do conhecimento abrange diversas classes de problemas, tendo em vista sua alta aplicabilidade, entre eles os problemas de roteamento, cobertura, localização de facilidades e, em particular, o problema abordado neste subprojeto: o Problema do Caixeiro Alugador.

O Problema do Caixeiro Alugador, ou *Traveling Car Renter Problem* (CaRS), foi introduzido por Goldbarg et al. (2012) como uma generalização do Problema do Caixeiro Viajante (PCV). Seu objetivo é minimizar o custo total de percorrer um conjunto de cidades, com a particularidade de que o trajeto pode ser realizado utilizando carros alugados, sendo que carros diferentes apresentam custos distintos para percorrer cada aresta. Esses custos podem ser interpretados como a soma do custo de locação, do consumo de combustível e do pagamento de pedágios. Adicionalmente, quando um carro é alugado em uma cidade e devolvido em outra distinta, considera-se o pagamento de uma taxa de retorno para que o veículo seja levado de volta à cidade de origem. Como o PCV é um problema NP-difícil (GUTIN; PUNNEN, 2002) e constitui caso particular do CaRS quando apenas um carro está disponível, o CaRS também é classificado como NP-difícil.

A formulação tradicional do CaRS, apresentada em Goldbarg et al. (2012), estabelece três restrições fundamentais: (i) o mesmo tipo de carro não pode ser alugado mais de uma vez no trajeto (sem repetição); (ii) o carro alugado pode ser devolvido em qualquer cidade (devolução irrestrita); e (iii) existe uma cidade base fixa, predefinida, onde o percurso deve obrigatoriamente começar e terminar. Na literatura, esse problema foi objeto de diversas pesquisas, com abordagens exatas e heurísticas. Goldbarg et al. (2013) apresentam um modelo de programação quadrática inteira, posteriormente reformulado em Goldbarg et al. (2017), que propõem três modelos para o problema. Da Silva e Ochi (2016) propuseram um algoritmo evolucionário híbrido (ALSP), e Lacroix et al. (2021) desenvolveram um modelo baseado em coloração de vértices. Em termos de variantes do problema, destacam-se o *Prize Collecting Traveling Car Renter Problem* (p-CaRS), proposto por da Silva Menezes et al. (2014), e o *Traveling Car Renter with Passengers* (CaRSP), proposto por de Araújo Sabry et al. (2020).

Apesar dessa diversidade de trabalhos, todos abordavam o problema sob a mesma ótica, mantendo as três restrições clássicas. Lima et al. (2025), em trabalho publicado nos Anais do SBPO 2025, propõem uma nova variante mais alinhada à realidade prática, na qual essas restrições são relaxadas:

1. **Não há exigência de cidade base** — o percurso pode iniciar em qualquer cidade da instância;
2. **O mesmo tipo de carro pode ser alugado múltiplas vezes** ao longo do trajeto;
3. **A devolução dos veículos é restrita** a um subconjunto específico de cidades — em particular, para aproveitar as instâncias existentes sem modificações, os autores consideram que os carros só podem ser devolvidos em cidades de índice ímpar.

A formulação matemática para essa variante, baseada no modelo DFJ apresentado em Goldbarg et al. (2017) e reproduzida em Lima et al. (2025), é descrita a seguir. Seja $G = (V, A)$ um grafo completo, onde $V$ é o conjunto de $n$ cidades e $A$ é o conjunto de arcos entre as cidades. Seja $C$ o conjunto de carros, todos disponíveis para aluguel em todas as cidades. O custo do carro $c \in C$ percorrer a aresta $(i,j) \in A$ é denotado por $d_{ij}^{c}$, e a taxa de retorno do carro $c$ alugado em $i$ e devolvido em $j$ (com $i \neq j$) é denotada por $\Omega_{ij}^{c}$. As variáveis binárias do modelo são: $f_{ij}^{c}$, igual a 1 quando o carro $c$ percorre a aresta $(i,j)$; $w_{ij}^{c}$, igual a 1 quando o carro $c$ é alugado em $j$ e devolvido em $i$; $a_{i}^{c}$, igual a 1 quando o carro $c$ é locado na cidade $i$; e $e_{i}^{c}$, igual a 1 quando o carro $c$ é devolvido na cidade $i$. O modelo, conforme Lima et al. (2025), é:

$$
\min \; \sum_{c \in C} \sum_{i,j \in V} f_{ij}^{c} \, d_{ij}^{c} \; + \; \sum_{c \in C} \sum_{i,j \in V} \Omega_{ij}^{c} \, w_{ij}^{c} \qquad (1)
$$

Sujeito a:

$$
\sum_{c \in C} \sum_{j \in V} f_{ij}^{c} = 1, \quad \forall \, i \in V \qquad (2)
$$

$$
\sum_{c \in C} \sum_{i \in V} f_{ij}^{c} = 1, \quad \forall \, j \in V \qquad (3)
$$

$$
a_{i}^{c} = \sum_{j \in V} f_{ij}^{c} \left( 1 - \sum_{x \in C,\, x \neq c} \sum_{h \in V} f_{hi}^{x} \right), \quad \forall \, c \in C,\; \forall \, i \in V \qquad (4)
$$

$$
e_{i}^{c} = \sum_{j \in V} f_{ji}^{c} \left( 1 - \sum_{x \in C,\, x \neq c} \sum_{h \in V} f_{ih}^{x} \right), \quad \forall \, c \in C,\; \forall \, i \in V \qquad (5)
$$

$$
w_{ij}^{c} = a_{j}^{c} \, e_{i}^{c}, \quad \forall \, c \in C,\; \forall \, i, j \in V \qquad (6)
$$

$$
\sum_{c \in C} \sum_{i \in V} a_{i}^{c} \geq 1 \qquad (7)
$$

$$
2 \leq u_i \leq n, \quad i = 2, \ldots, n \qquad (8)
$$

$$
u_i - u_j + 1 \leq (n - 1)(1 - f_{ij}^{c}), \quad i, j = 2, \ldots, n,\; \forall \, c \in C \qquad (9)
$$

$$
f_{ij}^{c}, \, a_{i}^{c}, \, e_{i}^{c}, \, w_{ij}^{c} \in \{0, 1\} \qquad (10)
$$

$$
u_i \in \mathbb{Z}^{+} \qquad (11)
$$

A função objetivo (Eq. (1)) minimiza a soma dos custos de percorrer as arestas com diferentes carros e os respectivos custos de retorno. As restrições (2) e (3) garantem que cada cidade seja visitada exatamente uma vez. As restrições (4) e (5), conforme adaptadas em Lima et al. (2025) a partir de Goldbarg et al. (2017), registram, respectivamente, o aluguel e a devolução do veículo em cada cidade. A restrição (6) registra se um carro $c$ foi locado em $j$ e devolvido em $i$. A restrição (7) assegura que pelo menos um carro é locado. As restrições (8) e (9), adaptadas da formulação de Miller-Tucker-Zemlin (MILLER; TUCKER; ZEMLIN, 1960) para o PCV, eliminam sub-rotas. Por fim, (10) define as variáveis binárias e (11) define $u_i$ como inteiros positivos. As restrições (4)–(6) são originalmente quadráticas, sendo linearizadas conforme sugerido em Liberti (2007) e implementadas no GLPK.

Apesar da disponibilidade de uma formulação matemática, a aplicação direta de métodos exatos é inviável para instâncias de maior porte. Conforme reportado na Tabela 2 de Lima et al. (2025), o solver GLPK falhou em encontrar a solução ótima para 17 das 29 instâncias não-Euclideanas dentro do limite de 10.000 segundos de processamento, com desvios percentuais (GAPs) em relação ao limite inferior variando de 6,3% (instância Arabia14n) a 34,4% (instância Brasil16n). Esse comportamento (i) confirma a inviabilidade dos métodos exatos para topologias não-Euclideanas e (ii) justifica a escolha de **restringir o escopo deste subprojeto a essas 29 instâncias não-Euclideanas**, onde há maior margem para contribuições heurísticas. As instâncias Euclideanas, para as quais o GLPK encontra o ótimo em quase todos os casos (Tabela 1 de Lima et al., 2025), estão fora do escopo deste trabalho.

Diante dessa complexidade computacional, este subprojeto se insere no esforço de investigar metaheurísticas adequadas à variante do CaRS de Lima et al. (2025). As metaheurísticas, sendo procedimentos de busca de propósito geral capazes de explorar grandes espaços de soluções, podem ser exploradas, também, em outros contextos correlatos. Em particular, este subprojeto está vinculado ao projeto guarda-chuva *"Modelagem e otimização de problemas de corte e processamento de bobinas de aço"*, na medida em que as estratégias algorítmicas estudadas (combinação de busca local intensiva com mecanismos de exploração) podem ser transpostas para problemas de corte e logística da cadeia siderúrgica.

## 2 Objetivos

Este subprojeto tem como objetivo geral investigar e implementar uma metaheurística híbrida ainda não explorada na literatura para a variante do Problema do Caixeiro Alugador proposta por Lima et al. (2025), combinando as técnicas *Clustering Search* (CS) e *Simulated Annealing* (SA), buscando soluções de boa qualidade em tempo computacional viável. Os objetivos específicos são:

a) Estudar em profundidade a variante do CaRS proposta por Lima et al. (2025), compreendendo sua formulação matemática, as restrições de cidade base, repetição de carros e devolução restrita, bem como os principais desafios computacionais envolvidos;

b) Realizar uma revisão bibliográfica sistemática sobre as metaheurísticas *Clustering Search*, *Simulated Annealing* e seus híbridos, aplicados a problemas combinatórios similares ao CaRS;

c) Implementar em linguagem C/C++ a metaheurística híbrida CS+SA para a variante em estudo, reaproveitando estruturas computacionais já existentes na base de código do projeto;

d) Analisar o desempenho do algoritmo proposto comparando os resultados obtidos com os valores reportados por Lima et al. (2025), incluindo o algoritmo memético dos próprios autores e os resultados do solver GLPK apresentados na Tabela 2 do artigo.

## 3 Metodologia

A metodologia deste subprojeto será desenvolvida em etapas. A primeira etapa consistirá no levantamento e estudo bibliográfico sobre a variante do CaRS de Lima et al. (2025) e sobre as metaheurísticas *Clustering Search* e *Simulated Annealing*, analisando estratégias adotadas em trabalhos correlatos da literatura.

A etapa seguinte corresponderá à construção computacional do problema e dos algoritmos a partir da linguagem de programação C/C++, considerando sua velocidade e poder computacional para grandes volumes de operações — escolha também adotada por Lima et al. (2025). A base de instâncias utilizada será composta pelas **29 instâncias não-Euclideanas** disponibilizadas pelos autores originais do CaRS no repositório `http://www.dimap.ufrn.br/lae/en/projects/CaRS.php`, com número de cidades $n$ variando de 10 a 17 e número de carros de 2 a 5, conforme reportado na Tabela 2 de Lima et al. (2025). As instâncias Euclideanas (Tabela 1 do mesmo artigo) **não fazem parte do escopo deste trabalho**, pois nelas o GLPK consegue encontrar a solução ótima em quase todos os casos.

A etapa final compreenderá a realização de experimentos computacionais e a avaliação dos resultados obtidos. Em conformidade com o protocolo experimental adotado por Lima et al. (2025), o algoritmo proposto será executado 30 vezes para cada instância, registrando-se o menor valor da função objetivo (Min), o valor médio (Média), a frequência com que a melhor solução foi alcançada (F) e o tempo médio de processamento (T(s)). Esses valores serão comparados aos resultados do algoritmo memético e do solver GLPK reportados na Tabela 2 de Lima et al. (2025).

A seguir, é apresentada uma breve descrição das metaheurísticas selecionadas e da hibridização proposta.

### 3.1 *Simulated Annealing* (SA)

O *Simulated Annealing* é um método de busca local que aceita movimentos de piora como forma de escapar de ótimos locais (KIRKPATRICK; GELLAT; VECCHI, 1983). De forma semelhante aos métodos tradicionais de busca local, o SA requer que sejam definidas uma estrutura de vizinhança e uma função de avaliação $f$ que associa a cada solução um valor numérico representando seu custo. O algoritmo inicia a partir de uma solução qualquer, e em cada iteração seleciona aleatoriamente uma solução $s'$ vizinha de $s$. É computada a variação $\Delta = f(s') - f(s)$. Se $\Delta < 0$, a solução $s'$ é aceita; caso contrário, $s'$ é aceita com probabilidade $e^{-\Delta/T}$, em que $T$ é um parâmetro chamado temperatura, que regula a probabilidade de aceitação de soluções piores. A temperatura é reduzida gradualmente segundo uma taxa de resfriamento, até atingir um valor próximo a zero, momento em que o sistema é dito estável. Neste subprojeto, o SA cumprirá o papel de **gerador de soluções para o *Clustering Search***.

### 3.2 *Clustering Search* (CS)

O *Clustering Search* é um método iterativo que procura dividir o espaço de busca e localizar regiões promissoras por meio do enquadramento dessas em *clusters* (CHAVES; LORENA, 2010). Um *cluster* pode ser definido por três atributos $C = \{c, v, r\}$: o **centro** $c_i$, uma solução que representa o *cluster* e identifica sua localização no espaço de busca; o **volume** $v_i$, que é a quantidade de soluções agrupadas no *cluster*; e o **índice de ineficácia** $r_i$, uma variável de controle que indica quantas vezes consecutivas a busca local foi aplicada no *cluster* $i$ sem melhorar a solução. O CS é composto por três componentes principais: uma metaheurística geradora de soluções, um processo de agrupamento e uma heurística de busca local. A cada iteração, uma solução $S$ é gerada pela metaheurística e enviada ao processo de agrupamento, que a aloca no *cluster* mais similar $C_j$. Quando o volume $v_j$ atinge um limitante $\lambda$, o *cluster* é considerado promissor, e a heurística de busca local é aplicada ao seu centro $c_j$. Se a busca local não melhorar a solução por $r_{max}$ vezes consecutivas, uma perturbação aleatória é aplicada ao centro, objetivando escapar de um possível ótimo local. O critério de parada do CS é geralmente definido pela metaheurística geradora.

### 3.3 Hibridização CS+SA proposta para a variante do CaRS

A hibridização proposta neste subprojeto utilizará o SA como **gerador de soluções** para o CS, em uma estratégia análoga à descrita em Chaves e Lorena (2010) e Oliveira, Mauri e Lorena (2012). A solução inicial do SA será gerada por uma **heurística construtiva gulosa-aleatória** que, a cada iteração, seleciona aleatoriamente um carro e constrói parte do percurso pelo critério do vizinho mais próximo, respeitando as restrições de devolução apenas em cidades de índice ímpar conforme estabelecido em Lima et al. (2025). A vizinhança explorada pelo SA contemplará três operadores: (i) troca do carro associado a uma cidade; (ii) reposicionamento de cidades no percurso (movimento do tipo 2-opt); e (iii) a aplicação combinada dos dois operadores anteriores.

Cada solução produzida pelo SA será encaminhada ao processo de agrupamento do CS, que avaliará a similaridade entre a solução corrente e o centro de cada *cluster* — considerando tanto o vetor de sequência de cidades quanto o vetor de associação cidade–carro. Quando o volume de um *cluster* atingir o limitante $\lambda$, uma **busca local de troca de cidades combinada com troca de carros** será aplicada ao centro do *cluster*, com o objetivo de intensificar a exploração de regiões promissoras do espaço de soluções. Solução inviáveis (decorrentes de devolução em cidade par) serão corrigidas conforme o procedimento de reparação descrito em Lima et al. (2025): substituição do carro associado pelo carro da cidade imediatamente posterior, até que o índice de devolução seja ímpar.

A escolha conjunta do CS e do SA se justifica por dois motivos: (i) o SA apresenta facilidade de implementação e não depende fortemente de uma boa solução inicial, características adequadas à variante do CaRS em estudo; e (ii) o CS demonstrou bom desempenho em diferentes problemas combinatórios (CHAVES; LORENA, 2010; OLIVEIRA; MAURI; LORENA, 2012) e, após consulta à literatura — incluindo os trabalhos citados em Lima et al. (2025) —, não foi encontrada uma aplicação dessa metaheurística para o CaRS ou suas variantes, caracterizando uma contribuição original deste subprojeto.

## 4 Plano de Trabalho / Cronograma

Abaixo estão listadas as tarefas que serão executadas ao longo do desenvolvimento do subprojeto até a sua finalização, descrevendo todo o procedimento.

**Quadro 1 — Lista de atividades previstas do Subprojeto**

| Atividade | Descrição |
|---|---|
| a) Levantamento bibliográfico sistemático | Realizar a revisão de artigos sobre o CaRS, suas variantes (em especial Lima et al., 2025) e sobre as metaheurísticas *Clustering Search* e *Simulated Annealing*. |
| b) Estudo da variante e da base de instâncias | Aprofundar o estudo da formulação matemática proposta em Lima et al. (2025) e analisar as 29 instâncias não-Euclideanas do repositório `dimap.ufrn.br/lae/`. |
| c) Implementação das estruturas computacionais | Desenvolver/refinar em C/C++ as estruturas de representação da solução, leitura de instâncias e cálculo da função objetivo conforme a variante de Lima et al. (2025). |
| d) Entrega do Relatório Parcial | Documentar o progresso da pesquisa até o meio do período. |
| e) Implementação da hibridização CS+SA | Codificar a metaheurística híbrida proposta, integrando o SA como gerador de soluções e o CS como mecanismo de identificação de regiões promissoras. |
| f) Realização de experimentos computacionais | Execução dos experimentos com as 29 instâncias não-Euclideanas (30 execuções por instância), variando parâmetros e coletando dados de qualidade da solução e tempo de execução. |
| g) Avaliação do desempenho do método proposto | Comparação dos resultados obtidos com o algoritmo memético e o GLPK reportados na Tabela 2 de Lima et al. (2025), calculando gaps e analisando eficiência. |
| h) Entrega do Relatório Final | Consolidação dos resultados e elaboração do relatório final. |

**Quadro 2 — Cronograma de atividades previstas do subprojeto (set./2026 a ago./2027)**

| Atividade | set | out | nov | dez | jan | fev | mar | abr | mai | jun | jul | ago |
|---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| a) Levantamento bibliográfico sistemático | X | X |   |   |   |   |   |   |   |   |   |   |
| b) Estudo da variante e da base de instâncias | X | X | X |   |   |   |   |   |   |   |   |   |
| c) Implementação das estruturas computacionais |   |   | X | X |   |   |   |   |   |   |   |   |
| d) Entrega do Relatório Parcial |   |   |   |   | X | X |   |   |   |   |   |   |
| e) Implementação da hibridização CS+SA |   |   |   |   |   |   | X | X |   |   |   |   |
| f) Realização de experimentos computacionais |   |   |   |   |   |   |   |   | X | X |   |   |
| g) Avaliação do desempenho do método proposto |   |   |   |   |   |   |   |   | X | X |   |   |
| h) Entrega do Relatório Final |   |   |   |   |   |   |   |   |   |   | X | X |

## Referências

CHAVES, A. A.; LORENA, L. A. N. Clustering search algorithm for the capacitated centered clustering problem. **Computers & Operations Research**, v. 37, p. 552–558, 2010.

DA SILVA, A. R. V.; OCHI, L. S. An efficient hybrid algorithm for the traveling car renter problem. **Expert Systems with Applications**, v. 64, p. 132–140, 2016.

DA SILVA MENEZES, M.; GOLDBARG, M. C.; GOLDBARG, E. F. A memetic algorithm for the prize-collecting traveling car renter problem. In: **IEEE Congress on Evolutionary Computation (CEC)**, 2014, Beijing. Proceedings... IEEE, 2014. p. 3258–3265.

DE ARAUJO SABRY, G.; GOLDBARG, M. C.; GOLDBARG, E. F. G.; DA SILVA MENEZES, M.; CALHEIROS, Z. S. A. Models and linearizations for the traveling car renter with passengers. **SpringerNature Complete Journals**, v. 15, n. 1, p. 59–81, 2020.

GOLDBARG, M. C.; ASCONAVIETA, P. H.; GOLDBARG, E. F. G. Memetic algorithm for the traveling car renter problem: an experimental investigation. **Memetic Computing**, v. 4, n. 2, p. 89–108, 2012.

GOLDBARG, M. C.; GOLDBARG, E. F.; ASCONAVIETA, P. H.; MENEZES, M. S.; LUNA, H. P. A transgenetic algorithm applied to the traveling car renter problem. **Expert Systems with Applications**, v. 40, n. 16, p. 6298–6310, 2013.

GOLDBARG, M. C.; GOLDBARG, E. F. G.; LUNA, H. P. L.; MENEZES, M. S.; CORRALES, L. Integer programming models and linearizations for the traveling car renter problem. **SpringerNature Complete Journals**, v. 12, n. 4, p. 743–761, 2017.

GUTIN, G.; PUNNEN, A. P. **The traveling salesman problem and its variations**. v. 12. New York: Springer Science & Business Media, 2002.

KIRKPATRICK, S.; GELLAT, D. C.; VECCHI, M. P. Optimization by Simulated Annealing. **Science**, v. 220, p. 671–680, 1983.

LACROIX, M.; SOLIS, Y. R.; CALVO, R. W. Efficient formulations for the traveling car renter problem and its quota variant. **SpringerNature Complete Journals**, v. 15, n. 6, p. 1905–1930, 2021.

LIBERTI, L. Compact linearization for binary quadratic problems. **4OR**, v. 5, n. 3, p. 231–245, 2007.

LIMA, F. R.; MENEZES, M. S.; MAIA, S. M. D. M.; MEZZOMO, I.; SABRY, G. A. Modelo matemático e algoritmo memético para uma variante do Problema do Caixeiro Alugador. In: **SIMPÓSIO BRASILEIRO DE PESQUISA OPERACIONAL**, 2025, [S. l.]. **Anais do SBPO 2025** [...]. [S. l.: s. n.], 2025. DOI: 10.59254/sbpo-2025-212586. Disponível em: https://proceedings.science/p/212586. Acesso em: 19 maio 2026.

MILLER, C. E.; TUCKER, A. W.; ZEMLIN, R. A. Integer programming formulation of traveling salesman problems. **Journal of the ACM**, v. 7, n. 4, p. 326–329, 1960.

OLIVEIRA, R. M.; MAURI, G. R.; LORENA, L. A. N. Clustering search for the berth allocation problem. **Expert Systems with Applications**, v. 39, n. 5, p. 5499–5505, 2012.
