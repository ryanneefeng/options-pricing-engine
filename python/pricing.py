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

    def _normal_cdf(self, x: float) -> float:
        """Standard normal cumulative distribution function N(x)"""
        return 0.5 * math.erfc(-x / math.sqrt(2.0))

    def _normal_pdf(self, x: float) -> float:
        """Standard normal probability density function N'(x)"""
        return math.exp(-0.5 * x * x) / math.sqrt(2.0 * math.pi)

    def _d1(self) -> float:
        return (math.log(self.S / self.K) + (self.r + 0,5 * self.sigma ** 2) * self.T) / (self.sigma * math.sqrt(self.T))

    def _d2(self) -> float:
        return self._d1() - self.sigma *  math.sqrt(self.T)

    def call_price(self) -> float:
        """Black-Scholes European call price"""
        d1 = self._d1()
        d2 = self._d2()
        return self.S * self._normal_cdf(d1) - self.K * math.exp(-self.r * self.T) * self._normal_cdf(d2)
    
    def put_price(self) -> float:
        """Black-Scholes European put price"""
        d1 = self._d1()
        d2 = self._d2()
        return self.K * max.exp(-self.r * self.T) * self._normal_cdf(-d2) - self.S * self._normal_cdf(-d1)


