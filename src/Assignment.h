#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <vector>
#include <string>
#include "Parser.h"
#include "MaxFlow.h"

/**
 * @brief Classe responsável por gerar atribuições de revisões e análise de risco.
 *
 * Encapsula os dois algoritmos principais do projeto:
 * - Atribuição de revisores a submissões, formulada como um problema de fluxo máximo
 *   e resolvida com o algoritmo de Edmonds-Karp.
 * - Análise de risco, que determina quais os revisores (ou conjuntos de K revisores)
 *   cuja ausência inviabiliza o processo de revisão.
 *
 * A rede de fluxo é construída da seguinte forma:
 * @code
 * Source --(cap = MinReviewsPerSubmission)--> [Submissao i]
 *                                                   |
 *                                          (cap = 1, se dominio compativel)
 *                                                   |
 *                                                   v
 *                                            [Revisor j] --(cap = MaxReviewsPerReviewer)--> Sink
 * @endcode
 *
 * Os nós de submissão recebem identificadores submissionId + 1000 e os de revisor
 * reviewerId + 2000, de forma a evitar colisões com os nós especiais -1 (source)
 * e -2 (sink).
 *
 * Um fluxo máximo igual a |Submissões| × MinReviewsPerSubmission indica uma
 * atribuição completa e válida.
 */
class Assignment {
public:

    /**
     * @brief Gera a atribuição de revisões usando o algoritmo de fluxo máximo de Edmonds-Karp.
     *
     * Constrói a rede de fluxo, executa o Edmonds-Karp e escreve o ficheiro de saída
     * com a atribuição obtida. Suporta os modos 1, 2 e 3 de correspondência de domínios,
     * controlados pelo campo control.generateAssignments:
     * - Modo 1: apenas domínios primários de submissões e revisores.
     * - Modo 2: domínios primário e secundário da submissão; apenas primário do revisor.
     * - Modo 3: todos os domínios primários e secundários de ambos os lados.
     *
     * Em caso de atribuição incompleta (fluxo insuficiente), reporta quais as submissões
     * com revisões em falta e o número de revisores adicionais necessários por domínio.
     *
     * Se control.riskAnalysis > 0, executa também a análise de risco e escreve os
     * resultados no mesmo ficheiro de saída.
     *
     * @param submissions Lista de submissões de artigos científicos.
     * @param reviewers   Lista de revisores disponíveis.
     * @param params      Parâmetros do problema (MinReviewsPerSubmission, MaxReviewsPerReviewer).
     * @param control     Configurações de controlo (modo, nível de risco, ficheiro de saída).
     * @param outputFile  Caminho para o ficheiro CSV de saída.
     * @return true se o ficheiro de saída foi escrito com sucesso; false caso contrário.
     *
     * @note Complexidade temporal: O(S·R + V·E²), onde S = número de submissões,
     *       R = número de revisores, V = vértices do grafo de fluxo e E = arestas
     *       do grafo de fluxo. O passo de Edmonds-Karp O(V·E²) é dominante.
     */
    static bool generateAssignment(const std::vector<Submission>& submissions,
                                   const std::vector<Reviewer>& reviewers,
                                   const Parameters& params,
                                   const Control& control,
                                   const std::string& outputFile);

    /**
     * @brief Efetua a análise de risco de forma autónoma.
     *
     * Reconstrói a rede de fluxo de raiz e determina quais os revisores (K = 1) ou
     * combinações de K revisores (K > 1) cuja ausência impede que todas as submissões
     * recebam o número mínimo de revisões exigido. Os resultados são escritos num
     * ficheiro de saída separado.
     *
     * O algoritmo funciona da seguinte forma para cada combinação de K revisores:
     * -# A capacidade da aresta revisor→sink de cada revisor da combinação é colocada
     *    a 0, simulando a sua ausência na rede.
     * -# Todos os fluxos do grafo são repostos a zero.
     * -# O algoritmo de Edmonds-Karp é executado sobre o grafo modificado.
     * -# O fluxo obtido é comparado com o fluxo esperado
     *    (|Submissões| × MinReviewsPerSubmission). Se for inferior, a combinação é
     *    considerada crítica e registada.
     * -# A capacidade da aresta revisor→sink é restaurada ao valor original
     *    (MaxReviewsPerReviewer) antes de se passar à combinação seguinte.
     *
     * As C(N, K) combinações são enumeradas através de um vector seletor booleano
     * de tamanho N (número de revisores), com K posições verdadeiras, iterando por
     * todas as permutações com std::next_permutation.
     *
     * @param submissions Lista de submissões de artigos científicos.
     * @param reviewers   Lista de revisores disponíveis.
     * @param params      Parâmetros do problema.
     * @param control     Configurações de controlo (o campo riskAnalysis é usado como K).
     * @param riskFile    Caminho para o ficheiro CSV de saída da análise de risco.
     * @return true se o ficheiro de saída foi escrito com sucesso; false caso contrário.
     *
     * @note Complexidade temporal:
     *       - K = 1: O(N·V·E²), onde N = número de revisores.
     *       - K > 1: O(C(N,K)·V·E²) — cresce exponencialmente com K, tornando-se
     *         inviável para valores elevados.
     */
    static bool generateRiskAnalysis(const std::vector<Submission>& submissions,
                                     const std::vector<Reviewer>& reviewers,
                                     const Parameters& params,
                                     const Control& control,
                                     const std::string& riskFile);
};

#endif