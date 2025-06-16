#include <iostream>
#include <fstream>
#include "Database.h"
#include "Pelanggan.h"
#include "Kendaraan.h"
#include "Servis.h"
#include "Teknisi.h"
#include "SukuCadang.h"
#include "ServisDetail.h"
#include "PembayaranServis.h"
#include <cstdlib> // Untuk system("cls") atau system("clear")
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream> // Diperlukan untuk format double ke string untuk SQL
#include <direct.h>     // untuk _mkdir
#include <sys/stat.h>   // untuk _stat

bool folderExists(const std::string& folder) {
    struct _stat info;
    return _stat(folder.c_str(), &info) == 0 && (info.st_mode & _S_IFDIR);
}

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
std::string escape_sql_string(const std::string& s) {
    std::string escaped_s = s;
    // Ganti semua ' dengan '' (single quote menjadi double single quote)
    size_t pos = 0;
    while ((pos = escaped_s.find("'", pos)) != std::string::npos) {
        escaped_s.replace(pos, 1, "''");
        pos += 2; // Lewati '' yang baru saja disisipkan
    }
    // Ini mungkin tidak cukup untuk semua karakter khusus atau binari.
    return escaped_s;
}

// Fungsi bantu untuk memformat double menjadi string yang aman untuk SQL DECIMAL(X,2)
std::string format_double_for_sql(double val) {
    std::stringstream ss;
    // Gunakan std::fixed dan std::setprecision untuk memastikan 2 desimal
    ss << std::fixed << std::setprecision(2) << val;
    return ss.str();
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
        case 5: { // Asumsi ini adalah case untuk "Lihat Semua Servis" atau "Lihat Riwayat Servis"
            // Menggunakan JOIN untuk mengambil nama pelanggan dan merk/plat nomor kendaraan
                auto* res = db.query(R"(
                SELECT
                    s.id,
                    s.id_kendaraan,
                    s.id_teknisi,
                    s.keluhan,
                    s.tanggal,
                    s.status,
                    k.merk AS merk_kendaraan,         -- Ambil merk dari tabel kendaraan
                    k.plat_nomor AS plat_kendaraan,   -- Ambil plat_nomor dari tabel kendaraan
                    p.nama AS nama_pelanggan          -- Ambil nama dari tabel pelanggan
                FROM
                    servis s
                JOIN
                    kendaraan k ON s.id_kendaraan = k.id
                JOIN
                    pelanggan p ON k.id_pelanggan = p.id
                WHERE
                    s.status = 'Selesai'              -- Jika Anda hanya ingin servis yang 'Selesai'
                ORDER BY
                    s.tanggal DESC                    -- Opsional: Urutkan berdasarkan tanggal terbaru
            )");

            if (!res) {
                std::cerr << "❌ Gagal mengambil data servis.\n";
                break;
            }

            std::cout << "\n=== Riwayat Servis Selesai ===\n"; // Ubah judul sesuai filter
            if (res->rowsCount() == 0) {
                std::cout << "Tidak ada riwayat servis dengan status 'Selesai'.\n";
            }
            else {
                while (res->next()) {
                    std::cout << "-------------------------------------------\n";
                    std::cout << "ID Servis      : " << res->getInt("id") << "\n";
                    std::cout << "Nama Pelanggan : " << res->getString("nama_pelanggan") << "\n";
                    std::cout << "Kendaraan      : " << res->getString("merk_kendaraan")
                        << " (Plat: " << res->getString("plat_kendaraan") << ")\n";
                    // Anda bisa tampilkan ID Kendaraan & Teknisi jika masih diperlukan, tapi sekarang ada detail nama
                    // std::cout << "Kendaraan ID   : " << res->getInt("id_kendaraan") << "\n";
                    // std::cout << "Teknisi ID     : " << res->getInt("id_teknisi") << "\n";
                    std::cout << "Keluhan        : " << res->getString("keluhan") << "\n";
                    std::cout << "Tanggal        : " << res->getString("tanggal") << "\n";
                    std::cout << "Status         : " << res->getString("status") << "\n";
                }
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
    } while (pilih != 6);
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
            sc.inputData(); // Pastikan fungsi inputData() di SukuCadang.cpp sudah benar

            // Meng-escape string input untuk mencegah SQL Injection
            std::string nama_escaped = escape_sql_string(sc.nama);
            std::string jenis_escaped = escape_sql_string(sc.jenis);
            std::string harga_sql_format = format_double_for_sql(sc.harga);

            // Perbaikan utama: Menambahkan koma setelah 'jenis' dan memastikan format yang benar
            std::string sql = "INSERT INTO suku_cadang (nama, jenis, stok, harga) VALUES ('" +
                nama_escaped + "', '" +
                jenis_escaped + "', " + // <--- KOMA YANG HILANG DITAMBAHKAN DI SINI
                std::to_string(sc.stok) + ", " +
                harga_sql_format + ")"; // Menggunakan string harga yang diformat

            // Coba eksekusi dan tangani kemungkinan error dari database
            try {
                db.execute(sql);
                std::cout << "✅ Suku cadang berhasil ditambahkan.\n";
            }
            catch (const std::exception& e) {
                // Asumsi kelas Database Anda melempar std::exception atau turunannya
                std::cerr << "❌ Gagal menambahkan suku cadang: " << e.what() << std::endl;
            }
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
                std::cout << "ID: " << res->getInt("id")
                    << ", Nama: " << res->getString("nama")
                    << ", Jenis: " << res->getString("jenis")
                    << ", Stok: " << res->getInt("stok")
                    << ", Harga: " << std::fixed << std::setprecision(2) << res->getDouble("harga") << "\n";
            }
            delete res;
            break;
        }
        case 3: {
            // Tampilkan suku cadang sebelum hapus
            auto* res = db.query("SELECT * FROM suku_cadang");
            std::cout << "\n=== Daftar Suku Cadang ===\n";
            if (res) {
                while (res->next()) {
                    std::cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                        << ", Jenis: " << res->getString("jenis")
                        << ", Stok: " << res->getInt("stok") << ", Harga: " << std::fixed << std::setprecision(2) << res->getDouble("harga") << "\n";
                }
                delete res;
            }
            else {
                std::cerr << "❌ Gagal mengambil data untuk tampilan hapus.\n";
                // Mungkin tidak ada suku cadang, tidak perlu lanjutkan hapus jika daftar kosong
                break;
            }

            int idHapus = getValidatedInt("Masukkan ID suku cadang yang ingin dihapus: ");
            try {
                db.execute("DELETE FROM suku_cadang WHERE id = " + std::to_string(idHapus));
                std::cout << "✅ Suku cadang berhasil dihapus.\n";
            }
            catch (const std::exception& e) {
                std::cerr << "❌ Gagal menghapus suku cadang: " << e.what() << std::endl;
            }
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

void menuPembayaranServis(Database& db) {
    int pilih;
    do {
        clearScreen();
        std::cout << "\n--- MENU PEMBAYARAN SERVIS ---\n";
        std::cout << "1. Tambah Pembayaran\n";
        std::cout << "2. Lihat Riwayat Pembayaran\n";
        std::cout << "3. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            // Tampilkan daftar servis selesai yang belum dibayar
            // Perbaikan JOIN pelanggan, dari alias 'm' ke 'k'
            auto* res = db.query(R"(
                SELECT s.id, s.id_kendaraan, s.id_teknisi, s.keluhan, s.tanggal, s.status,
                       k.plat_nomor, k.merk, p.nama
                FROM servis s
                JOIN kendaraan k ON s.id_kendaraan = k.id
                JOIN pelanggan p ON k.id_pelanggan = p.id
                WHERE s.status = 'Selesai' AND s.status_pembayaran = 'Belum Bayar'
            )");

            if (!res || !res->rowsCount()) {
                std::cout << "❌ Tidak ada servis yang sudah selesai dan belum dibayar.\n";
                if (res) delete res;
                break;
            }

            std::cout << "\n=== Daftar Servis Selesai & Belum Dibayar ===\n";
            while (res->next()) {
                std::cout << "ID Servis      : " << res->getInt("id") << "\n"
                    << "Pelanggan      : " << res->getString("nama") << "\n"
                    << "Plat Nomor     : " << res->getString("plat_nomor") << "\n" // Menggunakan plat_nomor
                    << "Merk Kendaraan : " << res->getString("merk") << "\n"       // Menggunakan merk
                    << "Keluhan        : " << res->getString("keluhan") << "\n"
                    << "Tanggal Servis : " << res->getString("tanggal") << "\n"
                    << "--------------------------------------------\n";
            }
            delete res;

            int id_servis;
            std::cout << "\nMasukkan ID Servis yang ingin dibayar: ";
            std::cin >> id_servis;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Pastikan buffer clear

            PembayaranService p;
            p.id_servis = id_servis;
            p.inputData();  // input tanggal_pembayaran, jumlah, metode, status

            std::string sqlInsert = "INSERT INTO pembayaran_servis (id_servis, tanggal_pembayaran, jumlah, metode, status) VALUES (" +
                std::to_string(p.id_servis) + ", '" +
                p.tanggal_pembayaran + "', " +
                std::to_string(p.jumlah) + ", '" +
                p.metode + "', '" +
                p.status + "')";

            db.execute(sqlInsert);

            std::string sqlUpdate = "UPDATE servis SET status_pembayaran = '" + p.status + "' WHERE id = " + std::to_string(p.id_servis);
            db.execute(sqlUpdate);

            std::cout << "✅ Pembayaran berhasil ditambahkan dan status diperbarui.\n";
            pause();
            break;
        }

        case 2: {
            std::cout << "\n=== RIWAYAT PEMBAYARAN SERVIS ===\n";
            // Perbaikan JOIN motor m ON s.id_kendaraan = m.id menjadi JOIN kendaraan k
            std::string sql = R"(
                SELECT ps.id_servis, ps.tanggal_pembayaran, ps.jumlah, ps.metode, ps.status,
                       s.keluhan, s.tanggal AS tanggal_servis, s.id_kendaraan,
                       k.plat_nomor, k.merk,
                       p.nama AS nama_pelanggan
                FROM pembayaran_servis ps
                JOIN servis s ON ps.id_servis = s.id
                JOIN kendaraan k ON s.id_kendaraan = k.id
                JOIN pelanggan p ON k.id_pelanggan = p.id
                ORDER BY ps.tanggal_pembayaran DESC
            )";

            auto* res = db.query(sql);
            if (!res || res->rowsCount() == 0) {
                std::cout << "❌ Belum ada data pembayaran yang tercatat.\n";
                if (res) delete res;
                pause();
                break;
            }

            while (res->next()) {
                std::cout << "ID Servis          : " << res->getInt("id_servis") << "\n"
                    << "Nama Pelanggan     : " << res->getString("nama_pelanggan") << "\n"
                    << "Plat Nomor         : " << res->getString("plat_nomor") << "\n" // Menggunakan plat_nomor
                    << "Merk Kendaraan     : " << res->getString("merk") << "\n"       // Menggunakan merk
                    << "Keluhan            : " << res->getString("keluhan") << "\n"
                    << "Tanggal Servis     : " << res->getString("tanggal_servis") << "\n"
                    << "Tanggal Pembayaran : " << res->getString("tanggal_pembayaran") << "\n"
                    << "Jumlah             : Rp" << res->getDouble("jumlah") << "\n"
                    << "Metode             : " << res->getString("metode") << "\n"
                    << "Status Pembayaran  : " << res->getString("status") << "\n"
                    << "---------------------------------------------\n";
            }
            delete res;
            pause();
            break;
        }

        case 3:
            std::cout << "↩️ Kembali ke menu utama...\n";
            pause();
            break;

        default:
            std::cout << "❌ Pilihan tidak valid!\n";
            pause();
            break;
        }

    } while (pilih != 3);
}



