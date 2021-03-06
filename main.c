#include "main.h"

float d2f(float x, float y, float d1, float d2)
{
	return powf(d2, 3) - 9 * sin(x) * d2 - sin(x) + 9 * exp(x) * d1 - (y / powf(x + 2, 3));
}

float Method_half_division(float a, float b, float x, float y, float d1)
{
	double c = 0;
	float eps = 1E-2;
	while(fabs(b - a) > eps) {
		c = (a + b) / 2;
		if(d2f(x, y, d1, a) * d2f(x, y, d1, c) < 0) {
			b = c;
		} else if (d2f(x, y, d1, c) * d2f(x, y, d1, b) < 0) {
			a = c;
		}
	}
	return (a + b) / 2;
}

float f(float x, float y, float d1)
{
	float a = 1, b = 0;
	float fa = 0, fb = 0;
	do {
		fa = d2f(x, y, d1, a--);
		fb = d2f(x, y, d1, b++);
	} while (fa * fb > 0);
	return Method_half_division(a, b, x, y, d1);
}

float RungeKutt2_time(float x0, float x1, float h, float y, float d1)
{
	float x = x0;
	float yt = 0, d1t = 0;
	for (; x < x1; x += h) {
		yt = y + (h / 2) * d1;
		d1t = d1 + (h / 2) * f(x, y, d1);
		y += h * d1t;
		d1 += h * f(x + h / 2, yt, d1t);
	}

	out_d1 = d1;
	return y;
}

float ShootingMethod(float x0, float x1, float y0, float y1, float h)
{
	float a = 1, b = 0;
	float fa = 0, fb = 0;
	float eps = 1E-4;

	do {
		fa = RungeKutt2_time(x0, x1, h, y0, a) - y1;
		fb = RungeKutt2_time(x0, x1, h, y0, b) - y1;
		a -= h;
		b += h;
	} while (fa * fb > 0);

	float g = 0;
	while(fabs(b - a) > eps) {
		g = (a + b) / 2;
		if((RungeKutt2_time(x0, x1, h, y0, a) - y1) * 
			(RungeKutt2_time(x0, x1, h, y0, g) - y1) < 0) {
			b = g;
		} else if ((RungeKutt2_time(x0, x1, h, y0, g) - y1) * 
			(RungeKutt2_time(x0, x1, h, y0, b) - y1) < 0) {
			a = g;
		}
	}
	return (a + b) / 2;
}

float DoubleCounting(float x0, float x1, float y0, float y1, float h)
{
	float delta = 1;
	float eps = 1E-4;
	float d1[2];
	int k = 0;
	for (float h0 = h; delta >= eps; h0 /= 2, k ^= 1) {
		d1[k] = ShootingMethod(x0, x1, y0, y1, h);

		if (h > h0)
			delta = fabsf(d1[k] - d1[k ^ 1]);
	}

	return d1[k];
}

float* DoubleCountingRunge(float *X, int n, float h, float x0, float x1, float y0, float d1)
{
	int n1 = n;
	int exit = 1, k = 0;
	float h1 = h;

	float *Y0 = malloc(sizeof(float) * n1);
	float *Y1 = malloc(sizeof(float) * n1);

	printf("Write Y0\n");
	for (int i = 0; i < n1; i++)
		Y0[i] = RungeKutt2_time(x0, X[i], h1, y0, d1);

	#if 1
	printf("k = %d\n", k);
	printf("h1 = %.8f\n", h1);
	for (int i = 0; i < n1; i++) {
		printf("X %f\t", X[i]);
		printf("Y0 %f\n", Y0[i]);
	}
	#endif

	printf("\n");

	do {
		k ^= 1;
		h1 /= 2;

		if (k == 0) {
			for (int i = 0; i < n1; i++) {
				Y0[i] = RungeKutt2_time(x0, X[i], h1, y0, d1);
				printf("X %f\t", X[i]);
				printf("Y0 %f\t", Y0[i]);
				printf("Y1 %f\t", Y1[i]);
				printf("Y` %f\n", out_d1);
			}
			printf("\n");
		} else {
			for (int i = 0; i < n1; i++) {
				Y1[i] = RungeKutt2_time(x0, X[i], h1, y0, d1);
				printf("X %f\t", X[i]);
				printf("Y0 %f\t", Y0[i]);
				printf("Y1 %f\t", Y1[i]);
				printf("Y` %f\n", out_d1);
			}
			printf("\n");
		}

		#if 0
		printf("k = %d\n", k);
		printf("h1 = %.8f\n", h1);
		for (int i = 0; i < n; i++) {
			printf("X %f\t", X[i]);
			printf("Y0 %f\t", Y0[i]);
			printf("Y1 %f\n", Y1[i]);
		}
		printf("\n");
		#endif

		exit = 0;

		for (int i = 0; i < n1; i++) {
			printf("%f\n", fabsf(Y0[i] - Y1[i]));

			if (fabsf(Y0[i] - Y1[i]) > eps) {
				printf("Break\n");
				exit = 1;

				if (k == 0) {
					for (int i = 0; i < n1; i++)
						Y1[i] = 0;
				} else {
					for (int i = 0; i < n1; i++)
						Y0[i] = 0;
				}

				break;
			}
		}
		printf("\n");
	} while (exit);

	#if 0
	for (int i = 0; i < n; i++) {
		printf("X %f\t", X[i]);
		printf("Y0 %f\t", Y0[i]);
		printf("Y1 %f\t", Y1[i]);
	}
	#endif

	if (k == 0) {
		free(Y1);
		return Y0;
	} else {
		free(Y0);
		return Y1;
	}

	return NULL;
}

