#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>

struct Clause {
    std::vector<int> literals;  // литералы в формате DIMACS: положительное число = x, отрицательное = ¬x
                                // клауза представляет собой результат дизъюнкции нескольких литералов 
};

struct Formula {
    int bool_val_amount;        // общее количество булевых переменных
    std::vector<Clause> clauses;// массив клауз
};

class Parser final {
public:
    /**
     * @brief Функция для парсинга входных данных. Входные данные взяты с сайта: 
     *  http://archive.dimacs.rutgers.edu/pub/challenge/satisfiability/benchmarks/cnf/
     * 
     *  Данные были за основу эксперимента в статье:
     *  https://sci2s.ugr.es/sites/default/files/files/Teaching/GraduatesCourses/Metaheuristicas/Bibliography/gtut.pdf
     */
    static Formula parse(const std::string& path);
};