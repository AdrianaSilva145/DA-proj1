#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Submission.h"
#include "Reviewer.h"
#include "Parameters.h"
#include "Control.h"

/**
 * @brief Classe responsavel pela leitura e parsing do ficheiro CSV de entrada.
 *
 * Le o ficheiro no formato definido pelo enunciado, com seccoes delimitadas por #:
 * #Submissions, #Reviewers, #Parameters e #Control.
 * Ignora comentarios (linhas comecadas por # ou conteudo apos #) e linhas vazias.
 * Limpa automaticamente espacos e aspas dos campos.
 */
struct Parser {
    /**
     * @brief Le e faz parsing do ficheiro CSV de entrada.
     *
     * Itera linha a linha, identificando a seccao atual pelo cabecalho e
     * processando cada linha de dados de acordo. Campos opcionais (dominio
     * secundario) sao tratados como -1 quando ausentes.
     *
     * @param filepath    Caminho para o ficheiro CSV de entrada.
     * @param submissions Vector a preencher com as submissoes lidas.
     * @param reviewers   Vector a preencher com os revisores lidos.
     * @param params      Estrutura a preencher com os parametros lidos.
     * @param control     Estrutura a preencher com as configuracoes de controlo lidas.
     * @return true se o ficheiro foi lido com sucesso, false caso contrario.
     *
     * @note Complexidade Temporal: O(L*F) onde L = linhas do ficheiro, F = campos por linha.
     */
    static bool parseCSV(const std::string &filepath, std::vector<Submission> &submissions,
                        std::vector<Reviewer> &reviewers, Parameters &params, Control &control);
};

#endif