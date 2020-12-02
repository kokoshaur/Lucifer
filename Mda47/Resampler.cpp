#include <cstdio>
#include <iostream>
#include <vector>
#pragma once
using namespace std;
class Resampler
{
private:
	FILE* inp;
	FILE* out;

	const char* pathInp;
	const char* pathOut;
	const char* pathDec;

	vector<char> block;

	int size;
	int skip;

	void openFile(const char* inpute, const char* outpute)
	{
		block.resize(size);

		inp = fopen(inpute, "rb");
		out = fopen(outpute, "w+b");

		if (inp == NULL)
			cout << "Не найден файл: " << inpute << endl;
		if (out == NULL)
			cout << "Не найден файл: " << outpute << endl;

		vector<char> buf(skip);
		fread(buf.data(), sizeof(char), skip, inp);
		fwrite(buf.data(), sizeof(char), skip, out);
		fflush(out);
	}

public:
	Resampler(int sizeBlock, const char* inpute, const char* outpute, const char* decrypro, int type)
	{
		size = sizeBlock;
		skip = type;

		block.resize(size);

		pathInp = inpute;
		pathOut = outpute;
		pathDec = decrypro;
	}

	void mod(bool isCrypto)
	{
		if (isCrypto)
			openFile(pathInp, pathOut);
		else
			openFile(pathOut, pathDec);
	}

	vector<char> nextBlock()
	{
		size_t were = fread(block.data(), sizeof(char), size, inp);

		if (were < size)
			block.resize(were);

		return block;
	}

	void setBlock(vector<char>& block)
	{
		fwrite(block.data(), sizeof(char), block.size(), out);
		fflush(out);
	}
};
