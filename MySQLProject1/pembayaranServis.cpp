#include "PembayaranServis.h"
#include <iostream>

void PembayaranService::inputData() {

    std::cout << "Tanggal Pembayaran (YYYY-MM-DD): ";
    std::getline(std::cin >> std::ws, tanggal_pembayaran);
    while (tanggal_pembayaran.empty()) {
        std::cout << "? Tanggal tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, tanggal_pembayaran);
    }

    std::cout << "Jumlah Pembayaran: ";
    std::cin >> jumlah;
    while (jumlah <= 0) {
        std::cout << "? Jumlah tidak valid. Masukkan ulang: ";
        std::cin >> jumlah;
    }

    std::cout << "Metode Pembayaran (Cash/Transfer/etc): ";
    std::getline(std::cin >> std::ws, metode);
    while (metode.empty()) {
        std::cout << "? Metode tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, metode);
    }

    std::cout << "Status (Lunas/Belum Lunas): ";
    std::getline(std::cin >> std::ws, status);
    while (status.empty()) {
        std::cout << "? Status tidak boleh kosong. Masukkan ulang: ";
        std::getline(std::cin >> std::ws, status);
    }
}

void PembayaranService::tampilkanData() {
    std::cout << "ID Servis         : " << id_servis << "\n";
    std::cout << "Tanggal Pembayaran: " << tanggal_pembayaran << "\n";
    std::cout << "Jumlah            : Rp" << jumlah << "\n";
    std::cout << "Metode            : " << metode << "\n";
    std::cout << "Status            : " << status << "\n";
}
