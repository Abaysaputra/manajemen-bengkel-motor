#include "Database.h" 
#include "Interface.h" 
#include "Utils.h"      
#include "Pelanggan.h"
#include "Kendaraan.h"
#include "Servis.h"
#include "Teknisi.h"
#include "SukuCadang.h"
#include "TransaksiSukuCadang.h"
#include "PembayaranServis.h"
#include <iostream>
#include <fstream>
#include <vector> 
#include <algorithm> // Untuk std::sort
#include <memory>    // Untuk std::unique_ptr

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

            
            string nama_escaped = escape_sql_string(p.getNama());
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
        cout << "1. Tambah Servis Baru\n";
        cout << "2. Lihat Servis Aktif (Status: Diproses)\n";
        cout << "3. Update Status Servis Menjadi 'Selesai'\n";
        cout << "4. Lihat Riwayat Servis (Status: Selesai)\n";
        cout << "5. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();

        switch (pilih) {
        case 1: { // Alur Tambah Servis Baru
            cout << "--- Pendaftaran Servis Baru ---\n";

            // 1. Tampilkan dan Pilih Kendaraan
            auto* resKendaraan = db.query("SELECT k.id, k.plat_nomor, k.merk, p.nama FROM kendaraan k JOIN pelanggan p ON k.id_pelanggan = p.id ORDER BY k.id DESC");
            if (!resKendaraan || resKendaraan->rowsCount() == 0) {
                cout << "❌ Tidak ada data kendaraan. Tambahkan kendaraan terlebih dahulu.\n";
                if (resKendaraan) delete resKendaraan;
                pause();
                break;
            }
            cout << "\n=== Daftar Kendaraan Terdaftar ===\n";
            while (resKendaraan->next()) {
                cout << "ID: " << resKendaraan->getInt("id") << ", Plat: " << resKendaraan->getString("plat_nomor")
                    << ", Merk: " << resKendaraan->getString("merk") << ", Pemilik: " << resKendaraan->getString("nama") << "\n";
            }
            delete resKendaraan;
            int id_kendaraan_dipilih = getValidatedInt("\nMasukkan ID Kendaraan yang akan diservis: ");

            // Validasi Kendaraan
            auto* validateKendaraan = db.query("SELECT id FROM kendaraan WHERE id = " + to_string(id_kendaraan_dipilih));
            if (!validateKendaraan || !validateKendaraan->next()) {
                cout << "❌ ID Kendaraan tidak valid.\n";
                if (validateKendaraan) delete validateKendaraan;
                pause();
                break;
            }
            delete validateKendaraan;

            // 2. Tampilkan dan Pilih Teknisi (Versi Lebih Efisien)
            auto* resTeknisi = db.query("SELECT id, nama, keahlian FROM teknisi");
            if (!resTeknisi || resTeknisi->rowsCount() == 0) {
                cout << "❌ Tidak ada data teknisi. Tambahkan teknisi terlebih dahulu.\n";
                if (resTeknisi) delete resTeknisi;
                pause();
                break;
            }
            cout << "\n=== Daftar Teknisi Tersedia ===\n";
            while (resTeknisi->next()) {
                cout << "ID: " << resTeknisi->getInt("id") << ", Nama: " << resTeknisi->getString("nama")
                    << ", Keahlian: " << resTeknisi->getString("keahlian") << "\n";
            }
            delete resTeknisi;
            int id_teknisi_dipilih = getValidatedInt("\nMasukkan ID Teknisi yang menangani: ");

            // Validasi Teknisi
            auto* validateTeknisi = db.query("SELECT id FROM teknisi WHERE id = " + to_string(id_teknisi_dipilih));
            if (!validateTeknisi || !validateTeknisi->next()) {
                cout << "❌ ID Teknisi tidak valid.\n";
                if (validateTeknisi) delete validateTeknisi;
                pause();
                break;
            }
            delete validateTeknisi;

            // 3. Input Keluhan dan Ongkos
            cout << "Masukkan keluhan servis: ";
            string keluhan;
            getline(cin, keluhan);
            cout << "Masukkan ongkos jasa/kerja (misal: 50000): ";
            double ongkos_servis;
            cin >> ongkos_servis;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            // 4. Insert data awal servis ke DB untuk mendapatkan ID Servis
            string sqlInsertServis = "INSERT INTO servis (id_kendaraan, id_teknisi, keluhan, tanggal, status, status_pembayaran, ongkos) VALUES ("
                + to_string(id_kendaraan_dipilih) + ", " + to_string(id_teknisi_dipilih) + ", '"
                + escape_sql_string(keluhan) + "', CURDATE(), 'Diproses', 'Belum Bayar', " + to_string(ongkos_servis) + ")";

            int id_servis_baru = 0;
            try {
                db.execute(sqlInsertServis);
                auto* resId = db.query("SELECT LAST_INSERT_ID() AS id");
                if (resId && resId->next()) id_servis_baru = resId->getInt("id");
                delete resId;
                if (id_servis_baru == 0) throw runtime_error("Gagal mendapatkan ID servis baru dari database.");

                cout << "\n✅ Servis baru berhasil didaftarkan dengan ID: " << id_servis_baru << "\n";

            }
            catch (const exception& e) {
                cerr << "❌ Gagal mendaftarkan servis: " << e.what() << endl;
                pause();
                break;
            }

            // 5. Loop untuk Menambahkan Suku Cadang
            char pakaiCadang;
            do {
                cout << "\nApakah ingin menambahkan suku cadang untuk servis ini? (y/n): ";
                cin >> pakaiCadang;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (pakaiCadang == 'y' || pakaiCadang == 'Y') {
                    auto* resCadang = db.query("SELECT id, nama, stok, harga FROM suku_cadang WHERE stok > 0");
                    if (!resCadang || resCadang->rowsCount() == 0) {
                        cout << "❌ Tidak ada suku cadang yang tersedia.\n";
                        if (resCadang) delete resCadang;
                        break;
                    }
                    cout << "\n--- Daftar Suku Cadang Tersedia ---\n";
                    while (resCadang->next()) {
                        cout << "ID: " << resCadang->getInt("id") << ", Nama: " << resCadang->getString("nama")
                            << ", Stok: " << resCadang->getInt("stok") << ", Harga: Rp" << fixed << setprecision(2) << resCadang->getDouble("harga") << "\n";
                    }
                    delete resCadang;

                    int id_sc_dipilih = getValidatedInt("\nMasukkan ID Suku Cadang: ");
                    int jumlah_pakai = getValidatedInt("Masukkan jumlah yang dipakai: ");

                    auto* validateSC = db.query("SELECT nama, stok, harga FROM suku_cadang WHERE id = " + to_string(id_sc_dipilih));
                    if (!validateSC || !validateSC->next()) {
                        cout << "❌ ID Suku Cadang tidak valid.\n";
                        if (validateSC) delete validateSC;
                        continue;
                    }

                    string nama_sc = validateSC->getString("nama");
                    int stok_tersedia = validateSC->getInt("stok");
                    double harga_sc = validateSC->getDouble("harga");
                    delete validateSC;

                    if (jumlah_pakai <= 0 || jumlah_pakai > stok_tersedia) {
                        cout << "❌ Jumlah tidak valid atau stok tidak mencukupi (Stok: " << stok_tersedia << ").\n";
                        continue;
                    }

                    try {
                        string sqlDetail = "INSERT INTO servis_detail (id_servis, id_suku_cadang, jumlah, harga_saat_transaksi) VALUES ("
                            + to_string(id_servis_baru) + ", " + to_string(id_sc_dipilih) + ", " + to_string(jumlah_pakai) + ", " + to_string(harga_sc) + ")";
                        db.execute(sqlDetail);

                        string sqlUpdateStok = "UPDATE suku_cadang SET stok = stok - " + to_string(jumlah_pakai) + " WHERE id = " + to_string(id_sc_dipilih);
                        db.execute(sqlUpdateStok);

                        cout << "✅ Suku cadang '" << nama_sc << "' berhasil ditambahkan ke servis.\n";
                    }
                    catch (const exception& e) {
                        cerr << "❌ Gagal menambahkan suku cadang: " << e.what() << endl;
                    }
                }
            } while (pakaiCadang == 'y' || pakaiCadang == 'Y');

            cout << "\n✅ Proses pendaftaran servis selesai.\n";
            pause();
            break;
        }
        case 2: { // Lihat Servis Aktif (Diproses)
            cout << "\n=== Daftar Servis Aktif (Status: Diproses) ===\n";
            string query = R"(
                SELECT s.id, s.tanggal, k.plat_nomor, p.nama AS nama_pelanggan, t.nama AS nama_teknisi, s.keluhan
                FROM servis s
                JOIN kendaraan k ON s.id_kendaraan = k.id
                JOIN pelanggan p ON k.id_pelanggan = p.id
                JOIN teknisi t ON s.id_teknisi = t.id
                WHERE s.status = 'Diproses'
                ORDER BY s.tanggal, s.id
            )";
            auto* res = db.query(query);
            if (!res || res->rowsCount() == 0) {
                cout << "Tidak ada servis yang sedang aktif diproses.\n";
            }
            else {
                while (res->next()) {
                    cout << "-------------------------------------------\n";
                    cout << "ID Servis      : " << res->getInt("id") << "\n";
                    cout << "Tanggal Masuk  : " << res->getString("tanggal") << "\n";
                    cout << "Pelanggan      : " << res->getString("nama_pelanggan") << "\n";
                    cout << "Kendaraan      : " << res->getString("plat_nomor") << "\n";
                    cout << "Teknisi        : " << res->getString("nama_teknisi") << "\n";
                    cout << "Keluhan        : " << res->getString("keluhan") << "\n";
                }
                cout << "-------------------------------------------\n";
            }
            if (res) delete res;
            pause();
            break;
        }
        case 3: { // Update Status Servis Menjadi 'Selesai'
            cout << "\n=== Update Status Servis Menjadi 'Selesai' ===\n";
            string query = R"(
                SELECT s.id, s.tanggal, k.plat_nomor, p.nama AS nama_pelanggan
                FROM servis s
                JOIN kendaraan k ON s.id_kendaraan = k.id
                JOIN pelanggan p ON k.id_pelanggan = p.id
                WHERE s.status = 'Diproses'
            )";
            auto* res = db.query(query);
            if (!res || res->rowsCount() == 0) {
                cout << "Tidak ada servis yang bisa di-update (semua sudah selesai atau belum ada servis).\n";
                if (res) delete res;
                pause();
                break;
            }
            cout << "Daftar servis yang masih 'Diproses':\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id") << " | Tgl: " << res->getString("tanggal")
                    << " | Plat: " << res->getString("plat_nomor") << " | Pelanggan: " << res->getString("nama_pelanggan") << "\n";
            }
            delete res;

            int id_to_update = getValidatedInt("\nMasukkan ID Servis yang ingin diubah statusnya menjadi 'Selesai': ");
            try {
                // Validasi dulu ID nya
                auto* validate = db.query("SELECT id FROM servis WHERE id = " + to_string(id_to_update) + " AND status = 'Diproses'");
                if (!validate || !validate->next()) {
                    cout << "❌ ID Servis tidak valid atau statusnya bukan 'Diproses'.\n";
                    if (validate) delete validate;
                    pause();
                    break;
                }
                delete validate;

                db.execute("UPDATE servis SET status = 'Selesai' WHERE id = " + to_string(id_to_update));
                cout << "✅ Status untuk Servis ID " << id_to_update << " berhasil diubah menjadi 'Selesai'.\n";
            }
            catch (const exception& e) {
                cerr << "❌ Gagal mengupdate status: " << e.what() << endl;
            }
            pause();
            break;
        }
        case 4: { // Lihat Riwayat Servis Selesai
            cout << "\n=== Riwayat Servis (Status: Selesai) ===\n";
            string query = R"(
                SELECT s.id, s.tanggal, k.plat_nomor, p.nama AS nama_pelanggan, s.status, s.status_pembayaran, s.ongkos
                FROM servis s
                JOIN kendaraan k ON s.id_kendaraan = k.id
                JOIN pelanggan p ON k.id_pelanggan = p.id
                WHERE s.status = 'Selesai'
                ORDER BY s.tanggal DESC, s.id DESC
            )";
            auto* res = db.query(query);
            if (!res || res->rowsCount() == 0) {
                cout << "Belum ada riwayat servis yang selesai.\n";
            }
            else {
                while (res->next()) {
                    cout << "-------------------------------------------\n";
                    cout << "ID Servis        : " << res->getInt("id") << "\n";
                    cout << "Tanggal Selesai  : " << res->getString("tanggal") << "\n"; // Sebenarnya ini tanggal masuk, tapi ok
                    cout << "Pelanggan        : " << res->getString("nama_pelanggan") << "\n";
                    cout << "Kendaraan        : " << res->getString("plat_nomor") << "\n";
                    cout << "Ongkos Jasa      : Rp" << fixed << setprecision(2) << res->getDouble("ongkos") << "\n";
                    cout << "Status Pembayaran: " << res->getString("status_pembayaran") << "\n";
                }
                cout << "-------------------------------------------\n";
            }
            if (res) delete res;
            pause();
            break;
        }
        case 5:
            cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
            pause();
        }

    } while (pilih != 5);
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
        cout << "2. Lihat Semua Teknisi\n";
        cout << "3. Hapus Teknisi\n";
        cout << "4. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            Teknisi t;
            t.inputData();

            // PERUBAHAN: Menggunakan t.getNama() karena 'nama' sekarang protected.
            string nama_escaped = escape_sql_string(t.getNama());
            string keahlian_escaped = escape_sql_string(t.keahlian);

            string sql = "INSERT INTO teknisi (nama, keahlian) VALUES ('" +
                nama_escaped + "', '" + keahlian_escaped + "')";
            try {
                db.execute(sql);
                cout << "✅ Teknisi berhasil ditambahkan.\n";
            }
            catch (const exception& e) {
                cerr << "❌ Gagal menambahkan teknisi: " << e.what() << endl;
            }
            pause();
            break;
        }
        case 2: {
            auto* res = db.query("SELECT * FROM teknisi");
            if (!res || res->rowsCount() == 0) {
                cout << "Belum ada data teknisi yang tercatat.\n";
            }
            else {
                cout << "\n=== Daftar Teknisi ===\n";
                while (res->next()) {
                    cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama")
                        << ", Keahlian: " << res->getString("keahlian") << "\n";
                }
            }
            if (res) delete res;
            pause();
            break;
        }
        case 3: {
            // Logika Hapus Teknisi. Disarankan menggunakan ON DELETE SET NULL di database.
            cout << "Fitur hapus teknisi sebaiknya ditangani oleh 'ON DELETE SET NULL' di database.\n";
            cout << "Jika belum di-set, referensi teknisi pada tabel servis akan diatur ke NULL secara manual oleh kode ini.\n\n";

            auto* res = db.query("SELECT id, nama FROM teknisi");
            if (!res || res->rowsCount() == 0) {
                cerr << "❌ Tidak ada data teknisi untuk dihapus.\n";
                if (res) delete res;
                pause();
                break;
            }

            cout << "\n=== Daftar Teknisi ===\n";
            while (res->next()) {
                cout << "ID: " << res->getInt("id") << ", Nama: " << res->getString("nama") << "\n";
            }
            delete res;

            int idHapus = getValidatedInt("\nMasukkan ID teknisi yang ingin dihapus: ");

            cout << "⚠️ Yakin ingin menghapus teknisi ID " << idHapus << "? (y/n): ";
            char confirm;
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                try {
                    // Atur servis yang terkait ke NULL
                    db.execute("UPDATE servis SET id_teknisi = NULL WHERE id_teknisi = " + to_string(idHapus));
                    // Hapus teknisi
                    db.execute("DELETE FROM teknisi WHERE id = " + to_string(idHapus));
                    cout << "✅ Teknisi berhasil dihapus dan referensi di servis telah di-update.\n";
                }
                catch (const exception& e) {
                    cerr << "❌ Gagal menghapus teknisi: " << e.what() << endl;
                }
            }
            else {
                cout << "Penghapusan dibatalkan.\n";
            }
            pause();
            break;
        }
        case 4:
            cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
            pause();
            break;
        }
    } while (pilih != 4);
}


