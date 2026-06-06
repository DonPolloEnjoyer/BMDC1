
// BMDC - Bit Mask Data Compressor
// BMA  - Bit Mask Archive
// BMDC-std-1.0

#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<locale.h>

typedef unsigned char byte;

void mass_print(byte* m, size_t len) {
	for (size_t i = 0; i < len; i++) {
		printf("%d ", m[i]);
	}
	puts("");
}

long  file_size(FILE* file) {
	fseek(file, 0, SEEK_END);
	long  s = ftell(file);
	fseek(file, 0, SEEK_SET);
	return s;
}

long long file_size64(FILE* file) {
	_fseeki64(file, 0, SEEK_END);
	long long s = _ftelli64(file);
	_fseeki64(file, 0, SEEK_SET);
	return s;
}

byte* to_bin8(byte num) {	
	//num %= 256;
	static byte bin[8];
	for(byte i = 0; i < 8; i++) {
		bin[7 - i] = (byte)(num >> i) & 0x00000001;
		//num >>= 2;
	}
	mass_print(bin, 8);
	return bin;
}

byte* to_bin4(byte num) {
	static byte bin4[4];
	for (byte i = 0; i < 4; i++) {
		bin4[3 - i] = (byte)(num >> i) & 0x01;
	}
	mass_print(bin4, 4);
	return bin4;
}

unsigned int powui(unsigned int x,unsigned int pow) {
	unsigned int p = 1;
	for  (unsigned int i = 1; i < pow; i++) {
		p *= x;
	}
	return p;
}

unsigned long long powull(byte x, byte pow) {
	unsigned long long p = 1;
	for (unsigned short i = 0; i < pow; i++) {
		p *= x;
	}
	return p;
}

byte powb(byte x, byte pow) {
	byte p = 1;
	for (unsigned short i = 0; i < pow; i++) {
		p *= x;
	}
	return p;
}

byte obshiy(byte c, int* count, byte* pred) {
	byte d = 0;
	for (char i = 0; i < 8; i++) {
		for (char j = 0; j < 8; j++) {
			byte c2 = c;
			c2 = ~c ^ ~(1 << i);
			if (i != j) { c2 = ~c2 ^ ~(1 << j); }
			if (c2 == *pred) {
				printf("%u %u OBSHII\n", i, j);
				(*count)++; d = 1; return 1;
			}
		}
		if (d) { return 1;}
	} return 0;
}

byte blocki_e2(byte c, int* blocki, byte* pred) {
	byte d = 0;
	for (char i = 0; i < 4; i++) {
		for (char j = 4; j < 8; j++) {
			byte c3 = c;
			c3 = ~c ^ ~(1 << i);
			c3 = ~c3 ^ ~(1 << j);
			if (c3 == *pred) {
				printf("%u %u BLOKI\n", i, j);
				(*blocki)++; d = 1; return 1;
			}
		}
		if (d) { return 1; }
	}
	return 0;
}

long long mass_sum(int* mass, size_t len) {
	long long sum = 0;
	for (size_t i = 0; i < len; i++) {
		sum += mass[i];
	}
	return sum;
}

byte * povtor_bytes16(FILE* file) {
	char b_sign;
	byte c;
	int col_bytes[256];
	for (int i = 0; i < 256; i++) {
		col_bytes[i] = 0;
	}
	if (file) {
		while ((b_sign = getc(file)) != EOF) {
			c = b_sign;
			printf("%d ", c); to_bin8(c);
			col_bytes[c] += 1;
		}
	}
	static byte bytes_p[16];
	int bytes_m[16];
	for (size_t j = 0; j < 16; j++) {
		int max = 0;
		byte mbyte = 0;
		for (uint16_t i = 0; i < 256; i++) {
			if (col_bytes[i] > max) {
				max = col_bytes[i];
				mbyte = i;
			}
		}
		bytes_m[j] = max;
		bytes_p[j] = mbyte;
		col_bytes[mbyte] = 0;
	}
	for (int i = 0; i < 16; i++) {
		printf("%d %d \n", bytes_p[i], bytes_m[i]);
	}
	printf("%d sum\n", mass_sum(bytes_m, 16));
	return bytes_p;
}

byte sgn_b(byte x) {
	if (x == 0) { return 0; }
	else { return 1; }
}






