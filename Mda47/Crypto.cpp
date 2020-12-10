#include "Resampler.cpp"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <fstream>
#pragma once

class Crypto
{
private:
	int r;
	const int table[16] = { 2,5,4,0,7,6,10,1,9,14,3,12,8,15,13,11 };
	const int myTable[16] = { 0,8,9,2,7,6,11,1,4,14,3,12,5,15,13,10 };
	vector<vector<char>> keys;
	
	Resampler* file;

	std::ofstream out;          // поток для записи

	bool isCrypto;

public:
	Crypto(Resampler* resampler, vector<char>& key,  int R = -1)
	{
		out.open("Resample\\avtocor.txt"); // окрываем файл для записи
		
		file = resampler;

		if (R < 0)
			r = key.size() / 16;
		else
			r = R;

		keys.resize(r);
		for (int i = 0; i < r; i++)
		{
			keys[i].resize(key.size());
			for (int j = 0; j < keys[i].size(); j++)
				keys[i][j] = key[(i + j)%key.size()] * 12345%321;
		}
	}

	void mod(bool isCrypto)
	{
		this->isCrypto = isCrypto;
		file->mod(isCrypto);
	}

	void start()
	{
		vector<char> block = file->nextBlock();

		while (block.size() != 0)
		{
			cryptoBlock(block);
			file->setBlock(block);
			block = file->nextBlock();
		}
	}

private:	
	void cryptoBlock(vector<char>& block)	//на основе сетей Фейстеля
	{
		if (isCrypto)
		{
			vector<char> oldBlock = block;
			forceKey(block);
			sandwith(block, isCrypto);
			mda(oldBlock, block);
		}
		else
		{
			sandwith(block, isCrypto);
			forceKey(block);
		}
	}

	void forceKey(vector<char>& block)	//сеть Фейстеля
	{
		bool flag = false;
		if (block.size() % 2 != 0)
		{
			block.resize(block.size() + 1);
			flag = true;
		}

		vector<char> left(block.size() / 2);
		vector<char> righ(block.size() / 2);
		vector<char> buf(block.size() / 2);

		for (int j = 0; j < left.size(); j++)
			left[j] = block[j];
		for (int j = righ.size(); j < block.size(); j++)
			righ[j - righ.size()] = block[j];

		int subj;
		if (isCrypto)
			subj = 0;
		else
			subj = r-1;
		
		#define INDEX abs(subj - i)
		#define OUTDEX (((r - subj - 1) % r >= 0) ? (r - subj - 1) % r : ((r - subj - 1) % r) + r)

		for (int i = 0; i < r - 1; i++)
		{
			for (int j = 0; j < left.size(); j++)
				buf[j] = left[j];
			for (int j = 0; j < left.size(); j++)
				left[j] = righ[j] ^ F(left[j], keys[INDEX][j]);
			for (int j = 0; j < righ.size(); j++)
				righ[j] = buf[j];
		}
		for (int j = 0; j < righ.size(); j++)
			righ[j] = righ[j] ^ F(left[j], keys[OUTDEX][j]);

		for (int j = 0; j < left.size(); j++)
			block[j] = left[j];
		for (int j = righ.size(); j < block.size(); j++)
			block[j] = righ[j - righ.size()];
		
		if (flag)
			block.resize(block.size() - 1);
	}

	char F(char subj, char key)
	{
		return subj + key;
	}

	void sandwith(vector<char>& block, bool isCrypto) //Сенгдвич падшего ангела
	{
		int buf1, buf2;
		char mask;
		for (int i = 0; i < block.size(); i++)
		{
			buf1 = buf2 = 0;
			mask = 0x0001;
			
			for (int j = 0; j < 4; j++)
			{
				buf1 += (block[i] & mask) ? pow(2, j) : 0;
				mask <<= 1;
			}
			for (int j = 4; j < 8; j++)
			{
				buf2 += (block[i] & mask) ? pow(2, j-4) : 0;
				mask <<= 1;
			}

			if (isCrypto)
				block[i] = table[buf1] + table[buf2] * 16;
			else
			{
				int a = 0, b = 0;
				for (int i = 0; i < 16; i++)
				{
					if (table[i] == buf1)
						a = i;
					if (table[i] == buf2)
						b = i;
				}

				block[i] = a + b * 16;
			}
		}
	}

	void show(char subj)
	{
		for (int i = 0; i < 8; i++) 
		{
			printf("%c", (subj & 0x80) ? '1' : '0');
			subj <<= 1;
		}
		printf("\n");
	}

	void mda(vector<char> oldB, vector<char> newB)	//Анализы (выводим по первому байту из каждого блока, так как блок слишком большой и экран засирается)
	{
		avtocor(newB);
		
		vector<int> aga(oldB.size()*8);
		int koef = 0;
		int numOfOne = 0;

		int dushnoVichislat = 0;
		for (int q = 0; q < oldB.size(); q++)
		{
			for (int i = 0; i < 8; i++)
			{
				aga[dushnoVichislat] = ((oldB[q] & 0x80) ? 1 : 0);
				cout << aga[dushnoVichislat] << " ";
				oldB[q] <<= 1;
				dushnoVichislat++;
			}
		}
		cout << endl;

		for (int q = 0; q < oldB.size(); q++)
		{
			for (int i = 0; i < 8; i++)
			{
				if (aga[i] != ((newB[q] & 0x80) ? 1 : 0))
				{
					cout << "\x1b[31m" << ((newB[q] & 0x80) ? '1' : '0') << "\x1b[0m" << " ";
					koef++;
				}
				else
					cout << ((newB[q] & 0x80) ? '1' : '0') << " ";

				if ((newB[q] & 0x80))
					numOfOne++;

				newB[q] <<= 1;
			}
		}
	
		cout << endl;
		cout << "Количество нулей и единиц: " << (oldB.size() * 8) - numOfOne << " к " << numOfOne << endl << endl;
	}

	void avtocor(vector<char> block)
	{
		for (int i = 0; i < block.size(); i++)
		{
			double werh = 0, niz = 0;
			for (int k = 0; k < r; k++)
				werh += (norm(block[k]) * norm(block[(int)((k + i) % r)]));

			for (int k = 0; k < r; k++)
				niz += (norm(block[k]) * norm(block[k]));

			double q = (double)(werh / niz);

			out << (double)q << " " << (int)i << endl;		//Вот тут лучше сделать какое нибудь нормирование q (ну чтобы оно не становилось отрицательным)
		}

		
	}

	char norm(char subj)
	{
		return (char)(subj * 2 - 1);
	}
};