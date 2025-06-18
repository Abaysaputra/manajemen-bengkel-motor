#include "SukuCadang.h"
#include <iostream>
#include <limits>  // untuk numeric_limits

using namespace std;

void SukuCadang::inputData() {
    cout << "Nama Suku Cadang: ";
    getline(cin >> ws, nama);
    while (nama.empty()) {
        cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, nama);
    }

    cout << "Masukkan Jenis Suku Cadang: ";
    getline(cin >> ws, jenis);
    while (jenis.empty()) {
        cout << "❌ Jenis tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, jenis);
    }

    cout << "Jumlah Stok: ";
    while (!(cin >> stok) || stok < 0) {
        cout << "❌ Input tidak valid atau stok negatif. Masukkan ulang: ";
        cin.clear();  // reset error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // buang input salah
    }

    cout << "Masukkan Harga: ";
    while (!(cin >> harga) || harga < 0) {
        cout << "❌ Input tidak valid atau harga negatif. Masukkan ulang: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cin.ignore(); // untuk buang newline jika ada input lanjutan
}

void SukuCadang::tampilkanData() {
    cout << "Nama: " << nama
        << "\nJenis: " << jenis
        << "\nStok: " << stok
        << "\nHarga: Rp" << harga << "\n";
}
