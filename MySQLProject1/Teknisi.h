#pragma once
#include "User.h" // Sertakan base class
#include <string>

// Teknisi sekarang adalah turunan dari User
class Teknisi : public User {
public:
    // id dan nama sudah diwarisi (protected) dari User.
    std::string keahlian;

    // Override fungsi virtual dari User
    void inputData() override;
    void tampilkanData() override;
};