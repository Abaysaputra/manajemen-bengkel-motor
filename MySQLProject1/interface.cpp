#include "Database.h" // Include header Database untuk koneksi ke database
#include "Interface.h" // Include header baru
#include "Utils.h"      // Include fungsi bantu
#include "Pelanggan.h"
#include "Kendaraan.h"
#include "Servis.h"
#include "Teknisi.h"
#include "SukuCadang.h"
#include "PembayaranServis.h"
#include <iostream>
#include <fstream>
#include <vector> // Untuk std::vector di menuPelanggan



void menuPelanggan(Database& db) {
    int pilih;
    do {
        clearScreen();
        std::cout << "\n--- MENU PELANGGAN ---\n";
        std::cout << "1. Tambah Pelanggan\n";
        std::cout << "2. Lihat Semua Pelanggan\n";
        std::cout << "3. Hapus Pelanggan\n";
        std::cout << "4. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            Pelanggan p;
            p.inputData();

            std::string nama_escaped = escape_sql_string(p.nama);
            std::string no_hp_escaped = escape_sql_string(p.no_hp);
            std::string alamat_escaped = escape_sql_string(p.alamat);

            std::string sql = "INSERT INTO pelanggan (nama, no_hp, alamat) VALUES ('" +
                nama_escaped + "', '" + no_hp_escaped + "', '" + alamat_escaped + "')";

            try {
                db.execute(sql);
                std::cout << "✅ Data pelanggan berhasil disimpan.\n";
            }
            catch (const std::exception& e) {
                std::cerr << "❌ Gagal menyimpan data pelanggan: " << e.what() << std::endl;
            }
            pause();
            break;
        }
        case 2: {
            auto* res = db.query("SELECT * FROM pelanggan");
            if (!res) {
                std::cerr << "❌ Gagal mengambil data pelanggan.\n";
                break;
            }

            std::cout << "\n=== Daftar Pelanggan ===\n";
            if (res->rowsCount() == 0) {
                std::cout << "Belum ada data pelanggan yang tercatat.\n";
            }
            else {
                while (res->next()) {
                    std::cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                        << ", No HP: " << res->getString("no_hp") << ", Alamat: " << res->getString("alamat") << "\n";
                }
            }
            delete res;
            pause();
            break;
        }
        case 3: {
            // Tampilkan daftar pelanggan sebelum hapus
            auto* res = db.query("SELECT * FROM pelanggan");
            if (!res || res->rowsCount() == 0) {
                std::cerr << "❌ Tidak ada data pelanggan untuk dihapus.\n";
                if (res) delete res;
                pause();
                break;
            }

            std::cout << "\n=== Daftar Pelanggan ===\n";
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                    << ", No HP: " << res->getString("no_hp") << ", Alamat: " << res->getString("alamat") << "\n";
            }
            delete res; // Hapus ResultSet setelah digunakan

            int idHapus = getValidatedInt("Masukkan ID pelanggan yang ingin dihapus: ");

            // Konfirmasi penghapusan
            std::cout << "⚠️ Apakah Anda yakin ingin menghapus pelanggan dengan ID " << idHapus << " dan semua data terkaitnya (kendaraan, servis, pembayaran)? (y/n): ";
            char confirm;
            std::cin >> confirm;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                try {
                    // --- Langkah 1: Cek keberadaan pelanggan ---
                    auto* checkRes = db.query("SELECT COUNT(*) AS count FROM pelanggan WHERE id = " + std::to_string(idHapus));
                    if (!checkRes || !checkRes->next() || checkRes->getInt("count") == 0) {
                        std::cout << "❌ Pelanggan dengan ID " << idHapus << " tidak ditemukan.\n";
                        if (checkRes) delete checkRes;
                        pause();
                        break;
                    }
                    delete checkRes; // Bebaskan resource

                    std::cout << "Menghapus data terkait...\n";

                    // --- Langkah 2: Dapatkan ID Kendaraan yang terkait dengan pelanggan ini ---
                    // Ini penting karena Servis dan Pembayaran Servis terhubung ke Kendaraan,
                    // dan Kendaraan terhubung ke Pelanggan.
                    std::string get_kendaraan_ids_sql = "SELECT id FROM kendaraan WHERE id_pelanggan = " + std::to_string(idHapus);
                    auto* kendaraanRes = db.query(get_kendaraan_ids_sql);

                    std::vector<int> kendaraan_ids;
                    if (kendaraanRes) {
                        while (kendaraanRes->next()) {
                            kendaraan_ids.push_back(kendaraanRes->getInt("id"));
                        }
                        delete kendaraanRes;
                    }

                    // --- Langkah 3: Hapus data di pembayaran_servis dan servis yang terkait dengan kendaraan ini ---
                    if (!kendaraan_ids.empty()) {
                        std::string kendaraan_id_list = "";
                        for (size_t i = 0; i < kendaraan_ids.size(); ++i) {
                            kendaraan_id_list += std::to_string(kendaraan_ids[i]);
                            if (i < kendaraan_ids.size() - 1) {
                                kendaraan_id_list += ",";
                            }
                        }

                        // Dapatkan ID Servis yang terkait dengan Kendaraan ini
                        std::string get_servis_ids_sql = "SELECT id FROM servis WHERE id_kendaraan IN (" + kendaraan_id_list + ")";
                        auto* servisRes = db.query(get_servis_ids_sql);
                        std::vector<int> servis_ids;
                        if (servisRes) {
                            while (servisRes->next()) {
                                servis_ids.push_back(servisRes->getInt("id"));
                            }
                            delete servisRes;
                        }

                        // Hapus dari pembayaran_servis
                        if (!servis_ids.empty()) {
                            std::string servis_id_list = "";
                            for (size_t i = 0; i < servis_ids.size(); ++i) {
                                servis_id_list += std::to_string(servis_ids[i]);
                                if (i < servis_ids.size() - 1) {
                                    servis_id_list += ",";
                                }
                            }
                            std::string delete_pembayaran_sql = "DELETE FROM pembayaran_servis WHERE id_servis IN (" + servis_id_list + ")";
                            db.execute(delete_pembayaran_sql);
                            std::cout << "   ✅ Data pembayaran servis terkait berhasil dihapus.\n";
                        }

                        // Hapus dari servis
                        std::string delete_servis_sql = "DELETE FROM servis WHERE id_kendaraan IN (" + kendaraan_id_list + ")";
                        db.execute(delete_servis_sql);
                        std::cout << "   ✅ Data servis terkait berhasil dihapus.\n";

                        // Hapus dari kendaraan
                        std::string delete_kendaraan_sql = "DELETE FROM kendaraan WHERE id_pelanggan = " + std::to_string(idHapus);
                        db.execute(delete_kendaraan_sql);
                        std::cout << "   ✅ Data kendaraan terkait berhasil dihapus.\n";

                    }
                    else {
                        std::cout << "   Tidak ada kendaraan terkait dengan pelanggan ini, melanjutkan penghapusan pelanggan.\n";
                    }

                    // --- Langkah 4: Hapus pelanggan itu sendiri ---
                    db.execute("DELETE FROM pelanggan WHERE id = " + std::to_string(idHapus));
                    std::cout << "✅ Pelanggan dengan ID " << idHapus << " dan semua data terkaitnya berhasil dihapus.\n";

                }
                catch (const std::exception& e) {
                    std::cerr << "❌ Terjadi kesalahan saat menghapus pelanggan atau data terkait: " << e.what() << std::endl;
                    std::cerr << "   Penghapusan mungkin tidak selesai sepenuhnya. Harap periksa database secara manual.\n";
                }
            }
            else {
                std::cout << "Pembatalan penghapusan pelanggan.\n";
            }
            pause(); // Pause setelah operasi hapus/batal
            break;
        }
        case 4:
            std::cout << "↩️ Kembali ke menu utama...\n";
            // TIDAK PERLU pause() di sini
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
            pause(); // Pause setelah pilihan tidak valid
            break;
        }
    } while (pilih != 4);
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
            pause();
            break;
        }
        case 3:
            std::cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
        }
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
            // daftar servis
            auto* res = db.query("SELECT * FROM servis");
            if (!res || !res->rowsCount()) {
                std::cout << "❌ Tidak ada data servis untuk dihapus.\n";
                if (res) delete res;
                break;
            }
            std::cout << "\n=== Daftar Servis ===\n";
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id")
                    << ", Kendaraan ID: " << res->getInt("id_kendaraan")
                    << ", Teknisi ID: " << res->getInt("id_teknisi")
                    << ", Keluhan: " << res->getString("keluhan")
                    << ", Tanggal: " << res->getString("tanggal")
                    << ", Status: " << res->getString("status") << "\n";
            }
            delete res;

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
                    k.merk AS merk_kendaraan,        
                    k.plat_nomor AS plat_kendaraan,   
                    p.nama AS nama_pelanggan          
                FROM
                    servis s
                JOIN
                    kendaraan k ON s.id_kendaraan = k.id
                JOIN
                    pelanggan p ON k.id_pelanggan = p.id
                WHERE
                    s.status = 'Selesai'             
                ORDER BY
                    s.tanggal DESC                  
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
            sc.inputData();


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
            pause(); // Pause setelah menampilkan daftar

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
                pause();
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
            pause();
        }

    } while (pilih != 4);
}

