#include <iostream>
#include <iomanip>
#include "../include/Option.h"
#include <limits>

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
			cout << "                 PUT OPTION" << endl;
        		cout << "======================================================" << endl;
        		cout << "Price:  $" << option.calculate_put_price() << endl;
        		cout << "Delta:   " << option.calculate_delta_put() << endl;
        		cout << "Gamma:   " << option.calculate_gamma() << endl;
        		cout << "Theta:   " << option.calculate_theta_put() << endl;
        		cout << "Vega:    " << option.calculate_vega() << endl;
        		cout << "Rho:     " << option.calculate_rho_put() << endl;
        		cout << endl;

        		cout << "======================================================" << endl;
        		cout << "                     VALIDATION                      " << endl;
        		cout << "======================================================" << endl;
        		double parity_error = option.verify_put_call_parity();
        		cout << "Put-Call Parity Error: " << scientific << setprecision(2) << parity_error << endl;
        		if (abs(parity_error) < 0.0001) {
                 		cout << "Calculations verified!" << endl;
        		}
        		else {
                		cout << "Warning: Large parity error detected" << endl;
        		}

        		cout << "======================================================" << endl;
			cout << "                    CALL ANALYSIS                       " << endl;
			cout << "======================================================" << endl;

			// Delta Analysis
			if (option.calculate_delta_call() <= 0.25){
    			cout << "Delta: Delta is pretty low, so the price movement won't help much. Basically, don't expect it to move unless something dramatic happens." << endl;
			}
			else if (option.calculate_delta_call() > 0.25 && option.calculate_delta_call() <= 0.50){
    			cout << "Delta: This option will move a bit with the stock, but still not something you would hedge with (more speculative than strategic)." << endl;
			}
			else if (option.calculate_delta_call() > 0.50 && option.calculate_delta_call() <= 0.70){
    			cout << "Delta: Good value. Reacts strongly to price changes and good for directional trades." << endl;
			}
			else if (option.calculate_delta_call() > 0.70 && option.calculate_delta_call() <= 0.90){
   	 		cout << "Delta: Good for leveraging with a lower upfront cost. Not cheap, but strong." << endl;
			}
			else if (option.calculate_delta_call() > 0.90 && option.calculate_delta_call() <= 1.00){ 
    			cout << "Delta: You're basically holding the stock. Not much optionality." << endl;
			}

			// Gamma Analysis
			if (option.calculate_gamma() < 0.01){
    			cout << "Gamma: Very stable, won't need rebalancing and not much convexity." << endl;
			}
			else if (option.calculate_gamma() >= 0.01 && option.calculate_gamma() < 0.03){
    			cout << "Gamma: Healthy value as the Delta will move noticeably. Good for trading and Gamma scalping." << endl;
			}
			else if (option.calculate_gamma() >= 0.03){
    			cout << "Gamma: High Gamma. Dangerous if hedging, but great for long options and are aiming for volatility pops." << endl;
			}

			// Theta Analysis
			if (option.calculate_theta_call() > -2){
    			cout << "Theta: Mild time decay, good for holding long-term." << endl;
			}
			else if (option.calculate_theta_call() >= -6 && option.calculate_theta_call() <= -2){
    			cout << "Theta: Moderate time decay. Should only hold if you expect a move soon." << endl;
			}
			else if (option.calculate_theta_call() < -6){
    			cout << "Theta: Heavy time decay: Good for shorting, risky for long-term." << endl;
			}

			// Vega Analysis
			if (option.calculate_vega() < 20){
    			cout << "Vega: Low volatility sensitivity. IV shifts won't affect price much." << endl;
			}
			else if (option.calculate_vega() >= 20 && option.calculate_vega() < 50){
    			cout << "Vega: Medium sensitivity. Good if you expect rising uncertainty." << endl;
			}
			else if (option.calculate_vega() >= 50){
    			cout << "Vega: High sensitivity. High profits, high risk." << endl;
			}

			// Rho Analysis
			if (option.calculate_rho_call() < 10){
    			cout << "Rho: Rate changes barely matter." << endl;
			}
			else if (option.calculate_rho_call() >= 10 && option.calculate_rho_call() < 40){
    			cout << "Rho: Moderate rate exposure." << endl;
			}
			else if (option.calculate_rho_call() >= 40){
    			cout << "Rho: Big rate sensitivity. Long-term options/high strike." << endl;
			}

			cout << "======================================================" << endl;
                        cout << "                    PUT ANALYSIS                       " << endl;
                        cout << "======================================================" << endl;

                        // Delta Analysis
			if (option.calculate_delta_put() > -0.25){
	    		cout << "Delta: Weak bearish exposure. Barely moves with the stock." << endl;
			}
			else if (option.calculate_delta_put() <= -0.25 && option.calculate_delta_put() > -0.65){
		    	cout << "Delta: Moderately bearish. Balanced downside protection." << endl;
			}
			else if (option.calculate_delta_put() <= -0.65){
		    	cout << "Delta: Strong bearish. Behaves almost like a short stock." << endl;
			}

			// Gamma Analysis (same for call and put)
			if (option.calculate_gamma() < 0.01){
    			cout << "Gamma: Very stable, won't need rebalancing and not much convexity." << endl;
			}
			else if (option.calculate_gamma() >= 0.01 && option.calculate_gamma() < 0.03){
		    	cout << "Gamma: Healthy value as the Delta will move noticeably. Good for trading and Gamma scalping." << endl;
			}
			else if (option.calculate_gamma() >= 0.03){
		    	cout << "Gamma: High Gamma. Dangerous if hedging, but great for long options and are aiming for volatility pops." << endl;
			}

			// Theta Analysis
			if (option.calculate_theta_put() > -1){
    			cout << "Theta: Very slow decay. Cheap to hold." << endl;
			}
			else if (option.calculate_theta_put() >= -3 && option.calculate_theta_put() <= -1){
    			cout << "Theta: Moderate time decay. Should only hold if you expect a move soon." << endl;
			}
			else if (option.calculate_theta_put() < -3){
   		 	cout << "Theta: Heavy time decay. Good for shorting, risky for long-term." << endl;
			}

			// Vega Analysis (same for call and put)
			if (option.calculate_vega() < 20){
    			cout << "Vega: Low volatility sensitivity. IV shifts won't affect price much." << endl;
			}
			else if (option.calculate_vega() >= 20 && option.calculate_vega() < 50){
    			cout << "Vega: Medium sensitivity. Good if you expect rising uncertainty." << endl;
			}
			else if (option.calculate_vega() >= 50){
    			cout << "Vega: High sensitivity. High profits, high risk." << endl;
			}

			// Rho Analysis
			if (option.calculate_rho_put() > -10){
    			cout << "Rho: Rate changes barely matter." << endl;
			}
			else if (option.calculate_rho_put() <= -10 && option.calculate_rho_put() > -40){
    			cout << "Rho: Moderate rate exposure." << endl;
			}
			else if (option.calculate_rho_put() <= -40){
    			cout << "Rho: Big rate sensitivity. Long-term options/high strike." << endl;
			}

		}
		catch (const exception& e) {
        		cerr << "Error: " << e.what() << endl;
        		continue;
		}
		cout << "\n";
    	}
    	return 0;
}
