#include "Kendaraan.h"
#include <iostream>

using namespace std;


void Kendaraan::inputData() {
    cout << "\nMasukkan ID Pelanggan (lihat daftar di atas): ";
    cin >> id_pelanggan;

    cin.ignore();
    cout << "Plat Nomor: ";
    getline(cin, plat_nomor);
    while (plat_nomor.empty()) {
        cout << "❌ Plat nomor tidak boleh kosong. Masukkan ulang: ";
        getline(cin, plat_nomor);
    }

    cout << "Merk: ";
    getline(cin, merk);
    while (merk.empty()) {
        cout << "❌ Merk tidak boleh kosong. Masukkan ulang: ";
        getline(cin, merk);
    }

    cout << "Tahun: ";
    cin >> tahun;
    while (tahun < 1900 || tahun > 2100) {
        cout << "❌ Tahun tidak valid. Masukkan tahun antara 1900-2100: ";
        cin >> tahun;
    }
}

void Kendaraan::tampilkanData() {
    cout << "Plat: " << plat_nomor << "\nMerk: " << merk << "\nTahun: " << tahun << "\n";
}
