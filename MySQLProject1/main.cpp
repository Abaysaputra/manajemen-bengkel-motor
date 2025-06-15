#include <iostream>
#include <fstream>
#include "Database.h"
#include "Pelanggan.h"
#include "Kendaraan.h"
#include "Servis.h"
#include "Teknisi.h"
#include "SukuCadang.h"
#include "ServisDetail.h"
#include <thread>
#include <chrono>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    std::cout << "\nTekan Enter untuk melanjutkan...";
    std::cin.ignore();
}

int getValidatedInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear(); std::cin.ignore(10000, '\n');
            std::cout << "❌ Input tidak valid! Masukkan angka.\n";
        }
        else {
            std::cin.ignore();
            return value;
        }
    }
}


void menuPelanggan(Database& db) {
    int pilih;
    do {
        clearScreen();
        std::cout << "\n--- MENU PELANGGAN ---\n";
        std::cout << "1. Tambah Pelanggan\n";
        std::cout << "2. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            Pelanggan p;
            p.inputData();
            std::string sql = "INSERT INTO pelanggan (nama, no_hp, alamat) VALUES ('" +
                p.nama + "', '" + p.no_hp + "', '" + p.alamat + "')";
            db.execute(sql);
            std::cout << "✅ Data pelanggan berhasil disimpan.\n";
            break;
        }
        case 2:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }
		pause(); 
    } while (pilih != 2);
}


void menuKendaraan(Database& db) {
    int pilih;
    do {
		clearScreen();
        std::cout << "\n--- MENU KENDARAAN ---\n";
        std::cout << "1. Tambah Kendaraan\n";
        std::cout << "2. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

		clearScreen();
        switch (pilih) {
        case 1: {
            Kendaraan k;
            k.inputData();
            std::string sql = "INSERT INTO kendaraan (id_pelanggan, merk, plat_nomor, tahun) VALUES (" +
                std::to_string(k.id_pelanggan) + ", '" + k.merk + "', '" +
                k.plat_nomor + "', " + std::to_string(k.tahun) + ")";
            db.execute(sql);
            std::cout << "✅ Data kendaraan berhasil ditambahkan.\n";
            break;
        }
        case 2:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }
		pause(); 
    } while (pilih != 2);
}

//pengurangan suku cadang saat servis
bool kurangiStokSukuCadang(Database& db, int id_suku_cadang, int jumlah_dipakai) {
    std::string query = "SELECT stok FROM suku_cadang WHERE id = " + std::to_string(id_suku_cadang);
    sql::ResultSet* res = db.query(query);
    if (!res || !res->next()) {
        std::cout << "❌ ID suku cadang tidak ditemukan.\n";
        delete res;
        return false;
    }

    int stok_sekarang = res->getInt("stok");
    delete res;

    if (stok_sekarang < jumlah_dipakai) {
        std::cout << "❌ Stok tidak mencukupi. Sisa stok: " << stok_sekarang << "\n";
        return false;
    }

    int stok_baru = stok_sekarang - jumlah_dipakai;
    db.execute("UPDATE suku_cadang SET stok = " + std::to_string(stok_baru) +
        " WHERE id = " + std::to_string(id_suku_cadang));
    return true;
}

