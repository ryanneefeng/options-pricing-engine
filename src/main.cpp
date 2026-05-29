#include <iostream>
#include <iomanip>
#include "../include/option.h"
#include <limits>
#include "../include/MonteCarlo.h"

using namespace std;

double get_positive_input(const string& prompt){
	double value;
	while (true){
		cout << prompt;
		if (cin >> value && value > 0){
			return value;
		}
		else {
			cout << "Error: Please enter a positive number.\n";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
}

int main() {
	cout << "======================================================" << endl;
	cout << "    Black-Scholes Options Pricing Engine v1.0" << endl;
    	cout << "======================================================" << endl;
    	cout << endl;

    	// Get user inputs
    	double S, K, T, r, sigma;

	int numOptions;
	cout << "How many options would you like to run? " << endl;
	cin >> numOptions;

	for (int i = 1; i <= numOptions; i++){

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
        		cout << "======================================================" << endl;
        		cout << "                 CALL OPTION" << endl;
        		cout << "======================================================" << endl;
        		cout << "Price:  $" << option.calculate_call_price() << endl;
       	 		cout << "Delta:   " << option.calculate_delta_call() << endl;
        		cout << "Gamma:   " << option.calculate_gamma() << endl;
        		cout << "Theta:   " << option.calculate_theta_call() << endl;
        		cout << "Vega:    " << option.calculate_vega() << endl;
        		cout << "Rho:     " << option.calculate_rho_call() << endl;
        		cout << endl;

       			// Display Put Option Results
        		cout << "======================================================" << endl;
				cout << "                     PUT OPTION" << endl;
        		cout << "======================================================" << endl;
        		cout << "Price:  $" << option.calculate_put_price() << endl;
        		cout << "Delta:   " << option.calculate_delta_put() << endl;
        		cout << "Gamma:   " << option.calculate_gamma() << endl;
        		cout << "Theta:   " << option.calculate_theta_put() << endl;
        		cout << "Vega:    " << option.calculate_vega() << endl;
        		cout << "Rho:     " << option.calculate_rho_put() << endl;
        		cout << endl;

			//Monte Carlo Simulation Section
			cout << "======================================================" << endl;
	            	cout << "        	MONTE CARLO SIMULATION          " << endl;
	            	cout << "======================================================" << endl;
	
			MonteCarloSimulator mc(S, K, T, r, sigma, 100000, option);

			// Call option via Monte Carlo
			auto call_mc = mc.call_price_with_ci();
			double call_mc_price = call_mc.first;
			double call_mc_ci = call_mc.second;
	
			cout << "CALL OPTION (100,000 simulations)" << endl;
			cout << "Monte Carlo Price: $" << fixed << setprecision(4) << call_mc_price << " ± $" << call_mc_ci << endl;
			cout << "95% CI: [$" << call_mc_price - call_mc_ci << ", $" << call_mc_price + call_mc_ci << "]" << endl;
			cout << "Black-Scholes Price: $" << option.calculate_call_price() << endl;
			cout << "Difference: $" << call_mc_price - option.calculate_call_price() << endl;
	
			// Put option via Monte Carlo
			auto put_mc = mc.put_price_with_ci();
			double put_mc_price = put_mc.first;
			double put_mc_ci = put_mc.second;

			cout << "\nPUT OPTION (100,000 simulations)" << endl;
			cout << "Monte Carlo Price: $" << fixed << setprecision(4) << put_mc_price << " ± $" << put_mc_ci << endl;
			cout << "95% CI: [$" << put_mc_price - put_mc_ci << ", $" << put_mc_price + put_mc_ci << "]" << endl;
			cout << "Black-Scholes Price: $" << option.calculate_put_price() << endl;
			cout << "Difference: $" << put_mc_price - option.calculate_put_price() << endl;
			cout << endl;

			// Control Variates Section
			cout << "======================================================" << endl;
			cout << "           MONTE CARLO (CONTROL VARIATES)            " << endl;
			cout << "======================================================" << endl;
			
			auto call_cv = mc.call_price_with_cv();
			double call_cv_price = call_cv.first;
			double call_cv_ci = call_cv.second;

			cout << "CALL OPTION (100,000 simulations)" << endl;
			cout << "Control Variate Price: $" << fixed << setprecision(4) << call_cv_price << " +/- $" << call_cv_ci << endl;
			cout << "95% CI: [$" << call_cv_price - call_cv_ci << ", $" << call_cv_price + call_cv_ci << "]" << endl;
			cout << "Black-Scholes Price:   $" << option.calculate_call_price() << endl;

			auto put_cv = mc.put_price_with_cv();
			double put_cv_price = put_cv.first;
			double put_cv_ci = put_cv.second;

			cout << "\nPUT OPTION (100,000 simulations)" << endl;
			cout << "Control Variate Price: $" << fixed << setprecision(4) << put_cv_price << " +/- $" << put_cv_ci << endl;
			cout << "95% CI: [$" << put_cv_price - put_cv_ci << ", $" << put_cv_price + put_cv_ci << "]" << endl;
			cout << "Black-Scholes Price:   $" << option.calculate_put_price() << endl;
			cout << endl;

			cout << "======================================================" << endl;
        		cout << "                      VALIDATION                      " << endl;
			cout << "======================================================" << endl;
	        	double parity_error = option.verify_put_call_parity();
	       		cout << "Put-Call Parity Error: " << scientific << setprecision(2) << parity_error << endl;
	       		if (abs(parity_error) < 0.0001) {
	                	cout << "Calculations verified!" << endl;
	        	}
	        	else {
	               		cout << "Warning: Large parity error detected" << endl;
	       		}
	
	       		// Greek Summary Table
			cout << "======================================================" << endl;
			cout << "                    GREEK SUMMARY                     " << endl;
			cout << "======================================================" << endl;
			cout << left << setw(10) << ""
	    		     << setw(14) << "CALL"
	      		     << setw(14) << "PUT" << endl;
			cout << "------------------------------------------------------" << endl;
			cout << fixed << setprecision(4);
			cout << setw(10) << "Delta:"
			     << setw(14) << option.calculate_delta_call()
			     << setw(14) << option.calculate_delta_put() << endl;
			cout << setw(10) << "Gamma:"
			     << setw(14) << option.calculate_gamma()
			     << setw(14) << option.calculate_gamma() << endl;
			cout << setw(10) << "Theta:"
			     << setw(14) << option.calculate_theta_call()
			     << setw(14) << option.calculate_theta_put() << endl;
			cout << setw(10) << "Vega:"
			     << setw(14) << option.calculate_vega()
			     << setw(14) << option.calculate_vega() << endl;
			cout << setw(10) << "Rho:"
			     << setw(14) << option.calculate_rho_call()
			     << setw(14) << option.calculate_rho_put() << endl;
			cout << "======================================================" << endl;
		
			// Implied Volatility Section
			cout << "======================================================" << endl;
                        cout << "              IMPLIED VOLATILITY SOLVER               " << endl;
                        cout << "======================================================" << endl;
			cout << "Enter observed market price for call (0 to skip): $";
			double call_market_price;
			cin >> call_market_price;

			if (call_market_price > 0) {
				try {
					double call_iv = option.implied_volatility_call(call_market_price);
					cout << fixed << setprecision(4);
					cout << "Call Implied Volatility: " << call_iv * 100 << "%" << endl;
				}
				catch (const exception& e) {
					cout << "Call IV: " << e.what() << endl;
				}
			}
			
			cout << "Enter observed market price for put (0 to skip): $";
			double put_market_price;
			cin >> put_market_price;

			if (put_market_price > 0) {
				try {
					double put_iv = option.implied_volatility_put(put_market_price);
					cout << fixed << setprecision(4);
					cout << "Put Implied Volatility: " << put_iv * 100 << "%" << endl;
				}
				catch (const exception& e) {
					cout << "Put IV: " << e.what() << endl;
				}
			}
			cout << "======================================================" << endl;
		}
		catch (const exception& e) {
        		cerr << "Error: " << e.what() << endl;
        		continue;
		}
		cout << "\n";
    	}
    	return 0;
}
