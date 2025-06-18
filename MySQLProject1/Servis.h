#pragma once
#pragma once
#include <string>
using namespace std;
class Servis {
public:
    int id;
    int id_kendaraan;
    int id_teknisi;
    string keluhan;
    string tanggal;
    string status;

    void inputData();
    void tampilkanData();
};
