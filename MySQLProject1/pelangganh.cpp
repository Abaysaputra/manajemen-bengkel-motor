#include "Pelanggan.h"
#include <iostream>

void Pelanggan::inputData() {
    // Panggil inputData() dari base class (User) untuk mengisi nama
    User::inputData();

    // Minta input untuk data spesifik Pelanggan
    std::cout << "No HP: ";
    std::getline(std::cin, no_hp);
    std::cout << "Alamat: ";
    std::getline(std::cin, alamat);
}

void Pelanggan::tampilkanData() {
    // Panggil tampilkanData() dari base class (User) untuk menampilkan id dan nama
    User::tampilkanData();

    // Tampilkan data spesifik Pelanggan
    std::cout << ", No HP: " << no_hp << ", Alamat: " << alamat << std::endl;
}