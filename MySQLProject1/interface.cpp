#include "Database.h" // Include header Database untuk koneksi ke database
#include "Interface.h" // Include header baru
#include "Utils.h"      // Include fungsi bantu
#include "Pelanggan.h"
#include "Kendaraan.h"
#include "Servis.h"
#include "Teknisi.h"
#include "SukuCadang.h"
#include "TransaksiSukuCadang.h"
#include "PembayaranServis.h"
#include <iostream>
#include <fstream>
#include <vector> // Untuk vector di menuPelanggan

using namespace std;

void menuPelanggan(Database& db) {
    int pilih;
    do {
        clearScreen();
        cout << "\n--- MENU PELANGGAN ---\n";
        cout << "1. Tambah Pelanggan\n";
        cout << "2. Lihat Semua Pelanggan\n";
        cout << "3. Hapus Pelanggan\n";
        cout << "4. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            Pelanggan p;
            p.inputData();

            string nama_escaped = escape_sql_string(p.nama);
            string no_hp_escaped = escape_sql_string(p.no_hp);
            string alamat_escaped = escape_sql_string(p.alamat);

            string sql = "INSERT INTO pelanggan (nama, no_hp, alamat) VALUES ('" +
                nama_escaped + "', '" + no_hp_escaped + "', '" + alamat_escaped + "')";

            try {
                db.execute(sql);
                cout << "✅ Data pelanggan berhasil disimpan.\n";
            }
            catch (const exception& e) {
                cerr << "❌ Gagal menyimpan data pelanggan: " << e.what() << endl;
            }
            pause();
            break;
        }
        case 2: {
            auto* res = db.query("SELECT * FROM pelanggan");
            if (!res) {
                cerr << "❌ Gagal mengambil data pelanggan.\n";
                break;
            }

            cout << "\n=== Daftar Pelanggan ===\n";
            if (res->rowsCount() == 0) {
                cout << "Belum ada data pelanggan yang tercatat.\n";
            }
            else {
                while (res->next()) {
                    cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
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
                cerr << "❌ Tidak ada data pelanggan untuk dihapus.\n";
                if (res) delete res;
                pause();
                break;
            }

            cout << "\n=== Daftar Pelanggan ===\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                    << ", No HP: " << res->getString("no_hp") << ", Alamat: " << res->getString("alamat") << "\n";
            }
            delete res; // Hapus ResultSet setelah digunakan

            int idHapus = getValidatedInt("Masukkan ID pelanggan yang ingin dihapus: ");

            // Konfirmasi penghapusan
            cout << "⚠️ Apakah Anda yakin ingin menghapus pelanggan dengan ID " << idHapus << " dan semua data terkaitnya (kendaraan, servis, pembayaran)? (y/n): ";
            char confirm;
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                try {
                    // --- Langkah 1: Cek keberadaan pelanggan ---
                    auto* checkRes = db.query("SELECT COUNT(*) AS count FROM pelanggan WHERE id = " + to_string(idHapus));
                    if (!checkRes || !checkRes->next() || checkRes->getInt("count") == 0) {
                        cout << "❌ Pelanggan dengan ID " << idHapus << " tidak ditemukan.\n";
                        if (checkRes) delete checkRes;
                        pause();
                        break;
                    }
                    delete checkRes; // Bebaskan resource

                    cout << "Menghapus data terkait...\n";

                    // --- Langkah 2: Dapatkan ID Kendaraan yang terkait dengan pelanggan ini ---
                    // Ini penting karena Servis dan Pembayaran Servis terhubung ke Kendaraan,
                    // dan Kendaraan terhubung ke Pelanggan.
                    string get_kendaraan_ids_sql = "SELECT id FROM kendaraan WHERE id_pelanggan = " + to_string(idHapus);
                    auto* kendaraanRes = db.query(get_kendaraan_ids_sql);

                    vector<int> kendaraan_ids;
                    if (kendaraanRes) {
                        while (kendaraanRes->next()) {
                            kendaraan_ids.push_back(kendaraanRes->getInt("id"));
                        }
                        delete kendaraanRes;
                    }

                    // --- Langkah 3: Hapus data di pembayaran_servis dan servis yang terkait dengan kendaraan ini ---
                    if (!kendaraan_ids.empty()) {
                        string kendaraan_id_list = "";
                        for (size_t i = 0; i < kendaraan_ids.size(); ++i) {
                            kendaraan_id_list += to_string(kendaraan_ids[i]);
                            if (i < kendaraan_ids.size() - 1) {
                                kendaraan_id_list += ",";
                            }
                        }

                        // Dapatkan ID Servis yang terkait dengan Kendaraan ini
                        string get_servis_ids_sql = "SELECT id FROM servis WHERE id_kendaraan IN (" + kendaraan_id_list + ")";
                        auto* servisRes = db.query(get_servis_ids_sql);
                        vector<int> servis_ids;
                        if (servisRes) {
                            while (servisRes->next()) {
                                servis_ids.push_back(servisRes->getInt("id"));
                            }
                            delete servisRes;
                        }

                        // Hapus dari pembayaran_servis
                        if (!servis_ids.empty()) {
                            string servis_id_list = "";
                            for (size_t i = 0; i < servis_ids.size(); ++i) {
                                servis_id_list += to_string(servis_ids[i]);
                                if (i < servis_ids.size() - 1) {
                                    servis_id_list += ",";
                                }
                            }
                            string delete_pembayaran_sql = "DELETE FROM pembayaran_servis WHERE id_servis IN (" + servis_id_list + ")";
                            db.execute(delete_pembayaran_sql);
                            cout << "   ✅ Data pembayaran servis terkait berhasil dihapus.\n";
                        }

                        // Hapus dari servis
                        string delete_servis_sql = "DELETE FROM servis WHERE id_kendaraan IN (" + kendaraan_id_list + ")";
                        db.execute(delete_servis_sql);
                        cout << "   ✅ Data servis terkait berhasil dihapus.\n";

                        // Hapus dari kendaraan
                        string delete_kendaraan_sql = "DELETE FROM kendaraan WHERE id_pelanggan = " + to_string(idHapus);
                        db.execute(delete_kendaraan_sql);
                        cout << "   ✅ Data kendaraan terkait berhasil dihapus.\n";

                    }
                    else {
                        cout << "   Tidak ada kendaraan terkait dengan pelanggan ini, melanjutkan penghapusan pelanggan.\n";
                    }

                    // --- Langkah 4: Hapus pelanggan itu sendiri ---
                    db.execute("DELETE FROM pelanggan WHERE id = " + to_string(idHapus));
                    cout << "✅ Pelanggan dengan ID " << idHapus << " dan semua data terkaitnya berhasil dihapus.\n";

                }
                catch (const exception& e) {
                    cerr << "❌ Terjadi kesalahan saat menghapus pelanggan atau data terkait: " << e.what() << endl;
                    cerr << "   Penghapusan mungkin tidak selesai sepenuhnya. Harap periksa database secara manual.\n";
                }
            }
            else {
                cout << "Pembatalan penghapusan pelanggan.\n";
            }
            pause(); // Pause setelah operasi hapus/batal
            break;
        }
        case 4:
            cout << "↩️ Kembali ke menu utama...\n";
            // TIDAK PERLU pause() di sini
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
            pause(); // Pause setelah pilihan tidak valid
            break;
        }
    } while (pilih != 4);
}