void menuPembayaranServis(Database& db) {
    int pilih;
    do {
        clearScreen();
        cout << "\n--- MENU PEMBAYARAN SERVIS ---\n";
        cout << "1. Proses Pembayaran Servis\n";
        cout << "2. Lihat Riwayat Pembayaran\n";
        cout << "3. Kembali\n";
        pilih = getValidatedInt("Pilihan: ");

        clearScreen();
        switch (pilih) {
        case 1: {
            // 1. Tampilkan daftar servis yang siap dibayar
            auto* res = db.query(R"(
                SELECT s.id, s.tanggal, k.plat_nomor, p.nama
                FROM servis s
                JOIN kendaraan k ON s.id_kendaraan = k.id
                JOIN pelanggan p ON k.id_pelanggan = p.id
                WHERE s.status = 'Selesai' AND s.status_pembayaran = 'Belum Bayar'
                ORDER BY s.id
            )");

            if (!res || !res->rowsCount()) {
                cout << "[INFO] Tidak ada servis yang siap untuk dibayar saat ini.\n";
                cout << "       Pastikan status servis sudah diubah menjadi 'Selesai' di Menu Servis.\n";
                if (res) delete res;
                pause();
                break;
            }

            cout << "\n=== Daftar Servis Siap Bayar ===\n";
            while (res->next()) {
                cout << "ID Servis: " << res->getInt("id")
                    << " | Tgl: " << res->getString("tanggal")
                    << " | Plat: " << res->getString("plat_nomor")
                    << " | Pelanggan: " << res->getString("nama") << "\n";
            }
            delete res;

            int id_servis_dipilih = getValidatedInt("\nMasukkan ID Servis yang ingin dibayar: ");

            // 2. Validasi dan Tampilkan Rincian Tagihan
            auto* resTagihan = db.query("SELECT ongkos FROM servis WHERE id = " + to_string(id_servis_dipilih) + " AND status = 'Selesai' AND status_pembayaran = 'Belum Bayar'");
            if (!resTagihan || !resTagihan->next()) {
                cout << "[ERROR] ID Servis tidak valid atau tidak siap dibayar.\n";
                if (resTagihan) delete resTagihan;
                pause();
                break;
            }
            double total_tagihan = resTagihan->getDouble("ongkos");
            delete resTagihan;

            cout << "\n--- RINCIAN TAGIHAN UNTUK SERVIS ID: " << id_servis_dipilih << " ---\n";
            cout << "Ongkos Jasa/Kerja: Rp" << fixed << setprecision(2) << total_tagihan << "\n";

            // PERBAIKAN: Menghapus ')' yang salah di dalam string query
            string detailQuery = "SELECT sd.jumlah, sd.harga_saat_transaksi, sc.nama "
                "FROM servis_detail sd "
                "JOIN suku_cadang sc ON sd.id_suku_cadang = sc.id "
                "WHERE sd.id_servis = " + to_string(id_servis_dipilih);
            auto* resDetail = db.query(detailQuery);

            if (resDetail && resDetail->rowsCount() > 0) {
                cout << "Suku Cadang Digunakan:\n";
                while (resDetail->next()) {
                    double sub_total = resDetail->getInt("jumlah") * resDetail->getDouble("harga_saat_transaksi");
                    cout << " - " << resDetail->getString("nama")
                        << " (" << resDetail->getInt("jumlah") << " x Rp" << fixed << setprecision(2) << resDetail->getDouble("harga_saat_transaksi") << ") = Rp"
                        << fixed << setprecision(2) << sub_total << "\n";
                    total_tagihan += sub_total;
                }
            }
            if (resDetail) delete resDetail;

            cout << "------------------------------------------\n";
            cout << "TOTAL TAGIHAN: Rp" << fixed << setprecision(2) << total_tagihan << "\n";
            cout << "------------------------------------------\n";

            // 3. Proses Pembayaran
            cout << "Masukkan metode pembayaran (Cash/Transfer): ";
            string metode;
            getline(cin, metode);

            try {
                db.execute("START TRANSACTION");

                string sqlInsert = "INSERT INTO pembayaran_servis (id_servis, tanggal_pembayaran, jumlah, metode, status) VALUES ("
                    + to_string(id_servis_dipilih) + ", CURDATE(), " + to_string(total_tagihan) + ", '"
                    + escape_sql_string(metode) + "', 'Lunas')";
                db.execute(sqlInsert);

                string sqlUpdate = "UPDATE servis SET status_pembayaran = 'Lunas' WHERE id = " + to_string(id_servis_dipilih);
                db.execute(sqlUpdate);

                db.execute("UPDATE kas_bengkel SET saldo_sekarang = saldo_sekarang + " + to_string(total_tagihan) + " WHERE id = 1");

                db.execute("COMMIT");
                cout << "\n[OK] Pembayaran berhasil diproses! Saldo kas telah diperbarui.\n";

            }
            catch (const exception& e) {
                db.execute("ROLLBACK");
                cerr << "[ERROR] Gagal memproses pembayaran: " << e.what() << endl;
            }

            pause();
            break;
        }
        case 2: { // Lihat Riwayat Pembayaran
            cout << "\n=== RIWAYAT PEMBAYARAN SERVIS ===\n";
            string sql = R"(
        SELECT ps.id, ps.id_servis, ps.tanggal_pembayaran, ps.jumlah, ps.metode, k.plat_nomor, p.nama AS nama_pelanggan
        FROM pembayaran_servis ps
        JOIN servis s ON ps.id_servis = s.id
        JOIN kendaraan k ON s.id_kendaraan = k.id
        JOIN pelanggan p ON k.id_pelanggan = p.id
        ORDER BY ps.tanggal_pembayaran DESC, ps.id DESC
    )";

            auto* res = db.query(sql);
            if (!res || res->rowsCount() == 0) {
                cout << "❌ Belum ada data pembayaran yang tercatat.\n";
            }
            else {
                while (res->next()) {
                    cout << "---------------------------------------------\n";
                    cout << "Tgl Bayar        : " << res->getString("tanggal_pembayaran") << "\n";
                    cout << "ID Servis        : " << res->getInt("id_servis") << "\n";
                    cout << "Pelanggan        : " << res->getString("nama_pelanggan") << "\n";
                    cout << "Plat Nomor       : " << res->getString("plat_nomor") << "\n";
                    cout << "Total Pembayaran : Rp" << fixed << setprecision(2) << res->getDouble("jumlah") << "\n";
                    cout << "Metode           : " << res->getString("metode") << "\n";
                }
                cout << "---------------------------------------------\n";
            }
            if (res) delete res;
            pause();
            break;
        }
        case 3:
            cout << "↩️ Kembali ke menu utama...\n";
            break;
        default:
            cout << "❌ Pilihan tidak valid!\n";
            pause();
        }
    } while (pilih != 3);
}