void menuTeknisi(Database& db) {
    int pilih;
    do {
        clearScreen();
        std::cout << "\n--- MENU TEKNISI ---\n";
        std::cout << "1. Tambah Teknisi\n";
        std::cout << "2. Hapus Teknisi\n";  // Opsi Hapus Teknisi
        std::cout << "3. Kembali\n";        // Opsi Kembali, diubah menjadi 3
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            Teknisi t;
            t.inputData();
            std::string sql = "INSERT INTO teknisi (nama, keahlian) VALUES ('" +
                escape_sql_string(t.nama) + "', '" + escape_sql_string(t.keahlian) + "')"; // Gunakan escape_sql_string
            try {
                db.execute(sql);
                std::cout << "✅ Teknisi berhasil ditambahkan.\n";
            }
            catch (const std::exception& e) {
                std::cerr << "❌ Gagal menambahkan teknisi: " << e.what() << std::endl;
            }
            pause(); // Pause setelah operasi selesai
            break;
        }
        case 2: { // Case untuk Hapus Teknisi
            // Tampilkan daftar teknisi sebelum hapus
            auto* res = db.query("SELECT * FROM teknisi");
            if (!res || res->rowsCount() == 0) { // Cek juga jika tidak ada data
                std::cout << "❌ Tidak ada data teknisi untuk dihapus.\n";
                if (res) delete res;
                pause(); // Pause sebelum kembali
                break;
            }
            std::cout << "\n=== Daftar Teknisi ===\n";
            while (res->next()) {
                std::cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                    << ", Keahlian: " << res->getString("keahlian") << "\n";
            }
            delete res; // Hapus ResultSet setelah digunakan

            int idHapus = getValidatedInt("Masukkan ID teknisi yang ingin dihapus: ");

            // Konfirmasi penghapusan
            std::cout << "⚠️ Apakah Anda yakin ingin menghapus teknisi dengan ID " << idHapus << "? (y/n): ";
            char confirm;
            std::cin >> confirm;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                try {
                    // Cek apakah ID teknisi ada sebelum menghapus
                    auto* checkRes = db.query("SELECT COUNT(*) AS count FROM teknisi WHERE id = " + std::to_string(idHapus));
                    if (!checkRes || !checkRes->next() || checkRes->getInt("count") == 0) {
                        std::cout << "❌ Teknisi dengan ID " << idHapus << " tidak ditemukan.\n";
                        if (checkRes) delete checkRes;
                        pause();
                        break;
                    }
                    delete checkRes;

                    std::string sqlUpdateServis = "UPDATE servis SET id_teknisi = NULL WHERE id_teknisi = " + std::to_string(idHapus);
                    db.execute(sqlUpdateServis);
                    std::cout << "   ✅ Referensi teknisi di tabel servis berhasil diperbarui (diatur NULL).\n";

                    db.execute("DELETE FROM teknisi WHERE id = " + std::to_string(idHapus));
                    std::cout << "✅ Teknisi berhasil dihapus.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "❌ Gagal menghapus teknisi: " << e.what() << std::endl;
                    // Pesan khusus jika gagal karena foreign key (jika belum dihandle sebelumnya)
                    if (std::string(e.what()).find("FOREIGN KEY constraint failed") != std::string::npos ||
                        std::string(e.what()).find("a foreign key constraint fails") != std::string::npos) {
                        std::cerr << "   Pastikan tidak ada servis yang masih terhubung dengan teknisi ini, atau atur ON DELETE SET NULL/CASCADE di database.\n";
                    }
                }
            }
            else {
                std::cout << "Pembatalan penghapusan teknisi.\n";
            }
            pause(); // Pause setelah operasi hapus/batal
            break;
        }
        case 3: // Case untuk Kembali, diubah dari 2 menjadi 3
            std::cout << "↩️ Kembali ke menu utama...\n";
            // TIDAK PERLU pause() di sini
            break;
        default:
            std::cout << "❌ Pilihan tidak valid!\n";
            pause(); // Pause setelah pilihan tidak valid
            break;
        }
        // HAPUS pause() di sini, karena sudah ditangani di setiap case yang sesuai
        // pause();
    } while (pilih != 3); // Kondisi loop juga diubah dari 2 menjadi 3
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
                pause(); // Pause sebelum kembali
                break;
            }

            std::cout << "\n=== Daftar Servis Selesai & Belum Dibayar ===\n";
            while (res->next()) {
                std::cout << "ID Servis      : " << res->getInt("id") << "\n"
                    << "Pelanggan      : " << res->getString("nama") << "\n"
                    << "Plat Nomor     : " << res->getString("plat_nomor") << "\n"
                    << "Merk Kendaraan : " << res->getString("merk") << "\n"
                    << "Keluhan        : " << res->getString("keluhan") << "\n"
                    << "Tanggal Servis : " << res->getString("tanggal") << "\n"
                    << "--------------------------------------------\n";
            }
            delete res;

            int id_servis_dipilih = getValidatedInt("Masukkan ID Servis yang ingin dibayar: ");

            // --- Validasi ID Servis yang diinput ---
            auto* validateRes = db.query("SELECT id FROM servis WHERE id = " + std::to_string(id_servis_dipilih) + " AND status = 'Selesai' AND status_pembayaran = 'Belum Bayar'");
            if (!validateRes || !validateRes->next()) {
                std::cout << "❌ ID Servis tidak valid atau sudah dibayar/belum selesai.\n";
                if (validateRes) delete validateRes;
                pause(); // Pause sebelum kembali
                break;
            }
            delete validateRes;

            PembayaranService p;
            p.id_servis = id_servis_dipilih; // Gunakan ID yang sudah divalidasi
            p.inputData();  // input tanggal_pembayaran, jumlah, metode, status

            // Pastikan string yang dimasukkan ke SQL di-escape
            std::string metode_escaped = escape_sql_string(p.metode);
            std::string status_escaped = escape_sql_string(p.status);
            std::string jumlah_sql_format = format_double_for_sql(p.jumlah);

            std::string sqlInsert = "INSERT INTO pembayaran_servis (id_servis, tanggal_pembayaran, jumlah, metode, status) VALUES (" +
                std::to_string(p.id_servis) + ", '" +
                p.tanggal_pembayaran + "', " +
                jumlah_sql_format + ", '" + // Gunakan format_double_for_sql
                metode_escaped + "', '" +
                status_escaped + "')";

            try {
                db.execute(sqlInsert);
                std::cout << "✅ Pembayaran berhasil ditambahkan.\n";

                std::string sqlUpdate = "UPDATE servis SET status_pembayaran = '" + status_escaped + "' WHERE id = " + std::to_string(p.id_servis);
                db.execute(sqlUpdate);
                std::cout << "✅ Status servis berhasil diperbarui.\n";

            }
            catch (const std::exception& e) {
                std::cerr << "❌ Gagal memproses pembayaran: " << e.what() << std::endl;
            }
            pause(); // Pause setelah operasi selesai
            break;
        }

        case 2: {
            std::cout << "\n=== RIWAYAT PEMBAYARAN SERVIS ===\n";
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
                pause(); // Pause sebelum kembali
                break;
            }

            while (res->next()) {
                std::cout << "ID Servis          : " << res->getInt("id_servis") << "\n"
                    << "Nama Pelanggan     : " << res->getString("nama_pelanggan") << "\n"
                    << "Plat Nomor         : " << res->getString("plat_nomor") << "\n"
                    << "Merk Kendaraan     : " << res->getString("merk") << "\n"
                    << "Keluhan            : " << res->getString("keluhan") << "\n"
                    << "Tanggal Servis     : " << res->getString("tanggal_servis") << "\n"
                    << "Tanggal Pembayaran : " << res->getString("tanggal_pembayaran") << "\n"
                    << "Jumlah             : Rp" << std::fixed << std::setprecision(2) << res->getDouble("jumlah") << "\n" // Format output harga
                    << "Metode             : " << res->getString("metode") << "\n"
                    << "Status Pembayaran  : " << res->getString("status") << "\n"
                    << "---------------------------------------------\n";
            }
            delete res;
            pause(); // Pause setelah menampilkan riwayat
            break;
        }

        case 3:
            std::cout << "↩️ Kembali ke menu utama...\n";
            // TIDAK PERLU pause() di sini, karena parent menu (main) akan menanganinya
            break;

        default:
            std::cout << "❌ Pilihan tidak valid!\n";
            pause(); // Pause setelah pilihan tidak valid
            break;
        }

    } while (pilih != 3); // Loop akan terus berjalan sampai pengguna memilih 3
}