void menuKendaraan(Database& db) {
    int pilih;
    do {
        clearScreen();
        cout << "\n--- MENU KENDARAAN ---\n";
        cout << "1. Tambah Kendaraan\n";
        cout << "2. Tampilkan Daftar Kendaraan\n";
        cout << "3. Kembali\n";

        pilih = getValidatedInt("Pilihan: ");
        clearScreen();

        switch (pilih) {
        case 1: {
            // Tampilkan daftar pelanggan terlebih dahulu
            auto* res = db.query("SELECT * FROM pelanggan");
            if (!res) {
                cerr << "❌ Gagal mengambil data pelanggan.\n";
                break;
            }

            cout << "\n=== Daftar Pelanggan ===\n";
            cout << "ID\tNama\t\tNo HP\t\tAlamat\n";
            cout << "----------------------------------------------------\n";
            while (res->next()) {
                cout << res->getInt("id") << "\t"
                    << res->getString("nama") << "\t\t"
                    << res->getString("no_hp") << "\t"
                    << res->getString("alamat") << "\n";
            }
            delete res;

            // Input kendaraan
            Kendaraan k;
            k.inputData();


            string sql = "INSERT INTO kendaraan (id_pelanggan, merk, plat_nomor, tahun) VALUES (" +
                to_string(k.id_pelanggan) + ", '" + k.merk + "', '" +
                k.plat_nomor + "', " + to_string(k.tahun) + ")";
            db.execute(sql);
            cout << "✅ Data kendaraan berhasil ditambahkan.\n";
            break;
        }
        case 2: {
            cout << "\n=== Daftar Kendaraan ===\n";
            auto* resKendaraan = db.query("SELECT * FROM kendaraan");
            if (!resKendaraan) {
                cerr << "❌ Gagal mengambil data kendaraan.\n";
                break;
            }

            while (resKendaraan->next()) {
                cout << "ID: " << resKendaraan->getInt("id") << ", Pelanggan ID: "
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
            cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
        }
    } while (pilih != 3);
}



//pengurangan suku cadang saat servis
bool kurangiStokSukuCadang(Database& db, int id_suku_cadang, int jumlah_dipakai) {
    string query = "SELECT stok FROM suku_cadang WHERE id = " + to_string(id_suku_cadang);
    sql::ResultSet* res = db.query(query);
    if (!res || !res->next()) {
        cout << "❌ ID suku cadang tidak ditemukan.\n";
        delete res;
        return false;
    }

    int stok_sekarang = res->getInt("stok");
    delete res;

    if (stok_sekarang < jumlah_dipakai) {
        cout << "❌ Stok tidak mencukupi. Sisa stok: " << stok_sekarang << "\n";
        return false;
    }

    int stok_baru = stok_sekarang - jumlah_dipakai;
    db.execute("UPDATE suku_cadang SET stok = " + to_string(stok_baru) +
        " WHERE id = " + to_string(id_suku_cadang));
    return true;
}

void menuServis(Database& db) {
    int pilih;
    do {
        clearScreen();
        cout << "\n--- MENU SERVIS ---\n";
        cout << "1. Tambah Servis\n";
        cout << "2. Lihat Service (Aktif)\n";
        cout << "3. Hapus Servis\n";
        cout << "4. Update Status Servis\n";
        cout << "5. Tampilkan riwayat servis\n";
        cout << "6. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");
        clearScreen();

        switch (pilih) {
        case 1: {
            // Tampilkan daftar kendaraan
            auto* resKendaraan = db.query("SELECT * FROM kendaraan");
            if (!resKendaraan || !resKendaraan->rowsCount()) {
                cout << "❌ Tidak ada data kendaraan. Tambahkan kendaraan terlebih dahulu.\n";
                delete resKendaraan;
                break;
            }

            cout << "\n=== Daftar Kendaraan ===\n";
            while (resKendaraan->next()) {
                cout << "ID: " << resKendaraan->getInt("id")
                    << ", Merk: " << resKendaraan->getString("merk")
                    << ", Plat: " << resKendaraan->getString("plat_nomor")
                    << ", Tahun: " << resKendaraan->getInt("tahun") << "\n";
            }
            delete resKendaraan;

            // Tampilkan daftar teknisi
            auto* resTeknisi = db.query("SELECT * FROM teknisi");
            if (!resTeknisi || !resTeknisi->rowsCount()) {
                cout << "❌ Tidak ada data teknisi. Tambahkan teknisi terlebih dahulu.\n";
                delete resTeknisi;
                break;
            }

            cout << "\n=== Daftar Teknisi ===\n";
            while (resTeknisi->next()) {
                cout << "ID: " << resTeknisi->getInt("id")
                    << ", Nama: " << resTeknisi->getString("nama")
                    << ", Keahlian: " << resTeknisi->getString("keahlian") << "\n";
            }
            delete resTeknisi;

            // Input servis
            Servis s;
            s.inputData();

            string sql = "INSERT INTO servis (id_kendaraan, keluhan, id_teknisi, status, tanggal) VALUES (" +
                to_string(s.id_kendaraan) + ", '" + s.keluhan + "', " +
                to_string(s.id_teknisi) + ", '" + s.status + "', '" + s.tanggal + "')";

            // Pilihan suku cadang
            char pakaiCadang;
            cout << "Apakah servis menggunakan suku cadang? (y/n): ";
            cin >> pakaiCadang;
            cin.ignore();

            if (pakaiCadang == 'y' || pakaiCadang == 'Y') {
                // Tampilkan suku cadang
                auto* resCadang = db.query("SELECT * FROM suku_cadang");
                if (!resCadang || !resCadang->rowsCount()) {
                    cout << "❌ Tidak ada data suku cadang. Tambahkan terlebih dahulu.\n";
                    delete resCadang;
                    break;
                }

                cout << "\n=== Daftar Suku Cadang ===\n";
                while (resCadang->next()) {
                    cout << "ID: " << resCadang->getInt("id")
                        << ", Nama: " << resCadang->getString("nama")
                        << ", Jenis: " << resCadang->getString("jenis")
                        << ", Stok: " << resCadang->getInt("stok")
                        << ", Harga: " << resCadang->getDouble("harga") << "\n";
                }
                delete resCadang;

                // Input ID & jumlah
                int idCadang = getValidatedInt("Masukkan ID suku cadang: ");
                int jumlahPakai = getValidatedInt("Masukkan jumlah yang dipakai: ");

                string sqlCheck = "SELECT stok FROM suku_cadang WHERE id = " + to_string(idCadang);
                sql::ResultSet* resCheck = db.query(sqlCheck);
                if (!resCheck || !resCheck->next()) {
                    cout << "❌ Suku cadang tidak ditemukan.\n";
                    if (resCheck) delete resCheck;
                    break;
                }

                int stokTersedia = resCheck->getInt("stok");
                delete resCheck;

                if (stokTersedia >= jumlahPakai) {
                    if (kurangiStokSukuCadang(db, idCadang, jumlahPakai)) {
                        db.execute(sql);
                        cout << "✅ Servis berhasil disimpan dan stok suku cadang dikurangi.\n";
                    }
                    else {
                        cout << "❌ Gagal mengurangi stok suku cadang.\n";
                    }
                }
                else {
                    cout << "❌ Stok tidak cukup! Maksimum: " << stokTersedia << "\n";
                }
            }
            else {
                db.execute(sql);
                cout << "✅ Servis berhasil disimpan.\n";
            }
            break;
        }

        case 2: {
            auto* res = db.query("SELECT * FROM servis WHERE status = 'Diproses'");
            if (!res) {
                cerr << "❌ Gagal mengambil data servis.\n";
                break;
            }

            while (res->next()) {
                cout << "\n[Servis ID: " << res->getInt("id") << "]\n";
                cout << "Kendaraan: " << res->getInt("id_kendaraan")
                    << ", Teknisi: " << res->getInt("id_teknisi") << "\n";
                cout << "Tanggal: " << res->getString("tanggal") << "\n";
                cout << "Keluhan: " << res->getString("keluhan") << "\n";
                cout << "Status: " << res->getString("status") << "\n";
            }
            delete res;
            if (res->rowsCount() == 0) {
                cout << "Tidak ada servis yang sedang diproses.\n";
			}
			pause(); 
            break;
        }

        case 3: {
            // daftar servis
            auto* res = db.query("SELECT * FROM servis");
            if (!res || !res->rowsCount()) {
                cout << "❌ Tidak ada data servis untuk dihapus.\n";
                if (res) delete res;
                break;
            }
            cout << "\n=== Daftar Servis ===\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id")
                    << ", Kendaraan ID: " << res->getInt("id_kendaraan")
                    << ", Teknisi ID: " << res->getInt("id_teknisi")
                    << ", Keluhan: " << res->getString("keluhan")
                    << ", Tanggal: " << res->getString("tanggal")
                    << ", Status: " << res->getString("status") << "\n";
            }
            delete res;


            int idHapus = getValidatedInt("ID Servis yang ingin dihapus: ");
            db.execute("DELETE FROM servis WHERE id = " + to_string(idHapus));
            cout << "✅ Servis berhasil dihapus.\n";
            break;
        }

        case 4: {
            auto* res = db.query("SELECT * FROM servis WHERE status != 'Selesai'");

            if (!res) {
                cerr << "❌ Gagal mengambil data servis.\n";
                break;
            }
            cout << "\n=== Daftar Servis Aktif ===\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id")
                    << ", Kendaraan ID: " << res->getInt("id_kendaraan")
                    << ", Teknisi ID: " << res->getInt("id_teknisi")
                    << ", Keluhan: " << res->getString("keluhan")
                    << ", Tanggal: " << res->getString("tanggal")
                    << ", Status: " << res->getString("status") << "\n";
            }
            delete res;

            int idUpdate = getValidatedInt("ID Servis yang ingin diupdate: ");
            string statusBaru;
            cout << "Status baru: ";
            getline(cin, statusBaru);
            string sql = "UPDATE servis SET status = '" + statusBaru + "' WHERE id = " + to_string(idUpdate);
            db.execute(sql);
            cout << "✅ Status berhasil diupdate.\n";
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
                cerr << "❌ Gagal mengambil data servis.\n";
                break;
            }

            cout << "\n=== Riwayat Servis Selesai ===\n"; // Ubah judul sesuai filter
            if (res->rowsCount() == 0) {
                cout << "Tidak ada riwayat servis dengan status 'Selesai'.\n";
            }
            else {
                while (res->next()) {
                    cout << "-------------------------------------------\n";
                    cout << "ID Servis      : " << res->getInt("id") << "\n";
                    cout << "Nama Pelanggan : " << res->getString("nama_pelanggan") << "\n";
                    cout << "Kendaraan      : " << res->getString("merk_kendaraan")
                        << " (Plat: " << res->getString("plat_kendaraan") << ")\n";
                    cout << "Keluhan        : " << res->getString("keluhan") << "\n";
                    cout << "Tanggal        : " << res->getString("tanggal") << "\n";
                    cout << "Status         : " << res->getString("status") << "\n";
                }
            }
            delete res;
			pause(); 
            break;
        }

        case 6:
            cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
        }

    } while (pilih != 6);
}

