#pragma once
#pragma once
#include <string>

class Servis {
public:
    int id;
    int id_kendaraan;
    int id_teknisi;
    std::string keluhan;
    std::string tanggal;
    std::string status;

    void inputData();
    void tampilkanData();
};
