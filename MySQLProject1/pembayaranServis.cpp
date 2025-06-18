#include "PembayaranServis.h"
#include <iostream>

using namespace std;

void PembayaranService::inputData() {

    cout << "Tanggal Pembayaran (YYYY-MM-DD): ";
    getline(cin >> ws, tanggal_pembayaran);
    while (tanggal_pembayaran.empty()) {
        cout << "? Tanggal tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, tanggal_pembayaran);
    }

    cout << "Jumlah Pembayaran: ";
    cin >> jumlah;
    while (jumlah <= 0) {
        cout << "? Jumlah tidak valid. Masukkan ulang: ";
        cin >> jumlah;
    }

    cout << "Metode Pembayaran (Cash/Transfer/etc): ";
    getline(cin >> ws, metode);
    while (metode.empty()) {
        cout << "? Metode tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, metode);
    }

    cout << "Status (Lunas/Belum Lunas): ";
    getline(cin >> ws, status);
    while (status.empty()) {
        cout << "? Status tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, status);
    }
}

void PembayaranService::tampilkanData() {
    cout << "ID Servis         : " << id_servis << "\n";
    cout << "Tanggal Pembayaran: " << tanggal_pembayaran << "\n";
    cout << "Jumlah            : Rp" << jumlah << "\n";
    cout << "Metode            : " << metode << "\n";
    cout << "Status            : " << status << "\n";
}
