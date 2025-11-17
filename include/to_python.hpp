#pragma once
#include <nlohmann/json.hpp>
#include <GRASP.hpp>

class Data final {
public:
    static nlohmann::json Prepare_data(
        const Formula& formula,
        const std::string& formula_name,
        const std::vector<int>& rcl_sizes,
        int iterations);
};