#ifndef SERVIS_DETAIL_H
#define SERVIS_DETAIL_H

#include <iostream>

using namespace std;
class ServisDetail {
public:
    int id_suku_cadang;
    int jumlah;

    void inputData() {
        cout << "ID Suku Cadang yang digunakan: ";
        cin >> id_suku_cadang;
        cout << "Jumlah yang digunakan: ";
        cin >> jumlah;
        cin.ignore();
    }            
};

#endif
