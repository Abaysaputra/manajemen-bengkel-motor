#pragma once
#include <string>
#include <iostream>

class User {
protected: 
    int id;
    std::string nama;

public:
    // Constructor
    User() : id(0) {}

    virtual ~User() = default;

    // Fungsi virtual yang akan di-override oleh kelas turunan
    virtual void inputData();
    virtual void tampilkanData();

    int getID() const;
    std::string getNama() const;
};