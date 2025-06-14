#pragma once
#include <string>

class Kendaraan {
public:
    int id;
    int id_pelanggan;
    std::string plat_nomor;
    std::string merk;
    int tahun;

    void inputData();
    void tampilkanData();
};
