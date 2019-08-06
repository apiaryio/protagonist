//#include <Windows.h>
//^ what up GYP, why does including the correct header fail? :)
#include <iostream>
#include <string>
#include <vector>
#include <Psapi.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <iterator>
#include "protagonist.h"

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "dbghelp.lib")

// Some versions of imagehlp.dll lack the proper packing directives themselves
// so we need to do it.
#pragma pack( push, before_imagehlp, 8 )
#include <imagehlp.h>
#pragma pack( pop, before_imagehlp )

struct module_data {
    std::string image_name;
    std::string module_name;
    void *base_address;
    DWORD load_size;
};
typedef std::vector<module_data> ModuleList;

HANDLE thread_ready;

bool show_stack(std::ostream &, HANDLE hThread, CONTEXT& c);
DWORD __stdcall TargetThread( void *arg );
void ThreadFunc1();
void ThreadFunc2();
DWORD Filter( EXCEPTION_POINTERS *ep );
void *load_modules_symbols( HANDLE hProcess, DWORD pid );


using namespace v8;
using namespace protagonist;

LONG WINAPI CrashHandler(EXCEPTION_POINTERS *ExceptionInfo) {
    std::cerr << "Crash Handler Called, is access violation? " << (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION);
    
    HANDLE thread;

    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
        GetCurrentProcess(), &thread, 0, false, DUPLICATE_SAME_ACCESS);
    std::cerr << "Walking stack.";
    show_stack(std::cerr, thread, *(ExceptionInfo->ContextRecord));
    std::cerr << "\nEnd of stack walk.\n";
    CloseHandle(thread);

    return EXCEPTION_EXECUTE_HANDLER;
    std::cerr << "Crash Handler Called, is access violation? " << (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION);
    return EXCEPTION_CONTINUE_SEARCH;
}

NAN_MODULE_INIT(InitAll) {
    SetUnhandledExceptionFilter(CrashHandler);
 
    Nan::Set(target, Nan::New<String>("parse").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(Parse)).ToLocalChecked());

    Nan::Set(target, Nan::New<String>("parseSync").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(ParseSync)).ToLocalChecked());

    Nan::Set(target, Nan::New<String>("validate").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(Validate)).ToLocalChecked());

    Nan::Set(target, Nan::New<String>("validateSync").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(ValidateSync)).ToLocalChecked());
}

NODE_MODULE(protagonist, InitAll)


class SymHandler { 
    HANDLE p;
public:
    SymHandler(HANDLE process, char const *path=NULL, bool intrude = false) : p(process) { 
        if (!SymInitialize(p, path, intrude)) 
            throw(std::logic_error("Unable to initialize symbol handler"));
    }
    ~SymHandler() { SymCleanup(p); }
};

#ifdef _M_X64
STACKFRAME64 init_stack_frame(CONTEXT c) {
    STACKFRAME64 s;
    s.AddrPC.Offset = c.Rip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.Rsp;
    s.AddrStack.Mode = AddrModeFlat;    
    s.AddrFrame.Offset = c.Rbp;
    s.AddrFrame.Mode = AddrModeFlat;
    return s;
}
#else
STACKFRAME64 init_stack_frame(CONTEXT c) {
    STACKFRAME64 s;
    s.AddrPC.Offset = c.Eip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.Esp;
    s.AddrStack.Mode = AddrModeFlat;    
    s.AddrFrame.Offset = c.Ebp;
    s.AddrFrame.Mode = AddrModeFlat;
    return s;
}
#endif

void sym_options(DWORD add, DWORD remove=0) {
    DWORD symOptions = SymGetOptions();
    symOptions |= add;
    symOptions &= ~remove;
    SymSetOptions(symOptions);
}

class symbol { 
    typedef IMAGEHLP_SYMBOL64 sym_type;
    sym_type *sym;
    static const int max_name_len = 1024;
public:
    symbol(HANDLE process, DWORD64 address) : sym((sym_type *)::operator new(sizeof(*sym) + max_name_len)) {
        memset(sym, '\0', sizeof(*sym) + max_name_len);
        sym->SizeOfStruct = sizeof(*sym);
        sym->MaxNameLength = max_name_len;
        DWORD64 displacement;

        if (!SymGetSymFromAddr64(process, address, &displacement, sym))
            throw(std::logic_error("Bad symbol"));
    }

    std::string name() { return std::string(sym->Name); }
    std::string undecorated_name() { 
        std::vector<char> und_name(max_name_len);
        UnDecorateSymbolName(sym->Name, &und_name[0], max_name_len, UNDNAME_COMPLETE);
        return std::string(&und_name[0], strlen(&und_name[0]));
    }
};

bool show_stack(std::ostream &os, HANDLE hThread, CONTEXT& c) {
    HANDLE process = GetCurrentProcess();
    int frame_number=0;
    DWORD offset_from_symbol=0;
    IMAGEHLP_LINE64 line = {0};

    SymHandler handler(process);

    sym_options(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

    void *base = load_modules_symbols(process, GetCurrentProcessId());

    STACKFRAME64 s = init_stack_frame(c);

    line.SizeOfStruct = sizeof line;

    IMAGE_NT_HEADERS *h = ImageNtHeader(base);
    DWORD image_type = h->FileHeader.Machine;

    do {
        if (!StackWalk64(image_type, process, hThread, &s, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
            return false;

        os << std::setw(3) << "\n" << frame_number << "\t";
        if ( s.AddrPC.Offset != 0 ) {
            std::cout << symbol(process, s.AddrPC.Offset).undecorated_name();

            if (SymGetLineFromAddr64( process, s.AddrPC.Offset, &offset_from_symbol, &line ) ) 
                    os << "\t" << line.FileName << "(" << line.LineNumber << ")";
        }
        else
            os << "(No Symbols: PC == 0)";
        ++frame_number;
    } while (s.AddrReturn.Offset != 0);
    return true;
}

class get_mod_info {
    HANDLE process;
    static const int buffer_length = 4096;
public:
    get_mod_info(HANDLE h) : process(h) {}

    module_data operator()(HMODULE module) { 
        module_data ret;
        char temp[buffer_length];
        MODULEINFO mi;

        GetModuleInformation(process, module, &mi, sizeof(mi));
        ret.base_address = mi.lpBaseOfDll;
        ret.load_size = mi.SizeOfImage;

        GetModuleFileNameEx(process, module, temp, sizeof(temp));
        ret.image_name = temp;
        GetModuleBaseName(process, module, temp, sizeof(temp));
        ret.module_name = temp;
        std::vector<char> img(ret.image_name.begin(), ret.image_name.end());
        std::vector<char> mod(ret.module_name.begin(), ret.module_name.end());
        SymLoadModule64(process, 0, &img[0], &mod[0], (DWORD64)ret.base_address, ret.load_size);
        return ret;
    }
};

void *load_modules_symbols(HANDLE process, DWORD pid) {
    ModuleList modules;

    DWORD cbNeeded;
    std::vector<HMODULE> module_handles(1);

    EnumProcessModules(process, &module_handles[0], module_handles.size() * sizeof(HMODULE), &cbNeeded);
    module_handles.resize(cbNeeded/sizeof(HMODULE));
    EnumProcessModules(process, &module_handles[0], module_handles.size() * sizeof(HMODULE), &cbNeeded);

    std::transform(module_handles.begin(), module_handles.end(), std::back_inserter(modules), get_mod_info(process));
    return modules[0].base_address;
}