int main() {

	char* locale = setlocale(LC_ALL, "");

	/*printf("file name: ");
	char* fname = malloc(200);
	scanf("%s", fname);
	FILE* file = fopen(fname, "rb");
	free(fname);*/
	
	FILE* file = fopen("..\\files\\STD.bin", "rb");
	//FILE* code = fopen("Code1.bin", "wb");
	
	_fseeki64(file, 0, SEEK_END);
	const int64_t fsize = _ftelli64(file);
	_fseeki64(file, 0, SEEK_SET);
	printf(" %d bytes\n\n", fsize);
	if (fsize < 0) { // 65535) {
		return 2; // ERROR // too long file
	}
	byte* raw_i = malloc(fsize);

	//srand(time(NULL));
	/*FILE* Trnd = fopen("Trnd.bin", "wb");
	if (Trnd) {
		for (int i = 0; i < 100; i++) {
			putc((byte) rand() % 256, Trnd);
		}
	}*/

	char d_one = 0;
	int count = 0;
	int blocki = 0;
	
	byte pred = 0;
	int comp = 0; int ncom = 0;
	byte* nbl_rawi = malloc(fsize * 4);
	int col_nibl[4];
	for (int i; i < 4; i++) {
		col_nibl[i] = 0;
	}
	//char b_sign;
	byte c;
	size_t barl = fsize / 100;
	if (barl == 0) { barl = 1; }
	printf(" Reading... \n\n");

	for (int i = 0; i < 100; i++) {printf(":");}
	printf("\r");

	for (uint64_t i = 0; i < fsize / 1; i++) {
		c = getc(file);
		//putc(c, code);
		byte nbl = 0; // d_bit (2 bits)
		for (char n = 0; n < 4; n++) {
			for (char b = 0; b < 2; b++) {
				byte dig = 1 & (c >> (7 - (1-b + 2*n)));
				nbl += dig * (1 + b); //powb(2, b); 
			}
			//printf("%d ", nbl);
			nbl_rawi[4 * i + n] = nbl;
			col_nibl[nbl] += 1;
			nbl = 0;
		}
		raw_i[_ftelli64(file) - 1] = c;
		//printf(" %d  ", c); to_bin8(c);
		if (!(i % barl)) { printf("|"); }
	}

	printf("\n\n Sorting... \n\n");

	byte nibl_p[4]; // pov dbits
	int nibl_m[4];  // kolichestvo
	for (byte j = 0; j < 4; j++) {
		int max = 0;
		byte mnibl = 0;
		for (byte i = 0; i < 4; i++) {
			if (col_nibl[i] > max) {
				max = col_nibl[i];
				mnibl = i;
			}
			printf("|");
		}
		nibl_m[j] = max;
		nibl_p[j] = mnibl;	
		col_nibl[mnibl] = 0;
	}
	puts("\n");
	for (byte i = 0; i < 4; i++) {
		printf("%d %d nbl times\n", nibl_p[i], nibl_m[i]);
	}
	printf("%d sum\n", mass_sum(nibl_m, 4));

	printf("\n\n Counting... \n\n");
	
	
	
	byte predc;
	size_t countPov = 0;
	size_t countNPov = 0;
	size_t countRaz = 0;
	barl = 4 * fsize / 100; if (barl == 0) { barl = 1; }
	for (int i = 0; i < 100; i++) { printf(":"); }
	printf("\r");
	for (uint64_t i = 0; i < 4 * fsize; i++) {
		if (i % 2 == 0) {
			byte nbl0_pov = nbl_rawi[i] == nibl_p[0] || nbl_rawi[i] == nibl_p[1];
			byte nbl1_pov = nbl_rawi[i + 1] == nibl_p[0] || nbl_rawi[i + 1] == nibl_p[1];
			if (nbl0_pov && nbl1_pov)
				countPov++;
			if ((!nbl0_pov) && (!nbl1_pov))
				countNPov++;
			if ((nbl0_pov && !nbl1_pov) || (nbl1_pov && !nbl0_pov))
				countRaz++;
			if (!(i % barl)) { printf("|"); }
		}
	}
	puts("\n");
	printf("%d pov \n", countPov);
	printf("%d Npov \n", countNPov);
	printf("%d Raz \n", countRaz);
	





	printf("Cycles: ");
	int cycles; scanf("%d", &cycles);

	for (int i = 0; i < 100; i++) { printf(":"); }
	printf("\r");

	byte* raw_o = malloc(fsize);
	for (uint64_t i = 0; i < fsize; i++) {
		raw_o[i] = 0;
	}
	byte bits_last = 8;
	byte CompD = 1;
	uint64_t bit_cur_i = 0;
	uint64_t bit_cur_o = 0;
	byte nbl0, nbl0_pov, nbl1, nbl1_pov;
	for (int raz = 0; raz < cycles; raz++) {
		for (uint64_t i = 0; i < (8 * fsize); i++) {
			uint64_t byte_cur_i = i / 8 ;
			byte bib_i = i % 8 ;

			nbl0 = nbl_rawi[i];
			nbl1 = nbl_rawi[i + 1];
			nbl0_pov = (nbl0 == nibl_p[0]) || (nbl0 == nibl_p[1]);
			nbl1_pov = (nbl1 == nibl_p[0]) || (nbl1 == nibl_p[1]);
			if (!CompD) {
				for (byte nc = 0; nc < 4; nc++) {
					byte bit = (nbl_rawi[i + (nc / 2)] >> nc) & 1;
					raw_o[i / 4] = raw_o[i / 4] | (bit << i);
					i++;
				}
			}
			else if (nbl0_pov && nbl1_pov && CompD) {
				raw_o[i / 4] = (raw_o[i / 4] >> bit_cur_o) | 1;
				bit_cur_o++; bit_cur_o %= 8;
				CompD = 0;
			}

		}
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	printf("end -- press any key");
	fclose(file);
	//fclose(code);
	
	char end[10];
	scanf("%10[^\n]", &end);
	//scanf("%10s", &end);
	printf("end");
	return 0;
}

// D:\Prog1\CCpp1\Compr1

