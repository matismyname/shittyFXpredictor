This is a very simple and not-at-all-good FX prediction algorithm.
What it does is take in a historical .csv file, calculate the day-deltas, infer the daily volatility and "probability" of the FX-rate going up or down to predict future FX-rates. The algorithm suffers from drift, which I tried to fix by including a "shock" event. It didn't work.
