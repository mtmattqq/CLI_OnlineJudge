#include <iostream>
#include <fstream>
#include <cstdint>
#include <algorithm>
#include <random>
#include <cassert>
#include <set>
#include <cstdint>
#include <chrono>

#include "Rng.h"
#include "GraphGen.h"

#define ALL(v) v.begin(),v.end()
using ll = long long;

const int SHIFT = 78634;

enum op {
    SLOW_GET = 1,
    FAST_GET,
    INSERT,
    REMOVE,
};

int solve(int i) {
    std::string file = "./Sol < " + std::to_string(i) + ".in" + "> " + std::to_string(i) + ".out";
    
    int exec_status = std::system(file.c_str());
    if(exec_status != 0) {
        std::cerr << "Execution failed.\n";
        return 1;
    }

    return 0;
}

void SubTesk1(int a){
    std::string fileName = std::to_string(a);

    std::ofstream ques(fileName + ".in");

    random_number_generater rng(a * 10 + SHIFT);

    int m{int(rng(99000, 100000))};
    ques << m << "\n";
    int64_t ops[]{
        2, 2, 2, 2, 2, 
        3, 3, 3, 3, 3, 
        3, 4
    };

    std::set<int64_t> s;
    for(int i{0}; i < m; ++i) {
        int64_t t{ops[rng(0, 11)]}, k;
        switch (t) {
        case SLOW_GET:
            k = rng(1e18);
            break;
        
        case FAST_GET:
            k = rng(1e18);
            break;
        
        case INSERT:
            k = rng(1e18);
            while(s.count(k)) k = rng(1e18);
            s.insert(k);
            break;
        
        case REMOVE:
            k = rng(1e18);
            if(s.count(k)) s.erase(k);
            break;
        
        default:
            std::cerr << "Something Wrong" << "\n";
            exit(-1);
            break;
        }
        ques << t << " " << k << "\n";
    }
}

void SubTesk2(int a){
    std::string fileName = std::to_string(a);

    std::ofstream ques(fileName + ".in");

    random_number_generater rng(a * 10 + SHIFT);

    int m{int(rng(99000, 100000))};
    ques << m << "\n";
    int ops[]{2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3};

    std::set<int64_t> s;
    for(int i{0}; i < m; ++i) {
        int64_t t{ops[rng(0, 11)]}, k;
        switch (t) {
        case SLOW_GET:
            k = rng(1e18);
            break;
        
        case FAST_GET:
            k = rng(1e18);
            break;
        
        case INSERT:
            k = rng(1e18);
            while(s.count(k)) k = rng(1e18);
            s.insert(k);
            break;
        
        case REMOVE:
            k = rng(1e18);
            if(s.count(k)) s.erase(k);
            break;
        
        default:
            std::cerr << "Something Wrong" << "\n";
            exit(-1);
            break;
        }
        ques << t << " " << k << "\n";
    }

    ques << rng(1000000) << " " << rng(1000000) << "\n";
}

#define REP(i,a,b) for(int i=(a);i<=(b);++i)
int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    
    int compile_status = system("g++ Sol.cpp -std=c++14 -O2 -o Sol");
    if(compile_status != 0) {
        std::cerr << "Compilation failed.\n";
        return 1;
    } else {
        std::cerr << "Compilation success\n";
    }

    const int TEST_CASE = 10;

    using time_point = std::chrono::steady_clock::time_point;
    time_point start = std::chrono::steady_clock::now();
    
    REP(i, 1, 5) {
        SubTesk1(i);
        std::cerr << "Finishing generating the test " << i << "\n";
    }

    REP(i, 6, TEST_CASE) {
        SubTesk2(i);
        std::cerr << "Finishing generating the test " << i << "\n";
    }

    REP(i, 1, TEST_CASE) {
        solve(i);
        std::cerr << "Successfully solve testcase : " << i << "\n";
    }

    time_point end = std::chrono::steady_clock::now();

    std::cerr << "Total time : " << 
        double(std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count()) / 1000.0 
         << "\n";
}
