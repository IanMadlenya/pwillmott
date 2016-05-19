/***************************** binomial.c **************************************
 * Implements the binomial model for pricing options
 * prices a European call option
 * you can derive the put with P = C - S + E * e^(-rt)
 * 
 * Author: Ronald Macmaster
 * Date: May 18, 2016
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* call option data structure */
typedef struct option{
	double asset; 
	double strike; 
	double expiry;
	double rate;
	double volatility; /* σ, asset's standard deviation */
} option;

double OptionPrice(option opt);
double Payoff(double price, double strike);

int main(){

	/* prompt the user for option parameters */
	option opt;
	printf("Enter the asset price: "); scanf("%lf", &opt.asset);
	printf("Enter the strike price: ");  scanf("%lf", &opt.strike);
	printf("Enter the expiry in months: ");  scanf("%lf", &opt.expiry);
	printf("Enter the interest rate as a percent: "); scanf("%lf", &opt.rate);
	printf("Enter the volatility: ");  scanf("%lf", &opt.volatility);
	printf("\n\n");

	/* print the option's value */
	opt.rate = opt.rate / 100.0; 
	opt.expiry = opt.expiry / 12.0;
	printf("The value of your option is: %.2lf\n", OptionPrice(opt));

}

/** OptionPrice() **************************************************************
 * Calculate the price the the option opt
 * divides the time interval into n steps
 * 
 * param opt: the option object
 ******************************************************************************/
double OptionPrice(option opt){
	
	const int n = 1000; /* number of steps */
	int idx = 0, jdx = 0;
	double step = opt.expiry / n;
	double discount = exp(-opt.rate * step);
	double ufactor = 0.5 * (discount + exp((opt.rate + opt.volatility * opt.volatility) * step));
	
	double u = ufactor + sqrt(ufactor * ufactor - 1);
	double v = 1.0 / u;
	double p = (exp(opt.rate * step) - v) / (u - v);

	/* asset and option storage */
	double price;
	double *assetPrices = (double *)malloc((n+1) * sizeof(double));
	double *optionPrices = (double *)malloc((n+1) * sizeof(double));
 	
	/* build binomial coefficients at the expiry level */
	assetPrices[0] = opt.asset;
	for(idx = 1; idx <= n; idx++){
		for(jdx = idx; jdx >= 1; jdx--){
			assetPrices[jdx] = u * assetPrices[jdx - 1];
		}
		assetPrices[0] = v * assetPrices[0];
	}

	#ifdef DEBUG1
	printf("u: %lf \t v: %lf \n", u, v);
	for(idx = 0; idx <= n; idx++){
		printf("%lf\n", assetPrices[idx]);
	}printf("\n");
	#endif
	
	/* calculate payoffs at expiry */
	for(jdx = 0; jdx <= n; jdx++){
		optionPrices[jdx] = Payoff(assetPrices[jdx], opt.strike);
	}

	#ifdef DEBUG2
	printf("strike: %lf\n", opt.strike);
	for(idx = 0; idx <= n; idx++){
		printf("%lf\n", optionPrices[idx]);
	}printf("\n");
	#endif
	
	/* backpropogate the option price to current value */
	for(idx = n; idx >= 1; idx--){
		for(jdx = 0; jdx < n; jdx++){
			optionPrices[jdx] = discount * (p * optionPrices[jdx + 1] + (1 - p) * optionPrices[jdx]);
		}
	}

	price = optionPrices[0];
	free(assetPrices);
	free(optionPrices);
	return price;
}

/** Payoff() *******************************************************************
 * Calculate the payoff of a particular option
 * acts as a modular function
 * 
 * param price: the price of the asset
 * param price: the strike price of the option
 ******************************************************************************/
double Payoff(double price, double strike){
	if(price > strike)
		return price - strike;
	else
		return 0;
}
