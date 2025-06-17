#include <iostream>
#include "pin.H"

/* libdft */
#include "libdft/libdft_config.h"
#include "libdft/bridge.h"
#include "libdft/libdft_api.h"
#include "libdft/tagmap.h"
#include "libdft/branch_pred.h"

ADDRINT                 main_low = 0, main_high = 0;
extern ins_desc_t       ins_desc[XED_ICLASS_LAST];

VOID DtaInstrumentComparison(INS ins)
{
    ADDRINT ip = INS_Address(ins);
    if (ip < main_low || ip > main_high) return;
    std::cout << "Address=0x" << std::hex << ip << std::endl;
}


VOID ImageLoad(IMG img, VOID* v)
{
    if (IMG_IsMainExecutable(img)) {
        main_low = IMG_LowAddress(img);
        main_high = IMG_HighAddress(img);
    }
}

VOID FiniCallback(INT32 code, VOID* v)
{
    std::cout << "[DBI] Analysis terminated" << std::endl;
}

int main(int argc, char* argv[])
{
    PIN_InitSymbols();
    if (PIN_Init(argc, argv)) {
        return -1;
    }

    if (unlikely(libdft_init() != 0)) goto err;

    std::cout << "[DBI] Starting analysis" << std::endl;

    ins_set_pre(&ins_desc[XED_ICLASS_CMP], DtaInstrumentComparison);

    PIN_AddFiniFunction(FiniCallback, NULL);
    IMG_AddInstrumentFunction(ImageLoad, nullptr);

    PIN_StartProgram();

    return EXIT_SUCCESS;

err:
    libdft_die();
    return EXIT_FAILURE;
}