// GANTI FUNGSI LAMA DENGAN INI SECARA KESELURUHAN

void menuTransaksiSukuCadang(Database& db) {
    clearScreen();
    cout << "\n=== MENU TRANSAKSI SUKU CADANG ===\n";
    cout << "Menu ini untuk mencatat pembelian (stok masuk) atau penjualan langsung (stok keluar).\n";

    // Tampilkan daftar suku cadang
    auto* res = db.query("SELECT id, nama, stok, harga FROM suku_cadang");
    if (!res || res->rowsCount() == 0) {
        cout << "❌ Tidak ada suku cadang tersedia.\n";
        if (res) delete res;
        pause();
        return; // Kembali jika tidak ada suku cadang
    }

    cout << "\n--- DAFTAR SUKU CADANG ---\n";
    while (res->next()) {
        cout << "ID: " << res->getInt("id") << " | Nama: " << res->getString("nama")
            << " | Stok: " << res->getInt("stok") << " | Harga Jual: Rp" << fixed << setprecision(2) << res->getDouble("harga") << "\n";
    }
    delete res;

    // Mulai proses transaksi
    int id_suku_cadang = getValidatedInt("\nMasukkan ID Suku Cadang yang ingin ditransaksikan: ");

    // Validasi ID Suku Cadang
    auto* validateSC = db.query("SELECT nama, stok, harga FROM suku_cadang WHERE id = " + to_string(id_suku_cadang));
    if (!validateSC || !validateSC->next()) {
        cout << "❌ ID Suku Cadang tidak valid.\n";
        if (validateSC) delete validateSC;
        pause();
        return;
    }
    string nama_sc = validateSC->getString("nama");
    int stok_sekarang = validateSC->getInt("stok");
    double harga_jual_sc = validateSC->getDouble("harga");
    delete validateSC;

    cout << "\nJenis Transaksi [1: Pembelian (Menambah Stok) / 2: Penjualan (Mengurangi Stok)]: ";
    int jenis_pilihan = getValidatedInt("");

    if (jenis_pilihan == 1) { // PROSES PEMBELIAN
        cout << "\n--- Proses Pembelian Suku Cadang: " << nama_sc << " ---\n";
        int jumlah_beli = getValidatedInt("Jumlah yang dibeli: ");
        cout << "Harga beli per item: Rp";
        double harga_beli_per_item;
        cin >> harga_beli_per_item;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        double total_pembelian = jumlah_beli * harga_beli_per_item;
        cout << "Total Pembelian: Rp" << fixed << setprecision(2) << total_pembelian << "\n";

        // Cek saldo kas
        auto* resKas = db.query("SELECT saldo_sekarang FROM kas_bengkel WHERE id = 1");
        if (!resKas || !resKas->next() || resKas->getDouble("saldo_sekarang") < total_pembelian) {
            cout << "❌ Saldo kas tidak mencukupi untuk melakukan pembelian ini.\n";
            if (resKas) delete resKas;
            pause();
            return;
        }
        delete resKas;

        // Mulai Transaksi Database yang Aman
        try {
            db.execute("START TRANSACTION");
            // 1. Kurangi saldo kas
            db.execute("UPDATE kas_bengkel SET saldo_sekarang = saldo_sekarang - " + to_string(total_pembelian) + " WHERE id = 1");
            // 2. Tambah stok suku cadang
            db.execute("UPDATE suku_cadang SET stok = stok + " + to_string(jumlah_beli) + " WHERE id = " + to_string(id_suku_cadang));
            // 3. Catat transaksi pembelian
            string sqlTrx = "INSERT INTO transaksi_suku_cadang (id_suku_cadang, tanggal, jumlah, total_harga, jenis) VALUES ("
                + to_string(id_suku_cadang) + ", CURDATE(), " + to_string(jumlah_beli) + ", " + to_string(total_pembelian) + ", 'Pembelian')";
            db.execute(sqlTrx);

            db.execute("COMMIT"); // Jika semua berhasil, simpan perubahan
            cout << "\n✅ Pembelian berhasil dicatat. Stok dan saldo kas telah diperbarui.\n";
        }
        catch (const exception& e) {
            db.execute("ROLLBACK"); // Jika ada 1 saja yang gagal, batalkan semua perubahan
            cerr << "\n❌ Terjadi kesalahan! Transaksi dibatalkan. Pesan error: " << e.what() << endl;
        }

    }
    else if (jenis_pilihan == 2) { // PROSES PENJUALAN
        cout << "\n--- Proses Penjualan Suku Cadang: " << nama_sc << " ---\n";
        int jumlah_jual = getValidatedInt("Jumlah yang dijual: ");

        if (jumlah_jual > stok_sekarang) {
            cout << "❌ Stok tidak mencukupi. Sisa stok: " << stok_sekarang << "\n";
            pause();
            return;
        }
        double total_penjualan = jumlah_jual * harga_jual_sc;
        cout << "Total Penjualan: Rp" << fixed << setprecision(2) << total_penjualan << "\n";

        // Mulai Transaksi Database yang Aman
        try {
            db.execute("START TRANSACTION");
            // 1. Tambah saldo kas
            db.execute("UPDATE kas_bengkel SET saldo_sekarang = saldo_sekarang + " + to_string(total_penjualan) + " WHERE id = 1");
            // 2. Kurangi stok suku cadang
            db.execute("UPDATE suku_cadang SET stok = stok - " + to_string(jumlah_jual) + " WHERE id = " + to_string(id_suku_cadang));
            // 3. Catat transaksi penjualan
            string sqlTrx = "INSERT INTO transaksi_suku_cadang (id_suku_cadang, tanggal, jumlah, total_harga, jenis) VALUES ("
                + to_string(id_suku_cadang) + ", CURDATE(), " + to_string(jumlah_jual) + ", " + to_string(total_penjualan) + ", 'Penjualan')";
            db.execute(sqlTrx);

            db.execute("COMMIT");
            cout << "\n✅ Penjualan berhasil dicatat. Stok dan saldo kas telah diperbarui.\n";
        }
        catch (const exception& e) {
            db.execute("ROLLBACK");
            cerr << "\n❌ Terjadi kesalahan! Transaksi dibatalkan. Pesan error: " << e.what() << endl;
        }

    }
    else {
        cout << "❌ Pilihan tidak valid.\n";
    }
    pause();
}
bool bandingkanTransaksi(const unique_ptr<Transaksi>& a, const unique_ptr<Transaksi>& b) {
    return a->getTanggal() > b->getTanggal(); // Urutkan dari terbaru ke terlama
}

