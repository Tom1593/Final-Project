#include <stdio.h>
#include <math.h>
//#include <pthread.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>

#define Num_Layers 5;
#define Num_Classes 10;
#define IN 784;  
#define layer1 128;
#define layer2_3 1024;
#define weight_len (IN*layer1+layer1*layer2_3 +layer2_3*layer2_3) 


static const int numInputs = 784;
static const int numHiddenLayer1Nodes = 128;
static const int numHiddenLayer2Nodes = 1024;
static const int numHiddenLayer3Nodes = 1024;
static const int numOutputs = 10;

char* PATH_to_params = "D:\\HW\\project\\Ternary-Weights-Network-master\\Params";
char* PATH_to_params_for_file = "D:\\HW\\project\\Ternary-Weights-Network-master\\Params\\";

//to store intermidiate results
double hidden1Layer[128];
double hidden2Layer[1024];
double hidden3Layer[1024];

double outputLayer[10];

//save the trained Bias
double hiddenBaisI1[784][128];
double hiddenBias12[128][1024];
double hiddenBias23[1024][1024];

double outputBias[1024][10];

//save the trained Weights
//double hiddenWeightsI1[784][128];
double hiddenWeights12[128][1024];
double hiddenWeights23[1024][1024];

double outputWeights[1024][10];

//kernel = weights
int sign(double val)
{
	if (val == 0)
		val = 1;					//can change later
	return val / abs(val);			//sign = x/|x|
}
int argmax_softmax(int layer_nodes,double* layer)
{
	float Sum = 0;
	double prob[10];
	double max = 0;
	int indx = 0;
	for (int i = 0; i < layer_nodes; i++) 
		Sum += exp(layer[i]);
	for (int i = 0; i < layer_nodes; i++)
		prob[i] = exp(layer[i]) / Sum;
	for (int i = 1; i < layer_nodes; ++i) {
		if (max < prob[i]) {
			max = prob[i];
			indx = i;
		}
	}

	return indx;
}

int argmax(int layer_nodes, double* layer)
{
	double max = 0;
	int indx = 0;
	for (int i = 1; i < layer_nodes; ++i) {
		if (max < layer[i]) {
			max = layer[i];
			indx = i;
		}
	}

	return indx;
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

// Compute hidden layer activation
void computeLayer(int layer1_nodes,int layer2_nodes,double** layer2_weights,
	double* layer2_bias,double* input_layer,double* output_layer,int flag)
{
	for (int j = 0; j < layer2_nodes; j++)
	{
		double activation = layer2_bias[j];
		for (int k = 0; k < layer1_nodes; k++) {
			activation += input_layer[k] * layer2_weights[k][j];
			if (!flag)
				output_layer[j] = sign(activation);
			else
				output_layer[j] = activation;
		}
	}

}



void loadParams(double** layer, int layer1_nodes, int layer2_nodes,char* Path,char* name_of_file) {
	FILE* file;
	char curr_path[1024];
	strcpy(curr_path, Path);
	strcat(curr_path, name_of_file);
	fopen_s(&file,curr_path, "r");
	//fopen_s(&file,"D:\\HW\\project\\Ternary-Weights-Network-master\\Params\\fc1.weight.txt", "r");
	int inter;
	int i = 0, j = 0;
	while (fscanf_s(file, "%d", &inter) != EOF)
	{
		if (i < layer1_nodes) {
			layer[i][j] = (double)inter;
			i++;
		}
		else 
		{
			i = 0;
			j++;
			layer[i][j] = inter;
			i++;
		}
	}
	fclose(file);
}
double** create_2dArray(int row, int col)
{
	int i;
	double** layer;
	layer = (double**)malloc(row * sizeof(double*));
	for (i = 0; i < row; i++)
		layer[i]  = (double*)malloc(col * sizeof(double));
	return layer;
}
char** list_dir_content(char* path,int* num_of_files)
{
	int i = 0;
	DIR* d;
	struct dirent* dir;
	//need to implament relative path
	d = opendir("D:\\HW\\project\\Ternary-Weights-Network-master\\Params");
	char** list_of_params;
	list_of_params = (char**)malloc(100 * sizeof(char*));
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if ((i < 100) && (strcmp(dir->d_name, ".") != 0)
				&& (strcmp(dir->d_name, "..") != 0))
			{
				list_of_params[i] = (char*)malloc((dir->d_namlen)+1 * sizeof(char));
				strcpy_s(list_of_params[i],(dir->d_namlen)+1, dir->d_name);
				i++;
			}
		}
		closedir(d);
	}
	*(num_of_files) = i;
	return list_of_params;
}
//need to free all used mem or this will be bad
void main() {
	
	char** dir_content;
	double** hiddenWeightsI1;
	int number_of_files = 0;
	//int img;
	//int prediction;
	//loadParams(hiddenWeightsI1,784,128);
	computeLayer(784, 128, hiddenWeightsI1, hiddenBaisI1, img, hidden1Layer,0);
	//computeLayer(128, 1024, hiddenWeights12, hiddenBias12, hidden1Layer, hidden2Layer,0);
	//computeLayer(1024, 1024, hiddenWeights23, hiddenBias23,hidden2Layer, hidden3Layer,0);
	//computeLayer(1024, 10, outputWeights, outputBias, hidden3Layer, outputLayer,0);
	//prediction = argmax(10,outputLayer);
	dir_content = list_dir_content(PATH_to_params,&number_of_files);
	hiddenWeightsI1 = create_2dArray(784, 128);
	loadParams(hiddenWeightsI1, 784, 128, PATH_to_params_for_file,dir_content[0]);

}