void menuSukuCadang(Database& db) {
    int pilih;
    do {
        clearScreen();
        cout << "\n--- MENU SUKU CADANG ---\n";
        cout << "1. Tambah Suku Cadang\n";
        cout << "2. Lihat Semua Suku Cadang\n";
        cout << "3. Hapus Suku Cadang\n";
        cout << "4. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            SukuCadang sc;
            sc.inputData();


            string nama_escaped = escape_sql_string(sc.nama);
            string jenis_escaped = escape_sql_string(sc.jenis);
            string harga_sql_format = format_double_for_sql(sc.harga);

            // Perbaikan utama: Menambahkan koma setelah 'jenis' dan memastikan format yang benar
            string sql = "INSERT INTO suku_cadang (nama, jenis, stok, harga) VALUES ('" +
                nama_escaped + "', '" +
                jenis_escaped + "', " + // <--- KOMA YANG HILANG DITAMBAHKAN DI SINI
                to_string(sc.stok) + ", " +
                harga_sql_format + ")"; // Menggunakan string harga yang diformat

            // Coba eksekusi dan tangani kemungkinan error dari database
            try {
                db.execute(sql);
                cout << "✅ Suku cadang berhasil ditambahkan.\n";
            }
            catch (const exception& e) {
                // Asumsi kelas Database Anda melempar exception atau turunannya
                cerr << "❌ Gagal menambahkan suku cadang: " << e.what() << endl;
            }
            break;
        }
        case 2: {
            auto* res = db.query("SELECT * FROM suku_cadang");
            if (!res) {
                cerr << "❌ Gagal mengambil data.\n";
                break;
            }
            cout << "\n=== Daftar Suku Cadang ===\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id")
                    << ", Nama: " << res->getString("nama")
                    << ", Jenis: " << res->getString("jenis")
                    << ", Stok: " << res->getInt("stok")
                    << ", Harga: " << fixed << setprecision(2) << res->getDouble("harga") << "\n";
            }
            delete res;
            pause(); // Pause setelah menampilkan daftar

            break;
        }
        case 3: {
            // Tampilkan suku cadang sebelum hapus
            auto* res = db.query("SELECT * FROM suku_cadang");
            cout << "\n=== Daftar Suku Cadang ===\n";
            if (res) {
                while (res->next()) {
                    cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                        << ", Jenis: " << res->getString("jenis")
                        << ", Stok: " << res->getInt("stok") << ", Harga: " << fixed << setprecision(2) << res->getDouble("harga") << "\n";
                }
                delete res;
                pause();
            }
            else {
                cerr << "❌ Gagal mengambil data untuk tampilan hapus.\n";
                // Mungkin tidak ada suku cadang, tidak perlu lanjutkan hapus jika daftar kosong
                break;
            }

            int idHapus = getValidatedInt("Masukkan ID suku cadang yang ingin dihapus: ");
            try {
                db.execute("DELETE FROM suku_cadang WHERE id = " + to_string(idHapus));
                cout << "✅ Suku cadang berhasil dihapus.\n";
            }
            catch (const exception& e) {
                cerr << "❌ Gagal menghapus suku cadang: " << e.what() << endl;
            }
            break;
        }
        case 4:
            cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
            pause();
        }

    } while (pilih != 4);
}

