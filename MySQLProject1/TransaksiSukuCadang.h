// include/TransaksiSukuCadang.h
#ifndef TRANSAKSI_SUKU_CADANG_H
#define TRANSAKSI_SUKU_CADANG_H

#include <iostream>
#include <string>
#include <limits>
#include <chrono>
#include <ctime> // Untuk time(), localtime_s(), strftime()
#include <cstdio> // Untuk snprintf di non-Windows atau jika ingin lebih cross-platform dari strftime

class TransaksiSukuCadang {
public:
    int id;
    int id_suku_cadang;
    std::string tanggal;
    int jumlah;
    std::string jenis;

    TransaksiSukuCadang(int id_suku_cadang_val = 0)
        : id(0), id_suku_cadang(id_suku_cadang_val), jumlah(0), jenis("Penjualan") {
        std::cout << "🛠️ Konstruktor TransaksiSukuCadang dipanggil.\n";

        // Menginisialisasi tanggal dengan tanggal saat ini (YYYY-MM-DD)
        std::time_t now = std::time(0);
        std::tm ltm_buf; // Buffer untuk localtime_s
        char buffer[80];

#ifdef _WIN32
        // Menggunakan localtime_s untuk Windows
        if (localtime_s(&ltm_buf, &now) == 0) { // localtime_s mengembalikan 0 jika berhasil
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &ltm_buf);
            tanggal = buffer;
        }
        else {
            tanggal = "ERROR"; // Atau tanggal default jika gagal
        }
#else
        // Menggunakan localtime_r (versi thread-safe di POSIX) atau localtime (di non-Windows)
        // Di Linux/macOS, localtime tidak memberikan warning ini, tapi localtime_r lebih disarankan untuk thread-safety
        std::tm* ltm_ptr = std::localtime(&now);
        if (ltm_ptr) { // Periksa apakah tidak null
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm_ptr);
            tanggal = buffer;
        }
        else {
            tanggal = "ERROR";
        }
#endif
    }

    ~TransaksiSukuCadang() {
        std::cout << "🧹 Destruktor TransaksiSukuCadang dipanggil.\n";
    }

    void inputData() {
        std::cout << "Tanggal (YYYY-MM-DD) [Kosongkan untuk tanggal saat ini]: ";
        std::string temp_tanggal;
        std::getline(std::cin, temp_tanggal);
        if (!temp_tanggal.empty()) {
            tanggal = temp_tanggal;
        }

        std::cout << "Jumlah: ";
        std::cin >> jumlah;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Jenis Transaksi [Pembelian/Penjualan]: ";
        std::getline(std::cin, jenis);
    }

};

#endif // TRANSAKSI_SUKU_CADANG_H