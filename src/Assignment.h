#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <vector>
#include <string>
#include "Parser.h"
#include "MaxFlow.h"

/**
 * @brief Classe responsável pela geração de atribuições de revisões e análise de risco.
 *
 * Encapsula os dois algoritmos principais do projeto:
 * - Atribuição de revisores a submissões, formulada como um problema de Max-Flow
 *   e resolvida com o algoritmo de Edmonds-Karp.
 * - Análise de risco, que determina quais os revisores (ou conjuntos de K revisores)
 *   cuja ausência inviabiliza o processo de revisão.
 *
 * A rede de fluxo é construída da seguinte forma:
 * @code
 * Source --(cap=MinReviewsPerSubmission)--> [Submissao i]
 *                                                |
 *                                       (cap=1, se dominio compativel)
 *                                                |
 *                                                v
 *                                         [Revisor j] --(cap=MaxReviewsPerReviewer)--> Sink
 * @endcode
 *
 * Um fluxo maximo igual a |Submissoes| x MinReviewsPerSubmission indica uma
 * atribuicao completa e valida.
 */
class Assignment {
public:

    /**
     * @brief Gera a atribuicao de revisoes usando o algoritmo de Max-Flow de Edmonds-Karp.
     *
     * Constroi uma rede de fluxo e determina a atribuicao otima de revisores a submissoes.
     * Suporta os modos 1, 2 e 3 de correspondencia de dominios (controlados por
     * control.generateAssignments). Se control.riskAnalysis > 0, inclui tambem a
     * analise de risco no ficheiro de saida.
     *
     * Em caso de atribuicao incompleta (fluxo insuficiente), reporta quais as submissoes
     * com revisoes em falta e o numero de revisores adicionais necessarios por dominio.
     *
     * @param submissions Lista de submissoes de artigos cientificos.
     * @param reviewers   Lista de revisores disponiveis.
     * @param params      Parametros do problema (MinReviewsPerSubmission, MaxReviewsPerReviewer).
     * @param control     Configuracoes de controlo (modo, nivel de risco, ficheiro de saida).
     * @param outputFile  Caminho para o ficheiro CSV de saida.
     * @return true se o ficheiro de saida foi escrito com sucesso, false caso contrario.
     *
     * @note Complexidade Temporal: O(S*R + V*E^2), onde S = numero de submissoes,
     *       R = numero de revisores, V = vertices do grafo, E = arestas do grafo.
     *       O passo de Edmonds-Karp O(V*E^2) e dominante.
     */
    static bool generateAssignment(const std::vector<Submission>& submissions,
                                   const std::vector<Reviewer>& reviewers,
                                   const Parameters& params,
                                   const Control& control,
                                   const std::string& outputFile);

    /**
     * @brief Efectua a analise de risco de forma autonoma (T2.2 e T2.3).
     *
     * Reconstroi a rede de fluxo de raiz e determina quais os revisores (K=1) ou
     * combinacoes de K revisores (K>1) cuja ausencia impede que todas as submissoes
     * recebam o numero minimo de revisoes exigido.
     *
     * O algoritmo para cada combinacao de K revisores:
     * -# Define a capacidade da aresta revisor->Sink a 0 (simula a ausencia do revisor).
     * -# Re-executa Edmonds-Karp e compara o fluxo com o maximo necessario.
     * -# Restaura a capacidade original para a proxima iteracao.
     *
     * Todas as C(N,K) combinacoes sao enumeradas com std::next_permutation num
     * vector selector booleano.
     *
     * @par Complexidade Temporal
     * - K=1: O(N*V*E^2), onde N = numero de revisores.
     * - K>1: O(C(N,K)*V*E^2) - exponencial em K; inviavel para K elevado.
     *
     * @par Abordagem alternativa para K elevado (T2.3)
     * Para valores elevados de K, uma abordagem baseada em corte minimo de vertices
     * e mais eficiente: um conjunto de K revisores e critico se e so se a sua remocao
     * reduz o fluxo maximo abaixo do limiar. Isto relaciona-se com encontrar cortes
     * de vertices minimos de tamanho K, resolavel em tempo polinomial para K fixo
     * usando divisao de nos na rede de fluxo.
     *
     * @param submissions Lista de submissoes de artigos cientificos.
     * @param reviewers   Lista de revisores disponiveis.
     * @param params      Parametros do problema.
     * @param control     Configuracoes de controlo (o campo riskAnalysis e usado como K).
     * @param riskFile    Caminho para o ficheiro CSV de saida da analise de risco.
     * @return true se o ficheiro de saida foi escrito com sucesso, false caso contrario.
     *
     * @note Complexidade Temporal: O(C(N,K)*V*E^2) - ver descricao acima.
     */
    static bool generateRiskAnalysis(const std::vector<Submission>& submissions,
                                     const std::vector<Reviewer>& reviewers,
                                     const Parameters& params,
                                     const Control& control,
                                     const std::string& riskFile);
};

#endif