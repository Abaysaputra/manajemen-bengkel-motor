#include "Servis.h"
#include <iostream>

using namespace std;

void Servis::inputData() {
    cout << "ID Kendaraan: ";
    cin >> id_kendaraan;
    cout << "ID Teknisi: ";
    cin >> id_teknisi;
    cin.ignore();

    cout << "Keluhan: ";
    getline(cin, keluhan);
    while (keluhan.empty()) {
        cout << "❌ Keluhan tidak boleh kosong. Masukkan ulang: ";
        getline(cin, keluhan);
    }

    cout << "Tanggal (YYYY-MM-DD): ";
    getline(cin, tanggal);
    while (tanggal.empty()) {
        cout << "❌ Tanggal tidak boleh kosong. Masukkan ulang: ";
        getline(cin, tanggal);
    }

    cout << "Status (Diproses, Selesai) : ";
    getline(cin, status);
    while (status.empty()) {
        cout << "❌ Status tidak boleh kosong. Masukkan ulang: ";
        getline(cin, status);
    }
}

void Servis::tampilkanData() {
    cout << "Tanggal: " << tanggal << "\nKeluhan: " << keluhan << "\nStatus: " << status << "\n";
}
