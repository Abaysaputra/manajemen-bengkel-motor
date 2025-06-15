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
		std::cout << "2. Lihat Semua Pelanggan\n";
        std::cout << "3. Kembali\n";
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
        case 2: {
            auto* res = db.query("SELECT * FROM pelanggan");
            if (!res) {
                std::cerr << "❌ Gagal mengambil data.\n";
                break;
            }
            std::cout << "\n=== Daftar Pelanggan ===\n";
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                    << ", No HP: " << res->getString("no_hp") << ", Alamat: " << res->getString("alamat") << "\n";
            }
            delete res;
            break;
			  }
        case 3:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }
		pause(); 
    } while (pilih != 3);
}

void menuKendaraan(Database& db) {
    int pilih;
    do {
        clearScreen();
        std::cout << "\n--- MENU KENDARAAN ---\n";
        std::cout << "1. Tambah Kendaraan\n";
        std::cout << "2. Tampilkan Daftar Kendaraan\n";
        std::cout << "3. Kembali\n";

        pilih = getValidatedInt("Pilihan: ");
        clearScreen();

        switch (pilih) {
        case 1: {
            // Tampilkan daftar pelanggan terlebih dahulu
            auto* res = db.query("SELECT * FROM pelanggan");
            if (!res) {
                std::cerr << "❌ Gagal mengambil data pelanggan.\n";
                break;
            }

            std::cout << "\n=== Daftar Pelanggan ===\n";
            std::cout << "ID\tNama\t\tNo HP\t\tAlamat\n";
            std::cout << "----------------------------------------------------\n";
            while (res->next()) {
                std::cout << res->getInt("id") << "\t"
                    << res->getString("nama") << "\t\t"
                    << res->getString("no_hp") << "\t"
                    << res->getString("alamat") << "\n";
            }
            delete res;

            // Input data kendaraan
            // Tampilkan daftar pelanggan sebelum input kendaraan
            auto* resPelanggan = db.query("SELECT * FROM pelanggan");
            if (!resPelanggan || !resPelanggan->rowsCount()) {
                std::cout << "❌ Tidak ada data pelanggan. Tambahkan pelanggan terlebih dahulu.\n";
                delete resPelanggan;
                break;
            }
            std::cout << "\n=== Daftar Pelanggan ===\n";
            while (resPelanggan->next()) {
                std::cout << "ID: " << resPelanggan->getInt("id") << ", Nama: " << resPelanggan->getString("nama")
                    << ", No HP: " << resPelanggan->getString("no_hp") << ", Alamat: " << resPelanggan->getString("alamat") << "\n";
            }
            delete resPelanggan;

            // Input kendaraan
            Kendaraan k;
            k.inputData();


            std::string sql = "INSERT INTO kendaraan (id_pelanggan, merk, plat_nomor, tahun) VALUES (" +
                std::to_string(k.id_pelanggan) + ", '" + k.merk + "', '" +
                k.plat_nomor + "', " + std::to_string(k.tahun) + ")";
            db.execute(sql);
            std::cout << "✅ Data kendaraan berhasil ditambahkan.\n";
            break;
        }
        case 2: {
            std::cout << "\n=== Daftar Kendaraan ===\n";
            auto* resKendaraan = db.query("SELECT * FROM kendaraan");
            if (!resKendaraan) {
                std::cerr << "❌ Gagal mengambil data kendaraan.\n";
                break;
            }

            while (resKendaraan->next()) {
                std::cout << "ID: " << resKendaraan->getInt("id") << ", Pelanggan ID: "
                    << resKendaraan->getInt("id_pelanggan") << ", Merk: "
                    << resKendaraan->getString("merk") << ", Plat Nomor: "
                    << resKendaraan->getString("plat_nomor") << ", Tahun: "
                    << resKendaraan->getInt("tahun") << "\n";
            }
            delete resKendaraan;
            break;
        }
        case 3:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }

        pause();
    } while (pilih != 3);
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
        std::cout << "2. Lihat Service (Aktif)\n";
        std::cout << "3. Hapus Servis\n";
        std::cout << "4. Update Status Servis\n";
		std::cout << "5. Tampilkan riwayat servis\n";
        std::cout << "6. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");
        clearScreen();

        switch (pilih) {
        case 1: {
            // Tampilkan daftar kendaraan
            auto* resKendaraan = db.query("SELECT * FROM kendaraan");
            if (!resKendaraan || !resKendaraan->rowsCount()) {
                std::cout << "❌ Tidak ada data kendaraan. Tambahkan kendaraan terlebih dahulu.\n";
                delete resKendaraan;
                break;
            }

            std::cout << "\n=== Daftar Kendaraan ===\n";
            while (resKendaraan->next()) {
                std::cout << "ID: " << resKendaraan->getInt("id")
                    << ", Merk: " << resKendaraan->getString("merk")
                    << ", Plat: " << resKendaraan->getString("plat_nomor")
                    << ", Tahun: " << resKendaraan->getInt("tahun") << "\n";
            }
            delete resKendaraan;

            // Tampilkan daftar teknisi
            auto* resTeknisi = db.query("SELECT * FROM teknisi");
            if (!resTeknisi || !resTeknisi->rowsCount()) {
                std::cout << "❌ Tidak ada data teknisi. Tambahkan teknisi terlebih dahulu.\n";
                delete resTeknisi;
                break;
            }

            std::cout << "\n=== Daftar Teknisi ===\n";
            while (resTeknisi->next()) {
                std::cout << "ID: " << resTeknisi->getInt("id")
                    << ", Nama: " << resTeknisi->getString("nama")
                    << ", Keahlian: " << resTeknisi->getString("keahlian") << "\n";
            }
            delete resTeknisi;

            // Input servis
            Servis s;
            s.inputData();

            std::string sql = "INSERT INTO servis (id_kendaraan, keluhan, id_teknisi, status, tanggal) VALUES (" +
                std::to_string(s.id_kendaraan) + ", '" + s.keluhan + "', " +
                std::to_string(s.id_teknisi) + ", '" + s.status + "', '" + s.tanggal + "')";

            // Pilihan suku cadang
            char pakaiCadang;
            std::cout << "Apakah servis menggunakan suku cadang? (y/n): ";
            std::cin >> pakaiCadang;
            std::cin.ignore();

            if (pakaiCadang == 'y' || pakaiCadang == 'Y') {
                // Tampilkan suku cadang
                auto* resCadang = db.query("SELECT * FROM suku_cadang");
                if (!resCadang || !resCadang->rowsCount()) {
                    std::cout << "❌ Tidak ada data suku cadang. Tambahkan terlebih dahulu.\n";
                    delete resCadang;
                    break;
                }

                std::cout << "\n=== Daftar Suku Cadang ===\n";
                while (resCadang->next()) {
                    std::cout << "ID: " << resCadang->getInt("id")
                        << ", Nama: " << resCadang->getString("nama")
						<< ", Jenis: " << resCadang->getString("jenis")
                        << ", Stok: " << resCadang->getInt("stok")
                        << ", Harga: " << resCadang->getDouble("harga") << "\n";
                }
                delete resCadang;

                // Input ID & jumlah
                int idCadang = getValidatedInt("Masukkan ID suku cadang: ");
                int jumlahPakai = getValidatedInt("Masukkan jumlah yang dipakai: ");

                std::string sqlCheck = "SELECT stok FROM suku_cadang WHERE id = " + std::to_string(idCadang);
                sql::ResultSet* resCheck = db.query(sqlCheck);
                if (!resCheck || !resCheck->next()) {
                    std::cout << "❌ Suku cadang tidak ditemukan.\n";
                    if (resCheck) delete resCheck;
                    break;
                }

                int stokTersedia = resCheck->getInt("stok");
                delete resCheck;

                if (stokTersedia >= jumlahPakai) {
                    if (kurangiStokSukuCadang(db, idCadang, jumlahPakai)) {
                        db.execute(sql);
                        std::cout << "✅ Servis berhasil disimpan dan stok suku cadang dikurangi.\n";
                    }
                    else {
                        std::cout << "❌ Gagal mengurangi stok suku cadang.\n";
                    }
                }
                else {
                    std::cout << "❌ Stok tidak cukup! Maksimum: " << stokTersedia << "\n";
                }
            }
            else {
                db.execute(sql);
                std::cout << "✅ Servis berhasil disimpan.\n";
            }
            break;
        }

        case 2: {
            auto* res = db.query("SELECT * FROM servis WHERE status = 'Diproses'");
            if (!res) {
                std::cerr << "❌ Gagal mengambil data servis.\n";
                break;
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
            break;
        }

        case 3: {
            int idHapus = getValidatedInt("ID Servis yang ingin dihapus: ");
            db.execute("DELETE FROM servis WHERE id = " + std::to_string(idHapus));
            std::cout << "✅ Servis berhasil dihapus.\n";
            break;
        }

        case 4: {
			auto* res = db.query("SELECT * FROM servis WHERE status != 'Selesai'");

            if (!res) {
                std::cerr << "❌ Gagal mengambil data servis.\n";
                break;
			}
			std::cout << "\n=== Daftar Servis Aktif ===\n";
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id")
                    << ", Kendaraan ID: " << res->getInt("id_kendaraan")
                    << ", Teknisi ID: " << res->getInt("id_teknisi")
                    << ", Keluhan: " << res->getString("keluhan")
                    << ", Tanggal: " << res->getString("tanggal")
                    << ", Status: " << res->getString("status") << "\n";
			}
			delete res;

            int idUpdate = getValidatedInt("ID Servis yang ingin diupdate: ");
            std::string statusBaru;                              
            std::cout << "Status baru: ";
            std::getline(std::cin, statusBaru);
            std::string sql = "UPDATE servis SET status = '" + statusBaru + "' WHERE id = " + std::to_string(idUpdate);
            db.execute(sql);
            std::cout << "✅ Status berhasil diupdate.\n";
            break;
        }
		case 5: {
            auto* res = db.query("SELECT * FROM servis WHERE status = 'Selesai'");
            if (!res) {
                std::cerr << "❌ Gagal mengambil data servis.\n";
                break;
            }
            std::cout << "\n=== Riwayat Servis ===\n";
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id")
                    << ", Kendaraan ID: " << res->getInt("id_kendaraan")
                    << ", Teknisi ID: " << res->getInt("id_teknisi")
                    << ", Keluhan: " << res->getString("keluhan")
                    << ", Tanggal: " << res->getString("tanggal")
                    << ", Status: " << res->getString("status") << "\n";
            }
            delete res;
            break;
		}
        
        case 6:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;

        default:
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
            std::string sql = "INSERT INTO suku_cadang (nama, jenis, stok, harga) VALUES ('" +
                sc.nama + "', '" + sc.jenis + "' " + std::to_string(sc.stok) + ", " +
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
					<< ", Jenis: " << res->getString("jenis")
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
