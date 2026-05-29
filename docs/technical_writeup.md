# Black-Scholes Options Pricing Engine - Technical Documentation

**Author:** Ryan Feng
**Date:** December 2025 –June 2026
**Context:** Self-directed project in quantitative finance

---

## Table of Contents
1. [Mathematical Foundation](#mathematical-foundation)
2. [Implementation Details](#implementation-details)
3. [Numerical Methods](#numerical-methods)
4. [Python Interface](#python-interface)
5. [Validation & Testing](#validation-testing)
6. [Performance Considerations](#performance)

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

## 4. Python Interface

### 4.1 Design

`pricing.py` implements the same `Option` class as C++ in pure Python, using:
- `math` for scalar calculations (log, exp, sqrt, erfc)
- `numpy` for vectorized Monte Carlo simulation

The Python Monte Carlo uses numpy vectorization instead of explicit loops:

```python
Z    = np.random.standard_normal(num_sims)          # all draws at once
ST1  = self.S * np.exp(drift*self.T + vol*Z)        # vectorized GBM
ST2  = self.S * np.exp(drift*self.T + vol*(-Z))     # antithetic paths
payoffs = 0.5 * (np.maximum(ST1 - self.K, 0) + np.maximum(ST2 - self.K, 0))
```

This is substantially faster than a Python for loop and more readable.

### 4.2 Historical Volatility

`backtest.py` computes realized volatility from daily log returns:

```python
log_returns = np.log(closes / closes.shift(1)).dropna()
annual_vol  = log_returns.std() * math.sqrt(252)
```

Log returns (vs simple returns) are used because:
- They are additive across time periods
- They are consistent with the GBM assumption in Black-Scholes
- 252 annualizes from daily to yearly (trading days per year)

### 4.3 Options Strip

`backtest.py` prices a strip of 7 strikes centered on the current price, spaced 5% apart, using live market data:

```
Options Strip — 30-Day European Options (AAPL)
===========================================================================
 Strike  Moneyness  Call Price  Put Price  Call Delta  Put Delta  IV (Call)
 265.25     1.1765     47.9200     0.0232      0.9962    -0.0038      22.00
 312.06     1.0000      8.4906     7.2108      0.5385    -0.4615      22.00
 358.87     0.8696      0.1182    45.4554      0.0170    -0.9830      22.00
===========================================================================
```

ATM call delta ≈ 0.54 (near the theoretical 0.5). Deep ITM calls have delta → 1, deep OTM calls have delta → 0.

---

## 5. Validation & Testing

### 5.1 Put-Call Parity

```
C - P = S₀ - Ke^(-rT)
```

Implementation shows parity errors < 10⁻¹⁴ — floating-point machine precision.

### 5.2 IV Solver Sanity Check

Feeding BS price back into IV solver recovers the original σ:
```
Call BS Price: $4.5817  →  Call IV: 20.0001%
Put  BS Price: $6.9892  →  Put  IV: 19.9999%
```
Residual error of 0.0001% reflects 4-decimal rounding of the input price.

### 5.3 Python vs C++ Cross-Validation

```python
opt = Option(100, 105, 0.5, 0.05, 0.20)
print(opt.call_price())   # 4.5817 — matches C++ exactly
print(opt.delta_call())   # 0.4612 — matches C++ exactly
print(opt.vega())         # 28.0757 — matches C++ exactly
```

---

## 6. Performance Considerations

| Component | Complexity | Typical Runtime |
|-----------|-----------|-----------------|
| Black-Scholes (C++) | O(1) | < 1ms |
| Greeks (C++) | O(1) | < 1ms |
| IV solver (C++) | O(k) | < 5ms |
| Monte Carlo 100k (C++) | O(n) | ~50–100ms |
| Monte Carlo 100k (Python/numpy) | O(n) | ~200–400ms |
| Options strip 7 strikes (Python) | O(7k) | < 1s |

### Variance Reduction Results

| Method | Call CI Width | Put CI Width |
|--------|--------------|--------------|
| Standard MC | ±$0.0508 | ±$0.0523 |
| + Antithetic variates | ±$0.0297 | ±$0.0214 |
| + Control variates | ±$0.0214 | ±$0.0296 |
