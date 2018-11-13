#ifndef MEMORY_MANAGER_HEAD_H_2015_03_06
#define MEMORY_MANAGER_HEAD_H_2015_03_06

namespace memorymanager // TODO �������� �� ���� ��������
{

    template <class T>
    class CMemoryManager
    {
    private:
        struct block
        {
            T* pdata;
            block *pnext;
            int firstFreeIndex;
            int usedCount;
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
        CMemoryManager(int _default_block_size, bool isDeleteElementsOnDestruct = false)
        {
            m_blkSize = _default_block_size;
            m_pBlocks = 0;
            m_pCurrentBlk = 0;
            m_isDeleteElementsOnDestruct = isDeleteElementsOnDestruct;
        }
        virtual ~CMemoryManager()
        {
            clear();
        }

        T* newObject()
        {
            if (m_pCurrentBlk == 0)
            {
                m_pBlocks = newBlock();
                m_pCurrentBlk = m_pBlocks;
            }

            if (m_pCurrentBlk->firstFreeIndex == -1) //full block
            {
                block *Block = m_pBlocks;
                block *prevBlock = 0;
                while (Block != 0 && Block->firstFreeIndex == -1) {
                    prevBlock = Block;
                    Block = Block->pnext;
                }

                if (Block != 0) {
                    m_pCurrentBlk = Block;
                } else //all blocks full
                {
                    m_pCurrentBlk = newBlock();
                    prevBlock->pnext = m_pCurrentBlk;
                }
            }

            int i = m_pCurrentBlk->firstFreeIndex;
            m_pCurrentBlk->firstFreeIndex = *(reinterpret_cast<int*>((m_pCurrentBlk->pdata) + i));

            new (m_pCurrentBlk->pdata + i) T;
            m_pCurrentBlk->usedCount++;

            return m_pCurrentBlk->pdata + i;
        }


        bool deleteObject(T* p)
        {
            block* tmpBlock = m_pBlocks;

            while (tmpBlock != 0 && (tmpBlock->pdata > p || tmpBlock->pdata + m_blkSize - 1 < p)) // find block
            {
                tmpBlock = tmpBlock->pnext;
            }

            if (tmpBlock == 0)
            {
                return false;
            }

            int i = p - tmpBlock->pdata; // remember index in block
            p->~T();
            tmpBlock->usedCount--;
            *(reinterpret_cast<int*>(p)) = tmpBlock->firstFreeIndex;
            tmpBlock->firstFreeIndex = i;

            return true;
        }

        void clear()
        {
            if (m_pBlocks == 0)
            {
                return;
            }

            if (!m_isDeleteElementsOnDestruct)
            {
                block* tmpBlock = m_pBlocks;

                while (tmpBlock != 0)
                {
                    if (tmpBlock->usedCount != 0) // check that blocks are empty
                    {
                        throw CException();
                    }
                    tmpBlock = tmpBlock->pnext;
                }

                tmpBlock = m_pBlocks;
                block *nextBlock = 0;
                while (tmpBlock != 0)
                {
                    nextBlock = tmpBlock->pnext;
                    delete[] tmpBlock->pdata;
                    delete tmpBlock;
                    tmpBlock = nextBlock;
                }
            }
            else
            {
                block* tmpBlock = m_pBlocks;
                block* nextBlock = 0;
                while (tmpBlock != 0)
                {
                    nextBlock = tmpBlock->pnext;
                    deleteBlock(tmpBlock);
                    tmpBlock = nextBlock;
                }
            }
            m_pCurrentBlk = 0;
            m_pBlocks = 0;
        }

    private:
        block* newBlock()
        {
            block* newBlock = new block;

            newBlock->firstFreeIndex = 0;
            newBlock->pnext = 0;
            newBlock->usedCount = 0;

            newBlock->pdata = reinterpret_cast<T*>(new char[m_blkSize * sizeof(T)]);
            T* newBlockData = newBlock->pdata;

            for (int i = 0; i < m_blkSize - 1; ++i)
            {
                *(reinterpret_cast<int*>(newBlockData + i)) = i + 1;
            }

            *(reinterpret_cast<int*>(newBlockData + m_blkSize - 1)) = -1;
            return newBlock;
        }

        void deleteBlock(block *p)
        {
            /*T* blockData = p->pdata;
            for (int i = 0; i < p->firstFreeIndex; ++i)
            {
                (blockData + i)->~T();
            }

            delete[] blockData;
            delete p;
             */
            bool* isDelete = new bool[m_blkSize];

            for (int i = 0; i < m_blkSize; ++i)
            {
                isDelete[i] = true;
            }

            T* pBlockData = p->pdata;
            int i = p->firstFreeIndex;

            while (i != -1)
            {
                isDelete[i] = false;
                i = *(reinterpret_cast<int*>(pBlockData + i));
            }

            for (int i = 0; i < m_blkSize; ++i)
            {
                if (isDelete[i])
                {
                    (pBlockData + i)->~T();
                }
            }

            delete[] pBlockData;
            delete p;
        }

        int m_blkSize;
        block* m_pBlocks;
        block *m_pCurrentBlk;
        bool m_isDeleteElementsOnDestruct;
    };

}; // namespace templates

#endif // #define MEMORY_MANAGER_HEAD_H_2015_03_06
