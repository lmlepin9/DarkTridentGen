#include <iostream>
#include <fstream>
#include <string>
#include "DMscattering.h"



/*

Code to plot BdNMC DM-electron xsec


*/




int main(){
    // masses in GeV
    double mX = 0.03;
    double mA = 0.05;
    //double alpha_D = 0.1;
    double alpha_D = 1.9894368e-08;
    double epsilon = 1e-3;
    double dm_energy = 0.4; // in GeV 
    std::cout << "Running script to retrieve DM-electron scattering xsec..." << std::endl;
    std::cout << "\n" << std::endl;
    std::cout << "Model parameters: " << std::endl;
    std::cout << "Chi mass: " << mX << std::endl;
    std::cout << "Dark photon mass: " << mA << std::endl;
    std::cout << "Epsilon: " << epsilon << std::endl;
    std::cout << "AlphaD: " << alpha_D << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "\n" << std::endl;

    double conversion_factor{0.3894*1e9};

    double xsec_val = sigma (dm_energy, mX, mA, epsilon, alpha_D);
    std::cout << "The value of the xsec at " << dm_energy <<  " GeV is: " << xsec_val*conversion_factor <<  " pb" << std::endl;
    std::cout << "Generating xsec csv file..." << std::endl;
    double e_start = mX+mA + 0.001;
    double e_end = 10.;
    double n_points = 100;
    double d_e = (e_end - e_start)/n_points;


    std::string output_file_name = "dm_electron_xsec_mx_" + std::to_string(mX) + "_bdnmc_test.csv";
    std::ofstream outputFile(output_file_name);

    // Check if the file opened successfully
    if (!outputFile) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    
    outputFile << "E_val,xsec_val" << std::endl;
    for(int i{0}; i < n_points; i++){
        double temp_energy = e_start + d_e*i;
        outputFile << temp_energy << "," << sigma (temp_energy, mX, mA, epsilon, alpha_D) << std::endl;
    }

    outputFile.close();
    return 0;
}