/* 
 * File:   main.cpp
 * Author: jagrosh
 * 
 * Adapted from https://github.com/SparkleTech-VR/OpenPulseConverter/blob/main/OpenPulseConverterSingleton.cpp
 * Utilizes https://github.com/libusb/hidapi
 */

#include <iostream>
#include <array>
#include <thread>
#include <chrono>

// https://github.com/libusb/hidapi
#include <hidapi.h>

#include <windows.h> 

#define LOG(x) std::cout << "[" << __FILE__ << " L" << __LINE__ << "] " << x << std::endl;
#define MAX_STR 255

const int VENDOR_ID = 0x1915;
const int RIGHT_GLOVE_PRODUCT_ID = 0xEEE0;
const int LEFT_GLOVE_PRODUCT_ID = 0xEEE1;
#define RIGHT_PIPE  "\\\\.\\pipe\\vrapplication\\input\\glove\\v2\\right"
#define LEFT_PIPE   "\\\\.\\pipe\\vrapplication\\input\\glove\\v2\\left"

// OpenGlove data structures
typedef struct OpenGloveInputData
{  
    std::array<std::array<float, 4>, 5> flexion;
    std::array<float, 5> splay;
    float joyX;
    float joyY;
    bool joyButton;
    bool trgButton;
    bool aButton;
    bool bButton;
    bool grab;
    bool pinch;
    bool menu;
    bool calibrate;
    float trgValue;
} OpenGloveInputData;

// Pulse data structures
#pragma pack(push, 1)
typedef struct FingerData
{
    unsigned char data[3];
    //unsigned int pull : 14;
    //unsigned int splay : 10;
} FingerData;

typedef struct GloveInputReport
{
    unsigned char reportId : 8;
    FingerData thumb, index, middle, ring, pinky;
} GloveInputReport;

union HIDBuffer
{
    GloveInputReport glove;
    unsigned char buffer[sizeof(glove)];
};
#pragma pack(pop)

class Glove
{
public:
    Glove(int vid, int pid, LPCSTR pipename) : m_handle{ hid_open(vid, pid, nullptr) }, m_wstring{}, m_buffer{},
        m_ogPipe{CreateFile(pipename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr)} {}
    virtual ~Glove() { hid_close(m_handle); }

    // true if the glove is connected
    const bool isValid() const { return m_handle; }
    const auto& read() { hid_read(m_handle, m_buffer.buffer, 25); return m_buffer; };

    // device info
    const std::string getManufacturer() { hid_get_manufacturer_string(m_handle, m_wstring, MAX_STR); std::wstring temp{ m_wstring }; return { temp.begin(), temp.end() }; }
    const std::string getProduct() { hid_get_product_string(m_handle, m_wstring, MAX_STR); std::wstring temp{ m_wstring }; return { temp.begin(), temp.end() }; }
    const std::string getSerialNumber() { hid_get_serial_number_string(m_handle, m_wstring, MAX_STR); std::wstring temp{ m_wstring }; return { temp.begin(), temp.end() }; }
    const std::string getIndexedString(const int i) { hid_get_indexed_string(m_handle, i, m_wstring, MAX_STR); std::wstring temp{ m_wstring }; return { temp.begin(), temp.end() }; }
private:
    // connection to glove
    hid_device* m_handle;
    // pipe to opengloves
    HANDLE m_ogPipe;

    // temp vars
    wchar_t m_wstring[MAX_STR];
    HIDBuffer m_buffer;
};

/*
 * Main function that runs upon execution
 */
int main(int argc, char** argv) 
{
    LOG("Hello World!");

    // initialize HID lib
    hid_init();

    // init gloves
    Glove left{ VENDOR_ID, LEFT_GLOVE_PRODUCT_ID, LEFT_PIPE };
    Glove right{ VENDOR_ID, RIGHT_GLOVE_PRODUCT_ID, RIGHT_PIPE };

    // print diagnostics
    if (!left.isValid() && !right.isValid())
    {
        LOG("No gloves are connected!")
        hid_exit();
        return 1;
    }

    if (left.isValid())
    {
        LOG("Left Glove Connected!");
        LOG("Manufacturer: " + left.getManufacturer());
        LOG("Product     : " + left.getProduct());
        LOG("SerialNumber: " + left.getSerialNumber());
    }
    else
    {
        LOG("Left Glove was not found!");
    }

    if (right.isValid())
    {
        LOG("Right Glove Connected!");
        LOG("Manufacturer: " + right.getManufacturer());
        LOG("Product     : " + right.getProduct());
        LOG("SerialNumber: " + right.getSerialNumber());
    }
    else
    {
        LOG("Right Glove was not found!");
    }

    // begin loop
    bool exit = false;
    while (!exit)
    {
        if (left.isValid())
        {
            const auto& buffer = left.read();
            OpenGloveInputData ogid{};
            // TODO: move data from buffer to ogid
            // TODO: write ogid to named pipe
        }

        if (right.isValid())
        {
            const auto& buffer = right.read();
            OpenGloveInputData ogid{};
            // TODO: move data from buffer to ogid
            // TODO: write ogid to named pipe

            // TODO: test code, remove later:
            printf("buffer: ");
            for (int i = 0; i < sizeof(buffer); i++)
                printf("%d ", buffer.buffer[i]);
            printf("\n");
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000000 / 67)); // 67 hz
    }

    // close the hidapi library
    hid_exit();

    return 0;
}