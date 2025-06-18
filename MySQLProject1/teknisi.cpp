#include "Teknisi.h"
#include <iostream>

using namespace std;

void Teknisi::inputData() {
    cout << "Nama Teknisi: ";
    getline(cin >> ws, nama);
    while (nama.empty()) {
        cout << "❌ Nama tidak boleh kosong. Masukkan ulang: ";
        getline(cin >> ws, nama);
    }

    cout << "Keahlian: ";
    getline(cin, keahlian);
    while (keahlian.empty()) {
        cout << "❌ Keahlian tidak boleh kosong. Masukkan ulang: ";
        getline(cin, keahlian);
    }
}

void Teknisi::tampilkanData() {
    cout << "Nama: " << nama << "\nKeahlian: " << keahlian << "\n";
}
