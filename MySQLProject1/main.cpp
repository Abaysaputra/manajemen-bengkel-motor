#include <iostream>
#include <fstream>
#include "Database.h"
#include "Pelanggan.h"
#include "Kendaraan.h"
#include "Servis.h"
#include "Teknisi.h"

int main() {
    Database db;
    db.connect();

    int pilihan;
    do {
        std::cout << "\n=== APLIKASI PENGELOLAAN BENGKEL MOTOR ===\n";
        std::cout << "1. Tambah Pelanggan\n";
        std::cout << "2. Tambah Kendaraan\n";
        std::cout << "3. Tambah Servis\n";
        std::cout << "4. Lihat Semua Servis\n";
        std::cout << "5. Hapus Servis\n";
        std::cout << "6. Tambah Teknisi\n";
        std::cout << "7. Update Status Servis\n";
        std::cout << "8. Ekspor Data Servis ke CSV\n";
        std::cout << "9. Keluar\n";
        std::cout << "Pilihan: ";
        std::cin >> pilihan;

        std::cin.ignore();  // Hindari masalah dengan getline setelah cin

        switch (pilihan) {
        case 1: {
            Pelanggan p;
            p.inputData();
            std::string sql = "INSERT INTO pelanggan (nama, no_hp, alamat) VALUES ('" + p.nama + "', '" + p.no_hp + "', '" + p.alamat + "')";
            db.execute(sql);
            std::cout << "✅ Data pelanggan berhasil disimpan.\n";
            break;
        }
        case 2: {
            Kendaraan k;
            k.inputData();
            std::string sql = "INSERT INTO kendaraan (id_pelanggan, plat_nomor, merk, tahun) VALUES (" +
                std::to_string(k.id_pelanggan) + ", '" + k.plat_nomor + "', '" + k.merk + "', " +
                std::to_string(k.tahun) + ")";
            db.execute(sql);
            std::cout << "✅ Kendaraan berhasil ditambahkan.\n";
            break;
        }
        case 3: {
            Servis s;
            s.inputData();
            std::string sql = "INSERT INTO servis (id_kendaraan, id_teknisi, keluhan, tanggal, status) VALUES (" +
                std::to_string(s.id_kendaraan) + ", " + std::to_string(s.id_teknisi) + ", '" + s.keluhan + "', '" +
                s.tanggal + "', '" + s.status + "')";
            db.execute(sql);
            std::cout << "✅ Servis berhasil ditambahkan.\n";
            break;
        }
        case 4: {
            auto* res = db.query("SELECT * FROM servis");
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
        case 5: {
            int idHapus;
            std::cout << "Masukkan ID Servis yang ingin dihapus: ";
            std::cin >> idHapus;
            db.execute("DELETE FROM servis WHERE id = " + std::to_string(idHapus));
            std::cout << "✅ Servis berhasil dihapus.\n";
            break;
        }
        case 6: {
            Teknisi t;
            t.inputData();
            std::string sql = "INSERT INTO teknisi (nama, keahlian) VALUES ('" + t.nama + "', '" + t.keahlian + "')";
            db.execute(sql);
            std::cout << "✅ Teknisi berhasil ditambahkan.\n";
            break;
        }
        case 7: {
            int idUpdate;
            std::string statusBaru;
            std::cout << "Masukkan ID Servis yang ingin diupdate: ";
            std::cin >> idUpdate;
            std::cin.ignore();
            std::cout << "Masukkan status baru: ";
            std::getline(std::cin, statusBaru);

            std::string sql = "UPDATE servis SET status = '" + statusBaru + "' WHERE id = " + std::to_string(idUpdate);
            db.execute(sql);
            std::cout << "✅ Status servis berhasil diupdate.\n";
            break;
        }
        case 8: {
            std::ofstream file("data_servis.csv");
            if (!file.is_open()) {
                std::cerr << "❌ Gagal membuat file CSV.\n";
            }
            else {
                file << "ID,Kendaraan,Teknisi,Keluhan,Tanggal,Status\n";
                auto* res = db.query("SELECT * FROM servis");
                while (res->next()) {
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
            break;
        }
        case 9:
            std::cout << "👋 Terima kasih telah menggunakan aplikasi ini!\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid. Silakan coba lagi.\n";
        }

    } while (pilihan != 9);

    return 0;
}
