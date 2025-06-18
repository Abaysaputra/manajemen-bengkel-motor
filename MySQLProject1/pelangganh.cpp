#include "Pelanggan.h"
#include <iostream>

using namespace std;

void Pelanggan::inputData() {
    cout << "Nama Pelanggan: ";
    getline(cin >> ws, nama);
    while (nama.empty()) {
        cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, nama);
    }

    cout << "No HP: ";
    getline(cin >> ws, no_hp);
    while (no_hp.empty()) {
        cout << "❌ No HP tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, no_hp);
    }

    cout << "Alamat: ";
    getline(cin >> ws, alamat);
    while (alamat.empty()) {
        cout << "❌ Alamat tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, alamat);
    }
}

void Pelanggan::tampilkanData() {
    cout << "Nama: " << nama << "\nNo HP: " << no_hp << "\nAlamat: " << alamat << "\n";
}
