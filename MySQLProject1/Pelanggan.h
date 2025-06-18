#pragma once
#include <string>

using namespace std;

class Pelanggan {
public:
    int id;
    string nama;
    string no_hp;
    string alamat;

    void inputData();
    void tampilkanData();
};
