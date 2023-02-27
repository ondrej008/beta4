#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <algorithm>
#include <vector>


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

    std::vector<int> nejlepsi;
    float nejlepsi_skore = INFINITY;

    uint64_t cislo_permutace = 0;

    do
    {
        if (cislo_permutace % (pocet_permutaci / 100) == 0)
        {
            printf("%llu%% hotovo\n", cislo_permutace / (pocet_permutaci / 100));
        }

        float skore = hodnoceni(mesta, mesta_index);

        if (skore < nejlepsi_skore)
        {
            printf("Lepsi reseni bylo nalezeno, predchozi %f, nove %f\n", nejlepsi_skore, skore);

            nejlepsi_skore = skore;
            nejlepsi = mesta_index;
        }

        cislo_permutace++;
    } while (std::next_permutation(mesta_index.begin(), mesta_index.end()));

    printf("Nejlepsi reseni s vzdalenosti %f se sklada z:\n", nejlepsi_skore);

    for (int i = 0; i < nejlepsi.size() - 1; i++)
    {
        printf("%d -> %d (%f)\n", nejlepsi[i], nejlepsi[i + 1], vzdalenost(mesta[nejlepsi[i]], mesta[nejlepsi[i + 1]]));
    }

    printf("%d -> %d (%f)\n", nejlepsi[nejlepsi.size() - 1], nejlepsi[0], vzdalenost(mesta[nejlepsi[nejlepsi.size() - 1]], mesta[nejlepsi[0]]));
}