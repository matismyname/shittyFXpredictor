#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

double *calculateParameters(char *filename);
double *monteCarlo(double p, double q, double step, double startValue, int iterations, int numOfPredictions);
double randomNumber(double p, double q, double step);

int main(int argc, char *argv[])
{
	double *params;
	double *predictions;
	FILE *outputFile;
	int numOfPredictions = atoi(argv[3]);
	int monteCarloIterations = atoi(argv[2]);
	double p = 0;
       	double q = 0;

	if (argc < 2)
	{
		printf("Please give a csv file with historical FX data!");
		return 1;
	}
	params = calculateParameters(argv[1]);
	p = params[0];
	q = params[1];
	predictions = monteCarlo(p, q, params[2], params[3], monteCarloIterations, numOfPredictions); //p, q, step, start value, monte Carlo iterations, number of predictions

	outputFile = fopen("output_file", "w");
	for (int i = 0; i < numOfPredictions; i++)
	{
		fprintf(outputFile, "%.6lf\n", predictions[i]);
	}

	fclose(outputFile);
	free(params);
	free(predictions);
	return 0;
}

#include <stdlib.h>

double *monteCarlo(double p, double q, double step,  double startValue, int iterations, int numOfPredictions) {
    double **simulations = (double **)malloc(iterations * sizeof(double*));
    double *res = (double*)malloc(numOfPredictions * sizeof(double*));
    double value = 0;
    double upProb = p;
    double downProb = q;
    double shockVal = 0;
    int shockIdx = 0;
    bool shock = false;

    for (int i = 0; i < iterations; i++) {
        simulations[i] = (double*)malloc(numOfPredictions * sizeof(double));
        value = startValue + randomNumber(upProb, downProb, step);

        for (int j = 0; j < numOfPredictions; j++) {
		shock = false;
		shockVal = randomNumber(0.8800, 0.1200, 1);
		if (shockVal == -1 || shock)
		{
			upProb = 0.2000;
			downProb = 0.8000;
			if (shockIdx < 60)
			{
				shock = true;
			}
			shockIdx++;
		}
		else
		{
			shockIdx = 0;
			upProb = p;
			downProb = q;
		}

            simulations[i][j] = value;
            value += randomNumber(upProb, downProb, step);
        }
    }

    for (int i = 0; i < numOfPredictions; i++) {
        value = 0;

        for (int j = 0; j < iterations; j++) {
            value += simulations[j][i];
        }

	res[i] = value / iterations;
    }

    free(simulations);
    return res;
}


double randomNumber(double p, double q, double step)
{
	int num = rand() % 10000;
	int p_tilde = (int)(p * 10000);
	int q_tilde = (int)(q * 10000);
	double value = 0;

	if (p_tilde < q_tilde)
	{
		if (num < p_tilde)
		{
			value = step;
		}
		else
		{
			value = -step;
		}
	}
	else
	{
		if (num < q_tilde)
		{
			value = -step;
		}
		else
		{
			value = step;
		}
	}

	return value;
}

double *calculateParameters(char *filename) {
    FILE *fptr = fopen(filename, "r");
    char buffer[1024];

    if (fptr == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    double *res = (double*)malloc(4 * sizeof(double));
    double *deltas = (double*)malloc(10000 * sizeof(double));
    int clm = 0;
    double open = 0;
    double close = 0;
    double delta = 0;
    long up = 0;
    long down = 0;
    long numOfCalcs = 0;
    double sumOfDeltas = 0;

    double p = 0;
    double q = 0;

    while (fgets(buffer, 1024, fptr)) {
        clm = 0;

        char *token = strtok(buffer, ",");
        while (token) {
            switch (clm) {
                case 1:
                    open = strtod(token, NULL);
                    break;
                case 4:
                    close = strtod(token, NULL);
                    break;
                default:
                    break;
            }

            token = strtok(NULL, ",");
            clm++;
        }
	
        delta = open - close;
        if (delta < 0) {
            down++;
            delta *= -1;
        } else {
            up++;
        }

        deltas[numOfCalcs] = delta;
        numOfCalcs++;
        clm = 0;
    }


    p = (double)up / numOfCalcs;
    q = (double)down / numOfCalcs;

    for (int i = 0; i < numOfCalcs; i++) {
        sumOfDeltas += deltas[i];
    }

    res[0] = p;
    res[1] = q;
    res[2] = (double)sumOfDeltas / numOfCalcs;
    res[3] = close;

    fclose(fptr);

    return res;
}

