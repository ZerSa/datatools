#include <iostream>
#include <vector>
#include <array>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <clocale>

#include "avltree.h"
#include "hash.h"


using namespace std;
using namespace avl_tree;
using namespace hash_table;

static const int MIN = 7;
static const int MAX = 20;
static const int HASH_TABLE_SIZE = 1000000;
static const int BLOCK_SIZE = 2000;
static const int FROM = 10000;
static const int TO = 1000000;
static const int STEP = 400000;

int randomInteger(int min, int max) {
    return rand() % (max - min + 1) + min;
}

string randomString(const int len)
{
    char *s = new char[len];
    static const char symbols[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzабвгдеёжзийклмнопрстуфхцчшщъыьэюяАБМГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    for (int i = 0; i < len; ++i)
    {
        s[i] = symbols[rand() % (sizeof(symbols) - 1)];
    }

    s[len] = '\0';
    return string(s);
}

struct SMyStruct
{
    string key1;
    string key2;

    SMyStruct() : key1(randomString(randomInteger(MIN, MAX))), key2(randomString(randomInteger(MIN, MAX))) {}
};

int cmp(const SMyStruct *a, const SMyStruct *b)
{
    int result = a->key1.compare(b->key1);
    if (result == 0)
    {
        return a->key2.compare(b->key2);
    } else
    {
        return result;
    }
}

unsigned int hashFunction(const SMyStruct *str)
{
    return std::hash<string>()(str->key1) + std::hash<string>()(str->key2);
}

bool compare(const SMyStruct &a, const SMyStruct &b)
{
    return cmp(&a, &b) < 0;
}

array<array<double, 5>, 3> countTimes(int n)
{
    array<array<double, 5>, 3> times;

    vector<SMyStruct> vct;
    vector<SMyStruct> vctCopy;
    vector<SMyStruct> vctForSearch;

    for (int i = 0; i < n; ++i)
    {
        vct.push_back(SMyStruct());
    }

    vctCopy = vct;

    for (int l = 0; l < 2 * n; ++l)
    {
        vctForSearch.push_back(SMyStruct());
    }

    CAVLTree<SMyStruct, cmp> tree;
    CHash<SMyStruct, hashFunction, cmp> table(HASH_TABLE_SIZE, BLOCK_SIZE);

    clock_t start, end;

    //  Размещение массива данных в структурах соответствующих алгоритму.
    start = clock();
    for (int i = 0; i < vct.size(); ++i)
    {
        tree.add(&vct[i]);
    }
    end = clock();
    times[0][0] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < vct.size(); ++i)
    {
        table.add(&vct[i]);
    }
    end = clock();
    times[1][0] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    sort(vctCopy.begin(), vctCopy.end(), compare);
    end = clock();
    times[2][0] = double(end - start) / (double) CLOCKS_PER_SEC;

    // Поиск первым способом. Найти один раз все объекты из исходных данных.
    start = clock();
    for (int i = 0; i < vct.size(); ++i)
    {
        tree.find(vct[i]);
    }
    end = clock();
    times[0][1] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < vct.size(); ++i)
    {
        table.find(vct[i]);
    }
    end = clock();
    times[1][1] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < vct.size(); ++i)
    {
        lower_bound(vctCopy.begin(), vctCopy.end(), vct[i], compare);
    }
    end = clock();
    times[2][1] = double(end - start) / (double) CLOCKS_PER_SEC;


    // Поиск вторым способом. Сгенерировать 2*N случайных объектов и произвести поиск их среди исходных данных.
    start = clock();
    for (int i = 0; i < vctForSearch.size(); ++i)
    {
        tree.find(vctForSearch[i]);
    }
    end = clock();
    times[0][2] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < vctForSearch.size(); ++i)
    {
        table.find(vctForSearch[i]);
    }
    end = clock();
    times[1][2] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < vctForSearch.size(); ++i)
    {
        lower_bound(vctCopy.begin(), vctCopy.end(), vctForSearch[i], compare);
    }
    end = clock();
    times[2][2] = double(end - start) / (double) CLOCKS_PER_SEC;


    // Сравнение времени удаления данных по одному элементу.

    start = clock();
    for (int i = 0; i < vct.size(); ++i)
    {
        tree.remove(vct[i]);
    }
    end = clock();
    times[0][4] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < vct.size(); ++i)
    {
        table.remove(vct[i]);
    }
    end = clock();
    times[1][4] = double(end - start) / (double) CLOCKS_PER_SEC;

    times[2][4] = 0;


    // Освобождение ресурсов используемых алгоритмом.

    for (int i = 0; i < vct.size(); ++i)
    {
        tree.add(&vct[i]);
        table.add(&vct[i]);
    }

    start = clock();
    tree.clear();
    end = clock();
    times[0][3] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    table.clear();
    end = clock();
    times[1][3] = double(end - start) / (double) CLOCKS_PER_SEC;

    start = clock();
    vct.clear();
    end = clock();
    times[2][3] = double(end - start) / (double) CLOCKS_PER_SEC;
    return times;
};

