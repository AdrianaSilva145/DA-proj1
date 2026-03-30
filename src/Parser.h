#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Submission.h"
#include "Reviewer.h"
#include "Parameters.h"
#include "Control.h"

/**
 * @brief Classe responsável pela leitura e parsing do ficheiro CSV de entrada.
 *
 * Lê o ficheiro no formato definido pelo enunciado, com secções delimitadas por #:
 * #Submissions, #Reviewers, #Parameters e #Control.
 * Ignora comentários (linhas iniciadas por # ou conteúdo após #) e linhas vazias.
 * Remove automaticamente espaços em branco e aspas dos campos lidos.
 */
struct Parser {
    /**
     * @brief Lê e faz parsing do ficheiro CSV de entrada.
     *
     * Itera o ficheiro linha a linha, identificando a secção corrente pelo
     * cabeçalho e processando cada linha de dados em conformidade. Os campos
     * opcionais (domínio secundário) são representados por -1 quando ausentes.
     *
     * @param filepath    Caminho para o ficheiro CSV de entrada.
     * @param submissions Vector a preencher com as submissões lidas.
     * @param reviewers   Vector a preencher com os revisores lidos.
     * @param params      Estrutura a preencher com os parâmetros lidos.
     * @param control     Estrutura a preencher com as configurações de controlo lidas.
     * @return true se o ficheiro foi lido com sucesso; false caso contrário.
     *
     * @note Complexidade temporal: O(L·F), onde L = número de linhas do ficheiro
     *       e F = número de campos por linha.
     */
    static bool parseCSV(const std::string &filepath, std::vector<Submission> &submissions,
                        std::vector<Reviewer> &reviewers, Parameters &params, Control &control);
};

#endif