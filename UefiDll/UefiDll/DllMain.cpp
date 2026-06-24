#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>

extern "C" __declspec(dllexport)
auto DllMain() -> int
{
    printf("=== UEFI DLL C++ runtime test ===\n");

    try
    {
        std::vector<int> Values{};

        for (int i = 1; i <= 8; ++i)
        {
            Values.push_back(i * 10);
        }

        printf("vector size: %u\n", static_cast<unsigned>(Values.size()));

        int Sum{};

        for (auto Value : Values)
        {
            Sum += Value;
        }

        printf("vector sum: %d\n", Sum);

        std::string Text = "std::string works";
        Text += " + append";

        printf("string: %s\n", Text.c_str());

        std::wstring Wide = L"std::wstring exception works";
        throw Wide;
    }
    catch (const std::wstring&)
    {
        printf("caught std::wstring exception\n");
    }
    catch (...)
    {
        printf("unexpected catch-all\n");
        return 1;
    }

    printf("=== all tests passed ===\n");
    return 0;
}