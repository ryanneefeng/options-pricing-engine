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

    def delta_call(self) -> float:
        """Call delta"""
        return self._normal_cdf(self._d1())

    def delta_put(self) -> float:
        """Put delta"""
        return self._normal_cdf(self._d1()) - 1.0

    def gamma(self) -> float:
        """Gamma (identical for calls and puts)"""
        return self._normal_pdf(self._d1()) / (self.S * self.sigma * math.sqrt(self.T))

    def vega(self) -> float:
        """Vega (identical for calls and puts)"""
        return self.S * math.sqrt(self.T) * self._normal_pdf(self._d1())

    def theta_call(self) -> float:
        """Call theta"""
        d1 = self._d1()
        d2 = self._d2()
        term1 = -(self.S * self._normal_pdf(d1) * self.sigma) / (2.0 * math.sqrt(self.T))
        term2 = self.r * self.K * math.exp(-self.r * self.T) * self._normal_cdf(d2)
        return term1 - term2

    def theta_put(self) -> float:
        """Put theta — time decay per year."""
        d1 = self._d1()
        d2 = self._d2()
        term1 = -(self.S * self._normal_pdf(d1) * self.sigma) / (2.0 * math.sqrt(self.T))
        term2 = self.r * self.K * math.exp(-self.r * self.T) * self._normal_cdf(-d2)
        return term1 + term2

    def rho_call(self) -> float:
        """Call rho"""
        return self.K * self.T * math.exp(-self.r * self.T) * self._normal_cdf(self._d2())

    def rho_put(self) -> float:
        """Put rho"""
        return -self.K * self.T * math.exp(-self.r * self.T) * self._normal_cdf(-self._d2())

    def implied_volatility_call(self, market_price: float, sigma_init: float = 0.20) -> float:
        """Newton-Raphson implied volatility solver for calls."""
        sigma  = sigma_init
        MAX_ITER = 100
        TOLERANCE = 1e-6

        for i in range(MAX_ITER):
            trial = Option(self.S, self.K, self.T, self.r, sigma)
            price = trial.call_price()
            diff = price - market_price
            v = trial.vega

            if abs(v) < 1e-10:
                raise ValueError("IV solver failed: vega too small")

            sigma = sigma - diff / v
            if sigma <= 0:
                sigma = 1e-6

            if abs(diff) < TOLERANCE:
                return sigma

        raise ValueError("IV solver did not converge")
    
    def implied_volatility_put(self, market_price: float, sigma_init: float = 0.20) -> float:
        """Newton-Raphson implied volatility solver for puts"""
        sigma  = sigma_init
        MAX_ITER = 100
        TOLERANCE = 1e-6

        for i in range(MAX_ITER):
            trial = Option(self.S, self.K, self.T, self.r, sigma)
            price = trial.put_price()
            diff = price - market_price
            v = trial.vega()

            if abs(v) < 1e-10:
                raise ValueError("IV solver failed: vega too small")

            sigma = sigma - diff / v

            if sigma <= 0:
                sigma = 1e-6

            if abs(diff) < TOLERANCE:
                return sigma

        raise ValueError("IV solver did not converge")

    def monte_carlo_call(self, num_sims: int = 100000) -> tuple:
        """
        Monte Carlo call price with antithetic variates
        Returns (price, confidence_interval)
        """
        import numpy as np

        Z = np.random.standard_normal(num_sims)

        # Antithetic paths — +Z and -Z
        ST1 = self.S * np.exp((self.r - 0.5 * self.sigma**2) * self.T + self.sigma * math.sqrt(self.T) * Z)
        ST2 = self.S * np.exp((self.r - 0.5 * self.sigma**2) * self.T + self.sigma * math.sqrt(self.T) * (-Z))

        # Average antithetic payoffs
        payoffs = 0.5 * (np.maximum(ST1 - self.K, 0) + np.maximum(ST2 - self.K, 0))

        # Discount
        price = np.mean(payoffs) * math.exp(-self.r * self.T)
        std_error = np.std(payoffs) / math.sqrt(num_sims)
        ci = 1.96 * std_error * math.exp(-self.r * self.T)

        return (price, ci)

    def monte_carlo_put(self, num_sims: int = 100000) -> tuple:
        """
        Monte Carlo put price with antithetic variates
        Returns (price, confidence_interval)
        """
        import numpy as np

        Z = np.random.standard_normal(num_sims)

        ST1 = self.S * np.exp((self.r - 0.5 * self.sigma**2) * self.T + self.sigma * math.sqrt(self.T) * Z)
        ST2 = self.S * np.exp((self.r - 0.5 * self.sigma**2) * self.T + self.sigma * math.sqrt(self.T) * (-Z))

        payoffs = 0.5 * (np.maximum(self.K - ST1, 0) + np.maximum(self.K - ST2, 0))

        price = np.mean(payoffs) * math.exp(-self.r * self.T)
        std_error = np.std(payoffs) / math.sqrt(num_sims)
        ci = 1.96 * std_error * math.exp(-self.r * self.T)

        return (price, ci)
