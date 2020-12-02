#include "Crypto.cpp"
#include "Resampler.cpp"

#define TXT 0
#define BMP 136

int main() {
	setlocale(LC_ALL, "Russian");

	vector<char> key(128);

	for (int i = 0; i < key.size(); i++)
	{
		key[i] = i * 100;
	}

	Resampler file(key.size(), "Resample\\in.txt", "Resample\\out.txt", "Resample\\decrypto.txt", TXT);
	//Resampler file(key.size(), "Resample\\in.bmp", "Resample\\out.bmp", "Resample\\decrypto.bmp", BMP);
	Crypto crypto(&file, key,  16);

	crypto.mod(true);
	crypto.start();
	cout << "Шифрование окончено" << endl;
	crypto.mod(false);
	crypto.start();
	cout << "Дешифрование окончено" << endl;

	return 0;
}