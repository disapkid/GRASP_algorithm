#include <iostream>
#include <fstream>
#include <GRASP.hpp>
#include <parser.hpp>
#include <to_python.hpp>
#include <chrono>

int main() {
    auto start = std::chrono::high_resolution_clock().now();

    Formula form = Parser::parse("../data/ssa7552-160.cnf");
    std::cout << "Parsing data" << std::endl;

    std::vector<int> rcls = {1, 2, 4, 8, 16, 32, 64, 128, 256};

    int iterations = 10;
    std::cout << "starting grasp" << std::endl;
    nlohmann::json data = Data::Prepare_data(
        form,
        "ssa7552-160.cnf",
        rcls,
        iterations
    );

    std::string out_path = "../data/result.json";
    std::ofstream out(out_path);

    out << data.dump(4);
    out.close();

    std::string cmd = "python3 ../python/result.py";
    int code = std::system(cmd.c_str());
    if (code != 0) {
        std::cerr << "Ошибка при вызове python-скрипта, код: " << code << "\n";
    }

    auto end = std::chrono::high_resolution_clock().now();
    auto res = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << res << " сек" << std::endl;
    return 0;
}