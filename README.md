# Black-Scholes Options Pricing Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Python](https://img.shields.io/badge/Python-3.8+-blue.svg)](https://python.org/)
[![Status](https://img.shields.io/badge/Status-Active-success.svg)]()

A quantitative options pricing engine implemented in both C++ and Python. Features analytical Black-Scholes pricing, complete Greeks, Monte Carlo simulation with variance reduction, implied volatility solving via Newton-Raphson, and a Python backtesting interface that prices live options strips using real market data.

## Features

- **European Options Pricing**
  - Analytical Black-Scholes formula
  - Monte Carlo simulation with 100,000 paths
  - 95% confidence intervals for MC estimates
  - Antithetic variates and control variates variance reduction
- **Complete Greeks Suite** (Delta, Gamma, Theta, Vega, Rho)
- **Implied Volatility Solver (Newton-Raphson method)**
- **Model Validation**
  - Put-call parity verification
  - Black-Scholes vs Monte Carlo comparison
- **Multi-option batch processing**

**Python Interface (python/):**
- Pure Python Option class mirroring the C++ implementation
- Historical volatility calculation from real market data via yfinance
- Options strip pricing across multiple strikes and maturities
- Live backtesting demo using real stock prices
  
## Mathematical Background

The Black-Scholes model provides analytical solutions for European option pricing under the following assumptions:
- Log-normal distribution of asset prices
- Constant volatility and risk-free rate
- No dividends during option lifetime
- No arbitrage opportunities
- Continuous trading

### Formula

**Call Option:**
```
C = S₀N(d₁) - Ke^(-rT)N(d₂)
```

**Put Option:**
```
P = Ke^(-rT)N(-d₂) - S₀N(-d₁)
```

**Where:**
```
d₁ = [ln(S₀/K) + (r + σ²/2)T] / (σ√T)
d₂ = d₁ - σ√T
N(x) = Standard normal cumulative distribution function
```

**Parameters:**
- `S₀` = Current stock price
- `K` = Strike price
- `T` = Time to expiration (years)
- `r` = Risk-free interest rate (annualized)
- `σ` = Volatility (annualized standard deviation)

### Monte Carlo Simulation

The Monte Carlo method simulates stock price paths using Geometric Brownian Motion:

**Stock Price at Maturity:**
```
S(T) = S₀ * exp((r - 0.5σ²)T + σ√T * Z)
```

Where `Z ~ N(0,1)` is a standard normal random variable.

**Option Price:**
```
Price = e^(-rT) * E[max(S(T) - K, 0)]  // Call
Price = e^(-rT) * E[max(K - S(T), 0)]  // Put
```

**Confidence Interval (95%):**
```
CI = 1.96 * (standard_deviation / √n)
```
**Variance Reduction — Antithetic Variates:**

For each draw Z, also simulate –Z. The two negatively correlated payoffs are averaged, reducing estimator variance by ~40–60% with no additional computational cost.

**Variance Reduction — Control Variates:**

Uses the simulated stock price as a control variable. Since E[S(T)] = S·e^(rT) is known exactly under the risk-neutral measure, deviations from this expectation correct the payoff estimate.

**Implied Volatility (Newton-Raphson)**

Black-Scholes has no closed-form inverse for σ. Given an observed market price, implied volatility is found via:
```
σ_new = σ_old - (BS_price(σ_old) - market_price) / Vega(σ_old)
```
Converges to tolerance 1e-6 in typically 4–6 iterations from σ₀ = 0.20.

### Greeks

| Greek | Measures | Call Formula | Put Formula |
|-------|----------|--------------|-------------|
| **Delta (Δ)** | Rate of change w.r.t. underlying price | N(d₁) | N(d₁) - 1 |
| **Gamma (Γ)** | Rate of change of Delta | N'(d₁)/(S₀σ√T) | N'(d₁)/(S₀σ√T) |
| **Theta (Θ)** | Rate of change w.r.t. time | Complex (see code) | Complex (see code) |
| **Vega (ν)** | Rate of change w.r.t. volatility | S₀√T N'(d₁) | S₀√T N'(d₁) |
| **Rho (ρ)** | Rate of change w.r.t. interest rate | KTe^(-rT)N(d₂) | -KTe^(-rT)N(-d₂) |

*Note: N'(x) = (1/√2π)e^(-x²/2) is the standard normal probability density function*

## Installation

**C++ Engine**
```bash
git clone https://github.com/ryaneefeng/options-pricing-engine.git
cd options-pricing-engine
make
./bin/pricer
```

**Python Interface**
```bash
pip install numpy pandas yfinance
cd python
python backtest.py
```

## C++ Example Session

```
g++ -std=c++17 -Wall -O2 -Iinclude -c src/main.cpp -o build/main.o
g++ -std=c++17 -Wall -O2 -Iinclude -c src/option.cpp -o build/option.o
g++ -std=c++17 -Wall -O2 -Iinclude -c src/MonteCarlo.cpp -o build/MonteCarlo.o
g++ -std=c++17 -Wall -O2 -Iinclude -o bin/pricer build/main.o build/option.o build/MonteCarlo.o
./bin/pricer

======================================================
    Black-Scholes Options Pricing Engine v1.0
======================================================
How many options would you like to run?
1
Enter Stock Price (S): $100
Enter Strike Price (K): $105
Enter Time to Maturity (T) in years: 0.5
Enter Risk-free Rate (r) as decimal (e.g., 0.05 for 5%): 0.05
Enter Volatility (sigma) as decimal (e.g., 0.20 for 20%): 0.2
Calculating...
======================================================
                 CALL OPTION
======================================================
Price:  $4.5817
Delta:   0.4612
Gamma:   0.0281
Theta:   -7.6919
Vega:    28.0757
Rho:     20.7672

======================================================
                     PUT OPTION
======================================================
Price:  $6.9892
Delta:   -0.5388
Gamma:   0.0281
Theta:   -2.5715
Vega:    28.0757
Rho:     -30.4366

======================================================
                MONTE CARLO SIMULATION
======================================================
CALL OPTION (100,000 simulations)
Monte Carlo Price: $4.6076 ± $0.0298
95% CI: [$4.5778, $4.6373]
Black-Scholes Price: $4.5817
Difference: $0.0259

PUT OPTION (100,000 simulations)
Monte Carlo Price: $6.9873 ± $0.0214
95% CI: [$6.9660, $7.0087]
Black-Scholes Price: $6.9892
Difference: $-0.0019

======================================================
                      VALIDATION
======================================================
Put-Call Parity Error: -7.11e-15
Calculations verified!
======================================================
                    GREEK SUMMARY
======================================================
          CALL          PUT
------------------------------------------------------
Delta:    0.4612        -0.5388
Gamma:    0.0281        0.0281
Theta:    -7.6919       -2.5715
Vega:     28.0757       28.0757
Rho:      20.7672       -30.4366
======================================================
======================================================
              IMPLIED VOLATILITY SOLVER
======================================================
Enter observed market price for call (0 to skip): $4.5817
Call Implied Volatility: 20.0001%
Enter observed market price for put (0 to skip): $6.9892
Put Implied Volatility: 19.9999%
======================================================

```
## Python Example Session

```
$ python backtest.py

Fetching data for AAPL...
Current Price:         $312.06
Historical Volatility:  22.00%
Time to Expiry:         30 days (0.0822 years)

Options Strip — 30-Day European Options (AAPL)
===========================================================================
 Strike  Moneyness  Call Price  Put Price  Call Delta  Put Delta  IV (Call)
 265.25     1.1765     47.9200     0.0232      0.9962    -0.0038      22.00
 280.85     1.1111     32.6699     0.3120      0.9614    -0.0386      22.00
 296.46     1.0526     18.8178     1.9990      0.8186    -0.1814      22.00
 312.06     1.0000      8.4906     7.2108      0.5385    -0.4615      22.00
 327.66     0.9524      2.8337    17.0929      0.2492    -0.7508      22.00
 343.27     0.9091      0.6816    30.4798      0.0786    -0.9214      22.00
 358.87     0.8696      0.1182    45.4554      0.0170    -0.9830      22.00
===========================================================================
```

## Programmatic Usage

**C++**

```cpp
#include "include/option.h"
#include "include/MonteCarlo.h"

int main() {
    Option opt(100.0,  // Stock price
               105.0,  // Strike price
               1.0,    // Time to maturity (years)
               0.05,   // Risk-free rate
               0.20);  // Volatility

    // Black-Scholes pricing
    double call_price = opt.calculate_call_price();
    double put_price  = opt.calculate_put_price();

    // Greeks
    double delta = opt.calculate_delta_call();
    double gamma = opt.calculate_gamma();
    double vega  = opt.calculate_vega();

    // Monte Carlo simulation
    MonteCarloSimulator mc(100.0, 105.0, 1.0, 0.05, 0.20, 100000);
    auto [mc_price, ci] = mc.call_price_with_ci();

    // Implied volatility — pass observed market price
    double call_iv = opt.implied_volatility_call(4.5817);  // returns 0.2000
    double put_iv  = opt.implied_volatility_put(6.9892);   // returns 0.2000

    return 0;
}
```

**Python**

```python
from pricing import Option

opt = Option(S=100, K=105, T=0.5, r=0.05, sigma=0.20)

# Pricing
print(opt.call_price())    # 4.5817
print(opt.put_price())     # 6.9892

# Greeks
print(opt.delta_call())    # 0.4612
print(opt.vega())          # 28.0757

# Implied volatility
print(opt.implied_volatility_call(4.5817))  # 0.2000

# Monte Carlo
price, ci = opt.monte_carlo_call()
print(f"${price:.4f} ± ${ci:.4f}")
```

## Project Structure
```
options-pricing-engine/
├── src/
│   ├── main.cpp          # User interface and program entry point
│   ├── option.cpp        # Black-Scholes and IV solver implementation
│   └── MonteCarlo.cpp    # Monte Carlo simulation implementation
├── include/
│   ├── option.h          # Option class header
│   └── MonteCarlo.h      # Monte Carlo simulator header
├── docs/
│   └── technical_writeup.md    # Detailed mathematical documentation
├── python/
│   ├── pricing.py        # Python Option class — BS pricing, Greeks, IV, MC
│   └── backtest.py       # Live backtesting demo using yfinance
├── bin/                  # Compiled executables (generated)
├── build/                # Object files (generated)
├── .gitignore
├── LICENSE
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── Makefile
└── README.md
```

## Educational Value

This project demonstrates:

**Financial Mathematics:**
- Options pricing theory
- Stochastic calculus concepts (Geometric Brownian Motion)
- Implied volatility and Newton-Raphson root-finding
- Monte Carlo methods in finance

**Software Engineering:**
- Object-oriented design in C++ and Python
- Header/implementation separation
- Clean code principles

**Numerical Computing:**
- Mathematical function approximation
- Iterative root-finding algorithms
- Statistical estimation and confidence intervals

## Technical Details

### Normal CDF Approximation

The standard normal cumulative distribution function N(x) is approximated using the error function:
```cpp
double normal_cdf(double x) {
    return 0.5 * erfc(-x * M_SQRT1_2);
}
```

This provides accuracy to 6+ decimal places, sufficient for financial calculations.

### Performance Characteristics

- **Black-Scholes Time Complexity**: O(1) per calculation
- **Monte Carlo Time Complexity**: O(n) where n = number of simulations
- **IV Solver (Newton-Raphson)**: O(k) iterations< 5ms
- **Options strip 7 strikes (Python)**: O(7k)< 1s

**Online Resources:**
- [MIT OpenCourseWare - Mathematical Methods for Quantitative Finance](https://ocw.mit.edu/courses/mathematics/)
- [QuantLib - Open-source quantitative finance library](https://www.quantlib.org/)
- [Options Industry Council - Educational Resources](https://www.optionseducation.org/)

## Author

Ryan Feng Cornell University | B.A. Mathematics, Minor: Computer Science | Class of 2029

- LinkedIn: linkedin.com/in/ryanneefeng
- Email: ryanneefeng@gmail.com
- GitHub: @ryanneefeng
  
## License
This project is licensed under the MIT License - see the LICENSE file for details.
