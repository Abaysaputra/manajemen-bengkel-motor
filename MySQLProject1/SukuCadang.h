#pragma once
#include <string>

using namespace std;

class SukuCadang {
public:
    int id;
    string nama;
	string jenis;
    int stok;
    double harga;

    void inputData();
    void tampilkanData();
};
