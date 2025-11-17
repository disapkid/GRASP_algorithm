#include <GRASP.hpp>
#include <cassert>

namespace {

    void build_occurrences(
        const Formula& formula,
        std::vector<std::vector<int>>& pos_occ,
        std::vector<std::vector<int>>& neg_occ
    ) {
        int n = formula.bool_val_amount;
        int m = static_cast<int>(formula.clauses.size());

        pos_occ.assign(n + 1, {});
        neg_occ.assign(n + 1, {});

        for (int ci = 0; ci < m; ++ci) {
            const Clause& cl = formula.clauses[ci];
            for (int lit : cl.literals) {
                int v = std::abs(lit);
                if (v < 1 || v > n) continue; 
                if (lit > 0)
                    pos_occ[v].push_back(ci);
                else
                    neg_occ[v].push_back(ci);
            }
        }
    }

    int init_clause_sat_counts(
        const Formula& formula,
        const std::vector<int>& x,
        std::vector<int>& clause_sat_count
    ) {
        int m = static_cast<int>(formula.clauses.size());
        clause_sat_count.assign(m, 0);
        int total_sat = 0;

        for (int ci = 0; ci < m; ++ci) {
            const Clause& cl = formula.clauses[ci];
            int cnt = 0;
            for (int lit : cl.literals) {
                int v = std::abs(lit);
                int val = x[v];          
                if (val == -1) continue; 
                bool lit_true = (lit > 0 ? (val == 1) : (val == 0));
                if (lit_true) {
                    ++cnt;
                }
            }
            clause_sat_count[ci] = cnt;
            if (cnt > 0) ++total_sat;
        }

        return total_sat;
    }

    int compute_flip_delta(
        int v,
        const Formula& formula,
        const std::vector<int>& x,
        const std::vector<std::vector<int>>& pos_occ,
        const std::vector<std::vector<int>>& neg_occ,
        const std::vector<int>& clause_sat_count
    ) {
        int old_val = x[v];
        int new_val = 1 - old_val;

        int delta_clauses = 0;

        for (int ci : pos_occ[v]) {
            int t = clause_sat_count[ci];
            int t_new = t;

            bool was_true  = (old_val == 1);
            bool will_true = (new_val == 1);

            if (was_true && !will_true) {
                t_new -= 1;
            } else if (!was_true && will_true) {
                t_new += 1;
            }

            if (t > 0 && t_new == 0)      delta_clauses--;
            else if (t == 0 && t_new > 0) delta_clauses++;
        }

        for (int ci : neg_occ[v]) {
            int t = clause_sat_count[ci];
            int t_new = t;

            bool was_true  = (old_val == 0);
            bool will_true = (new_val == 0);

            if (was_true && !will_true) {
                t_new -= 1;
            } else if (!was_true && will_true) {
                t_new += 1;
            }

            if (t > 0 && t_new == 0)      delta_clauses--;
            else if (t == 0 && t_new > 0) delta_clauses++;
        }

        return delta_clauses;
    }

    void apply_flip(
        int v,
        const Formula& formula,
        std::vector<int>& x,
        const std::vector<std::vector<int>>& pos_occ,
        const std::vector<std::vector<int>>& neg_occ,
        std::vector<int>& clause_sat_count,
        int& current_satis
    ) {
        int old_val = x[v];
        int new_val = 1 - old_val;

        for (int ci : pos_occ[v]) {
            int t = clause_sat_count[ci];
            int t_new = t;

            bool was_true  = (old_val == 1);
            bool will_true = (new_val == 1);

            if (was_true && !will_true) {
                t_new -= 1;
            } else if (!was_true && will_true) {
                t_new += 1;
            }

            if (t > 0 && t_new == 0)      --current_satis;
            else if (t == 0 && t_new > 0) ++current_satis;

            clause_sat_count[ci] = t_new;
        }

        for (int ci : neg_occ[v]) {
            int t = clause_sat_count[ci];
            int t_new = t;

            bool was_true  = (old_val == 0);
            bool will_true = (new_val == 0);

            if (was_true && !will_true) {
                t_new -= 1;
            } else if (!was_true && will_true) {
                t_new += 1;
            }

            if (t > 0 && t_new == 0)      --current_satis;
            else if (t == 0 && t_new > 0) ++current_satis;

            clause_sat_count[ci] = t_new;
        }

        x[v] = new_val;
    }

} 

