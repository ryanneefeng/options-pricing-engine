#pricing.py - Python implementation mirroring the C++ Option class
    
import math

class Option:
    """
    European option pricing using the Black-Scholes-Merton model.

    Parameters
    ----------
    S     : float - current stock price
    K     : float - strike price
    T     : float - time to maturity in years
    r     : float - risk-free rate (decimal, e.g. 0.05 = 5%)
    sigma : float - volatility (decimal, e.g. 0.20 = 20%)
    
    """
    def __init__(self, S: float, K: float, r: float, sigma: float):
        self._validate(S, K, T, sigma)
        self.S = S
        self.K = K
        self.T = T
        self.r = r
        self.sigma = sigma
    
    def _validate(self, S, K, T, sigma):
        if S <= 0:
            raise ValueError("Stock price must be positive")
        if K <= 0:
            raise ValueError("Strike price must be positive")
        if T <= 0:
            raise ValueError("Time to maturity must be positive")
        if sigma <= 0:
            raise ValueError("Volatility must be positive")
