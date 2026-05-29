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
    daily_vol   = log_returns.std()
    annual_vol  = daily_vol * math.sqrt(252)
    current_price = float(closes.iloc[-1])

    return annual_vol, current_price, data
