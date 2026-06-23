# DllLoader

A modern C++ PE/DLL loader for UEFI with CRT, STL, import resolution, relocation, and manual image mapping support.

## Features

* Manual PE image mapping
* Base relocation support
* Import resolution
* Export resolution
* Custom runtime environment
* MSVC CRT support
* C++ runtime support
* STL support in loaded DLLs

## Supported Runtime Features

### CRT

* malloc / free
* calloc / realloc
* memcpy / memmove / memset / memcmp
* strlen / wcslen
* strcmp / strncmp
* wcscmp / wcsncmp
* strcpy / strncpy
* wcscpy / wcsncpy

### C++

* operator new / delete
* operator new[] / delete[]
* std::nothrow new
* sized delete

### STL

* std::string
* std::vector
* std::unique_ptr
* std::shared_ptr
* std::weak_ptr
* std::optional
* std::variant
* std::any
* std::function
* std::array
* std::pair
* std::tuple
* std::deque
* std::list
* std::set
* std::unordered_set
* std::map
* std::unordered_map
* std::sort

## Notes

The loader provides a custom host runtime through `UEFIppHost.dll`, allowing loaded DLLs to use CRT, C++, and a large subset of the MSVC STL without relying on the Windows loader.

See `UefiDll/DllMain.cpp` for example DLL code and runtime validation tests.

## Example

```cpp
static auto TestCrt() -> bool
{
    auto* M = static_cast<char*>(malloc(32));
    if (!M) return false;

    memset(M, 'A', 31);
    M[31] = 0;

    if (strlen(M) != 31) return false;
    free(M);

    auto* C = static_cast<unsigned char*>(calloc(8, 4));
    if (!C) return false;

    for (int i = 0; i < 32; ++i)
        if (C[i] != 0) return false;

    C = static_cast<unsigned char*>(realloc(C, 64));
    if (!C) return false;
    free(C);

    char A[16]{};
    char B[16]{};

    strcpy(A, "Hello");
    memcpy(B, A, 6);

    if (strcmp(A, B) != 0) return false;
    if (strncmp(A, "Hel", 3) != 0) return false;
    if (memcmp(A, B, 6) != 0) return false;

    char Move[16] = "abcdef";
    memmove(Move + 2, Move, 4);

    if (Move[2] != 'a') return false;
    if (Move[3] != 'b') return false;
    if (Move[4] != 'c') return false;
    if (Move[5] != 'd') return false;

    wchar_t W1[16]{};
    wchar_t W2[16]{};

    wcscpy(W1, L"Hello");
    wcsncpy(W2, W1, 6);

    if (wcslen(W1) != 5) return false;
    if (wcscmp(W1, W2) != 0) return false;
    if (wcsncmp(W1, L"Hel", 3) != 0) return false;

    return true;
}

static auto TestNewDelete() -> bool
{
    auto* A = new int{ 123 };
    if (!A || *A != 123) return false;
    delete A;

    auto* B = new int[8];
    if (!B) return false;

    for (int i = 0; i < 8; ++i)
        B[i] = i * 3;

    for (int i = 0; i < 8; ++i)
        if (B[i] != i * 3) return false;

    delete[] B;

    auto* C = new(std::nothrow) char[32];
    if (!C) return false;
    delete[] C;

    return true;
}

static auto TestStringVectorAlgorithm() -> bool
{
    std::string Text = "Hello";
    Text += " UEFI";

    if (Text != "Hello UEFI") return false;
    if (Text.size() != 10) return false;

    std::vector<int> Values{ 5, 1, 4, 2, 3 };
    std::sort(Values.begin(), Values.end());

    for (int i = 0; i < 5; ++i)
        if (Values[i] != i + 1) return false;

    return true;
}

static auto TestBasicStlTypes() -> bool
{
    std::unique_ptr<int> Ptr = std::make_unique<int>(42);
    if (!Ptr || *Ptr != 42) return false;

    std::optional<int> Opt{};
    if (Opt.has_value()) return false;

    Opt = 55;
    if (!Opt || *Opt != 55) return false;

    std::array<int, 4> Arr{ 1, 2, 3, 4 };
    if (Arr.size() != 4) return false;
    if (Arr[0] != 1 || Arr[3] != 4) return false;

    std::pair<int, int> Pair{ 10, 20 };
    if (Pair.first != 10 || Pair.second != 20) return false;

    std::tuple<int, int, int> Tup{ 1, 2, 3 };
    if (std::get<0>(Tup) != 1) return false;
    if (std::get<1>(Tup) != 2) return false;
    if (std::get<2>(Tup) != 3) return false;

    std::function<int(int)> Fn = [](int x)
        {
            return x * 2;
        };

    if (!Fn) return false;
    if (Fn(21) != 42) return false;

    return true;
}

static auto TestSharedWeakPtr() -> bool
{
    auto Shared = std::make_shared<int>(123);
    std::weak_ptr<int> Weak = Shared;

    {
        auto Locked = Weak.lock();
        if (!Locked || *Locked != 123) return false;
    }

    Shared.reset();

    return Weak.expired();
}

static auto TestVariantAny() -> bool
{
    std::variant<int, const char*> Var{ 42 };

    if (!std::holds_alternative<int>(Var)) return false;
    if (std::get<int>(Var) != 42) return false;

    Var = "Hello";

    if (!std::holds_alternative<const char*>(Var)) return false;

    std::any Any = 77;

    if (!Any.has_value()) return false;
    if (std::any_cast<int>(Any) != 77) return false;

    Any = const_cast<char*>("UEFI");

    if (std::any_cast<char*>(Any)[0] != 'U') return false;

    return true;
}

static auto TestDequeList() -> bool
{
    std::deque<int> Deque{};
    Deque.push_back(1);
    Deque.push_back(2);
    Deque.push_front(0);

    if (Deque.size() != 3) return false;
    if (Deque[0] != 0 || Deque[1] != 1 || Deque[2] != 2) return false;

    std::list<int> List{};
    List.push_back(3);
    List.push_back(1);
    List.push_back(2);
    List.sort();

    int Expected = 1;

    for (auto Value : List)
        if (Value != Expected++) return false;

    return true;
}

static auto TestAssociativeContainers() -> bool
{
    std::set<int> Set{};
    Set.insert(3);
    Set.insert(1);
    Set.insert(2);

    if (Set.size() != 3) return false;
    if (!Set.contains(1) || !Set.contains(2) || !Set.contains(3)) return false;

    std::unordered_set<int> HashSet{};
    HashSet.insert(10);
    HashSet.insert(20);
    HashSet.insert(30);

    if (HashSet.size() != 3) return false;
    if (!HashSet.contains(10) || !HashSet.contains(20) || !HashSet.contains(30)) return false;

    std::map<int, int> Map{};
    Map[1] = 10;
    Map[2] = 20;
    Map[3] = 30;

    if (Map.size() != 3) return false;
    if (Map[1] != 10 || Map[2] != 20 || Map[3] != 30) return false;

    std::unordered_map<int, int> HashMap{};
    HashMap[10] = 100;
    HashMap[20] = 200;
    HashMap[30] = 300;

    if (HashMap.size() != 3) return false;
    if (HashMap[10] != 100 || HashMap[20] != 200 || HashMap[30] != 300) return false;

    return true;
}
```

## Showcase

[![Showcase](https://img.youtube.com/vi/nBW1Z3q9wao/maxresdefault.jpg)](https://www.youtube.com/watch?v=nBW1Z3q9wao)