#include "SukuCadang.h"
#include <iostream>
#include <limits>  // untuk std::numeric_limits

void SukuCadang::inputData() {
    std::cout << "Nama Suku Cadang: ";
    std::getline(std::cin >> std::ws, nama);
    while (nama.empty()) {
        std::cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, nama);
    }

    std::cout << "Masukkan Jenis Suku Cadang: ";
    std::getline(std::cin >> std::ws, jenis);
    while (jenis.empty()) {
        std::cout << "❌ Jenis tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, jenis);
    }

    std::cout << "Jumlah Stok: ";
    while (!(std::cin >> stok) || stok < 0) {
        std::cout << "❌ Input tidak valid atau stok negatif. Masukkan ulang: ";
        std::cin.clear();  // reset error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // buang input salah
    }

    std::cout << "Masukkan Harga: ";
    while (!(std::cin >> harga) || harga < 0) {
        std::cout << "❌ Input tidak valid atau harga negatif. Masukkan ulang: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cin.ignore(); // untuk buang newline jika ada input lanjutan
}

void SukuCadang::tampilkanData() {
    std::cout << "Nama: " << nama
        << "\nJenis: " << jenis
        << "\nStok: " << stok
        << "\nHarga: Rp" << harga << "\n";
}
