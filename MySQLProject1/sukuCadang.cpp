#include "SukuCadang.h"
#include <iostream>

void SukuCadang::inputData() {
    std::cout << "Nama Suku Cadang: ";
    std::getline(std::cin >> std::ws, nama);
    while (nama.empty()) {
        std::cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, nama);
    }

    std::cout << "Harga: ";
    std::cin >> harga;
    while (harga <= 0) {
        std::cout << "❌ Harga harus lebih dari 0. Masukkan ulang: ";
        std::cin >> harga;
    }
}

void SukuCadang::tampilkanData() {
    std::cout << "Nama: " << nama << "\nHarga: Rp" << harga << "\n";
}
