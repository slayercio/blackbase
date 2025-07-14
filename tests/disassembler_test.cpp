#include <print>

#ifdef BLACKBASE_ENABLE_DISASSEMBLER
#include <blackbase/blackbase.hpp>
#include <blackbase/disassembler/disassembler.hpp>

const uint8_t sampleData[] = {
    0x90,                                       // NOP
    0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,   // MOV RAX, 1
    0x48, 0xC7, 0xC1, 0x02, 0x00, 0x00, 0x00,   // MOV RCX, 2
    0x48, 0x01, 0xC8,                           // ADD RAX, RCX
    0x48, 0x8B, 0xD1,                           // MOV RDX, RCX
    0x48, 0x83, 0xC0, 0x0A,                     // ADD RAX, 0xA
    0x50,                                       // PUSH RAX
    0x51,                                       // PUSH RCX
    0x52,                                       // PUSH RDX
    0x58,                                       // POP RAX
    0x59,                                       // POP RCX
    0x5A,                                       // POP RDX
    0x48, 0x83, 0xEC, 0x20,                     // SUB RSP, 0x20 (Allocate stack space)
    0x48, 0x89, 0x44, 0x24, 0x10,               // MOV QWORD PTR [RSP+0x10], RAX
    0x48, 0x8B, 0x4C, 0x24, 0x10,               // MOV RCX, QWORD PTR [RSP+0x10]
    0x48, 0x83, 0xC4, 0x20,                     // ADD RSP, 0x20 (Deallocate stack space)
    0xB8, 0x05, 0x00, 0x00, 0x00,               // MOV EAX, 5 (32-bit immediate)
    0x48, 0x31, 0xC0,                           // XOR RAX, RAX
    0xE8, 0x00, 0x00, 0x00, 0x00,               // CALL +0x0 (Relative call)
    0xEB, 0x05,                                 // JMP +0x5 (Short jump)
    0x90, 0x90, 0x90, 0x90, 0x90,               // NOPs for jump target
    0x31, 0xC0,                                 // XOR EAX, EAX
    0x48, 0x83, 0xF8, 0x0A,                     // CMP RAX, 0xA
    0x74, 0x05,                                 // JE +0x5 (Jump if equal)
    0x90, 0x90, 0x90, 0x90, 0x90,               // NOPs for JE target
    0x48, 0x83, 0xF8, 0x05,                     // CMP RAX, 0x5
    0x75, 0x05,                                 // JNE +0x5 (Jump if not equal)
    0x90, 0x90, 0x90, 0x90, 0x90,               // NOPs for JNE target
    0xC3                                        // RET
};

const uint8_t sampleDataTwo[] = {
    // mov, add, sub
    0x48, 0x89, 0xD8,                         // mov rax, rbx
    0x48, 0x01, 0xD0,                         // add rax, rdx
    0x48, 0x29, 0xC8,                         // sub rax, rcx

    // stack ops
    0x50,                                     // push rax
    0x51,                                     // push rcx
    0x58,                                     // pop rax
    0x59,                                     // pop rcx

    // memory deref
    0x48, 0x8B, 0x05, 0xB8, 0x13, 0x00, 0x00, // mov rax, [rip + 0x13B8]

    // jump, call, ret
    0xE8, 0x34, 0x12, 0x00, 0x00,             // call 0x1234
    0x75, 0x0A,                               // jne short +0x0A
    0xEB, 0xFE,                               // jmp -0x2 (infinite loop)
    0xC3,                                     // ret

    // xor, cmp, test
    0x31, 0xC0,                               // xor eax, eax
    0x85, 0xC0,                               // test eax, eax
    0x3B, 0xC1,                               // cmp eax, ecx

    // lea
    0x48, 0x8D, 0x04, 0x85, 0x34, 0x12, 0x00, 0x00, // lea rax, [rax*4 + 0x1234]

    // movaps
    0x0F, 0x28, 0xC1,                         // movaps xmm0, xmm1

    // conditional moves
    0x0F, 0x44, 0xC1,                         // cmove eax, ecx
    0x0F, 0x45, 0xC2,                         // cmovne eax, edx

    // setcc
    0x0F, 0x94, 0xC0,                         // sete al
    0x0F, 0x95, 0xC1,                         // setne cl

    // syscall-like sequence (int 0x2e not usable in long mode but included for flavor)
    0xCD, 0x2E,                               // int 0x2e
    0x0F, 0x05,                               // syscall

    // more SIMD
    0x66, 0x0F, 0x6F, 0xC1,                   // movdqa xmm0, xmm1
    0xF3, 0x0F, 0x10, 0x05, 0x34, 0x12, 0x00, 0x00, // movss xmm0, [rip + 0x1234]

    // call rax
    0xFF, 0xD0,                               // call rax

    // mov to segment register (rare, will show as invalid on some decoders)
    0x8E, 0xD8,                               // mov ds, eax (invalid in 64-bit mode)
};

void test_sample(const uint8_t* data, size_t size)
{
    auto disassembler = blackbase::disassembler::CreateDisassembler();
    if (!disassembler)
    {
        std::print("Failed to create disassembler instance.\n");
        return;
    }

    std::print("\033[34m[Disassembling sample data...]\033[0m\n");
    auto instructions = disassembler->Disassemble(
        const_cast<uint8_t*>(data),
        size,
        reinterpret_cast<uint64_t>(data)
    );

    if (instructions.empty())
    {
        std::print("No instructions disassembled.\n");
        return;
    }

    for (const auto& instruction : instructions)
    {
        if (instruction->IsValid())
        {
            std::print("Address: 0x{:016X} -> {}\n",
                       instruction->GetAddress(),
                       instruction->ToString());
        }
        else
        {
            std::print("Invalid instruction at address 0x{:016X}\n", instruction->GetAddress());
        }
    }
}

int main()
{
    test_sample(sampleData, sizeof(sampleData));
    std::print("\n\n\n\n\n\n\n");
    test_sample(sampleDataTwo, sizeof(sampleDataTwo));

    if (*(std::uint8_t*)&test_sample == 0xE9) // a thunk
    {
        auto thunk = (uint8_t*) &test_sample;
        auto actualFunction = thunk + 5 + *reinterpret_cast<int32_t*>(thunk + 1);

        std::print("\n\n\033[34m[Disassembling thunk function at address 0x{:016X}]\033[0m\n", reinterpret_cast<uint64_t>(actualFunction));
        test_sample(actualFunction, 0x100); // Disassemble 256 bytes of the thunk function
    }
    else
    {
        std::print("\n\n\033[34m[No thunk function detected]\033[0m\n");
        test_sample(reinterpret_cast<uint8_t*>(&test_sample), 0x100); // Disassemble 256 bytes of the test function
    }
}

#else

int main()
{
    std::print("Disassembler is not enabled in this build.\n");
    return 0;
}

#endif