void menuTeknisi(Database& db) {
    int pilih;
    do {
        clearScreen();
        cout << "\n--- MENU TEKNISI ---\n";
        cout << "1. Tambah Teknisi\n";
        cout << "2. Hapus Teknisi\n";  // Opsi Hapus Teknisi
        cout << "3. Kembali\n";        // Opsi Kembali, diubah menjadi 3
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            Teknisi t;
            t.inputData();
            string sql = "INSERT INTO teknisi (nama, keahlian) VALUES ('" +
                escape_sql_string(t.nama) + "', '" + escape_sql_string(t.keahlian) + "')"; // Gunakan escape_sql_string
            try {
                db.execute(sql);
                cout << "✅ Teknisi berhasil ditambahkan.\n";
            }
            catch (const exception& e) {
                cerr << "❌ Gagal menambahkan teknisi: " << e.what() << endl;
            }
            pause(); // Pause setelah operasi selesai
            break;
        }
        case 2: { // Case untuk Hapus Teknisi
            // Tampilkan daftar teknisi sebelum hapus
            auto* res = db.query("SELECT * FROM teknisi");
            if (!res || res->rowsCount() == 0) { // Cek juga jika tidak ada data
                cout << "❌ Tidak ada data teknisi untuk dihapus.\n";
                if (res) delete res;
                pause(); // Pause sebelum kembali
                break;
            }
            cout << "\n=== Daftar Teknisi ===\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                    << ", Keahlian: " << res->getString("keahlian") << "\n";
            }
            delete res; // Hapus ResultSet setelah digunakan

            int idHapus = getValidatedInt("Masukkan ID teknisi yang ingin dihapus: ");

            // Konfirmasi penghapusan
            cout << "⚠️ Apakah Anda yakin ingin menghapus teknisi dengan ID " << idHapus << "? (y/n): ";
            char confirm;
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                try {
                    // Cek apakah ID teknisi ada sebelum menghapus
                    auto* checkRes = db.query("SELECT COUNT(*) AS count FROM teknisi WHERE id = " + to_string(idHapus));
                    if (!checkRes || !checkRes->next() || checkRes->getInt("count") == 0) {
                        cout << "❌ Teknisi dengan ID " << idHapus << " tidak ditemukan.\n";
                        if (checkRes) delete checkRes;
                        pause();
                        break;
                    }
                    delete checkRes;

                    string sqlUpdateServis = "UPDATE servis SET id_teknisi = NULL WHERE id_teknisi = " + to_string(idHapus);
                    db.execute(sqlUpdateServis);
                    cout << "   ✅ Referensi teknisi di tabel servis berhasil diperbarui (diatur NULL).\n";

                    db.execute("DELETE FROM teknisi WHERE id = " + to_string(idHapus));
                    cout << "✅ Teknisi berhasil dihapus.\n";
                }
                catch (const exception& e) {
                    cerr << "❌ Gagal menghapus teknisi: " << e.what() << endl;
                    // Pesan khusus jika gagal karena foreign key (jika belum dihandle sebelumnya)
                    if (string(e.what()).find("FOREIGN KEY constraint failed") != string::npos ||
                        string(e.what()).find("a foreign key constraint fails") != string::npos) {
                        cerr << "   Pastikan tidak ada servis yang masih terhubung dengan teknisi ini, atau atur ON DELETE SET NULL/CASCADE di database.\n";
                    }
                }
            }
            else {
                cout << "Pembatalan penghapusan teknisi.\n";
            }
            pause(); // Pause setelah operasi hapus/batal
            break;
        }
        case 3: // Case untuk Kembali, diubah dari 2 menjadi 3
            cout << "↩️ Kembali ke menu utama...\n";
            // TIDAK PERLU pause() di sini
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
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
        cout << "\n--- MENU PEMBAYARAN SERVIS ---\n";
        cout << "1. Tambah Pembayaran\n";
        cout << "2. Lihat Riwayat Pembayaran\n";
        cout << "3. Kembali\n";
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
                cout << "❌ Tidak ada servis yang sudah selesai dan belum dibayar.\n";
                if (res) delete res;
                pause(); // Pause sebelum kembali
                break;
            }

            cout << "\n=== Daftar Servis Selesai & Belum Dibayar ===\n";
            while (res->next()) {
                cout << "ID Servis      : " << res->getInt("id") << "\n"
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
            auto* validateRes = db.query("SELECT id FROM servis WHERE id = " + to_string(id_servis_dipilih) + " AND status = 'Selesai' AND status_pembayaran = 'Belum Bayar'");
            if (!validateRes || !validateRes->next()) {
                cout << "❌ ID Servis tidak valid atau sudah dibayar/belum selesai.\n";
                if (validateRes) delete validateRes;
                pause(); // Pause sebelum kembali
                break;
            }
            delete validateRes;

            PembayaranService p;
            p.id_servis = id_servis_dipilih; // Gunakan ID yang sudah divalidasi
            p.inputData();  // input tanggal_pembayaran, jumlah, metode, status

            // Pastikan string yang dimasukkan ke SQL di-escape
            string metode_escaped = escape_sql_string(p.metode);
            string status_escaped = escape_sql_string(p.status);
            string jumlah_sql_format = format_double_for_sql(p.jumlah);

            string sqlInsert = "INSERT INTO pembayaran_servis (id_servis, tanggal_pembayaran, jumlah, metode, status) VALUES (" +
                to_string(p.id_servis) + ", '" +
                p.tanggal_pembayaran + "', " +
                jumlah_sql_format + ", '" + // Gunakan format_double_for_sql
                metode_escaped + "', '" +
                status_escaped + "')";

            try {
                db.execute(sqlInsert);
                cout << "✅ Pembayaran berhasil ditambahkan.\n";

                string sqlUpdate = "UPDATE servis SET status_pembayaran = '" + status_escaped + "' WHERE id = " + to_string(p.id_servis);
                db.execute(sqlUpdate);
                cout << "✅ Status servis berhasil diperbarui.\n";

            }
            catch (const exception& e) {
                cerr << "❌ Gagal memproses pembayaran: " << e.what() << endl;
            }
            pause(); // Pause setelah operasi selesai
            break;
        }

        case 2: {
            cout << "\n=== RIWAYAT PEMBAYARAN SERVIS ===\n";
            string sql = R"(
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
                cout << "❌ Belum ada data pembayaran yang tercatat.\n";
                if (res) delete res;
                pause(); // Pause sebelum kembali
                break;
            }

            while (res->next()) {
                cout << "ID Servis          : " << res->getInt("id_servis") << "\n"
                    << "Nama Pelanggan     : " << res->getString("nama_pelanggan") << "\n"
                    << "Plat Nomor         : " << res->getString("plat_nomor") << "\n"
                    << "Merk Kendaraan     : " << res->getString("merk") << "\n"
                    << "Keluhan            : " << res->getString("keluhan") << "\n"
                    << "Tanggal Servis     : " << res->getString("tanggal_servis") << "\n"
                    << "Tanggal Pembayaran : " << res->getString("tanggal_pembayaran") << "\n"
                    << "Jumlah             : Rp" << fixed << setprecision(2) << res->getDouble("jumlah") << "\n" // Format output harga
                    << "Metode             : " << res->getString("metode") << "\n"
                    << "Status Pembayaran  : " << res->getString("status") << "\n"
                    << "---------------------------------------------\n";
            }
            delete res;
            pause(); // Pause setelah menampilkan riwayat
            break;
        }

        case 3:
            cout << "↩️ Kembali ke menu utama...\n";
            // TIDAK PERLU pause() di sini, karena parent menu (main) akan menanganinya
            break;

        default:
            cout << "❌ Pilihan tidak valid!\n";
            pause(); // Pause setelah pilihan tidak valid
            break;
        }

    } while (pilih != 3); // Loop akan terus berjalan sampai pengguna memilih 3
}

