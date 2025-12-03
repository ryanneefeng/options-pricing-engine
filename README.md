# Black-Scholes Options Pricing Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Status](https://img.shields.io/badge/Status-Active-success.svg)]()

A high-performance C++ implementation of the Black-Scholes-Merton model for pricing European options with complete Greeks calculations made by a student who realized they wanted to get into quant and CS way too late.

## Features

- **European Options Pricing**: Call and Put options using closed-form Black-Scholes formula
- **Complete Greeks Suite**: Delta, Gamma, Theta, Vega, and Rho calculations
- **Input Validation**: Robust error handling for invalid parameters
- **High Performance**: Optimized O(1) calculations
- **Clean Architecture**: Object-oriented design with clear separation of concerns
- **Well-Documented**: Comprehensive inline comments and technical documentation

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

### Greeks

"Greeks" measure the sensitivity of option prices to various parameters. Basically they tell us and other people whether an option is worth buying, selling, or keeping:

| Greek | Measures | Call Formula | Put Formula |
|-------|----------|--------------|-------------|
| **Delta (Δ)** | Rate of change w.r.t. underlying price | N(d₁) | N(d₁) - 1 |
| **Gamma (Γ)** | Rate of change of Delta | N'(d₁)/(S₀σ√T) | N'(d₁)/(S₀σ√T) |
| **Theta (Θ)** | Rate of change w.r.t. time | Complex (see code) | Complex (see code) |
| **Vega (ν)** | Rate of change w.r.t. volatility | S₀√T N'(d₁) | S₀√T N'(d₁) |
| **Rho (ρ)** | Rate of change w.r.t. interest rate | KTe^(-rT)N(d₂) | -KTe^(-rT)N(-d₂) |

*Note: N'(x) = (1/√2π)e^(-x²/2) is the standard normal probability density function*

Ex.
 Call:
   Delta = 0.5422
     Delta is basically 0.54, so the call behaves like owning 54 shares for every 100 options. Higher Delta means option is "stock-like". If Delta was closer to 1.0 then you'd basically just have the stock.
   Gamma = 0.0198
     Higher Gamma means your Delta hedge will become unstable faster. If Gamma was much higher you'd have to rebalance your hedge A LOT (Gamma scalping).
   Theta = -6.2771
     Basically, the option loses around $6.28 in value per day. Negative means it is losing value (obviously). If Theta was small in magnitude, teh option wouldn't change much as time goes on.
   Vega = 39.705
    With every +1% in volatility, the option price goes up around $0.40 (cus 39.7 rounds to 40). If it were real big, the options value would be sensitve to valirity shifts.
  Pull:
   Delta = -0.4578
     Delta is roughly -0.46, which means the put behaves like being *short* 46 shares for every 100 options. Since Delta is closer to 0 than -1, the option isn’t super sensitive to small price movements.
     If Delta were closer to -1.0, you'd basically be holding the OPPOSITE of the stock.
   Gamma = 0.0198
    This value is pretty small, meaning your Delta hedge won’t swing around too violently. If Gamma were way higher, you’d have to rebalance constantly.
   Theta = -1.2832
    The option loses about $1.28 in value per day due to time decay.
   Vega = 39.6705
    With every +1% change in volatility, the put price moves about $0.40.
   Rho = -53.6776
    Interest rates rising by 1% drops the put’s value by about $0.54.Negative Rho is normal for puts.
  
### Installation

```bash
# Clone the repository
git clone https://github.com/ryaneefeng/options-pricing-engine.git
cd options-pricing-engine

# Compile the program
g++ -std=c++17 src/main.cpp src/Option.cpp -o bin/pricer -I include

# Run the program
./pricer
```
**Example Session:**
```
=== Black-Scholes Options Pricing Engine v1.0 ===

Enter Stock Price (S): 100
Enter Strike Price (K): 105
Enter Time to Maturity (T) in years: 1.0
Enter Risk-free Rate (r) as decimal: 0.05
Enter Volatility (σ) as decimal: 0.20

Calculating...

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                 CALL OPTION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Price:  $8.0214
Delta:   0.5422
Gamma:   0.0198
Theta:  -6.2771
Vega:   39.6705
Rho:    46.2015

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                 PUT OPTION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Price:  $7.9004
Delta:  -0.4578
Gamma:   0.0198
Theta:  -1.2832
Vega:   39.6705
Rho:   -53.6776
```

