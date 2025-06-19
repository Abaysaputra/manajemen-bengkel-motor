#include "Servis.h"
#include <iostream>

// Fungsi ini tidak lagi dipanggil oleh menu 'Tambah Servis' yang baru,
// tapi bisa disimpan jika ada keperluan lain di masa depan.
void Servis::inputData() {
    // Implementasi inputData() yang lama tidak lagi sesuai dengan alur program.
    // Jika fungsi ini memang harus ada, isinya perlu disesuaikan.
    // Untuk saat ini, kita bisa kosongkan atau beri pesan.
    std::cout << "Fungsi Servis::inputData() tidak lagi digunakan dalam alur utama." << std::endl;
}

// Fungsi ini bisa kita buat lebih informatif jika ingin digunakan untuk debugging
// atau menampilkan detail objek servis.
void Servis::tampilkanData() {
    std::cout << "--- Detail Objek Servis ---" << std::endl;
    std::cout << "ID Servis      : " << id << std::endl;
    std::cout << "ID Kendaraan   : " << id_kendaraan << std::endl;
    std::cout << "ID Teknisi     : " << id_teknisi << std::endl;
    std::cout << "Tanggal        : " << tanggal << std::endl;
    std::cout << "Keluhan        : " << keluhan << std::endl;
    std::cout << "Status         : " << status << std::endl;
    std::cout << "Status Bayar   : " << status_pembayaran << std::endl;
    std::cout << "Ongkos Jasa    : " << ongkos << std::endl;
    std::cout << "--------------------------" << std::endl;
}