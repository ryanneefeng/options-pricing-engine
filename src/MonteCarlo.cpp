#include "../include/MonteCarlo.h"
#include <random>
#include <cmath>
#include <algorithm>

// Constructors
MonteCarloSimulator::MonteCarloSimulator(double S_, double K_, double T_, double r_, double sigma_, int sims): S(S_), K(K_), T(T_), r(r_), sigma(sigma_), num_simulations(sims), option_ptr(nullptr) {}
MonteCarloSimulator::MonteCarloSimulator(double S_, double K_, double T_, double r_, double sigma_, int sims, const Option& opt): S(S_), K(K_), T(T_), r(r_), sigma(sigma_), num_simulations(sims), option_ptr(&opt) {}

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
	//Draw 1 random Z from N(0,1)
        static thread_local std::mt19937 gen(std::random_device{}());
        std::normal_distribution<> normal(0.0, 1.0);
	for (int i = 0; i < num_simulations; i++) {
		double Z = normal(gen);
		//Simulate stock price with (+)Z
		double ST1 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*Z);
		//Simulate stock price with (-)Z
		double ST2 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*(-Z));
		//Average of the two payoffs (variance reduction)
		double payoff = 0.5 * (std::max(ST1 - K, 0.0) + std::max(ST2 - K, 0.0));
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
	//Draw 1 random Z from N(0,1)
        static thread_local std::mt19937 gen(std::random_device{}());
        std::normal_distribution<> normal(0.0, 1.0);
        for (int i = 0; i < num_simulations; i++) {
                double Z = normal(gen);
                //Simulate stock price with (+)Z
                double ST1 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*Z);
                //Simulate stock price with (-)Z
                double ST2 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*(-Z));
                //Average of the two payoffs (variance reduction)
                double payoff = 0.5 * (std::max(K - ST1, 0.0) + std::max(K - ST2, 0.0));
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

std::pair<double, double> MonteCarloSimulator::call_price_with_cv() {
	double sum_payoff = 0.0;
	double sum_payoff_sq = 0.0;
	// E[S_T] = S * exp(rT) under risk-neutral measure — known exactly
	double expected_ST = S * std::exp(r * T);
	static thread_local std::mt19937 gen(std::random_device{}());
	std::normal_distribution<> normal(0.0, 1.0);
	for (int i = 0; i < num_simulations; i++) {
		double Z = normal(gen);
		double ST1 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*Z);
		double ST2 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*(-Z));
		// Antithetic payoff
		double raw_payoff = 0.5 * (std::max(ST1 - K, 0.0) + std::max(ST2 - K, 0.0));
		// Control: average simulated stock price minus its known expectation
		// E[control] = 0, so subtracting it doesn't bias the estimate
		// Call payoff is positively correlated with ST, so this reduces variance
		double control = 0.5 * (ST1 + ST2) - expected_ST;
		double adjusted_payoff = raw_payoff - control;
		sum_payoff += adjusted_payoff;
		sum_payoff_sq += adjusted_payoff * adjusted_payoff;
	}
	double n = static_cast<double>(num_simulations);
	double avg = sum_payoff / n;
	double variance = (sum_payoff_sq / n) - (avg * avg);
	double std_error = std::sqrt(variance / n);
	double price = avg * std::exp(-r * T);
	double ci = 1.96 * std_error * std::exp(-r * T);
	
	return {price, ci};
}

std::pair<double, double> MonteCarloSimulator::put_price_with_cv() {
	double sum_payoff = 0.0;
	double sum_payoff_sq = 0.0;
	double expected_ST = S * std::exp(r * T);
	static thread_local std::mt19937 gen(std::random_device{}());
	std::normal_distribution<> normal(0.0, 1.0);
	for (int i = 0; i < num_simulations; i++) {
		double Z = normal(gen);
		double ST1 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*Z);
		double ST2 = S * std::exp((r - 0.5*sigma*sigma)*T + sigma*std::sqrt(T)*(-Z));
		double raw_payoff = 0.5 * (std::max(K - ST1, 0.0) + std::max(K - ST2, 0.0));
		// Put payoff is negatively correlated with ST so we ADD the control instead of subtracting
		double control = 0.5 * (ST1 + ST2) - expected_ST;
		double adjusted_payoff = raw_payoff + control;
		sum_payoff += adjusted_payoff;
		sum_payoff_sq += adjusted_payoff * adjusted_payoff;
	}
	double n = static_cast<double>(num_simulations);
	double avg = sum_payoff / n;
	double variance = (sum_payoff_sq / n) - (avg * avg);
	double std_error = std::sqrt(variance / n);
	double price = avg * std::exp(-r * T);
	double ci = 1.96 * std_error * std::exp(-r * T);

	return {price, ci};
}
