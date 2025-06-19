#pragma once
#include <string>

class Transaksi {
protected:
    int id;
    std::string tanggal;
    double total_nilai;
    std::string deskripsi;

public:
    Transaksi(int id_val, const std::string& tgl_val, double nilai_val, const std::string& desk_val)
        : id(id_val), tanggal(tgl_val), total_nilai(nilai_val), deskripsi(desk_val) {
    }

    virtual ~Transaksi() = default;

    int getID() const { return id; }
    std::string getTanggal() const { return tanggal; }
    double getTotalNilai() const { return total_nilai; }
    std::string getDeskripsi() const { return deskripsi; }

    virtual std::string getTipeTransaksi() const = 0;
}; // <-- Pastikan tidak ada spasi sebelum #endif jika Anda menggunakan include guard tradisional