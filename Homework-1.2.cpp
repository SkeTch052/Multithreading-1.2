#include <iostream>
#include <thread>
#include <execution>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>
#include <locale.h>
#include <iomanip>

void random_gen(std::vector<int> &vec, int size) {
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist(0, size);
    std::generate(std::execution::par, vec.begin(), vec.end(), [&]() {
        return dist(gen);
        });
}

std::vector<int> vec_sum(const std::vector<int>& vec1, const std::vector<int>& vec2, int size) {
    std::vector<int> result(size);

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < size; i++) {
        result[i] = vec1[i] + vec2[i];
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds> (end - start);
    std::cout << std::fixed << std::setprecision(7) << diff.count() / 1000000.0 << "s\t";
    return result;
}

std::vector<int> vec_sum_2tr(const std::vector<int>& vec1, const std::vector<int>& vec2, int size) {
    std::vector<int> result(size);

    auto start = std::chrono::steady_clock::now();

    std::thread thread1([&]() {
        for (int i = 0; i < size / 2; i++) {
            result[i] = vec1[i] + vec2[i];
        }
    });
    std::thread thread2([&]() {
        for (int i = size / 2; i < size; i++) {
            result[i] = vec1[i] + vec2[i];
        }
    });

    thread1.join();
    thread2.join();

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds> (end - start);
    std::cout << std::fixed << std::setprecision(7) << diff.count() / 1000000.0 << "s\t";
    return result;
}

std::vector<int> vec_sum_multithread(const std::vector<int>& vec1, const std::vector<int>& vec2, int size, int thr_count) {

    std::vector<int> result(size);
    std::vector<std::thread> threads;
    int chunk_size = size / 4;

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < 4; i++) {
        threads.emplace_back([&, i]() {
            int start = i * chunk_size;
            int end = (i + 1) * chunk_size;
            for (int j = start; j < end; j++) {
                result[j] = vec1[j] + vec2[j];
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds> (end - start);
    std::cout << std::fixed << std::setprecision(7) << diff.count() / 1000000.0 << "s\t";
    return result;
}

int main()
{
    setlocale(LC_ALL, "RUSSIAN");

    const unsigned int SIZE_1_000 = 1'000,
                       SIZE_10_000 = 10'000, 
                       SIZE_100_000 = 100'000, 
                       SIZE_1_000_000 = 1'000'000;

    //___________________________________________ ЗАПОЛНЕНИЕ ВЕКТОРОВ _____________________________________________________

    std::vector<int> vec1_000_1(SIZE_1_000); random_gen(vec1_000_1, SIZE_1_000);
    std::vector<int> vec1_000_2(SIZE_1_000); random_gen(vec1_000_2, SIZE_1_000);

    std::vector<int> vec10_000_1(SIZE_10_000); random_gen(vec10_000_1, SIZE_10_000);
    std::vector<int> vec10_000_2(SIZE_10_000); random_gen(vec10_000_2, SIZE_10_000);

    std::vector<int> vec100_000_1(SIZE_100_000); random_gen(vec100_000_1, SIZE_100_000);
    std::vector<int> vec100_000_2(SIZE_100_000); random_gen(vec100_000_2, SIZE_100_000);

    std::vector<int> vec1_000_000_1(SIZE_1_000_000); random_gen(vec1_000_000_1, SIZE_1_000_000);
    std::vector<int> vec1_000_000_2(SIZE_1_000_000); random_gen(vec1_000_000_2, SIZE_1_000_000);

    //________________________________________________ 1 ПОТОК ____________________________________________________________
    
    std::thread thr1([&]() { 
        std::cout << "Колличество аппартаных ядер - " << std::thread::hardware_concurrency() << "\n" << std::endl;
        std::cout << "\t \t" << "  1'000      \t" << "  10'000      \t" << "  100'000      \t" << " 1'000'000      " << std::endl;
        std::cout << "1 поток \t";
        vec_sum(vec1_000_1, vec1_000_2, SIZE_1_000); 
        vec_sum(vec10_000_1, vec10_000_2, SIZE_10_000);
        vec_sum(vec100_000_1, vec100_000_2, SIZE_100_000);
        vec_sum(vec1_000_000_1, vec1_000_000_2, SIZE_1_000_000);
    });
    thr1.join();

    //_______________________________________________ 2 ПОТОКА ____________________________________________________________
    std::cout << "\n2 потока\t";
    
    vec_sum_2tr(vec1_000_1, vec1_000_2, SIZE_1_000);
    vec_sum_2tr(vec10_000_1, vec10_000_2, SIZE_10_000);
    vec_sum_2tr(vec100_000_1, vec100_000_2, SIZE_100_000);
    vec_sum_2tr(vec1_000_000_1, vec1_000_000_2, SIZE_1_000_000);

    //_______________________________________________ 4 ПОТОКА ____________________________________________________________
    std::cout << "\n4 потока\t";

    vec_sum_multithread(vec1_000_1, vec1_000_2, SIZE_1_000, 4);
    vec_sum_multithread(vec10_000_1, vec10_000_2, SIZE_10_000, 4);
    vec_sum_multithread(vec100_000_1, vec100_000_2, SIZE_100_000, 4);
    vec_sum_multithread(vec1_000_000_1, vec1_000_000_2, SIZE_1_000_000, 4);

    //_______________________________________________ 8 ПОТОКОВ ____________________________________________________________
    std::cout << "\n8 потоков\t";

    vec_sum_multithread(vec1_000_1, vec1_000_2, SIZE_1_000, 8);
    vec_sum_multithread(vec10_000_1, vec10_000_2, SIZE_10_000, 8);
    vec_sum_multithread(vec100_000_1, vec100_000_2, SIZE_100_000, 8);
    vec_sum_multithread(vec1_000_000_1, vec1_000_000_2, SIZE_1_000_000, 8);

    //_______________________________________________ 16 ПОТОКОВ ___________________________________________________________
    std::cout << "\n16 потоков\t";

    vec_sum_multithread(vec1_000_1, vec1_000_2, SIZE_1_000, 16);
    vec_sum_multithread(vec10_000_1, vec10_000_2, SIZE_10_000, 16);
    vec_sum_multithread(vec100_000_1, vec100_000_2, SIZE_100_000, 16);
    vec_sum_multithread(vec1_000_000_1, vec1_000_000_2, SIZE_1_000_000, 16);

    //_______________________________________________ 32 ПОТОКА ____________________________________________________________
    std::cout << "\n32 потока\t";

    vec_sum_multithread(vec1_000_1, vec1_000_2, SIZE_1_000, 32);
    vec_sum_multithread(vec10_000_1, vec10_000_2, SIZE_10_000, 32);
    vec_sum_multithread(vec100_000_1, vec100_000_2, SIZE_100_000, 32);
    vec_sum_multithread(vec1_000_000_1, vec1_000_000_2, SIZE_1_000_000, 32);
    
    return 0;
}