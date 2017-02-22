#ifndef MOCKER_H
#define MOCKER_H

#if __x86_64__
#define CALL_INSTRUCTION_SIZE 12
#else
#define CALL_INSTRUCTION_SIZE 5
#endif

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

template <typename func_ptr>
class Mocker
{
private:
    union function {
        func_ptr pointer;
        void * address;
    };
    char new_instructions[CALL_INSTRUCTION_SIZE];
    char old_instructions[CALL_INSTRUCTION_SIZE];
    function a;
    function b;
    void renderInstructions(void * address, void * jumpto)
    {
#if __x86_64__
            //Save the old instructions for later
            memcpy(old_instructions, address, CALL_INSTRUCTION_SIZE);
            new_instructions[0] = 0x48;             // mov $<address> %rax
            new_instructions[1] = 0xB8;
            *(int64_t *)(new_instructions + 2) = (int64_t)jumpto;
            new_instructions[10] = 0xFF;            // jmpq *%rax
            new_instructions[11] = 0xE0;
#else
            //Save the old instructions for later
            memcpy(old_instructions, address, CALL_INSTRUCTION_SIZE);
            int32_t offset = (int32_t)jumpto - ((int32_t)address + CALL_INSTRUCTION_SIZE);
            new_instructions[0] = (char)0xE9;        // jump relative 32bit immediate opcode
            *(int32_t *)(new_instructions+1) = offset;   // address to jump to
#endif
    }

public:
    // The constructor sets up the function mock and it exists for
    // the lifetime of the object.
    Mocker(func_ptr _a, func_ptr _b)// : a((void*)_a), b((void *)_b)
    {
        a.pointer = _a;
        b.pointer = _b;
        unsigned long page_size = sysconf(_SC_PAGESIZE);
        unsigned long page_align = ~(page_size - 1);
        unsigned long start_page = (unsigned long)((unsigned long)a.address & page_align);
        unsigned long end_page = (unsigned long) (((unsigned long)a.address 
                    + CALL_INSTRUCTION_SIZE + page_size) & page_align);
        size_t len = end_page - start_page;
        if(mprotect((void *)start_page, len, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
        {
            printf("Exec to Write failed: ");
            perror("mprotect");
            printf("\n");
            assert(0);
        }
        renderInstructions(a.address, b.address);
        memcpy(a.address, (void *)new_instructions, CALL_INSTRUCTION_SIZE);
        if(mprotect((void *)start_page, len, PROT_EXEC) != 0)
        {
            printf("Write to Exec failed: ");
            perror("mprotect");
            printf("\n");
            assert(0);
        }
    }
    // The destructor tears down the mocked function that was set
    // up by the constructor.
    ~Mocker()
    {
        unsigned long page_size = sysconf(_SC_PAGESIZE);
        unsigned long page_align = ~(page_size - 1);
        unsigned long start_page = (unsigned long)((unsigned long)a.address & page_align);
        unsigned long end_page = (unsigned long) (((unsigned long)a.address 
                    + CALL_INSTRUCTION_SIZE + page_size) & page_align);
        size_t len = end_page - start_page;

        if(mprotect((void *)start_page, len, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
        {
            printf("Exec to Write failed: ");
            perror("mprotect");
            printf("\n");
            assert(0);
        }
        memcpy(a.address, (void *)old_instructions, CALL_INSTRUCTION_SIZE);
        if(mprotect((void *)start_page, len, PROT_EXEC) != 0)
        {
            printf("Write to Exec failed: ");
            perror("mprotect");
            printf("\n");
            assert(0);
        }
    }
};
#endif