void menuLaporan(Database& db) {
    clearScreen();

    std::string folder = "data";
    if (!folderExists(folder)) {
        _mkdir(folder.c_str());  // Buat folder jika belum ada
    }

    std::string filePath = folder + "/data_servis.csv";
    std::ofstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "❌ Gagal membuat file CSV.\n";
        pause();
        return;
    }

    // Header CSV
    file << "ID Servis,ID Kendaraan,ID Teknisi,Keluhan,Tanggal Servis,Status Servis,"
        << "Tanggal Pembayaran,Jumlah,Metode Pembayaran,Status Pembayaran\n";

    // ✅ QUERY diperbaiki: pastikan join berdasarkan id servis (id di servis = id_servis di pembayaran)
    std::string query = R"(
        SELECT 
            s.id AS id_servis, 
            s.id_kendaraan, 
            s.id_teknisi, 
            s.keluhan, 
            s.tanggal AS tanggal_servis, 
            s.status AS status_servis,
            p.tanggal_pembayaran, 
            p.jumlah, 
            p.metode, 
            p.status AS status_pembayaran
        FROM servis s
        JOIN pembayaran_servis p ON s.id = p.id_servis
        WHERE p.status = 'Lunas'
    )";

    auto* res = db.query(query);
    if (!res || !res->rowsCount()) {
        std::cout << "❌ Tidak ada data pembayaran lunas untuk diekspor.\n";
        if (res) delete res;
        pause();
        return;
    }

    while (res->next()) {
        file << res->getInt("id_servis") << ","
            << res->getInt("id_kendaraan") << ","
            << res->getInt("id_teknisi") << ","
            << "\"" << res->getString("keluhan") << "\","
            << res->getString("tanggal_servis") << ","
            << res->getString("status_servis") << ","
            << res->getString("tanggal_pembayaran") << ","
            << res->getDouble("jumlah") << ","
            << res->getString("metode") << ","
            << res->getString("status_pembayaran") << "\n";
    }

    delete res;
    file.close();

    std::cout << "✅ Data servis dan pembayaran lunas berhasil diekspor ke: " << filePath << "\n";
    pause();
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
		std::cout << "6. Kelola Pembayaran\n";
        std::cout << "7. Laporan & Ekspor CSV\n";
        std::cout << "8. Keluar\n";
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
            menuLaporan(db);
            break;
        case 8:
            std::cout << "👋 Terima kasih telah menggunakan aplikasi!\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }

        if (pilihan != 7) pause(); // Tahan output sebelum clear ulang
    } while (pilihan != 7);

    return 0;
}
