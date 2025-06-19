#pragma once
#include "User.h" // Sertakan base class
#include <string>

// Pelanggan sekarang adalah turunan dari User
class Pelanggan : public User {
public:
    // id dan nama sudah diwarisi (protected) dari User.
    // Kita hanya perlu deklarasikan anggota yang spesifik untuk Pelanggan.
    std::string no_hp;
    std::string alamat;

    // Override fungsi virtual dari User
    void inputData() override;
    void tampilkanData() override;
};