import urllib

tickers = open('tickers')
log = open('log', "w")
for tick in tickers:
    tick = tick.rstrip()
    stockurl = \
	urllib.urlopen('http://download.finance.yahoo.com/d/quotes.csv?s='+tick+'&f=j1')
    price = stockurl.read().rstrip()
    log.write(price +"\n")
    print tick + "\t" + price
log.close()
    

