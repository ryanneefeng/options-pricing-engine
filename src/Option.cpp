#include "../include/Option.h"
#include <cmath>
#include <stdexcept>

// Mathematical constants
const double PI = 3.14159265358979323846;
const double SQRT_2PI = 2.506628274631000502;
const double M_SQRT1_2 = 0.7071067811865476;

/**
 * Constructor: Initializes option with given parameters
 */
Option::Option(double stock_price, double strike_price, double time_to_maturity,
               double risk_free_rate, double volatility)
    : S(stock_price), K(strike_price), T(time_to_maturity),
      r(risk_free_rate), sigma(volatility) {
    
    if (!validate_inputs()) {
        throw std::invalid_argument("Invalid option parameters");
    }
}

/**
 * Validate all input parameters
 */
bool Option::validate_inputs() const {
    if (S <= 0) {
        throw std::invalid_argument("Stock price must be positive");
    }
    if (K <= 0) {
        throw std::invalid_argument("Strike price must be positive");
    }
    if (T < 0) {
        throw std::invalid_argument("Time to maturity cannot be negative");
    }
    if (T == 0) {
        throw std::invalid_argument("Time to maturity cannot be zero (option expired)");
    }
    if (sigma < 0) {
        throw std::invalid_argument("Volatility cannot be negative");
    }
    if (sigma == 0) {
        throw std::invalid_argument("Volatility cannot be zero");
    }
    return true;
}

/**
 * Calculate d1 parameter for Black-Scholes formula
 * d1 = [ln(S/K) + (r + sigma²/2)T] / (sigma * sqrt(T))
 */
double Option::d1() const {
    return (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
}

/**
 * Calculate d2 parameter for Black-Scholes formula
 * d2 = d1 - sigma * sqrt(T)
 */
double Option::d2() const {
    return d1() - sigma * sqrt(T);
}

/**
 * Standard normal cumulative distribution function
 * Uses the error function for accurate calculation
 */
double Option::normal_cdf(double x) const {
    // Using the complementary error function for better accuracy
    // N(x) = 0.5 * [1 + erf(x/sqrt(2))]
    return 0.5 * erfc(-x * M_SQRT1_2);
}

/**
 * Standard normal probability density function
 * N'(x) = (1/sqrt(2π)) * e^(-x²/2)
 */
double Option::normal_pdf(double x) const {
    return exp(-0.5 * x * x) / SQRT_2PI;
}

/**
 * Calculate European call option price
 * C = S*N(d1) - K*e^(-rT)*N(d2)
 */
double Option::calculate_call_price() const {
    double d1_val = d1();
    double d2_val = d2();
    
    double term1 = S * normal_cdf(d1_val);
    double term2 = K * exp(-r * T) * normal_cdf(d2_val);
    
    return term1 - term2;
}

/**
 * Calculate European put option price
 * P = K*e^(-rT)*N(-d2) - S*N(-d1)
 */
double Option::calculate_put_price() const {
    double d1_val = d1();
    double d2_val = d2();
    
    double term1 = K * exp(-r * T) * normal_cdf(-d2_val);
    double term2 = S * normal_cdf(-d1_val);
    
    return term1 - term2;
}

/**
 * Calculate Delta for call option
 * Measures sensitivity of call price to stock price change
 * Delta_call = N(d1)
 */
double Option::calculate_delta_call() const {
    return normal_cdf(d1());
}

/**
 * Calculate Delta for put option
 * Measures sensitivity of put price to stock price change
 * Delta_put = N(d1) - 1
 */
double Option::calculate_delta_put() const {
    return normal_cdf(d1()) - 1.0;
}

/**
 * Calculate Gamma (same for call and put)
 * Measures rate of change of Delta with respect to stock price
 * Gamma = N'(d1) / (S * sigma * sqrt(T))
 */
double Option::calculate_gamma() const {
    double d1_val = d1();
    return normal_pdf(d1_val) / (S * sigma * sqrt(T));
}

/**
 * Calculate Theta for call option
 * Measures sensitivity of call price to passage of time
 * Theta is typically negative (time decay)
 */
double Option::calculate_theta_call() const {
    double d1_val = d1();
    double d2_val = d2();
    
    double term1 = -(S * normal_pdf(d1_val) * sigma) / (2.0 * sqrt(T));
    double term2 = r * K * exp(-r * T) * normal_cdf(d2_val);
    
    return term1 - term2;
}

/**
 * Calculate Theta for put option
 * Measures sensitivity of put price to passage of time
 */
double Option::calculate_theta_put() const {
    double d1_val = d1();
    double d2_val = d2();
    
    double term1 = -(S * normal_pdf(d1_val) * sigma) / (2.0 * sqrt(T));
    double term2 = r * K * exp(-r * T) * normal_cdf(-d2_val);
    
    return term1 + term2;
}

/**
 * Calculate Vega (same for call and put)
 * Measures sensitivity of option price to volatility changes
 * Vega = S * sqrt(T) * N'(d1)
 */
double Option::calculate_vega() const {
    double d1_val = d1();
    return S * sqrt(T) * normal_pdf(d1_val);
}

/**
 * Calculate Rho for call option
 * Measures sensitivity of call price to interest rate changes
 * Rho_call = K * T * e^(-rT) * N(d2)
 */
double Option::calculate_rho_call() const {
    double d2_val = d2();
    return K * T * exp(-r * T) * normal_cdf(d2_val);
}

/**
 * Calculate Rho for put option
 * Measures sensitivity of put price to interest rate changes
 * Rho_put = -K * T * e^(-rT) * N(-d2)
 */
double Option::calculate_rho_put() const {
    double d2_val = d2();
    return -K * T * exp(-r * T) * normal_cdf(-d2_val);
}