int GRASP::evaluate(const Formula &formula, const std::vector<int> &x)
{
    int total = 0;
    for(auto& clause : formula.clauses) {
        bool is_true = false;

        for(auto& val : clause.literals) {
            int index = abs(val);
            int literal_val = x[index];
            if((val > 0 && literal_val == 1) || (val < 0 && literal_val == 0)) {
                is_true = true;
                break;
            }

        }

        if(is_true) total++;
    }

    return total;
}

void GRASP::grasp_construct(const Formula& formula, std::vector<int>& x, int rcl_size, std::mt19937& mt) {
    const int n = formula.bool_val_amount;
    const int m = static_cast<int>(formula.clauses.size());

    x.assign(n + 1, -1);

    std::vector<std::vector<int>> pos_occ, neg_occ;
    build_occurrences(formula, pos_occ, neg_occ);

    std::vector<int> clause_sat_count(m, 0);
    int current_satis = 0; 

    std::vector<int> unassigned(n);
    std::iota(unassigned.begin(), unassigned.end(), 1);

    while (!unassigned.empty()) {
        std::vector<Candidate> candidates;
        candidates.reserve(unassigned.size() * 2);

        for (int v : unassigned) {
            int delta0 = 0;
            for (int ci : neg_occ[v]) {
                if (clause_sat_count[ci] == 0) {
                    ++delta0;
                }
            }

            int delta1 = 0;
            for (int ci : pos_occ[v]) {
                if (clause_sat_count[ci] == 0) {
                    ++delta1;
                }
            }

            candidates.push_back({v, 0, delta0});
            candidates.push_back({v, 1, delta1});
        }

        std::sort(candidates.begin(), candidates.end(),
                  [](const Candidate& a, const Candidate& b) {
                      return a.delta > b.delta;
                  });

        int k = std::min(rcl_size, static_cast<int>(candidates.size()));
        if (k <= 0) {
            for (int v : unassigned) x[v] = 0;
            break;
        }

        std::uniform_int_distribution<int> dist(0, k - 1);
        Candidate chosen = candidates[dist(mt)];

        int v   = chosen.index;
        int val = chosen.value;

        x[v] = val;
        current_satis += chosen.delta;

        const std::vector<int>& occ_true = (val == 1 ? pos_occ[v] : neg_occ[v]);
        for (int ci : occ_true) {
            if (clause_sat_count[ci] == 0) {
                clause_sat_count[ci] = 1;
            } else {
                clause_sat_count[ci] += 1;
            }
        }

        auto it = std::find(unassigned.begin(), unassigned.end(), v);
        if (it != unassigned.end()) unassigned.erase(it);
    }

#ifndef NDEBUG
    for (int v = 1; v <= n; ++v) {
        assert(x[v] == 0 || x[v] == 1);
    }
#endif
}

void GRASP::local_search(const Formula& formula, std::vector<int>& x) {
    const int n = formula.bool_val_amount;

    std::vector<std::vector<int>> pos_occ, neg_occ;
    build_occurrences(formula, pos_occ, neg_occ);

    std::vector<int> clause_sat_count;
    int current_satis = init_clause_sat_counts(formula, x, clause_sat_count);

    while (true) {
        int best_delta = 0;
        int best_v = -1;

        for (int v = 1; v <= n; ++v) {
            int val = x[v];
            if (val != 0 && val != 1) {
                continue;
            }

            int delta = compute_flip_delta(v, formula, x, pos_occ, neg_occ, clause_sat_count);
            if (delta > best_delta) {
                best_delta = delta;
                best_v = v;
            }
        }

        if (best_delta <= 0 || best_v == -1) {
            break;
        }

        apply_flip(best_v, formula, x, pos_occ, neg_occ, clause_sat_count, current_satis);
    }

#ifndef NDEBUG
    int check = GRASP::evaluate(formula, x);
    assert(check == current_satis);
#endif
}

int GRASP::iteration(const Formula& formula, int rcl_size, std::mt19937& mt) {
    std::vector<int> x;
    GRASP::grasp_construct(formula, x, rcl_size, mt);
    GRASP::local_search(formula, x);

    int res = GRASP::evaluate(formula, x);
    return res;
}