void menuRiwayatTransaksiGabungan(Database& db) {
    clearScreen();
    cout << "\n--- RIWAYAT SEMUA TRANSAKSI BENGKEL ---\n";

    vector<unique_ptr<Transaksi>> semua_transaksi;

    try {
        // 1. Ambil data dari pembayaran_servis
        auto* resBayar = db.query("SELECT ps.id, ps.tanggal_pembayaran, ps.jumlah, ps.metode, p.nama FROM pembayaran_servis ps JOIN servis s ON ps.id_servis = s.id JOIN kendaraan k ON s.id_kendaraan = k.id JOIN pelanggan p ON k.id_pelanggan = p.id");
        if (resBayar) {
            while (resBayar->next()) {
                string desk = "Pembayaran servis oleh " + resBayar->getString("nama");
                semua_transaksi.push_back(make_unique<PembayaranServis>(
                    resBayar->getInt("id"),
                    0,
                    resBayar->getString("tanggal_pembayaran"),
                    resBayar->getDouble("jumlah"),
                    resBayar->getString("metode"),
                    desk
                ));
            }
            delete resBayar;
        }

        // 2. Ambil data dari transaksi_suku_cadang
        auto* resSC = db.query("SELECT t.id, t.tanggal, t.jumlah, t.total_harga, t.jenis, s.nama FROM transaksi_suku_cadang t JOIN suku_cadang s ON t.id_suku_cadang = s.id");
        if (resSC) {
            while (resSC->next()) {
                string desk = resSC->getString("jenis") + " " + to_string(resSC->getInt("jumlah")) + "x " + resSC->getString("nama");
                semua_transaksi.push_back(make_unique<TransaksiSukuCadang>(
                    resSC->getInt("id"),
                    0,
                    resSC->getString("tanggal"),
                    resSC->getInt("jumlah"),
                    resSC->getDouble("total_harga"),
                    resSC->getString("jenis"),
                    desk
                ));
            }
            delete resSC;
        }

        sort(semua_transaksi.begin(), semua_transaksi.end(), bandingkanTransaksi);

        if (semua_transaksi.empty()) {
            cout << "\nBelum ada transaksi yang tercatat.\n";
        }
        else {
            cout << setprecision(2) << fixed;
            for (const auto& trx : semua_transaksi) {
                cout << "--------------------------------------------------\n";
                cout << "Tanggal   : " << trx->getTanggal() << "\n";
                cout << "Tipe      : " << trx->getTipeTransaksi() << "\n";
                cout << "Deskripsi : " << trx->getDeskripsi() << "\n";
                // Tampilkan nilai sebagai positif atau negatif
                if (trx->getTotalNilai() >= 0) {
                    cout << "Nilai     : +Rp" << trx->getTotalNilai() << "\n";
                }
                else {
                    cout << "Nilai     : -Rp" << -trx->getTotalNilai() << "\n";
                }
            }
            cout << "--------------------------------------------------\n";
        }

    }
    catch (const exception& e) {
        cerr << "[ERROR] Gagal mengambil riwayat transaksi: " << e.what() << endl;
    }

    // pause() akan dipanggil dari main.cpp, jadi tidak perlu di sini
}

