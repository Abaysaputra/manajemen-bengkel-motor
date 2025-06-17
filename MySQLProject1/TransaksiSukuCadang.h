#pragma once
#include <iostream>
#include <string>

class TransaksiSukuCadang {
public:
    int id;
    int id_suku_cadang;
    std::string tanggal;
    int jumlah;
    std::string jenis; // "Pembelian" atau "Penjualan"

    // Constructor
    TransaksiSukuCadang(int id_suku_cadang = 0) : id_suku_cadang(id_suku_cadang), jumlah(0), jenis("Penjualan") {
        std::cout << "🛠️  Konstruktor TransaksiSukuCadang dipanggil.\n";
    }

    // Destructor
    ~TransaksiSukuCadang() {
        std::cout << "🧹 Destruktor TransaksiSukuCadang dipanggil.\n";
    }

    void inputData() {
        std::cin.ignore();
        std::cout << "Tanggal (YYYY-MM-DD): ";
        std::getline(std::cin, tanggal);

        std::cout << "Jumlah: ";
        std::cin >> jumlah;
        std::cin.ignore();

        std::cout << "Jenis Transaksi [Pembelian/Penjualan]: ";
        std::getline(std::cin, jenis);
    }
};
