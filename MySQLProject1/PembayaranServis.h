#pragma once
#include <string>


class PembayaranService {
public:
    int id_pembayaran;
    int id_servis;
    std::string tanggal_pembayaran;
    double jumlah;
    std::string metode;
    std::string status;

    void inputData();
    void tampilkanData();
};
