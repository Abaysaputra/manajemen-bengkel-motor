#include <iostream>
#include <fstream>
#include "Database.h"
#include "Interface.h"
#include "Utils.h"

using namespace std;

// Pastikan semua fungsi menu dideklarasikan di interface.h
// Jika belum, saya akan sertakan juga isi interface.h di bawah

int main() {
    Database db;
    db.connect();

    int pilihan;
    do {
        clearScreen();

        cout << "\n=== MENU UTAMA SISTEM MANAJEMEN BENGKEL ===\n";
        cout << "-------------------------------------------\n";
        cout << "OPERASIONAL:\n";
        cout << "  1. Kelola Pelanggan\n";
        cout << "  2. Kelola Kendaraan\n";
        cout << "  3. Kelola Servis\n";
        cout << "  4. Kelola Teknisi\n";
        cout << "  5. Kelola Suku Cadang\n";
        cout << "  6. Kelola Pembayaran\n";
        cout << "  7. Transaksi Suku Cadang (Jual/Beli Langsung)\n";
        cout << "-------------------------------------------\n";
        cout << "LAPORAN:\n";
        cout << "  8. Laporan Keuangan (Ringkasan)\n";
        cout << "  9. Riwayat Semua Transaksi (Detail)\n"; // <-- MENU BARU DITAMBAHKAN DI SINI
        cout << "  10. Laporan & Ekspor CSV\n";
        cout << "-------------------------------------------\n";
        cout << "  11. Keluar\n"; // <-- Keluar sekarang nomor 11
        cout << "===========================================\n";

        pilihan = getValidatedInt("Pilihan: ");

        clearScreen();

        switch (pilihan) {
        case 1:
            menuPelanggan(db);
            break;
        case 2:
            menuKendaraan(db);
            break;
        case 3:
            menuServis(db);
            break;
        case 4:
            menuTeknisi(db);
            break;
        case 5:
            menuSukuCadang(db);
            break;
        case 6:
            menuPembayaranServis(db);
            break;
        case 7:
            menuTransaksiSukuCadang(db);
            break;
        case 8:
            menuLaporanKeuangan(db);
            break;
        case 9:
            // Panggil fungsi baru yang sudah kita buat di jawaban sebelumnya
            menuRiwayatTransaksiGabungan(db);
            break;
        case 10:
            menuLaporan(db);
            break;
        case 11:
            cout << "👋 Terima kasih telah menggunakan aplikasi!\n";
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
        }

        // Pause hanya jika pilihan bukan keluar
        if (pilihan != 11) {
            pause();
        }

    } while (pilihan != 11);

    return 0;
}