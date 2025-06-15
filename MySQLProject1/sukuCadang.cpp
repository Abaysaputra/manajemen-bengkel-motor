#include "SukuCadang.h"
#include <iostream>

void SukuCadang::inputData() {
    std::cout << "Nama Suku Cadang: ";
    std::getline(std::cin >> std::ws, nama);
    while (nama.empty()) {
        std::cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, nama);
    }

    std::cout << "Masukkan Jenis Suku Cadang (Mesin, Rem, Kelistrikan, Suspensi, Interior) ";
    std::getline(std::cin >> std::ws, jenis);
    while (jenis.empty()) {
        std::cout << "❌ Jenis tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, jenis);
    }

	std::cout << "Jumlah Stok: ";
    std::cin >> stok;
    while (stok < 0) {
        std::cout << "❌ Stok tidak boleh negatif. Masukkan ulang: ";
        std::cin >> stok;
    }
    std::cout << "Harga: Rp";
    std::cin >> harga;
    while (harga < 0) {
        std::cout << "❌ Harga tidak boleh negatif. Masukkan ulang: Rp";
        std::cin >> harga;
	}
}

void SukuCadang::tampilkanData() {
    std::cout << "Nama: " << nama << "\nHarga: Rp" << harga << "\n";
}
