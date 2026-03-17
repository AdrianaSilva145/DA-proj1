#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "ParseCSV.cpp"

int main(int argc, char* argv[]) {
    if (argc >= 3 && std::string(argv[1]) == "-b") {            //ta no modo batch (o certo)
        std::string inputfile = argv[2];
        std::string riskfile = (argc==4) ? argv[3] : "";        //pode nao haver um riskfile a ser passado
        std::cout << "Executing:  " << inputfile << std::endl;
        if (!parseCSV(inputfile)) return 1;

        //TODO: continuar a fazer isto com cenas max flow

        return 0;
    }

    int option = -1;            //nao ta no batch
    while (option != 0) {
        showMenu();
        if (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n'); //limpar lixo do buffer, 10000 é um número grande o suficiente
            continue;
        }

        switch (option) {
            case 1:
                std::cout << "File Name: " << std::endl;
            //TODO: ler input e chamar o parser
            break;
            case 2:
                std::cout << "Executing Max-Flow algorithm..." << std::endl;
            //TODO: tarefa relativa ao riskanalysis etc
            break;
            case 3:
                std::cout << "Showing Results..." << std::endl;
            //TODO: possivelmente mostrar atribuições no terminal
            case 0:
                std::cout << "Exiting..." << std::endl;
            default:
                std::cout << "Invalid Option!" << option << std::endl;
        }
    }
}