void menuServis(Database& db) {
    int pilih;
    do {
		clearScreen();
        std::cout << "\n--- MENU SERVIS ---\n";
        std::cout << "1. Tambah Servis\n";
        std::cout << "2. Lihat Semua Servis\n";
        std::cout << "3. Hapus Servis\n";
        std::cout << "4. Update Status Servis\n";
        std::cout << "5. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

		clearScreen();
        if (pilih == 1) {
            Servis s;
            s.inputData();

            std::string sql = "INSERT INTO servis (id_kendaraan, keluhan, id_teknisi, status, tanggal) VALUES (" +
                std::to_string(s.id_kendaraan) + ", '" + s.keluhan + "', " +
                std::to_string(s.id_teknisi) + ", " + ", '" +
                s.status + "', '" + s.tanggal + "')";

            // Tambahkan pilihan penggunaan suku cadang
            char pakaiCadang;
            std::cout << "Apakah servis menggunakan suku cadang? (y/n): ";
            std::cin >> pakaiCadang;
            std::cin.ignore();

            if (pakaiCadang == 'y' || pakaiCadang == 'Y') {
                int idCadang = getValidatedInt("Masukkan ID suku cadang: ");
                int jumlahPakai = getValidatedInt("Masukkan jumlah yang dipakai: ");

                if (kurangiStokSukuCadang(db, idCadang, jumlahPakai)) {
                    db.execute(sql);
                    std::cout << "✅ Servis berhasil disimpan dan stok suku cadang dikurangi.\n";
                }
                else {
                    std::cout << "❌ Servis dibatalkan karena stok tidak cukup.\n";
                }
            }
            else {
                db.execute(sql);
                std::cout << "✅ Servis berhasil disimpan.\n";
            }
            break;
        }
        else if (pilih == 2) {
            // Lihat Servis
            auto* res = db.query("SELECT * FROM servis");
            if (!res) {
                std::cerr << "❌ Gagal mengambil data.\n";
                return;
            }
            while (res->next()) {
                std::cout << "\n[Servis ID: " << res->getInt("id") << "]\n";
                std::cout << "Kendaraan: " << res->getInt("id_kendaraan")
                    << ", Teknisi: " << res->getInt("id_teknisi") << "\n";
                std::cout << "Tanggal: " << res->getString("tanggal") << "\n";
                std::cout << "Keluhan: " << res->getString("keluhan") << "\n";
                std::cout << "Status: " << res->getString("status") << "\n";
            }
            delete res;
        }
        else if (pilih == 3) {
            // Hapus Servis
            int idHapus;
            std::cout << "ID Servis yang ingin dihapus: ";
            std::cin >> idHapus;
            db.execute("DELETE FROM servis WHERE id = " + std::to_string(idHapus));
            std::cout << "✅ Servis berhasil dihapus.\n";
        }
        else if (pilih == 4) {
            // Update Status
            int idUpdate;
            std::string statusBaru;
            std::cout << "ID Servis yang ingin diupdate: ";
            std::cin >> idUpdate; std::cin.ignore();
            std::cout << "Status baru: ";
            std::getline(std::cin, statusBaru);
            std::string sql = "UPDATE servis SET status = '" + statusBaru + "' WHERE id = " + std::to_string(idUpdate);
            db.execute(sql);
            std::cout << "✅ Status berhasil diupdate.\n";
        }
        else if (pilih == 5) {
            std::cout << "↩️ Kembali ke menu utama...\n";
        }
        else {
            std::cout << "❌ Pilihan tidak valid!\n";
        }
		pause(); 
    } while (pilih != 5);
}
void menuSukuCadang(Database& db) {
    int pilih;
    do {
		clearScreen();
        std::cout << "\n--- MENU SUKU CADANG ---\n";
        std::cout << "1. Tambah Suku Cadang\n";
        std::cout << "2. Lihat Semua Suku Cadang\n";
        std::cout << "3. Hapus Suku Cadang\n";
        std::cout << "4. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

		clearScreen();
        switch (pilih) {
        case 1: {
            SukuCadang sc;
            sc.inputData();
            std::string sql = "INSERT INTO suku_cadang (nama, stok, harga) VALUES ('" +
                sc.nama + "', " + std::to_string(sc.stok) + ", " +
                std::to_string(sc.harga) + ")";
            db.execute(sql);
            std::cout << "✅ Suku cadang berhasil ditambahkan.\n";
            break;
        }
        case 2: {
            auto* res = db.query("SELECT * FROM suku_cadang");
            if (!res) {
                std::cerr << "❌ Gagal mengambil data.\n";
                break;
            }
            std::cout << "\n=== Daftar Suku Cadang ===\n";
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                    << ", Stok: " << res->getInt("stok") << ", Harga: " << res->getDouble("harga") << "\n";
            }
            delete res;
            break;
        }
        case 3: {
            int idHapus = getValidatedInt("Masukkan ID suku cadang yang ingin dihapus: ");
            db.execute("DELETE FROM suku_cadang WHERE id = " + std::to_string(idHapus));
            std::cout << "✅ Suku cadang berhasil dihapus.\n";
            break;
        }
        case 4:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }
		pause();
    } while (pilih != 4);
}

void menuTeknisi(Database& db) {
    int pilih;
    do {
		clearScreen();
        std::cout << "\n--- MENU TEKNISI ---\n";
        std::cout << "1. Tambah Teknisi\n";
        std::cout << "2. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

		clearScreen();
        switch (pilih) {
        case 1: {
            Teknisi t;
            t.inputData();
            std::string sql = "INSERT INTO teknisi (nama, keahlian) VALUES ('" +
                t.nama + "', '" + t.keahlian + "')";
            db.execute(sql);
            std::cout << "✅ Teknisi berhasil ditambahkan.\n";
            break;
        }
        case 2:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }
		pause();
    } while (pilih != 2);
}


void menuLaporan(Database& db) {
	clearScreen();
    std::ofstream file("data_servis.csv");
    if (!file.is_open()) {
        std::cerr << "❌ Gagal membuat file CSV.\n";
        return;
    }
    file << "ID,Kendaraan,Teknisi,Keluhan,Tanggal,Status\n";
    auto* res = db.query("SELECT * FROM servis");
    while (res && res->next()) {
        file << res->getInt("id") << ","
            << res->getInt("id_kendaraan") << ","
            << res->getInt("id_teknisi") << ","
            << "\"" << res->getString("keluhan") << "\","
            << res->getString("tanggal") << ","
            << res->getString("status") << "\n";
    }

    delete res;
    file.close();
    std::cout << "✅ Data servis berhasil diekspor ke data_servis.csv\n";
}

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
        std::cout << "6. Laporan & Ekspor CSV\n";
        std::cout << "7. Keluar\n";
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
            menuLaporan(db);
            break;
        case 7:
            std::cout << "👋 Terima kasih telah menggunakan aplikasi!\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }

        if (pilihan != 7) pause(); // Tahan output sebelum clear ulang
    } while (pilihan != 7);

    return 0;
}
