#include "Kendaraan.h"
#include <iostream>



void Kendaraan::inputData() {
    std::cout << "ID Pelanggan: ";
    std::cin >> id_pelanggan;

    std::cin.ignore();
    std::cout << "Plat Nomor: ";
    std::getline(std::cin, plat_nomor);
    while (plat_nomor.empty()) {
        std::cout << "❌ Plat nomor tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin, plat_nomor);
    }

    std::cout << "Merk: ";
    std::getline(std::cin, merk);
    while (merk.empty()) {
        std::cout << "❌ Merk tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin, merk);
    }

    std::cout << "Tahun: ";
    std::cin >> tahun;
    while (tahun < 1900 || tahun > 2100) {
        std::cout << "❌ Tahun tidak valid. Masukkan tahun antara 1900-2100: ";
        std::cin >> tahun;
    }
}

void Kendaraan::tampilkanData() {
    std::cout << "Plat: " << plat_nomor << "\nMerk: " << merk << "\nTahun: " << tahun << "\n";
}
