#include "User.h"

// Implementasi fungsi-fungsi dari User.h
void User::inputData() {
    std::cout << "Nama: ";
    std::getline(std::cin, nama);
}

void User::tampilkanData() {
    std::cout << "ID: " << id << ", Nama: " << nama;
}

int User::getID() const {
    return id;
}

std::string User::getNama() const {
    return nama;
}