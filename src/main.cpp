#include <iostream>
#include <iomanip>
#include "../include/Option.h"

using namespace std;

int main() {
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
    cout << "    Black-Scholes Options Pricing Engine v1.0" << endl;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
    cout << endl;
    
    // Get user inputs
    double S, K, T, r, sigma;
    
    cout << "Enter Stock Price (S): $";
    cin >> S;
    
    cout << "Enter Strike Price (K): $";
    cin >> K;
    
    cout << "Enter Time to Maturity (T) in years: ";
    cin >> T;
    
    cout << "Enter Risk-free Rate (r) as decimal (e.g., 0.05 for 5%): ";
    cin >> r;
    
    cout << "Enter Volatility (sigma) as decimal (e.g., 0.20 for 20%): ";
    cin >> sigma;
    
    cout << endl;
    cout << "Calculating..." << endl;
    cout << endl;
    
    try {
        // Create option object
        Option option(S, K, T, r, sigma);
        
        // Set output precision
        cout << fixed << setprecision(4);
        
        // Display Call Option Results
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
        cout << "                 CALL OPTION" << endl;
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
        cout << "Price:  $" << option.calculate_call_price() << endl;
        cout << "Delta:   " << option.calculate_delta_call() << endl;
        cout << "Gamma:   " << option.calculate_gamma() << endl;
        cout << "Theta:   " << option.calculate_theta_call() << endl;
        cout << "Vega:    " << option.calculate_vega() << endl;
        cout << "Rho:     " << option.calculate_rho_call() << endl;
        cout << endl;
        
        // Display Put Option Results
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
        cout << "                 PUT OPTION" << endl;
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
        cout << "Price:  $" << option.calculate_put_price() << endl;
        cout << "Delta:   " << option.calculate_delta_put() << endl;
        cout << "Gamma:   " << option.calculate_gamma() << endl;
        cout << "Theta:   " << option.calculate_theta_put() << endl;
        cout << "Vega:    " << option.calculate_vega() << endl;
        cout << "Rho:     " << option.calculate_rho_put() << endl;
        cout << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
