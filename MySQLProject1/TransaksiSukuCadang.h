#pragma once
#include "Transaksi.h" // Sertakan base class
#include <string>

class TransaksiSukuCadang : public Transaksi {
private:
    // Atribut spesifik
    int id_suku_cadang;
    int jumlah_barang;
    std::string jenis_transaksi; // "Pembelian" atau "Penjualan"

public:
    // Constructor memanggil constructor base class
    TransaksiSukuCadang(int id_trx, int id_sc_val, const std::string& tgl_val, int jumlah_val, double total_harga_val, const std::string& jenis_val, const std::string& desk_val);

    // Override pure virtual function dari Transaksi
    std::string getTipeTransaksi() const override;

    // Getter spesifik
    int getIDSukuCadang() const { return id_suku_cadang; }
    int getJumlahBarang() const { return jumlah_barang; }
    std::string getJenisTransaksi() const { return jenis_transaksi; }
};