### Programmatic Usage

```cpp
#include "include/Option.h"

int main() {
    // Create an option object
    Option opt(100.0,  // Stock price
               105.0,  // Strike price
               1.0,    // Time to maturity (years)
               0.05,   // Risk-free rate
               0.20);  // Volatility
    
    // Calculate prices
    double call_price = opt.calculate_call_price();
    double put_price = opt.calculate_put_price();
    
    // Calculate Greeks
    double delta = opt.calculate_delta_call();
    double gamma = opt.calculate_gamma();
    double vega = opt.calculate_vega();
    
    return 0;
}
```

## Project Structure

```
options-pricing-engine/
├── src/
│   ├── main.cpp          # User interface and program entry point
│   └── Option.cpp        # Option class implementation
├── include/
│   └── Option.h          # Option class header and declarations
├── docs/
│   ├── technical_writeup.md    # Detailed mathematical documentation
│   └── examples.md             # Usage examples and test cases
├── bin/                  # Compiled executables (generated)
├── .gitignore
├── LICENSE
├── Makefile
└── README.md
```
## Educational Value

This project demonstrates:

**Financial Mathematics:**
- Options pricing theory
- Risk-neutral valuation
- Stochastic calculus concepts
- Probability distributions

**Software Engineering:**
- Object-oriented design in C++
- Header/implementation separation
- Clean code principles
- Documentation best practices

**Numerical Computing:**
- Mathematical function approximation
- Floating-point precision handling
- Performance optimization

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

- **Time Complexity**: O(1) for all calculations
- **Space Complexity**: O(1) (only stores 5 parameters)
- **Typical Runtime**: < 1ms per calculation on modern hardware

### Precision & Accuracy

- All calculations use `double` precision (IEEE 754 64-bit)
- Greeks accurate to 4+ decimal places
- Prices accurate to $0.01 or better

## Future Enhancements

Planned features for future versions:

- [ ] **American Options**: Early exercise using binomial trees or Monte Carlo
- [ ] **Implied Volatility**: Newton-Raphson solver for implied vol
- [ ] **Volatility Smile**: Support for non-constant volatility
- [ ] **Dividend Yield**: Extend model for dividend-paying stocks
- [ ] **Exotic Options**: Barrier options, Asian options
- [ ] **Portfolio Analysis**: Multi-option portfolio Greeks
- [ ] **GUI Interface**: Qt-based graphical interface
- [ ] **Python Bindings**: PyBind11 wrapper for Python integration
- [ ] **Unit Tests**: Comprehensive test suite with Google Test
- [ ] **Benchmarking**: Performance comparison suite

## Resources & References

**Academic Papers:**
- Black, F., & Scholes, M. (1973). "The Pricing of Options and Corporate Liabilities." *Journal of Political Economy*, 81(3), 637-654.
- Merton, R. C. (1973). "Theory of Rational Option Pricing." *Bell Journal of Economics and Management Science*, 4(1), 141-183.

**Online Resources:**
- [MIT OpenCourseWare - Mathematical Methods for Quantitative Finance](https://ocw.mit.edu/courses/mathematics/)
- [QuantLib - Open-source quantitative finance library](https://www.quantlib.org/)
- [Options Industry Council - Educational Resources](https://www.optionseducation.org/)

## Author

**Ryan Feng**  
Fordham University | Class of 2028  
Mathematics & Computer Science

Aspiring Quantitative Researcher with interests in derivatives pricing, algorithmic trading, and financial engineering.

**Connect:**
- LinkedIn: [linkedin.com/in/ryanneefeng](https://linkedin.com/in/ryanneefeng)
- Email: ryanneefeng@gmail.com
- GitHub: [@ryaneefeng](https://github.com/ryaneefeng)
