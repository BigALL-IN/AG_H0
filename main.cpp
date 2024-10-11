#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include "functions.h"
#include "params.h"
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

bitstring Gen_num(){
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
		for (int j = i * cs; j < cs+cs*i; ++j) {
			aux.push_back(vc[j]);
		}
		results[i] = Convert(aux);
	}
	return Rastrigin(results);
}

std::vector<bitstring> Neighbourhood(bitstring vc) {
	std::vector<bitstring> neigh;
	for (int i = 0; i < vc.size(); i++) {
		neigh.push_back(vc);
		neigh[i][i] = !neigh[i][i];
	}
	return neigh;
}

bitstring Improve(std::vector<bitstring> neigh, bool impr) {
	double init = Eval(neigh[0]);
	switch (impr) {
	case 0:
		for (int i = 1; i < neigh.size(); i++) {
			double candidate = Eval(neigh[i]);
			if (init > candidate) {
				return neigh[i];
			}
		}
		return neigh[0];  
		break;

	case 1:
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


int main() {
	int t = 0;
	bitstring best = Gen_num();
	int counter = 0;
	double bestcandidate = Eval(best);
	while (t < settings.it) {
		

		bool reachedLocal = false;
		bitstring vc = Gen_num();
		double initcandidate = Eval(vc);
		while (!reachedLocal) {
			std::cout << "loop count: "<< ++counter << "\n";
			bitstring vn = Improve(Neighbourhood(vc), 1);
			if (Eval(vn) < initcandidate) {
				vc = vn;
			}
			else reachedLocal = true;
		} 
		t++;
		double testcandidate = Eval(vc);
		if (testcandidate < bestcandidate) {
			best = vc;
			bestcandidate = testcandidate;
		}
		std::cout << "#" << t << " " << "candidate: " << bestcandidate << "\n";

	
	}
	return 0;
}



