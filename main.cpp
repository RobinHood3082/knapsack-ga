#include "bits/stdc++.h"

using namespace std;

std::random_device dev;
std::mt19937 rng(dev());

class subset {
public:
    string dna;
    int fitness;

    subset() {
        dna = "";
        fitness = 0;
    }

    subset(const string& _dna, int _fitness) {
        dna = _dna;
        fitness = _fitness;
    }

    int get_fitness() {return fitness;}

    int calc_fitness(const vector <vector <int>>& profit_vs_weight, const vector <int>& limits) {
        int m = limits.size();
        fitness = 0;

        vector <int> cur_limits(m, 0);
        int n = profit_vs_weight.size();
        for (int i = 0; i < n; i++) {
            if (dna[i] == '0') continue;

            for (int j = 0; j < m; j++) {
                cur_limits[j] += profit_vs_weight[i][j + 1];
                if (cur_limits[j] > limits[j]) {
                    return fitness = -1;
                }

                fitness += profit_vs_weight[i][0];
            }
        }

        return fitness;   
    }

    bool operator<(const subset& other) const {
        return fitness > other.fitness;
    }
};

pair <subset, subset> selection_by_fitness(const set <subset>& st) {
    int ttl = 0;
    for (const subset& s : st) {
        ttl += s.fitness;
    }

    std::uniform_int_distribution<std::mt19937::result_type> dist(1,ttl);

    auto it = st.begin(), jt = st.begin();
    int r1 = dist(rng);

    int accum = 0;
    for (auto kt = st.begin(); kt != st.end(); ++kt) {
        accum += kt->fitness;
        it = kt;
        if (accum >= r1) break;
    }

    do {
        int r2 = dist(rng);
        accum = 0;
        for (auto kt = st.begin(); kt != st.end(); ++kt) {
            accum += kt->fitness;
            jt = kt;
            if (accum >= r2) break;
        } 

    } while (it != jt);

    return {*it, *jt};
}

subset crossover(const subset& parent1, const subset& parent2) {
    std::uniform_int_distribution<std::mt19937::result_type> dist1(0,1);

    int n = parent1.dna.size();
    subset child = parent1;  
    
    for (int i = 0; i < n; i++) {
        int r = dist1(rng);
        if (r == 1) child.dna[i] = parent2.dna[i];
    }

    return child;
}

void mutation(subset& child, double mutation_rate) {
    std::uniform_int_distribution<std::mt19937::result_type> rand_gen(1,1000000);

    int mutation_variable = (mutation_rate * 1000000);

    int n = child.dna.size();
    for (int i = 0; i < n; i++) {
        int r = rand_gen(rng);
        if (r <= mutation_variable) {
            // flip dna
            int x = child.dna[i] - '0';
            x = 1 - x;
            child.dna[i] = '0' + x;
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;
    int population_size;
    cin >> population_size;
    int max_gen;
    cin >> max_gen;
    
    vector <vector<int>> data_matrix;
    for (int i = 0; i < n; i++) {
        data_matrix.push_back(vector <int>(m + 1, 0));
        for (int j = 0; j <= m; j++) cin >> data_matrix[i][j];
    }

    vector <int> limits;
    for (int i = 0; i < m; i++) {
        int x; cin >> x;
        limits.push_back(x);
    }

    double mutation_rate;
    cin >> mutation_rate;

    set <subset> st;

    std::uniform_int_distribution<std::mt19937::result_type> dist1(0,1);
    
    int ans = 0;
    int cnt = 0;
    subset candidate;
    while (st.size() < population_size and cnt < 1000000) {
        cnt++;
        string dna = "";
        for (int i = 0; i < n; i++) {
            int r = dist1(rng);
            dna += r + '0';
        }

        subset s(dna, 0);
        if (s.calc_fitness(data_matrix, limits) == -1) continue;

        st.insert(s);
        if (s.fitness > ans) {
            ans = s.fitness;
            candidate = s;
        }
    }

    if (st.empty()) {
        cout << "NO SUITABLE POPULATION FOUND\n";
        return 0;
    }

    for (int g = 0; g < max_gen; g++) {
        cout << "\nGeneration <" << g << "> current_max: " << ans << endl;
        cout << "===" << endl;

        cnt = 0;
        for (const subset& s : st) {
            cout << "Population " << cnt++ << " : DNA = [" << s.dna << "] : FITNESS = " << s.fitness << endl; 
        }

        pair <subset, subset> p = selection_by_fitness(st);
        cout << "SELECTION COMPLETE" << endl;
        subset child = crossover(p.first, p.second);
        mutation(child, mutation_rate);
        cout << "Child : DNA = [" << child.dna <<"] ";

        if (child.calc_fitness(data_matrix, limits) == -1) {
            cout << "NOT_SUITABLE" << endl;
            continue;
        }
        
        cout << "FITNESS =  " << child.fitness << endl;
        if (child.fitness > ans) {
            ans = child.fitness;
            candidate = child;
        }

        st.insert(child);
        if (st.size() > population_size) st.erase(prev(st.end()));

        cout << "==============" << endl;
    }

    cout << endl << endl;
    cout << "Final Candidate: " << candidate.dna << endl;
    cout << "Fitness: " << candidate.fitness << endl;
}