void printTable(array<array<double, 5>, 3> times, int n, ofstream &file)
{

    file << n << ", Время сортировки/внесения данных, Время поиска №1, Время поиска №2, Время освобождения, Удаление по одному" << std::endl;

    for (int i = 0; i < 3; ++i)
    {
        if (i == 0)
        {
            file << "АВЛ дерево, ";
        }
        else if (i == 1)
        {
            file << "Хеш-таблица, ";
        }
        else if (i == 2)
        {
            file << "Отсортированный массив, ";
        }
        for (int j = 0; j < 5; ++j)
        {
            file << times[i][j] << ", ";
        }
        file << std::endl;
    }
    file << std::endl;

}

int main()
{

    setlocale(LC_CTYPE, "rus");
    srand(time(0));

    vector<array<array<double, 5>, 3>> timesTables;
    for (int i = FROM; i <= TO; i += STEP)
    {
        array<array<double, 5>, 3> tmpTable = countTimes(i);
        timesTables.push_back(tmpTable);
    }

    ofstream file;
    file.open("graphs3.csv");
    file << "Сортировка/внесение данных" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP)
    {
        file << i << ", ";
    }
    file << std::endl;
    file << "АВЛ дерево, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][0][0] << ", ";
    }
    file << std::endl;
    file << "Хеш таблица, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][1][0] << ", ";
    }
    file << std::endl;
    file << "Массив, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][2][0] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Поиск 1" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP)
    {
        file << i << ", ";
    }
    file << std::endl;
    file << "АВЛ дерево, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][0][1] << ", ";
    }
    file << std::endl;
    file << "Хеш таблица, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][1][1] << ", ";
    }
    file << std::endl;
    file << "Массив, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][2][1] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Поиск 2" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP)
    {
        file << i << ", ";
    }
    file << std::endl;
    file << "АВЛ дерево, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][0][2] << ", ";
    }
    file << std::endl;
    file << "Хеш таблица, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][1][2] << ", ";
    }
    file << std::endl;
    file << "Массив, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][2][2] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Освобождение всего" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP)
    {
        file << i << ", ";
    }
    file << std::endl;
    file << "АВЛ дерево, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][0][3] << ", ";
    }
    file << std::endl;
    file << "Хеш таблица, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][1][3] << ", ";
    }
    file << std::endl;
    file << "Массив, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][2][3] << ", ";
    }

    file << std::endl;
    file << std::endl;
    file << "Удаление по одному" << std::endl;
    file << ", ";
    for (int i = FROM; i < TO; i += STEP)
    {
        file << i << ", ";
    }
    file << std::endl;
    file << "АВЛ дерево, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][0][4] << ", ";
    }
    file << std::endl;
    file << "Хеш таблица, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][1][4] << ", ";
    }
    file << std::endl;
    file << "Массив, ";
    for (int i = 0; i < timesTables.size(); ++i)
    {
        file << timesTables[i][2][4] << ", ";
    }

    file.close();

    return 0;
}