float NIntegr(float a, float b, float y0, float d1)
{
	float n0 = 100; 
	float n = n0;
	float h;

	float S0 = 0;
	float delta = 1;
	float eps = 1E-2;

	for (;delta >= eps; n *= 2) {
		float S1 = 0;
		h = (b - a) / n;
		for (int i = 0; i < n; i++) {
			RungeKutt2_time(a, a + i * h, h, y0, d1);
			float y1 = out_d1;
			RungeKutt2_time(a, a + (i + 1) * h, h, y0, d1);
			float y2 = out_d1;
			S1 += fabsf(y1 + y2) / 2;
		}

		S1 *= h;
		delta = fabsf(S1 - S0);
		S0 = S1;
	}

	return S0;
}

float simpsons_rule(float *XIntegr, float *YIntegr, int n, float h)
{
	// // float n0 = n; 
	// // float n1 = n0;
	// // float h1 = h;

	// // float S0 = 0;
	// // float delta = 1;
	// // float eps = 1E-2;

	// float a = XIntegr[0];
	// float b = XIntegr[n - 1];

	// // for (;delta >= eps; n1 *= 2) {
	// // 	float S1 = 0;
	// 	// h = (b - a) / n1;
	// 	for (int i = 0, k = 0; i < n; i++, k ^= 1) {
	// 		if (i == 0 || i == n - 1) {
	// 			S1 += f(a + i * h);
	// 		} else if (k == 0) {
	// 			S1 += f(a + i * h) * 2;
	// 		} else if (k == 1) {
	// 			S1 += f(a + i * h) * 4;
	// 		}
	// 	}
	// 	S1 *= h / 3;

	// // 	delta = fabsf(S1 - S0);

	// // 	S0 = S1;
	// // }

	// return S1;

	float sum = 0;
	float a = XIntegr[0];
	float b = XIntegr[n - 1];
	for(int i = 0; i <= n; i++)
		sum += (i % 2 ? 4 : 2) * YIntegr[i];
	return (YIntegr[0] + sum + YIntegr[n - 1]) * h / 3;
}

int main()
{
	float x0 = 0, x1 = 1, y0 = 3, y1 = 1;
	float h = 0.2;
	int n = 6;

	float d1 = DoubleCounting(x0, x1, y0, y1, h);

	float *X = malloc(sizeof(float) * n);
	for (int i = 0; i < n; i++)
		X[i] = h * i;

	float *Y = DoubleCountingRunge(X, n, h, x0, x1, y0, d1);
	if (Y == NULL) {
		printf("kek\n");
		return 0;
	}

	FILE *res = fopen("res.txt", "w");
	for (int i = 0; i < n; i++) {
		fprintf(res, "%.3f %.3f\n", X[i], Y[i]);
	}
	fclose(res);

	FILE *out = fopen("out.txt", "w");
	for (float i = x0; i <= x1; i += (h / 10)) {
		fprintf(out, "%.3f %.3f\n", i, Lagrange(i, X, Y, n));
	}
	fclose(out);

	int n2 = 100;
	float h2 = 1 / n2;
	float *XIntegr = malloc(sizeof(float) * (n2 + 1));
	float *YIntegr = malloc(sizeof(float) * (n2 + 1));
	for (int i = 0; i <= 100; i++) {
		XIntegr[i] = h2 * i;
		RungeKutt2_time(x0, XIntegr[i], h2, y0, d1);
		YIntegr[i] = out_d1;
	}

	float I = NIntegr(x0, x1, y0, d1);
	printf("I = %.3f\n", I);

	free(XIntegr);
	free(YIntegr);

	free(X);
	free(Y);

	return 0;
}