void menuLaporan(Database& db) {
    clearScreen();

    std::string folder = "data";
    if (!folderExists(folder)) {
        if (!createFolder(folder)) { // <--- GUNAKAN createFolder YANG BARU
            std::cerr << "❌ Gagal membuat folder 'data'. Pastikan izin sudah benar.\n";
            pause();
            return;
        }
    }

    std::string filePath = folder + "/data_servis.csv";
    std::ofstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "❌ Gagal membuat file CSV: " << filePath << ". Pastikan folder ada dan izin sudah benar.\n";
        pause();
        return;
    }

    // Header CSV
    file << "ID Servis,ID Kendaraan,ID Teknisi,Keluhan,Tanggal Servis,Status Servis,"
        << "Tanggal Pembayaran,Jumlah,Metode Pembayaran,Status Pembayaran\n";

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
            << "\"" << escape_sql_string(res->getString("keluhan")) << "\"," // Pastikan keluhan di-escape untuk CSV juga
            << res->getString("tanggal_servis") << ","
            << res->getString("status_servis") << ","
            << res->getString("tanggal_pembayaran") << ","
            << std::fixed << std::setprecision(2) << res->getDouble("jumlah") << "," // Format double untuk CSV
            << escape_sql_string(res->getString("metode")) << ","
            << escape_sql_string(res->getString("status_pembayaran")) << "\n";
    }

    delete res;
    file.close();

    std::cout << "✅ Data servis dan pembayaran lunas berhasil diekspor ke: " << filePath << "\n";
    pause();
}