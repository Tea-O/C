#include <return_codes.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void swap_(float **var1, float **var2)
{
	float *temp = *var1;
	*var1 = *var2;
	*var2 = temp;
}

float epsGenerate(float **arr, int row, int num)
{
	int bufSize = 7;
	char *str;
	if ((str = (char *)malloc(bufSize * sizeof(char))) == NULL)
	{
		return 100;
	}
	int minEps = 0;
	for (int i = 0; i <= num; i++)
	{
		gcvt(arr[row][i], bufSize, str);
		int intCount = 0;
		for (int j = 0; j < bufSize; j++)
		{
			if (str[j] != 46)
			{
				intCount++;
			}
			else
			{
				intCount++;
				break;
			}
		}
		int floatCount = bufSize - intCount;
		for (int j = intCount; j >= bufSize; j++)
		{
			if (str[j] == 48)
			{
				floatCount--;
			}
			else
			{
				break;
			}
		}
		minEps = minEps > floatCount ? minEps : floatCount;
	}
	free(str);
	return powf(10, -((float)minEps));
}

int checkDeter(float **arr, int num)
{
	int flag = 0;
	for (int i = 0; i < num; i++)
	{
		if (fabsf(arr[i][i]) < 0.00001)
		{
			flag = 1;
			break;
		}
	}
	return flag;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Wrong count of argument");
		return ERROR_INVALID_PARAMETER;
	}
	int num;
	float varieble;
	float **arr = NULL;
	FILE *stream;
	FILE *out;
	int *visit = NULL;
	if ((stream = fopen(argv[1], "r")) == NULL)
	{
		printf("%s Cannot open file ", argv[1]);
		return ERROR_NOT_FOUND;
	}

	if ((out = fopen(argv[2], "w")) == NULL)
	{
		printf("%s Cannot open file ", argv[2]);
		fclose(stream);
		return ERROR_NOT_FOUND;
	}

	fscanf(stream, "%i", &num);

	if ((visit = (int *)malloc(num * sizeof(int))) == NULL)
	{
		printf("Not enough memory resources are available");
		fclose(stream);
		fclose(out);
		return ERROR_MEMORY;
	}

	for (int i = 0; i < num; i++)
	{
		visit[i] = 0;
	}

	if ((arr = (float **)malloc(num * sizeof(float *))) == NULL)
	{
		printf("Not enough memory resources are available");
		free(visit);
		fclose(stream);
		fclose(out);
		return ERROR_MEMORY;
	}

	for (int i = 0; i < num; i++)
	{
		if ((arr[i] = (float *)malloc((num + 1) * sizeof(float))) == NULL)
		{
			for (int t = 0; t < i; t++)
			{
				free(arr[t]);
			}
			free(arr);
			free(visit);
			fclose(stream);
			fclose(out);
			return ERROR_MEMORY;
		}
		for (int j = 0; j < num + 1; j++)
		{
			fscanf(stream, "%f", &varieble);
			arr[i][j] = varieble;
		}
	}

	int mxRow;
	int step = 0;
	float eps;

	for (int i = 0; i < num; i++)
	{
		// go in row
		mxRow = step;
		for (int k = i + 1; k < num; k++)
		{
			// go in row + 1
			// find max elem in col
			if (fabsf(arr[i][i]) < fabsf(arr[k][step]))
			{
				mxRow = k;
			}
		}
		if ((eps = epsGenerate(arr, i, num)) >= 100)
		{
			printf("Not enough memory resources are available");
			for (int t = 0; t < num; t++)
			{
				free(arr[t]);
			}
			free(arr);
			free(visit);
			fclose(stream);
			fclose(out);
			return ERROR_MEMORY;
		}
		if (fabsf(arr[mxRow][i]) < eps)
		{
			continue;
		}
		// swap current row with mxRow
		swap_(&arr[i], &arr[mxRow]);
		for (int k = i + 1; k < num; k++)
		{
			float mul3 = arr[k][i] / arr[step][i];
			for (int j = 0; j < num + 1; j++)
			{
				arr[k][j] -= arr[step][j] * mul3;
			}
		}
		visit[step] = 1;
		step++;
	}

	// check Determiner
	int locFlag = checkDeter(arr, num);

	float *ans = NULL;

	if ((ans = (float *)malloc(num * sizeof(float))) == NULL)
	{
		printf("Not enough memory resources are available");
		for (int t = 0; t < num; t++)
		{
			free(arr[t]);
		}
		free(arr);
		free(visit);
		fclose(stream);
		fclose(out);
		return ERROR_MEMORY;
	}

	if (visit[num - 1] != 0)
	{
		ans[num - 1] = arr[num - 1][num] / arr[num - 1][num - 1];
	}
	for (int i = num - 2; i >= 0; i--)
	{
		float sumElem = 0;
		if (visit[i] != 0)
		{
			for (int j = i + 1; j < num; j++)

			{
				float per = ans[j];
				sumElem += (arr[i][j] * per);
			}
			ans[i] = (arr[i][num] - sumElem) / arr[i][i];
		}
	}

	int ansFlag = 0;

	for (int i = 0; i < num; i++)
	{
		float checkSum = 0;
		for (int j = 0; j <= num; j++)
		{
			checkSum += arr[i][j] * ans[j];
		}
		if ((eps = epsGenerate(arr, i, num)) >= 100)
		{
			printf("Not enough memory resources are available");
			for (int t = 0; t < num; t++)
			{
				free(arr[t]);
			}
			free(arr);
			free(visit);
			free(ans);
			fclose(stream);
			fclose(out);
			return ERROR_MEMORY;
		}
		if (eps < fabsf(checkSum - arr[i][num]))
		{
			ansFlag = 1;
			break;
		}
	}
	// check for linearly independent arg in system
	for (int i = 0; i < num; i++)
	{
		if (visit[i] == 0)
		{
			ansFlag = 2;
			break;
		}
	}

	if (locFlag != 1)
	{
		for (int i = 0; i < num; i++)
		{
			fprintf(out, "%g\n", ans[i]);
		}
	}
	else
	{
		if (ansFlag == 1)
		{
			fprintf(out, "no solution");
		}
		else if (ansFlag == 2)
		{
			fprintf(out, "many solution");
		}
	}

	// clear memory
	for (int t = 0; t < num; t++)
	{
		free(arr[t]);
	}
	free(arr);
	free(visit);
	free(ans);
	fclose(stream);
	fclose(out);
	return 0;
}