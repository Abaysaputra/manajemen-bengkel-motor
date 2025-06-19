#include "TransaksiSukuCadang.h"

// Implementasi Constructor
TransaksiSukuCadang::TransaksiSukuCadang(int id_trx, int id_sc_val, const std::string& tgl_val, int jumlah_val, double total_harga_val, const std::string& jenis_val, const std::string& desk_val)
// Jika jenisnya Pembelian, nilai transaksi kita buat negatif
    : Transaksi(id_trx, tgl_val, (jenis_val == "Pembelian" ? -total_harga_val : total_harga_val), desk_val),
    id_suku_cadang(id_sc_val),
    jumlah_barang(jumlah_val),
    jenis_transaksi(jenis_val) {
}

// Implementasi fungsi yang di-override
std::string TransaksiSukuCadang::getTipeTransaksi() const {
    // Kita bisa buat lebih spesifik
    return "Transaksi Suku Cadang (" + jenis_transaksi + ")";
}