void menuTransaksiSukuCadang(Database& db) {
    int pilih;
    do {
        clearScreen();
        cout << "\n=== MENU TRANSAKSI SUKU CADANG ===\n";
        cout << "1. Tambah Transaksi\n";
        cout << "2. Lihat Riwayat Transaksi\n";
        cout << "3. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");
        clearScreen();

        switch (pilih) {
        case 1: {
            // Tampilkan suku cadang
            auto* res = db.query("SELECT * FROM suku_cadang");
            if (!res || res->rowsCount() == 0) {
                cout << "❌ Tidak ada suku cadang tersedia.\n";
                if (res) delete res;
                pause();
                break;
            }

            cout << "\n=== DAFTAR SUKU CADANG ===\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id")
                    << " | Nama: " << res->getString("nama")
                    << " | Jenis: " << res->getString("jenis")
                    << " | Stok: " << res->getInt("stok")
                    << " | Harga: Rp" << res->getDouble("harga") << "\n";
            }
            delete res;

            int id_suku_cadang;
            cout << "\nMasukkan ID Suku Cadang yang ingin ditransaksikan: ";
            cin >> id_suku_cadang;

            TransaksiSukuCadang trx(id_suku_cadang);
            trx.inputData();

            // Simpan ke database
            string sql = "INSERT INTO transaksi_suku_cadang (id_suku_cadang, tanggal, jumlah, jenis) VALUES (" +
                to_string(trx.id_suku_cadang) + ", '" +
                trx.tanggal + "', " +
                to_string(trx.jumlah) + ", '" +
                trx.jenis + "')";

            db.execute(sql);
            cout << "✅ Transaksi berhasil ditambahkan.\n";
            pause();
            break;
        }

        case 2: {
            string sql = R"(
                SELECT t.id, s.nama AS suku_cadang, s.jenis, t.tanggal, t.jumlah, t.jenis AS jenis_transaksi
                FROM transaksi_suku_cadang t
                JOIN suku_cadang s ON t.id_suku_cadang = s.id
                ORDER BY t.tanggal DESC
            )";

            auto* res = db.query(sql);
            if (!res || res->rowsCount() == 0) {
                cout << "❌ Belum ada transaksi tercatat.\n";
                if (res) delete res;
                pause();
                break;
            }

            cout << "\n=== RIWAYAT TRANSAKSI SUKU CADANG ===\n";
            while (res->next()) {
                cout << "ID Transaksi   : " << res->getInt("id") << "\n"
                    << "Suku Cadang    : " << res->getString("suku_cadang") << " (" << res->getString("jenis") << ")\n"
                    << "Tanggal        : " << res->getString("tanggal") << "\n"
                    << "Jumlah         : " << res->getInt("jumlah") << "\n"
                    << "Jenis Transaksi: " << res->getString("jenis_transaksi") << "\n"
                    << "-----------------------------------------\n";
            }
            delete res;
            pause();
            break;
        }

        case 3:
            cout << "↩️ Kembali ke menu utama...\n";
            break;

        default:
            cout << "❌ Pilihan tidak valid.\n";
            pause();
            break;
        }
    } while (pilih != 3);
}



