#ifndef MONTECARLO_H
#define MONTECARLO_H

#include <utility>
#include "option.h"
class MonteCarloSimulator {
private:
	double S; // Current stock price
	double K; // Strike price
	double T; // Time to maturity
	double r; // Risk-free rate
	double sigma; // Volatility
	int num_simulations; // Number of paths to simulate
	const Option* option_ptr;  // nullptr if no control variate option provided

public:
	// Constructor
	MonteCarloSimulator(double S_, double K_, double T_, double r_, double sigma_, int sims = 100000);
	// Overloaded constructor that also accepts an Option reference for control variates
        MonteCarloSimulator(double S_, double K_, double T_, double r_, double sigma_, int sims, const Option& opt);

	// Calculate call option price using Monte Carlo
	double price_call_mc();

	// Calculate put option price using Monte Carlo
	double price_put_mc();

	// Get price with confidence interval
	std::pair<double, double> call_price_with_ci();
	std::pair<double,double> put_price_with_ci();

	// Helper function for a single simulation path
	double simulate_stock_path();
	
	// Control variates
	std::pair<double, double> call_price_with_cv();
	std::pair<double, double> put_price_with_cv();
};

#endif
