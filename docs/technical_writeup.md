# Black-Scholes Options Pricing Engine - Technical Documentation

**Author:** Ryan Feng
**Date:** December 2025 –June 2026
**Context:** Self-directed project in quantitative finance

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

The Black-Scholes model was developed by Fischer Black, Myron Scholes, and Robert Merton to provide a closed-form solution for European option pricing under the following assumptions:

- Asset prices follow Geometric Brownian Motion
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

**Variables:**
- S₀ = Current stock price
- K = Strike price
- T = Time to expiration (years)
- r = Risk-free interest rate
- σ = Volatility (standard deviation of log returns)
- N(x) = Standard normal cumulative distribution function

### 1.3 The Greeks

The Greeks measure the sensitivity of option prices to changes in underlying parameters.

| Greek | Measures | Call | Put |
|-------|----------|------|-----|
| Delta (Δ) | Price sensitivity to underlying | N(d₁) | N(d₁) - 1 |
| Gamma (Γ) | Rate of change of Delta | N'(d₁)/(S₀σ√T) | N'(d₁)/(S₀σ√T) |
| Theta (Θ) | Time decay | Complex (see code) | Complex (see code) |
| Vega (ν) | Volatility sensitivity | S₀√T N'(d₁) | S₀√T N'(d₁) |
| Rho (ρ) | Interest rate sensitivity | KTe^(-rT)N(d₂) | -KTe^(-rT)N(-d₂) |

Note that Gamma and Vega are identical for calls and puts under Black-Scholes.

---

## 2. Implementation Details

### 2.1 Architecture

The implementation follows object-oriented design with clear separation of concerns:

- `Option` class — encapsulates all option parameters, Black-Scholes pricing, Greeks, and IV solver
- `MonteCarloSimulator` class — independent Monte Carlo simulation with variance reduction
- Input validation in constructor via `validate_inputs()`, which throws `std::invalid_argument` for invalid parameters

### 2.2 Key Design Decisions

**Error Handling:**
- Constructor validates all inputs and throws immediately on invalid parameters
- IV solver throws `std::runtime_error` on non-convergence or degenerate vega
- All exceptions caught and reported cleanly in `main.cpp`

**Numerical Precision:**
- All calculations use `double` precision (IEEE 754 64-bit)
- Constants defined to machine precision
- Normal CDF computed via `erfc` for accuracy to 6+ decimal places

---

## 3. Numerical Methods

### 3.1 Normal CDF Approximation

The standard normal CDF N(x) is computed using the complementary error function:
```cpp
double normal_cdf(double x) {
    return 0.5 * erfc(-x * M_SQRT1_2);
}
```
This provides accuracy to 6+ decimal places, sufficient for financial applications.

### 3.2 Monte Carlo Simulation

Stock prices at maturity are simulated using the exact GBM solution:
```
S(T) = S₀ · exp((r - 0.5σ²)T + σ√T · Z),   Z ~ N(0,1)
```

**Variance Reduction — Antithetic Variates:**

For each draw Z, the simulation also evaluates –Z. Since the normal distribution is symmetric, –Z is also a valid draw. Averaging the payoffs from +Z and –Z paths reduces variance by exploiting the negative correlation between the two paths:

```cpp
double ST1 = S * exp((r - 0.5*sigma*sigma)*T + sigma*sqrt(T)*Z);
double ST2 = S * exp((r - 0.5*sigma*sigma)*T + sigma*sqrt(T)*(-Z));
double payoff = 0.5 * (max(ST1 - K, 0.0) + max(ST2 - K, 0.0));
```

In practice this reduces the 95% confidence interval width by approximately 40–60% compared to standard Monte Carlo with the same number of simulations.

### 3.3 Implied Volatility — Newton-Raphson

Black-Scholes has no closed-form inverse for σ. Given an observed market price, the implied volatility is found numerically via Newton-Raphson iteration:

```
σ_new = σ_old - (BS(σ_old) - market_price) / Vega(σ_old)
```

Vega serves as the derivative since it measures ∂Price/∂σ exactly. Iteration continues until |BS(σ) - market_price| < 10⁻⁶. A temporary `Option` object is constructed at each trial σ to avoid mutating the original object's state.

**Convergence:** typically 4–6 iterations from a σ₀ = 0.20 starting guess for liquid equity options.

---

## 4. Validation & Testing

### 4.1 Put-Call Parity

The implementation verifies correctness using put-call parity:
```
C - P = S₀ - Ke^(-rT)
```

This relationship must hold for all European options under no-arbitrage. The implementation consistently shows parity errors < 10⁻¹⁴, confirming numerical correctness.

### 4.2 IV Solver Sanity Check

Feeding the Black-Scholes price back into the IV solver should recover the original σ exactly. Example with S=100, K=105, T=0.5, r=0.05, σ=0.20:

```
Call BS Price:  $4.5817  →  Call IV: 20.0001%
Put  BS Price:  $6.9892  →  Put  IV: 19.9999%
```

The residual error of ~0.0001% reflects floating-point rounding in the 4-decimal market price input.

### 4.3 Example Output

```
$ ./bin/pricer
======================================================
    Black-Scholes Options Pricing Engine v1.0
======================================================
How many options would you like to run? 1
Enter Stock Price (S): $100
Enter Strike Price (K): $105
Enter Time to Maturity (T) in years: 0.5
Enter Risk-free Rate (r) as decimal (e.g., 0.05 for 5%): 0.05
Enter Volatility (sigma) as decimal (e.g., 0.20 for 20%): 0.20

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
Monte Carlo Price: $4.5861 ± $0.0297
95% CI: [$4.5564, $4.6157]
Black-Scholes Price: $4.5817
Difference: $0.0044

PUT OPTION (100,000 simulations)
Monte Carlo Price: $6.9899 ± $0.0214
95% CI: [$6.9686, $7.0113]
Black-Scholes Price: $6.9892
Difference: $0.0007

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

---

## 5. Performance Considerations

| Component | Time Complexity | Typical Runtime |
|-----------|----------------|-----------------|
| Black-Scholes pricing | O(1) | < 1ms |
| Greeks calculation | O(1) | < 1ms |
| Monte Carlo (100k paths, antithetic) | O(n) | ~50–100ms |
| IV solver (Newton-Raphson) | O(k) iterations | < 5ms |

All calculations use `double` precision (IEEE 754 64-bit). The Monte Carlo simulator uses thread-local Mersenne Twister (mt19937) seeded via `std::random_device` for high-quality randomness.
