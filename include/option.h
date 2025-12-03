#ifndef OPTION_H
#define OPTION_H

class Option {
private:
    	double S;      // stock price
	double K;      // strike price
    	double T;      // time to maturity
    	double r;      // risk-free rate
    	double sigma;  // volatility

    	bool validate_inputs() const;

    	double d1() const;
    	double d2() const;

    	double normal_cdf(double x) const;
    	double normal_pdf(double x) const;

public:
    Option(double stock_price, double strike_price, double time_to_maturity, double risk_free_rate, double volatility); 
	double calculate_call_price() const;
    	double calculate_put_price() const;

    	double calculate_delta_call() const;
    	double calculate_delta_put() const;

    	double calculate_gamma() const;

    	double calculate_theta_call() const;
    	double calculate_theta_put() const;

	double calculate_vega() const;
    	double verify_put_call_parity() const;
    	double calculate_rho_call() const;
    	double calculate_rho_put() const;
};

#endif
