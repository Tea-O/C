#include "return_codes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isIHDR(const u_char *arr);
int isIEND(const u_char *arr);
int isIDAT(const u_char *arr);
long long pow2(int x);
long long chunkLength(const u_char *arr);
long long pow2(x)
{
	long long res = 1;
	for (int i = 0; i < x; i++)
	{
		res *= 16;
	}
	return res;
}

#define ZLIB
#if defined(ZLIB)
	#include <zlib.h>
#elif defined(DEFLATE)
	#error "deflate library isn't saported"
#elif defined(ISL - 1)
	#error "ISL-1 library isn't saported"
#else
	#error "undefind macros"
#endif

u_char png[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Wrong count of argument");
		return ERROR_INVALID_PARAMETER;
	}
	FILE *stream = fopen(argv[1], "rb");
	if ((stream) == NULL)
	{
		printf("%s Cannot open file ", argv[1]);
		return ERROR_NOT_FOUND;
	}
	unsigned char *data = NULL;
	if ((data = malloc(8 * sizeof(unsigned char))) == NULL)
	{
		printf("Not enough memory resources are available");
		fclose(stream);
		return ERROR_MEMORY;
	}
	if ((fread(data, 1, 8, stream) != 8))
	{
		printf("Invalid Data");
		fclose(stream);
		free(data);
		return ERROR_INVALID_DATA;
	}
	if (memcmp(png, data, 8) != 0)
	{
		printf("It isn't PNG");
		fclose(stream);
		free(data);
		return ERROR_INVALID_DATA;
	}
	unsigned char *chunk = NULL;
	chunk = malloc(4 * sizeof(unsigned char));
	if (chunk == NULL)
	{
		printf("Not enough memory resources are available");
		fclose(stream);
		free(data);
		return ERROR_MEMORY;
	}
	long long chuckSize = 0;
	if ((fread(chunk, 1, 4, stream)) != 4)
	{
		printf("Invalid Data");
		fclose(stream);
		free(data);
		free(chunk);
		return ERROR_INVALID_DATA;
	}
	chuckSize = chunkLength(chunk);
	if ((fread(chunk, 1, 4, stream)) != 4)
	{
		printf("Invalid Data");
		fclose(stream);
		free(data);
		free(chunk);
		return ERROR_INVALID_DATA;
	}
	int res = 0;
	res = isIHDR(chunk);
	if (!(res) || (chuckSize != 13))
	{
		printf("Invalid Data");
		fclose(stream);
		free(data);
		free(chunk);
		return ERROR_INVALID_DATA;
	}
	unsigned char *dataIHDR = NULL;
	dataIHDR = malloc(chuckSize * sizeof(unsigned char));
	if ((fread(dataIHDR, 1, 13, stream)) != 13)
	{
		printf("Invalid Data");
		free(dataIHDR);
		fclose(stream);
		free(data);
		free(chunk);
		return ERROR_INVALID_DATA;
	}
	fseek(stream, 4, SEEK_CUR);
	unsigned char *dataChunk = NULL;
	dataChunk = malloc(0 * sizeof(unsigned char));
	if (dataChunk == NULL)
	{
		printf("Not enough memory resources are available");
		free(dataChunk);
		free(dataIHDR);
		fclose(stream);
		free(data);
		free(chunk);
		free(dataChunk);
		return ERROR_MEMORY;
	}
	unsigned char *dataIDAT = NULL;
	unsigned char *subChunk = NULL;
	dataIDAT = malloc(1 * sizeof(unsigned char));
	if (dataIDAT == NULL)
	{
		printf("Not enough memory resources are available");
		free(dataChunk);
		free(dataIHDR);
		fclose(stream);
		free(data);
		free(chunk);
		free(dataIDAT);
		return ERROR_MEMORY;
	}

	int flagIDAT = 0;
	long long globalLength = 0;
	while (!isIEND(chunk))
	{
		if ((fread(chunk, 1, 4, stream)) != 4)
		{
			printf("Invalid Data");
			free(dataChunk);
			free(dataIHDR);
			fclose(stream);
			free(data);
			free(chunk);
			free(dataIDAT);
			return ERROR_INVALID_DATA;
		}
		chuckSize = chunkLength(chunk);
		if ((fread(chunk, 1, 4, stream)) != 4)
		{
			printf("Invalid Data");
			free(dataChunk);
			free(dataIHDR);
			fclose(stream);
			free(data);
			free(chunk);
			free(dataIDAT);
			return ERROR_INVALID_DATA;
		}
		if (!isIDAT(chunk))
		{
			fseek(stream, chuckSize + 4, SEEK_CUR);
		}
		else
		{
			flagIDAT = 1;
			subChunk = dataIDAT;
			if ((dataIDAT = realloc(dataIDAT, (globalLength + chuckSize) * sizeof(unsigned char))) == NULL)
			{
				printf("Not enough memory resources are available");
				free(subChunk);
				free(dataChunk);
				free(dataIHDR);
				fclose(stream);
				free(data);
				free(chunk);
				free(dataIDAT);
				return ERROR_MEMORY;
			}
			unsigned char *IDAT2 = NULL;
			IDAT2 = malloc(chuckSize * sizeof(unsigned char));
			if (IDAT2 == NULL)
			{
				printf("Not enough memory resources are available");
				free(dataChunk);
				free(dataIHDR);
				fclose(stream);
				free(data);
				free(chunk);
				free(dataIDAT);
				free(IDAT2);
				return ERROR_MEMORY;
			}
			if ((long long)(fread(IDAT2, 1, chuckSize, stream)) != chuckSize)
			{
				printf("Invalid Data");
				free(dataChunk);
				free(dataIHDR);
				fclose(stream);
				free(data);
				free(chunk);
				free(dataIDAT);
				free(IDAT2);
				return ERROR_INVALID_DATA;
			}
			for (long i = globalLength; i < globalLength + chuckSize; i++)
			{
				dataIDAT[i] = IDAT2[i - globalLength];
			}
			free(IDAT2);
			fseek(stream, 4, SEEK_CUR);
			globalLength += chuckSize;
		}
	}
	unsigned char high[4];
	for (int i = 0; i < 4; i++)
	{
		high[i] = dataIHDR[i + 4];
	}
	unsigned char weigh[4];
	for (int i = 0; i < 4; i++)
	{
		weigh[i] = dataIHDR[i];
	}
	u_char colorType = dataIHDR[9];
	unsigned char *filtr = NULL;
	long long outHigh = (chunkLength(high) + 1);
	long long outWeight = chunkLength(weigh);
	size_t sizeFil = outHigh * outWeight * (colorType + 1) * sizeof(unsigned char);
	filtr = malloc(sizeFil);
#if defined(ZLIB)
	int ans = uncompress(filtr, &sizeFil, dataIDAT, globalLength);
#endif
	for (int i = 0; i < outHigh; i++)
	{
		for (int j = 0; j < outWeight; j++)
		{
			// filter...
		}
	}
	free(dataChunk);
	free(dataIHDR);
	fclose(stream);
	free(data);
	free(chunk);
	free(dataIDAT);
	return 0;
}

long long chunkLength(const u_char *arr)
{
	int step = 0;
	long long length = 0;
	for (int i = 3; i >= 0; i--)
	{
		length += arr[i] * pow2(step);
		step += 2;
	}
	return length;
}

int isIHDR(const u_char *arr)
{
	if (arr[0] == 0x49 && (arr[1] == 0x48 || arr[1] == 0x68) && arr[2] == 0x44 && (arr[3] == 0x52 || arr[3] == 0x72))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int isIEND(const u_char *arr)
{
	if (arr[0] == 'I' && (arr[1] == 'E') && arr[2] == 'N' && (arr[3] == 'D'))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int isIDAT(const u_char *arr)
{
	if (arr[0] == 'I' && (arr[1] == 'D') && arr[2] == 'A' && (arr[3] == 'T'))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
