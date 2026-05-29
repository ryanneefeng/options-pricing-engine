"""
Options Pricing Backtest Demo
Pulls historical stock data via yfinance, computes historical volatility,
and prices a strip of European options using Black-Scholes.

"""

import math
import datetime
import numpy as np
import pandas as pd
import yfinance as yf
from pricing import Option


def get_historical_volatility(ticker: str, period: str = "1y") -> tuple:
    """
    Download historical price data and compute annualized volatility.
    Returns (sigma, current_price, dataframe)
    
    """
    # Download price history
    data = yf.download(ticker, period=period, auto_adjust=True, progress=False)

    # Use closing prices
    closes = data["Close"].squeeze()

    # Daily log returns: ln(P_t / P_t-1)
    # This is the standard way to compute returns for volatility estimation
    log_returns = np.log(closes / closes.shift(1)).dropna()

    # Annualized volatility = daily std dev * sqrt(252)
    # 252 = number of trading days in a year
    daily_vol= log_returns.std()
    annual_vol = daily_vol * math.sqrt(252)
    current_price = float(closes.iloc[-1])

    return annual_vol, current_price, data

def price_options_strip(ticker: str, expiry_days: int = 30, num_strikes: int = 7, r: float = 0.05) -> pd.DataFrame:
    """
    Price a strip of European options across strikes centered on current price.

    Parameters
    ----------
    ticker : stock ticker e.g. 'AAPL'
    expiry_days  : days until expiration
    num_strikes  : number of strikes to price (odd number, centered at ATM)
    r : risk-free rate
    
    """
    # Pull historical vol and current price
    sigma, S, _ = get_historical_volatility(ticker)
    T = expiry_days / 365.0

    # Generate strikes centered on current price
    # e.g. if S=150 and spacing=5%, strikes = [127.5, 135, 142.5, 150, 157.5, 165, 172.5]
    spacing = 0.05
    half = num_strikes // 2
    strikes = [S * (1 + spacing * i) for i in range(-half, half + 1)]

    # Price each strike
    rows = []
    for K in strikes:
        opt = Option(S, K, T, r, sigma)
        call = opt.call_price()
        put = opt.put_price()
        call_delta = opt.delta_call()
        put_delta = opt.delta_put()
        call_iv = opt.implied_volatility_call(call)
        moneyness = S / K  # > 1 = ITM call, < 1 = OTM call

        rows.append({
            "Strike":round(K, 2),
            "Moneyness":round(moneyness, 4),
            "Call Price":round(call, 4),
            "Put Price":round(put, 4),
            "Call Delta":round(call_delta, 4),
            "Put Delta":round(put_delta, 4),
            "IV (Call)":round(call_iv * 100, 2),
            "Sigma Used":round(sigma * 100, 2),
        })

    df = pd.DataFrame(rows)
    return df, S, sigma, T

if __name__ == "__main__":
    ticker = "AAPL"

    print(f"\nFetching data for {ticker}...")
    df, S, sigma, T = price_options_strip(ticker, expiry_days=30)

    print(f"\nCurrent Price: ${S:.2f}")
    print(f"Historical Volatility: {sigma*100:.2f}%")
    print(f"Time to Expiry: 30 days ({T:.4f} years)")
    print(f"\nOptions Strip — 30-Day European Options ({ticker})")
    print("=" * 75)
    print(df.to_string(index=False))
    print("=" * 75)
