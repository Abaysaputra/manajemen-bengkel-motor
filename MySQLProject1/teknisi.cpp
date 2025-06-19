#include "Teknisi.h"
#include <iostream>

void Teknisi::inputData() {
    // Panggil inputData() dari base class (User) untuk mengisi nama
    User::inputData();

    // Minta input untuk data spesifik Teknisi
    std::cout << "Keahlian: ";
    std::getline(std::cin, keahlian);
}

void Teknisi::tampilkanData() {
    // Panggil tampilkanData() dari base class (User) untuk menampilkan id dan nama
    User::tampilkanData();

    // Tampilkan data spesifik Teknisi
    std::cout << ", Keahlian: " << keahlian << std::endl;
}