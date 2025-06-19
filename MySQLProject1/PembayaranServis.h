#pragma once
#include "Transaksi.h" // Sertakan base class
#include <string>

// PembayaranServis sekarang adalah sebuah Transaksi
class PembayaranServis : public Transaksi {
private:
    // Atribut spesifik untuk PembayaranServis
    int id_servis;
    std::string metode_pembayaran;

public:
    // Constructor memanggil constructor base class
    PembayaranServis(int id_pembayaran, int id_servis_val, const std::string& tgl_val, double jumlah_val, const std::string& metode_val, const std::string& desk_val);

    // Override pure virtual function dari Transaksi
    std::string getTipeTransaksi() const override;

    // Getter spesifik
    int getIDServis() const { return id_servis; }
    std::string getMetodePembayaran() const { return metode_pembayaran; }
};