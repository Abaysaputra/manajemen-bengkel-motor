#include "Pelanggan.h"
#include <iostream>

void Pelanggan::inputData() {
    std::cout << "Nama Pelanggan: ";
    std::getline(std::cin >> std::ws, nama);
    while (nama.empty()) {
        std::cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, nama);
    }

    std::cout << "No HP: ";
    std::getline(std::cin >> std::ws, no_hp);
    while (no_hp.empty()) {
        std::cout << "❌ No HP tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, no_hp);
    }

    std::cout << "Alamat: ";
    std::getline(std::cin >> std::ws, alamat);
    while (alamat.empty()) {
        std::cout << "❌ Alamat tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, alamat);
    }
}

void Pelanggan::tampilkanData() {
    std::cout << "Nama: " << nama << "\nNo HP: " << no_hp << "\nAlamat: " << alamat << "\n";
}
