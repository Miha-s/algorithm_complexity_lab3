#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <chrono>

using namespace std;

constexpr uint8_t generator = 0b0110011011;

// Reflect function for 8-bit data
uint8_t Reflect8(uint8_t data)
{
    uint8_t reflection = 0x00;
    for (int i = 0; i < 8; i++)
    {
        if (data & (1 << i))
            reflection |= (1 << (7 - i));
    }
    return reflection;
}

// Algorithm 1: Simple Sequential Algorithm
uint8_t Compute_CRC8_Simple(const vector<uint8_t> &data)
{
    uint8_t crc = 0;

    for (auto currByte : data)
    {
        crc ^= currByte;
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ generator;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// Algorithm 2: Table-based Algorithm
vector<uint8_t> GenerateCRCTable(uint8_t generator)
{
    vector<uint8_t> table(256);
    for (int dividend = 0; dividend < 256; dividend++)
    {
        uint8_t currByte = static_cast<uint8_t>(dividend);
        for (int bit = 0; bit < 8; bit++)
        {
            if (currByte & 0x80)
                currByte = (currByte << 1) ^ generator;
            else
                currByte <<= 1;
        }
        table[dividend] = currByte;
    }
    return table;
}

uint8_t Compute_CRC8_Table(const vector<uint8_t> &data, const vector<uint8_t> &table)
{
    uint8_t crc = 0;
    for (auto b : data)
    {
        uint8_t dataByte = crc ^ b;
        crc = table[dataByte];
    }
    return crc;
}

// Algorithm 3: Reflected Sequential Algorithm
uint8_t Compute_CRC8_Simple_Reflected(const vector<uint8_t> &data)
{
    const uint8_t reflected_generator = Reflect8(generator); 
    uint8_t crc = 0;

    for (auto currByte : data)
    {
        uint8_t dataByte = Reflect8(currByte);
        crc ^= dataByte;
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x01)
                crc = (crc >> 1) ^ reflected_generator;
            else
                crc >>= 1;
        }
    }
    crc = Reflect8(crc);
    return crc;
}

// Algorithm 4: Reflected Table-based Algorithm
vector<uint8_t> GenerateCRCTable_Reflected(uint8_t generator)
{
    vector<uint8_t> table(256);
    for (int dividend = 0; dividend < 256; dividend++)
    {
        uint8_t currByte = static_cast<uint8_t>(dividend);
        for (int bit = 0; bit < 8; bit++)
        {
            if (currByte & 0x01)
                currByte = (currByte >> 1) ^ generator;
            else
                currByte >>= 1;
        }
        table[dividend] = currByte;
    }
    return table;
}

uint8_t Compute_CRC8_Table_Reflected(const vector<uint8_t> &data, const vector<uint8_t> &table)
{
    uint8_t crc = 0;
    for (auto b : data)
    {
        uint8_t dataByte = crc ^ Reflect8(b);
        crc = table[dataByte];
    }
    crc = Reflect8(crc);
    return crc;
}

int main()
{
    // Generate random data of 1000 bits (125 bytes)
    vector<uint8_t> data(125);
    srand(1);
    for (auto &byte : data)
    {
        byte = static_cast<uint8_t>(rand() % 256);
    }

    // Generate tables
    auto table = GenerateCRCTable(generator);
    auto table_reflected = GenerateCRCTable_Reflected(Reflect8(generator));

    // Compute CRCs
    uint8_t crc1 = Compute_CRC8_Simple(data);
    cout << "CRC (Simple Sequential): " << hex << uppercase << setw(2) << setfill('0') << (int)crc1 << endl;

    uint8_t crc2 = Compute_CRC8_Table(data, table);
    cout << "CRC (Table-based): " << hex << uppercase << setw(2) << setfill('0') << (int)crc2 << endl;

    uint8_t crc3 = Compute_CRC8_Simple_Reflected(data);
    cout << "CRC (Reflected Sequential): " << hex << uppercase << setw(2) << setfill('0') << (int)crc3 << endl;

    uint8_t crc4 = Compute_CRC8_Table_Reflected(data, table_reflected);
    cout << "CRC (Reflected Table-based): " << hex << uppercase << setw(2) << setfill('0') << (int)crc4 << endl;

    // Measure execution times
    const int iterations = 10000;
    chrono::high_resolution_clock::time_point start, end;
    chrono::duration<double, milli> elapsed;

    // Timing Simple Sequential Algorithm
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {
        Compute_CRC8_Simple(data);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Simple Sequential Time: " << elapsed.count() << " ms" << endl;

    // Timing Table-based Algorithm
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {
        Compute_CRC8_Table(data, table);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Table-based Time: " << elapsed.count() << " ms" << endl;

    // Timing Reflected Sequential Algorithm
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {
        Compute_CRC8_Simple_Reflected(data);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Reflected Sequential Time: " << elapsed.count() << " ms" << endl;

    // Timing Reflected Table-based Algorithm
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++)
    {
        Compute_CRC8_Table_Reflected(data, table_reflected);
    }
    end = chrono::high_resolution_clock::now();
    elapsed = end - start;
    cout << "Reflected Table-based Time: " << elapsed.count() << " ms" << endl;

    return 0;
}