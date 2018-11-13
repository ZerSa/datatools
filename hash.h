#ifndef HASH_HEAD_H_2015_03_12
#define HASH_HEAD_H_2015_03_12

#include "mm.h" // TODO ������������ ���� memorymanager

using namespace memorymanager;

namespace hash_table
{

    template <class T, unsigned int(*HashFunc)(const T* pElement), int(*Compare)(const T *pElement, const T* pElement2) >
    class CHash
    {
    private:
        struct leaf
        {
            T* pData;
            leaf *pnext;
        };

    public:
        class CException
        {
        public:
            CException()
            {
            }
        };

    public:
        CHash(int hashTableSize, int default_block_size) : m_Memory(CMemoryManager<leaf>(default_block_size, true)),
                                                           m_tableSize(hashTableSize)
        {
            m_pTable = new leaf *[hashTableSize];
            for (int i = 0; i < hashTableSize; ++i)
            {
                m_pTable[i] = 0;
            }
        }
        virtual ~CHash()
        {
            clear();
            delete[] m_pTable;
        }

        bool add(T* pElement)
        {
            if (pElement == 0)
            {
                throw CException();
            }

            unsigned int index = 0;
            if (findLeaf(pElement, index) != 0)
            {
                return false;
            }

            leaf* newLeaf = m_Memory.newObject();
            newLeaf->pData = pElement;
            newLeaf->pnext = m_pTable[index];

            m_pTable[index] = newLeaf;
            return true;

        }
        bool update(T* pElement)
        {
            if (pElement == 0)
            {
                throw CException();
            }

            unsigned int index = 0;
            leaf* leafPlace = findLeaf(pElement, index);

            if (leafPlace == 0)
            {
                leaf* newLeaf = m_Memory.newObject();
                newLeaf->pData = pElement;
                newLeaf->pnext = m_pTable[index];

                m_pTable[index] = newLeaf;
            }
            else
            {
                leafPlace->pData = pElement;
            }

            return true;
        }


        T* find(const T& pElement)
        {
            unsigned int index = 0;
            leaf* leafPlace = findLeaf(&pElement, index);
            if (leafPlace == 0)
            {
                return 0;
            }
            else
            {
                return leafPlace->pData;
            }
        }

        bool remove(const T& element)
        {
            unsigned int index = HashFunc(&element) % m_tableSize;
            leaf* pCurrent = m_pTable[index];
            leaf* pPrev = 0;
            while (pCurrent != 0 && Compare(&element, pCurrent->pData) != 0)
            {
                pPrev = pCurrent;
                pCurrent = pCurrent->pnext;
            }

            if (pCurrent == 0)
            {
                return false;
            }

            if (pPrev == 0)
            {
                m_pTable[index] = pCurrent->pnext;
            }
            else
            {
                pPrev->pnext = pCurrent->pnext;
            }
            m_Memory.deleteObject(pCurrent);
            return true;
        }

        void clear()
        {
            m_Memory.clear();
            for (int i = 0; i < m_tableSize; ++i)
            {
                m_pTable[i] = 0;
            }
        }
    private:
        // ���������� ��������� ���� � � ���������� idx ���������� ���������� ������ ���-�������
		// ������ ������� ������� ������������ � add, update, find
        leaf *findLeaf(const T* pElement, unsigned int & idx)
        {
            idx = HashFunc(pElement) % m_tableSize;
            leaf *pCurrent = m_pTable[idx];
            while (pCurrent != 0 && Compare(pElement, pCurrent->pData) != 0)
            {
                pCurrent = pCurrent->pnext;
            }
            return pCurrent;
        }

        int m_tableSize;
        leaf** m_pTable;
        CMemoryManager<leaf> m_Memory;
    };

}; // namespace templates

#endif // #define HASH_HEAD_H_2015_03_12
