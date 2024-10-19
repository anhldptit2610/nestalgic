#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "common.h"

#define PRESSED             1
#define RELEASED            0

#define BTN_A               0
#define BTN_B               1
#define BTN_SELECT          2
#define BTN_START           3
#define BTN_UP              4
#define BTN_DOWN            5
#define BTN_LEFT            6
#define BTN_RIGHT           7

class Controller {
private:
    bool btn[8]{};
    bool strobe;
    int sequence;
public:
    bool *GetAllButtons();
    void Write(uint16_t, uint8_t);
    uint8_t Read(uint16_t);
    /* constructor, destructor */
    Controller();
    ~Controller();
};

#endif