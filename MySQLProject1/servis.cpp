#include "Servis.h"
#include <iostream>

void Servis::inputData() {
    std::cout << "ID Kendaraan: ";
    std::cin >> id_kendaraan;
    std::cout << "ID Teknisi: ";
    std::cin >> id_teknisi;
    std::cin.ignore();

    std::cout << "Keluhan: ";
    std::getline(std::cin, keluhan);
    while (keluhan.empty()) {
        std::cout << "❌ Keluhan tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin, keluhan);
    }

    std::cout << "Tanggal (YYYY-MM-DD): ";
    std::getline(std::cin, tanggal);
    while (tanggal.empty()) {
        std::cout << "❌ Tanggal tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin, tanggal);
    }

    std::cout << "Status: ";
    std::getline(std::cin, status);
    while (status.empty()) {
        std::cout << "❌ Status tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin, status);
    }
}

void Servis::tampilkanData() {
    std::cout << "Tanggal: " << tanggal << "\nKeluhan: " << keluhan << "\nStatus: " << status << "\n";
}