void menuLaporan(Database& db) {
    clearScreen();

    string folder = "data";
    if (!folderExists(folder)) {
        if (!createFolder(folder)) { // <--- GUNAKAN createFolder YANG BARU
            cerr << "❌ Gagal membuat folder 'data'. Pastikan izin sudah benar.\n";
            pause();
            return;
        }
    }

    string filePath = folder + "/data_servis.csv";
    ofstream file(filePath);

    if (!file.is_open()) {
        cerr << "❌ Gagal membuat file CSV: " << filePath << ". Pastikan folder ada dan izin sudah benar.\n";
        pause();
        return;
    }

    // Header CSV
    file << "ID Servis,ID Kendaraan,ID Teknisi,Keluhan,Tanggal Servis,Status Servis,"
        << "Tanggal Pembayaran,Jumlah,Metode Pembayaran,Status Pembayaran\n";

    string query = R"(
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
        cout << "❌ Tidak ada data pembayaran lunas untuk diekspor.\n";
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
            << fixed << setprecision(2) << res->getDouble("jumlah") << "," // Format double untuk CSV
            << escape_sql_string(res->getString("metode")) << ","
            << escape_sql_string(res->getString("status_pembayaran")) << "\n";
    }

    delete res;
    file.close();

    cout << "✅ Data servis dan pembayaran lunas berhasil diekspor ke: " << filePath << "\n";
    pause();
}