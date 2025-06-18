#pragma once
#include <iostream>
#include <string>
using namespace std;
class TransaksiSukuCadang {
public:
    int id;
    int id_suku_cadang;
    string tanggal;
    int jumlah;
    string jenis; // "Pembelian" atau "Penjualan"

    // Constructor
    TransaksiSukuCadang(int id_suku_cadang = 0) : id_suku_cadang(id_suku_cadang), jumlah(0), jenis("Penjualan") {
        cout << "🛠️  Konstruktor TransaksiSukuCadang dipanggil.\n";
    }

    // Destructor
    ~TransaksiSukuCadang() {
        cout << "🧹 Destruktor TransaksiSukuCadang dipanggil.\n";
    }

    void inputData() {
        cin.ignore();
        cout << "Tanggal (YYYY-MM-DD): ";
        getline(cin, tanggal);

        cout << "Jumlah: ";
        cin >> jumlah;
        cin.ignore();

        cout << "Jenis Transaksi [Pembelian/Penjualan]: ";
        getline(cin, jenis);
    }
};
