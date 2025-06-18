#pragma once
#include <string>
using namespace std;
class Kendaraan {
public:
    int id;
    int id_pelanggan;
    string plat_nomor;
    string merk;
    int tahun;

    void inputData();
    void tampilkanData();
};
