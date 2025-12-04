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

## 2, Implementation Details

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

This fundamental relationship must hold for European options. Our implementation shows parity errors < 10⁻⁶, confirming mathematical correctness.

### 4.2 Test Cases

[TO BE COMPLETED DURING WINTER BREAK]

---

## 5. Performance Considerations

### 5.1 Computational Complexity

All calculations have O(1) time complexity - constant time regardless of input size.

[TO BE COMPLETED DURING WINTER BREAK]

