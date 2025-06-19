#pragma once
#include <string>
#include <iostream>

class User {
protected: // Data disembunyikan dari akses luar, hanya bisa diakses oleh kelas ini dan turunannya
    int id;
    std::string nama;

public:
    // Constructor
    User() : id(0) {}

    virtual ~User() = default;

    // Fungsi virtual yang akan di-override oleh kelas turunan
    virtual void inputData();
    virtual void tampilkanData();

    // --- Public Getters ---
    // Menyediakan akses 'read-only' yang aman ke data protected
    int getID() const;
    std::string getNama() const;
};