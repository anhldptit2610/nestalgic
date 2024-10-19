#include "controller.h"
#include <cstring>


void Controller::Write(uint16_t addr, uint8_t val)
{
    if (strobe && !NTHBIT(val, 0))
        sequence = 0;
    strobe = NTHBIT(val, 0);
}

uint8_t Controller::Read(uint16_t addr)
{
    uint8_t ret = 0xff;

    switch (addr) {
    case 0x4016:
        if (sequence == 0xff) {
            ret = 0x00 | (uint8_t)btn[BTN_A];
        } else {
            ret = 0x00 | (uint8_t)btn[sequence];
            sequence = (sequence == 7) ? 0xff : sequence + 1;
        }
        break;
    case 0x4017:
        break;
    default:
        break;
    };
    return ret;
}

bool * Controller::GetAllButtons() { return btn; }

/* constructor, destructor */
Controller::Controller()
{
    strobe = false;
    sequence = 0xff;
    memset(btn, RELEASED, sizeof(btn));
}

Controller::~Controller() {}
