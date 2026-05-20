# Black-Scholes Options Pricing Engine - Technical Documentation

**Author:** Ryan Feng
**Date:** December 2025
**Course Context:** Self-study in quantitative finance

---

## Table of Contents
1. [Mathematical Foundation](#mathematical-foundation)
2. [Implementation Details](#implementation-details)
3. [Numerical Methods](#numerical-methods)
4. [Validation & Testing](#validation-testing)
5. [Performance Considerations](#performance)

---

## 1. Mathematical Foundation

### 1.1 The Black-Scholes-Merton Model

The Black-Scholes model was developed by Fischer Black, Myron Scholes, and Robert Merton to provide a closed-form solution for European option pricing under the assumptions listed below.

**Assumptions:**
- Asset Prices follow geometric Brownian motion
- Risk-free rate and volatility are constant
- No transaction costs or taxes
- Assets are infinitely divisible
- No dividends during option life
- Markets are efficient (no arbitrage)

### 1.2 The Pricing Formula

**Call Option Price:**
```
C = S₀N(d₁) - Ke^(-rT)N(d₂)
```

**Put Option Price:**
```
P = Ke^(-rT)N(-d₂) - S₀N(-d₁)
```

Where:
```
d₁ = [ln(S₀/K) + (r + σ²/2)T] / (σ√T)
d₂ = d₁ - σ√T
```

**Variables: **
- S₀ = Current stock price
- K = Strike price
- T = Time to expiration (years)
- r = Risk-free interest rate
- σ = Volatility (standard deviation of returns)
- N(x) = Standard normal cumulative distribution function

### 1.3 The Greeks

[TO BE COMPLETED DURING WINTER BREAK]

---

## 2. Implementation Details

### 2.1 Architecture

The implementation follows object-oriented design principles with clear separation of concerns:

**Class Structure:**
- `Option` class encapsulates all option parameters
- Private helper methods for d₁, d₂ calculations
- Public methods for pricing and Greeks
- Input validation in constructor

### 2.2 Key Design Decisions

**Error Handling:**
- Constructor validates all inputs
- Throws `std::invalid_argument` for invalid parameters
- Prevents undefined behavior from division by zero

**Numerical Precision:**
- Uses `double` precision (IEEE 754 64-bit)
- Constants defined to machine precision
- Careful ordering of operations to minimize rounding error

[TO BE COMPLETED DURING WINTER BREAK]

---

## 3. Numerical Methods

### 3.1 Normal CDF Approximation

The standard normal cumulative distribution function N(x) is computed using the complementary error function:
```cpp
N(x) = 0.5 * erfc(-x / √2)
```

This provides accuracy to 6+ decimal places, which is sufficient for financial applications.

[TO BE COMPLETED DURING WINTER BREAK]

---

## 4. Validation & Testing

### 4.1 Put-Call Parity

The implementation verifies correctness using put-call parity:
```
C - P = S₀ - Ke^(-rT)
```

This fundamental relationship must hold for European options. My implementation shows parity errors < 10⁻⁶, confirming mathematical correctness.

### 4.2 Test Cases

```
======================================================
    Black-Scholes Options Pricing Engine v1.0
======================================================

How many options would you like to run?
2
Enter Stock Price (S): $100
Enter Strike Price (K): $105
Enter Time to Maturity (T) in years: 1.0
Enter Risk-free Rate (r) as decimal (e.g., 0.05 for 5%): 0.05
Enter Volatility (sigma) as decimal (e.g., 0.20 for 20%): 0.20

Calculating...

======================================================
                 CALL OPTION
======================================================
Price:  $8.0214
Delta:   0.5422
Gamma:   0.0198
Theta:   -6.2771
Vega:    39.6705
Rho:     46.2015

======================================================
                 PUT OPTION
======================================================
Price:  $7.9004
Delta:   -0.4578
Gamma:   0.0198
Theta:   -1.2832
Vega:    39.6705
Rho:     -53.6776

======================================================
                     VALIDATION
======================================================
Put-Call Parity Error: 7.11e-15
Calculations verified!
======================================================
                    CALL ANALYSIS
======================================================
Delta: Good value. Reacts strongly to price changes and good for directional trades.
Gamma: Healthy value as the Delta will move noticeably. Good for trading and Gamma scalping.
Theta: Heavy time decay: Good for shorting, risky for long-term.
Vega: Medium sensitivity. Good if you expect rising uncertainty.
Rho: Big rate sensitivity. Long-term options/high strike.
======================================================
                    PUT ANALYSIS
======================================================
Delta: Moderately bearish. Balanced downside protection.
Gamma: Healthy value as the Delta will move noticeably. Good for trading and Gamma scalping.
Theta: Moderate time decay. Should only hold if you expect a move soon.
Vega: Medium sensitivity. Good if you expect rising uncertainty.
Rho: Big rate sensitivity. Long-term options/high strike.

Enter Stock Price (S): $50
Enter Strike Price (K): $60
Enter Time to Maturity (T) in years: 4
Enter Risk-free Rate (r) as decimal (e.g., 0.05 for 5%): 0.03
Enter Volatility (sigma) as decimal (e.g., 0.20 for 20%): 0.10

Calculating...

======================================================
                 CALL OPTION
======================================================
Price:  $2.7005
Delta:   0.4162
Gamma:   0.0390
Theta:   -1.0309
Vega:    39.0110
Rho:     72.4394

======================================================
                 PUT OPTION
======================================================
Price:  $5.9157
Delta:   -0.5838
Gamma:   0.0390
Theta:   0.5655
Vega:    39.0110
Rho:     -140.4215

======================================================
                     VALIDATION
======================================================
Put-Call Parity Error: -3.55e-15
Calculations verified!
======================================================
                    CALL ANALYSIS
======================================================
Delta: This option will move a bit with the stock, but still not something you would hedge with (more speculative than strategic).
Gamma: High Gamma. Dangerous if hedging, but great for long options and are aiming for volatility pops.
Theta: Mild time decay, good for holding long-term.
Vega: Medium sensitivity. Good if you expect rising uncertainty.
Rho: Big rate sensitivity. Long-term options/high strike.
======================================================
                    PUT ANALYSIS
======================================================
Delta: Moderately bearish. Balanced downside protection.
Gamma: High Gamma. Dangerous if hedging, but great for long options and are aiming for volatility pops.
Theta: Very slow decay. Cheap to hold.
Vega: Medium sensitivity. Good if you expect rising uncertainty.
Rho: Big rate sensitivity. Long-term options/high strike.
```

---

## 5. Performance Considerations

### 5.1 Computational Complexity

All calculations have O(1) time complexity - constant time regardless of input size.

[TO BE COMPLETED DURING WINTER BREAK]

