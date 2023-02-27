#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>


struct City
{
    float x, y;
};

uint64_t factorial(uint64_t num)
{
    if (num <= 1) return 1;
    return factorial(num - 1) * num;
}

float vzdalenost(City& a, City& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return sqrtf(dx * dx + dy * dy);
}

float hodnoceni(std::vector<City>& mesta, std::vector<int>& indexy)
{
    float suma = 0.0;

    for (int i = 0; i < indexy.size() - 1; i++)
    {
        City& a = mesta[indexy[i + 0]];
        City& b = mesta[indexy[i + 1]];

        suma += vzdalenost(a, b);
    }

    suma += vzdalenost(mesta[indexy[0]], mesta[indexy[indexy.size() - 1]]);

    return suma;
}

std::vector<int> nejlepsi;
float nejlepsi_skore = INFINITY;
std::mutex lock;

void thread_runner(int id, std::vector<City>& mesta, std::vector<int> mesta_index, uint64_t pocet_permutaci)
{
    uint64_t cislo_permutace = 0;

    printf("Thread %d %llu", id, pocet_permutaci);

    do
    {
        if (cislo_permutace % (pocet_permutaci / 10) == 0)
        {
            printf("Thread %d: %llu%% hotovo\n", id, cislo_permutace / (pocet_permutaci / 10));
        }

        float skore = hodnoceni(mesta, mesta_index);

        if (skore < nejlepsi_skore)
        {
            //printf("Lepsi reseni bylo nalezeno, predchozi %f, nove %f\n", nejlepsi_skore, skore);
            lock.lock();
            nejlepsi_skore = skore;
            nejlepsi = mesta_index;
            lock.unlock();
        }

        cislo_permutace++;
    } while (std::next_permutation(mesta_index.begin(), mesta_index.end()) && cislo_permutace < pocet_permutaci);

    
}

int main()
{
    int pocet_mest;

    printf("Vyberte pocet mest pro Travelling Salesman Problem: ");

    scanf_s("%d", &pocet_mest);

    if (pocet_mest <= 1)
    {
        printf("Prilis maly pocet mest, musi byt vetsi nez 1\n");
        exit(1);
    }

    if (pocet_mest > 20)
    {
        printf("Prilis velky pocet mest, musi byt mensi nez 21\n");
        exit(1);
    }

    uint64_t pocet_permutaci = factorial(pocet_mest);

    printf("Pro %d mest budeme muset spocitat %llu permutaci\n", pocet_mest, pocet_permutaci);

    srand(time(NULL));

    std::vector<City> mesta;

    for (int i = 0; i < pocet_mest; i++)
    {
        float x = 100.0f * (rand() / (float) RAND_MAX);
        float y = 100.0f * (rand() / (float) RAND_MAX);

        mesta.push_back({x, y});

        printf("Mesto %d vytvoreno v {%06.3f, %06.3f}\n", i, x, y);
    }

    std::vector<int> mesta_index;

    for (int i = 0; i < mesta.size(); i++)
    {
        mesta_index.push_back(i);
    }

    std::sort(mesta_index.begin(), mesta_index.end());

    unsigned int pocet_threadu = std::thread::hardware_concurrency();
    //unsigned int pocet_threadu = 1;

    printf("Pocet threadu na zarizeni je %u, kazdy thread bude pocitat %llu permutaci\n", pocet_threadu, pocet_permutaci / pocet_threadu);

    std::vector<std::thread> thready;

    for (uint64_t permutace = 0; permutace < pocet_permutaci; permutace++)
    {
        if (permutace % (pocet_permutaci / pocet_threadu) == 0)
        {
            thready.push_back(std::thread(thread_runner, thready.size(), std::ref(mesta), mesta_index, (pocet_permutaci / pocet_threadu)));
        }

        std::next_permutation(mesta_index.begin(), mesta_index.end());
    }

    for (int i = 0; i < thready.size(); i++)
    {
        thready[i].join();
    }

    printf("Nejlepsi reseni s vzdalenosti %f se sklada z:\n", nejlepsi_skore);

    for (int i = 0; i < nejlepsi.size() - 1; i++)
    {
        printf("%d -> %d (%f)\n", nejlepsi[i], nejlepsi[i + 1], vzdalenost(mesta[nejlepsi[i]], mesta[nejlepsi[i + 1]]));
    }

    printf("%d -> %d (%f)\n", nejlepsi[nejlepsi.size() - 1], nejlepsi[0], vzdalenost(mesta[nejlepsi[nejlepsi.size() - 1]], mesta[nejlepsi[0]]));
}