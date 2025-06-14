#include "Teknisi.h"
#include <iostream>

void Teknisi::inputData() {
    std::cout << "Nama Teknisi: ";
    std::getline(std::cin >> std::ws, nama);
    while (nama.empty()) {
        std::cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, nama);
    }

    std::cout << "Keahlian: ";
    std::getline(std::cin, keahlian);
    while (keahlian.empty()) {
        std::cout << "❌ Keahlian tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin, keahlian);
    }
}

void Teknisi::tampilkanData() {
    std::cout << "Nama: " << nama << "\nKeahlian: " << keahlian << "\n";
}
