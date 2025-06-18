#pragma once
#include <string>

using namespace std;

class PembayaranService {
public:
    int id_pembayaran;
    int id_servis;
    string tanggal_pembayaran;
    double jumlah;
    string metode;
    string status;

    void inputData();
    void tampilkanData();
};
