#pragma once
#include <string>

class Servis {
public:
    int id;
    int id_kendaraan;
    int id_teknisi;
    std::string keluhan;
    std::string tanggal; // Akan di-generate otomatis
    std::string status;
    std::string status_pembayaran;
    double ongkos; // ATRIBUT BARU

    void inputData(); // Hanya untuk input awal (kendaraan, teknisi, keluhan)
    void tampilkanData();
};