#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>
#include <thread>
#include <future>

#include "functions.h"
#include "params.h"

#include<chrono>
#include<fstream>

std::ofstream out("Date.out");



using bitstring = std::vector<bool>;

std::random_device rd;
std::mt19937 gen(rd());
std::bernoulli_distribution dis(0.5);
params settings;

int Nbit() {
    int N = (settings.b - settings.a) * pow(10, settings.p);
    int bits = static_cast<int>(std::ceil(log2(N)));

    return bits;
}

int Cbit() {
    int bit = Nbit();
    return bit * settings.d;
}

bitstring Gen_num() {
    int bitcount = Cbit();
    bitstring vc(bitcount);
    for (int i = 0; i < vc.size(); i++) {
        vc[i] = dis(gen);
    }
    return vc;
}
double Convert(bitstring vc) {
    long long decval = 0;
    for (bool bit : vc) {
        decval = (decval << 1) | bit;
    }
    double finval = settings.a + ((static_cast<double>(decval) * (settings.b - settings.a)) / (pow(2, Nbit()) - 1));
    return finval;
}

double Eval(bitstring vc) {
    //double num = Convert(vc);
    int vs = vc.size(); // vector size
    int cs = vs / settings.d; //chunk size
    std::vector<double> results(settings.d);
    bitstring aux;
    for (int i = 0; i < settings.d; ++i) {
        aux.clear();
        for (int j = i * cs; j < cs + cs * i; ++j) {
            aux.push_back(vc[j]);
        }
        results[i] = Convert(aux);
    }
    return Sphere(results);
}

std::vector<bitstring> Neighbourhood(bitstring vc) {
    std::vector<bitstring> neigh;
    for (int i = 0; i < vc.size(); i++) {
        neigh.push_back(vc);
        neigh[i][i] = !neigh[i][i];
    }
    return neigh;
}

bitstring Improve(std::vector<bitstring> neigh, bitstring vc, bool impr) {
    double init;
    switch (impr) {
    case 0:
        init = Eval(vc);
        for (int i = 0; i < neigh.size(); i++) {
            double candidate = Eval(neigh[i]);
            if (init > candidate) {
                return neigh[i];
            }
        }
        return vc;
        break;

    case 1:
        init = Eval(neigh[0]);
        bitstring vn = neigh[0];
        for (int i = 1; i < neigh.size(); i++) {
            double candidate = Eval(neigh[i]);
            if (init > candidate) {
                init = candidate;
                vn = neigh[i];
            }
        }
        return vn;
        break;
    }
}

double local_vc() {
    bool reachedLocal = false;
    bitstring vc = Gen_num();
    double initcandidate = Eval(vc);

    do {
        bitstring vn = Improve(Neighbourhood(vc), vc, 1);

        if (Eval(vn) < initcandidate)
        {
            vc = vn;
            initcandidate = Eval(vc);
        }
        else
        {
            reachedLocal = true;
        }

       /// std::cout << "I am computing\n";

    } while (!reachedLocal);

    return initcandidate;
}







int main()
{

    int counter = 0;
    while (counter < 30) {
        int t = 0;
        bitstring best = Gen_num();
        double bestcandidate = Eval(best);
        int nthreads = 1;

        



        auto start = std::chrono::high_resolution_clock::now();

        while (t < settings.it) {
            //https://stackoverflow.com/questions/7686939/c-simple-return-value-from-stdthread/7687116#7687116
            std::vector<std::future<double>> futures(nthreads);
            std::vector<double> candidates(nthreads);

            for (int i = 0; i < nthreads; i++) {
                futures[i] = std::async(local_vc);
            }


            std::cout << "new candidates: ";
            for (int i = 0; i < nthreads; i++) {
                candidates[i] = futures[i].get();
                // std::cout << candidates[i] << " ";
            }

            // https://www.geeksforgeeks.org/how-to-find-minimum-element-in-vector-in-cpp/
            double minCandidate = *std::min_element(candidates.begin(), candidates.end());

            if (minCandidate < bestcandidate) {
                bestcandidate = minCandidate;
            }

            t += nthreads;
            std::cout << "\n\n\n" << t << " ------- " << bestcandidate << " <=> " << minCandidate << "\n\n\n";
        }

        std::cout <<"Rezulatul: "<< counter<<"/30";
        counter++;
        std::cout << "#" << t << " " << "candidate: " << bestcandidate << "\n";

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        out << bestcandidate << " " << duration.count()<<'\n';
        out.flush();
        
    }
    out.close();
    
    return 0;
}


