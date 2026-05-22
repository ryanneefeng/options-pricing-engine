#include "../include/MonteCarlo.h"
#include <random>
#include <cmath>
#include <algorithm>

// Constructor
MonteCarloSimulator::MonteCarloSimulator(double S_, double K_, double T_, double r_, double sigma_, int sims): S(S_), K(K_), T(T_), r(r_), sigma(sigma_), num_simulations(sims) {}

// One simulated stock price at maturity using geometric Brownian motion
double MonteCarloSimulator::simulate_stock_path() {
	static thread_local std::mt19937 gen(std::random_device{}());
	std::normal_distribution<> normal(0.0, 1.0);

	double Z = normal(gen);

	// S(T) = S(0) * exp((r - 0.5 * sigma^2) * T + sigma * sqrt(T) * Z)
	double exponent = (r - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * Z;
	double ST = S * std::exp(exponent);

	return ST;
}

// Price European call using Monte Carlo
double MonteCarloSimulator::price_call_mc() {
	double sum_payoff = 0.0;

	for (int i = 0; i < num_simulations; ++i) {
		double ST = simulate_stock_path();
		double payoff = std::max(ST - K, 0.0);
		sum_payoff += payoff;
	}

	double average_payoff = sum_payoff / static_cast<double>(num_simulations);
	double price = average_payoff * std::exp(-r * T);

	return price;
}

// Call price with 95% confidence interval
std::pair <double, double> MonteCarloSimulator::call_price_with_ci() {
	double sum_payoff = 0.0;
	double sum_payoff_sq = 0.0;

	for (int i = 0; i < num_simulations; i++) {
		double ST = simulate_stock_path();
		double payoff = std::max(ST - K, 0.0);
		sum_payoff += payoff;
		sum_payoff_sq += payoff * payoff;
	}
	double n = static_cast<double>(num_simulations);
	double avg = sum_payoff/n;
	double variance = (sum_payoff_sq / n) - (avg * avg);
	double std_error = std::sqrt(variance / n);

	double price = avg * std::exp(-r * T);
	double ci = 1.96 * std_error * std::exp(-r * T);

	return {price, ci};
}

// Put price with 95% confidence interval
std::pair<double, double> MonteCarloSimulator::put_price_with_ci() {
    	double sum_payoff = 0.0;
    	double sum_payoff_sq = 0.0;

    	for (int i = 0; i < num_simulations; ++i) {
        	double ST = simulate_stock_path();
        	double payoff = std::max(K - ST, 0.0);
        	sum_payoff += payoff;
        	sum_payoff_sq += payoff * payoff;
    	}

    	double n = static_cast<double>(num_simulations);
    	double avg = sum_payoff / n;
    	double variance = (sum_payoff_sq / n) - (avg * avg);
    	double std_error = std::sqrt(variance / n);

    	double price = avg * std::exp(-r * T);
	double ci = 1.96 * std_error * std::exp(-r * T);

    	return {price, ci};
}
