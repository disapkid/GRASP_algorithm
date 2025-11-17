#include <parser.hpp>

Formula Parser::parse(const std::string &path) {
    Formula result;

    std::ifstream fstream(path);
    std::string line;

    while (getline(fstream, line)) {
        if (line.empty() || line[0] == 'c') continue;

        if (line[0] == 'p') {
            std::istringstream iss(line);
            std::string p, cnf;
            int num_vars, num_clauses;

            iss >> p >> cnf >> num_vars >> num_clauses;

            if (!iss || cnf != "cnf") {
                std::cerr << "Ошибка парсинга строки p: " << line << "\n";
                continue;
            }

            result.bool_val_amount = num_vars;
            result.clauses.reserve(num_clauses);
            continue;
        }

        std::istringstream iss(line);
        int lit;
        Clause clause;

        while (iss >> lit && lit != 0) {
            clause.literals.push_back(lit);
        }

        if (!clause.literals.empty()) {
            result.clauses.push_back(std::move(clause));
        }
    }

    return result;
}
