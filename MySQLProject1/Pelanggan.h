#pragma once
#include <string>

class Pelanggan {
public:
    int id;
    std::string nama;
    std::string no_hp;
    std::string alamat;

    void inputData();
    void tampilkanData();
};