void menuLaporanKeuangan(Database& db) {
    clearScreen();
    cout << "\n--- LAPORAN KEUANGAN BENGKEL ---\n";

    try {
        // 1. Ambil Saldo Kas Terkini
        auto* resKas = db.query("SELECT saldo_sekarang, terakhir_update FROM kas_bengkel WHERE id = 1");
        double saldo_kas = 0;
        string terakhir_update = "N/A";
        if (resKas && resKas->next()) {
            saldo_kas = resKas->getDouble("saldo_sekarang");
            terakhir_update = resKas->getString("terakhir_update");
        }
        delete resKas;

        // 2. Hitung Total Pendapatan dari Servis
        auto* resServis = db.query("SELECT SUM(jumlah) AS total FROM pembayaran_servis WHERE status = 'Lunas'");
        double pendapatan_servis = 0;
        if (resServis && resServis->next()) {
            pendapatan_servis = resServis->getDouble("total");
        }
        delete resServis;

        // 3. Hitung Total Pendapatan dari Penjualan Suku Cadang Langsung
        auto* resJualSC = db.query("SELECT SUM(total_harga) AS total FROM transaksi_suku_cadang WHERE jenis = 'Penjualan'");
        double pendapatan_jual_sc = 0;
        if (resJualSC && resJualSC->next()) {
            pendapatan_jual_sc = resJualSC->getDouble("total");
        }
        delete resJualSC;

        // 4. Hitung Total Pengeluaran dari Pembelian Suku Cadang
        auto* resBeliSC = db.query("SELECT SUM(total_harga) AS total FROM transaksi_suku_cadang WHERE jenis = 'Pembelian'");
        double pengeluaran_beli_sc = 0;
        if (resBeliSC && resBeliSC->next()) {
            pengeluaran_beli_sc = resBeliSC->getDouble("total");
        }
        delete resBeliSC;

        // Tampilkan Laporan
        cout << setprecision(2) << fixed;
        cout << "\nSALDO KAS BENGKEL SAAT INI\n";
        cout << "Rp" << saldo_kas << "  (Terakhir Update: " << terakhir_update << ")\n";
        cout << "========================================\n\n";

        cout << "PENDAPATAN:\n";
        cout << "  - Dari Jasa Servis & Suku Cadang: Rp" << pendapatan_servis << "\n";
        cout << "  - Dari Penjualan Suku Cadang    : Rp" << pendapatan_jual_sc << "\n";
        cout << "  -------------------------------------- +\n";
        cout << "  TOTAL PENDAPATAN                : Rp" << (pendapatan_servis + pendapatan_jual_sc) << "\n\n";

        cout << "PENGELUARAN:\n";
        cout << "  - Untuk Pembelian Suku Cadang   : Rp" << pengeluaran_beli_sc << "\n";
        cout << "  -------------------------------------- -\n";
        cout << "  TOTAL PENGELUARAN               : Rp" << pengeluaran_beli_sc << "\n\n";

    }
    catch (const exception& e) {
        cerr << "[ERROR] Gagal mengambil data laporan: " << e.what() << endl;
    }

   
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