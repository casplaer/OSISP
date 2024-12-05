#include <iostream>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <ctime>

void* fiber = NULL;
void* fiber1 = NULL;

void WINAPI testFunc(LPVOID lpParam)
{
    int i = 500;

    while (i--)
    {
        std::cout << '1';
    }
}

int main() {
    fiber = ConvertThreadToFiber(NULL);
    if (fiber == NULL)
    {
        std::cout << "123";
    }
    SwitchToFiber(fiber);
    

    fiber1 = CreateFiber(0, testFunc, NULL);
    SwitchToFiber(fiber1);

    DeleteFiber(fiber1);

    return 0;
}
