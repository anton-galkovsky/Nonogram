#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <ctime>
using namespace std;
struct Cell {
	int x, y;
};
struct Row {
	long long spot, info;
};
struct Hypothesis {
	Cell cell;
	Row oldRows[64];
};
void printBytes(long long x, int length) {
	for (int i = length; i > 0; i--) {
		cout << ((x >> (i - 1) & 1) == 1 ? "#" : " ");
		x -= ((x >> (i - 1)) << (i - 1));
	}
}
Row replenish(int a[], int length, Row row, int size) {
	Row r { row.spot, row.info };
	long long one = 1;
	long long t = (one << size) - 1;
	long long p = 0;
	for (long long l = (one << length) - 1; l <= ((one << (length)) - 1) << a[0]; l = (l + (l & -l)) | ((l ^ l + (l & -l)) >> 2) / (l & -l)) {
		long long e = 0;
		int i = 1;
		for (int j = length + a[0]; j >= 0; j--) {
			if (((l >> j) & 1) == 0) 
				e <<= 1;
			else {
				e = (e << a[i]) + (one << a[i]) - 1;
				if (i < length)
					e <<= 1;
				i++;
			}
		}
		if (((e & r.info) ^ (r.spot & r.info)) == 0) {
			t &= e;
			p |= e;
		}
	}
	r.spot = (r.spot | t) & p;
	r.info = (r.info | t) | ((~p) & ((one << size) - 1));
	return r;
}
Row replace(Row r, int x, int i, int size) {
	r.spot = ((((r.spot >> (size - x)) << 1) + i) << (size - x - 1)) + (r.spot & (((long long)1 << (size - x - 1)) - 1));
	r.info = ((((r.info >> (size - x)) << 1) + 1) << (size - x - 1)) + (r.info & (((long long)1 << (size - x - 1)) - 1));
	return r;
}
bool isFinished(Row r[64], int width, int height) {
	for (int i = 0; i < height; i++)
	if (r[i].info != (((long long)1 << width) - 1))
		return false;
	return true;
}
Cell getNextCell(Row r[64], int width, int height) {
	for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++)
	if (((r[i].info >> j) & 1) == 0) {
		Cell c { width - j - 1, i };
		return c;
	}
	Cell c { -1, -1 };
	return c;
}
void copyArray(Row row[], Row copy[], int size) {
	for (int i = 0; i < size; i ++) {
		copy[i].spot = row[i].spot;
		copy[i].info = row[i].info;
	}
}
int main() {
	int time = clock();
	setlocale(LC_ALL, "rus");
	long long hypothesis = 0;
	int width = 0;
	int height = 0;
	int w[64][32];
	int h[64][32];
	int wSize[64];
	int hSize[64];
	bool isReplenished = true;
	bool hasError = false;
	int level = 0;
	Hypothesis hyp[64];

	ifstream fin("123.txt"); 

	fin >> height; 
	for (int i = 0; i < height; i++) {
		fin >> wSize[i];
		for (int j = 1; j <= wSize[i]; j++)
			fin >> w[i][j];
	}
	fin >> width;
	for (int i = 0; i < width; i++) {
		fin >> hSize[i];
		for (int j = 1; j <= hSize[i]; j++)
			fin >> h[i][j];
	}
	fin.close();

	for (int i = 0; i < height; i++) {
		w[i][0] = width;
		for (int j = 1; j <= wSize[i]; j++) {
			w[i][0] -= w[i][j] + 1;
		}
		w[i][0]++;
	}
	for (int i = 0; i < width; i++) {
		h[i][0] = height;
		for (int j = 1; j <= hSize[i]; j++) {
			h[i][0] -= h[i][j] + 1;
		}
		h[i][0]++;
	}
	
	long long wChanges = ((long long) 1 << height) - 1;
	long long hChanges = ((long long) 1 << width) - 1;
	Row row[64];
	for (int i = 0; i < height; i++) {
		row[i].info = 0;
		row[i].spot = 0;
	}

	while (isReplenished) {
		isReplenished = false;
		for (int i = 0; i < height; i++) {
			if ((((wChanges >> (height - i - 1)) & 1) == 1)) {
				Row r = replenish(w[i], wSize[i], row[i], width);
				if (r.spot == 0 && r.info == ((long long)1 << width) - 1) {
					hasError = true;
					break;
				}
				for (int k = 0; (r.info >> k) != (row[i].info >> k); k++)
					if (((r.info >> k) & 1) != ((row[i].info >> k) & 1))
						hChanges |= (long long)1 << k;
				if (r.info != row[i].info) {
					isReplenished = true;
					row[i] = r;
				}
			}
		}
		wChanges = 0;
		if (!hasError) {
			for (int i = 0; i < width; i++) {
				if ((((hChanges >> i) & 1) == 1)) {
					Row rowi;
					rowi.spot = (long long)0;
					rowi.info = (long long)0;
					for (int j = 0; j < height; j++) {
						rowi.spot <<= 1;
						rowi.spot += (row[j].spot >> i) & 1;
						rowi.info <<= 1;
						rowi.info += (row[j].info >> i) & 1;
					}
					Row r = replenish(h[width - i - 1], hSize[width - i - 1], rowi, height);
					if (r.spot == 0 && r.info == ((long long)1 << height) - 1) {
						hasError = true;
						break;
					}
					for (int k = 0; (r.info >> k) != (rowi.info >> k); k++)
					if (((r.info >> k) & 1) != ((rowi.info >> k) & 1))
						wChanges |= (long long)1 << k;
					if (r.info != rowi.info) {
						isReplenished = true;
						for (int j = height - 1; j >= 0; j--) {
							row[j].spot = ((((row[j].spot >> (i + 1)) << 1) + ((r.spot >> (height - j - 1)) & 1)) << i) + (row[j].spot & (((long long)1 << i) - 1));
							row[j].info = ((((row[j].info >> (i + 1)) << 1) + ((r.info >> (height - j - 1)) & 1)) << i) + (row[j].info & (((long long)1 << i) - 1));
						}
					}
				}
			}
		}
		hChanges = 0;
		if (hasError) {
			if (level == 0)
				break;
			hasError = false;
			while ((hypothesis & 1) == 0) {
				hypothesis >>= 1;
				level--;
			}
			hypothesis--;
			copyArray(hyp[level - 1].oldRows, row, height);
			row[hyp[level - 1].cell.y] = replace(row[hyp[level - 1].cell.y], hyp[level - 1].cell.x, 0, width);
			wChanges |= (long long)1 << (height - hyp[level - 1].cell.y - 1);
			hChanges |= (long long)1 << (width - hyp[level - 1].cell.x - 1);
			if (hypothesis == 0)
				level = 0;
			isReplenished = true;
		}
		else if (!isReplenished && !isFinished(row, width, height)) {
			hypothesis = (hypothesis << 1) + 1;
			Cell cell = getNextCell(row, width, height);
			level++;
			hyp[level - 1].cell = cell;
			copyArray(row, hyp[level - 1].oldRows, height);
			row[cell.y] = replace(row[cell.y], cell.x, 1, width);
			isReplenished = true;
			wChanges |= (long long)1 << (height - cell.y - 1);
			hChanges |= (long long)1 << (width - cell.x - 1);
		}
	}
	int t = clock() - time;
	cout << "program duration = " << (t / 1000 / 60) << " m " << (t / 1000 % 60) << " s " << (t % 1000) << " ms" << endl;
	if (!hasError) {
		cout << endl;
		for (int i = 0; i < height; i++) {
			printBytes(row[i].spot, width);
			cout << endl;
		}
	} else
		cout << "Has error";
	cout << endl;
	return EXIT_SUCCESS;
}