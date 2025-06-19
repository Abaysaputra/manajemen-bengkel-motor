#ifndef SERVIS_DETAIL_H
#define SERVIS_DETAIL_H

#include <iostream>

class ServisDetail {
public:
    int id_suku_cadang;
    int jumlah;

    void inputData() {
        std::cout << "ID Suku Cadang yang digunakan: ";
        std::cin >> id_suku_cadang;
        std::cout << "Jumlah yang digunakan: ";
        std::cin >> jumlah;
        std::cin.ignore();
    }            
};

#endif
