#include <iostream>
#include <fstream>
#include <cmath>
#include <exception>
#include <random>
#include <string>
#include <sstream>
#include <ctime>

#include "metro.h"

using namespace std;	

int main(int argc, char** argv){
	
	
	double	k = 8.6173303e-5; // eV/K
	double	J; // całka wymiany
	double	B; // pole 

	int L = 32; // wielkość siatki
	int NMC = 200; // ilość iteracji MC 
	
	ofstream out;
	out.exceptions (ofstream::failbit | ifstream::badbit | ofstream::badbit);	
	
	ifstream in;
	in.exceptions (ofstream::failbit | ifstream::badbit | ofstream::badbit);	

	string param_name = "input.param";
	
	// read perameters
	
	try{
		in.open(param_name, std::ios::in);
	
		in >> param_name >> J;
		in >> param_name >> B;
		
		in.close();
	
	}catch(exception& e){
	      cout << e.what() << endl;
	}

	cout << "J = " << J << endl;
	cout << "H = " << B << endl;


	// calculate Curie temperature
	double TCurie = abs(2*J/(k*log(1+sqrt(2))));
	cout << "TC = " << TCurie << endl;
	// calculate Tunit temperature
	double Tunit = abs(J/k);
	cout << "Tunit = " << Tunit << endl;

	
	// simulate from Tmin to Tmax with step = Tstep
	double Tstep = 0.015*Tunit;
	double	Tmin = 1*Tunit;
	double Tmax = 8*Tunit;
	
	// Creating operating table
	int ** tablica = new int* [L];
	for(int i = 0; i < L; ++i){
		tablica[i] = new int [L];
	}
	// Creating operating table
	int ** tablica_tmp = new int* [L];
	for(int i = 0; i < L; ++i){
		tablica_tmp[i] = new int [L];
	}


	try{
		// generator liczb losowych
		default_random_engine gen;
		gen.seed(time(0));
		
		// wypełnianie tablicy 
		wypelnijLosowo(tablica, L, gen);
	//	wypelnijSpinUP(tablica, L);

		// Checking the table data
		sprawdz(tablica, L);
		

		string out_name = "output.magnet";
		
		double M=0;
		double T = Tunit*1.5;
		double Hmax = 1.05;
		double Hstep = Hmax / 100.;
		out.open( out_name  , std::ios::out );
		while (B <= Hmax) {

			for (int i = 0; i < NMC; ++i){
				montecarlo(tablica, L, J, B, k*T, gen);
				//sekwencyjnie(tablica, L, J, H*mi, k*T, gen);
			}
			M = 0;
			for(int x = 0; x < L; ++x){
				for(int y = 0; y < L; ++y){
				M += tablica[x][y];
				}
			}
			M /= L*L;
			out << M << '\t' << B << endl;
			B += Hstep;
		}
		while (B >= -Hmax) {

			for (int i = 0; i < NMC; ++i){
				montecarlo(tablica, L, J, B, k*T, gen);
				//sekwencyjnie(tablica, L, J, H*mi, k*T, gen);
			}
			M = 0;
			for(int x = 0; x < L; ++x){
				for(int y = 0; y < L; ++y){
				M += tablica[x][y];
				}
			}
			M /= L*L;
			out << M << '\t' << B << endl;
			B -= Hstep;
		}
		while (B <= 0) {

			for (int i = 0; i < NMC; ++i){
				montecarlo(tablica, L, J, B, k*T, gen);
				//sekwencyjnie(tablica, L, J, H*mi, k*T, gen);
			}
			M = 0;
			for(int x = 0; x < L; ++x){
				for(int y = 0; y < L; ++y){
				M += tablica[x][y];
				}
			}
			M /= L*L;
			out << M << '\t' << B << endl;
			B += Hstep;
		}
		out.close();
		out_name = "output.dat";
		out.open( out_name , std::ios::out );
		// save output data
	  	for(int i = 0; i < L; ++i) {
			for(int j = 0; j < L; ++j){
				out << tablica[i][j] << '\t';
			}
			out << endl;
   		}
		out.close();

	}catch(exception& e){
	      cout << e.what() << endl;
	}

	return 0;
}

