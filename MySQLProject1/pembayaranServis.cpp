#include "PembayaranServis.h"

// Implementasi Constructor
PembayaranServis::PembayaranServis(int id_pembayaran, int id_servis_val, const std::string& tgl_val, double jumlah_val, const std::string& metode_val, const std::string& desk_val)
// Panggil constructor base class (Transaksi) untuk menginisialisasi anggota yang diwariskan
    : Transaksi(id_pembayaran, tgl_val, jumlah_val, desk_val),
    id_servis(id_servis_val),
    metode_pembayaran(metode_val) {
}

// Implementasi fungsi yang di-override
std::string PembayaranServis::getTipeTransaksi() const {
    return "Pembayaran Servis";
}