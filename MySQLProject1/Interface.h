#pragma once
#ifndef INTERFACE_H
#define INTERFACE_H

#include "Database.h" // Perlu tahu kelas Database
#include <string>     // Untuk std::string

// Deklarasi fungsi-fungsi menu
void menuPelanggan(Database& db);
void menuKendaraan(Database& db);
void menuServis(Database& db);
void menuTeknisi(Database& db);
void menuSukuCadang(Database& db);
void menuPembayaranServis(Database& db);
void menuLaporan(Database& db);

#endif // INTERFACE_H