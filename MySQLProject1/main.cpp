#include <iostream>
#include <fstream>
#include "Database.h"
#include "Interface.h"
#include "Utils.h"

 
int main() {
    Database db;
    db.connect();

    int pilihan;
    do {
        clearScreen(); // Bersihkan layar sebelum tampilkan menu

        std::cout << "\n=== MENU UTAMA ===\n";
        std::cout << "1. Kelola Pelanggan\n";
        std::cout << "2. Kelola Kendaraan\n";
        std::cout << "3. Kelola Servis\n";
        std::cout << "4. Kelola Teknisi\n";
        std::cout << "5. Kelola Suku Cadang\n";
		std::cout << "6. Kelola Pembayaran\n";
		std::cout << "7. Transaksi Suku Cadang\n";
        std::cout << "8. Laporan & Ekspor CSV\n";
        std::cout << "9. Keluar\n";
        pilihan = getValidatedInt("Pilihan: ");

        clearScreen(); // Bersihkan sebelum eksekusi aksi menu

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
            menuLaporan(db);
            break;
        case 9:
            std::cout << "👋 Terima kasih telah menggunakan aplikasi!\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }

        if (pilihan != 8) pause(); // Tahan output sebelum clear ulang
    } while (pilihan != 8);

    return 0;
}
