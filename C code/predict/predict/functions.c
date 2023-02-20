#include <stdio.h>
#include <math.h>

#define Num_Layers 5;
#define Num_Classes 10;
#define IN 784;  
#define layer1 128;
#define layer2_3 1024;
#define weight_len (IN*layer1+layer1*layer2_3 +layer2_3*layer2_3 

//kernel = weights
int sign(double val)
{
	if (val == 0)
		val = 1;					//can change later
	return val / abs(val);			//sign = x/|x|
}

void quantization(double* arr, int len) 
{
	for (int i = 0; i < len; i++)
		*(arr + i) = sign(*(arr + i));
}
/*void ReLU(double* img_arr, int len, double bias)
{
	int cnt = 0;
	for (int i = 0; i < len; i++)
		{
			*(img_arr + i) = Max(*(img_arr + i), 0);
		}
}*/


//output = activation(dot(input_quantizer(input), kernel_quantizer(kernel)))