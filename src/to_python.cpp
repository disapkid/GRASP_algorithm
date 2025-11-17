#include <to_python.hpp>

using namespace nlohmann;

json Data::Prepare_data(
        const Formula& formula,
        const std::string& formula_name,
        const std::vector<int>& rcl_sizes,
        int iterations) {
    json result;
    result["formula"] = formula_name;
    result["iterations"] = iterations;

    std::random_device rd;
    std::mt19937 gen(rd());

    json results_json = json::array();

    for (int rcl : rcl_sizes) {
        long long total = 0;
        int best_val = std::numeric_limits<int>::min();
        int worst_val = std::numeric_limits<int>::max();

        std::unordered_map<int, int> distribution;
        distribution.reserve(iterations);

        for (int i = 0; i < iterations; i++) {
            int value = GRASP::iteration(formula, rcl, gen);

            total += value;
            distribution[value]++;

            if (value > best_val) best_val = value;
            if (value < worst_val) worst_val = value;
        }

        double mean = static_cast<double>(total) / iterations;

        json dist_json;
        for (auto& p : distribution) {
            dist_json[std::to_string(p.first)] = p.second;
        }

        json entry;
        entry["rcl_size"] = rcl;
        entry["mean"] = mean;
        entry["best"] = best_val;
        entry["worst"] = worst_val;
        entry["distribution"] = dist_json;

        results_json.push_back(entry);
    }

    result["results"] = results_json;

    return result;
}