#pragma once
#include <string>

class SukuCadang {
public:
    int id;
    std::string nama;
	std::string jenis;
    int stok;
    double harga;

    void inputData();
    void tampilkanData();
};
