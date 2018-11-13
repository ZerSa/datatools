#ifndef AVL_HEAD_H_2015_03_31
#define AVL_HEAD_H_2015_03_31

#include "mm.h" // TODO ������������ ���� memorymanager

using namespace memorymanager;

namespace avl_tree // TODO �������� �� ���� ��������
{

    template <class T, int(*Compare)(const T *pElement, const T* pElement2) >
    class CAVLTree
    {
    private:
        struct leaf
        {
            T* pData;
            leaf* pLeft;
            leaf* pRight;
            int balanceFactor;

            leaf(T* data) : pData(data), pLeft(0), pRight(0), balanceFactor(1) {}
            leaf() {}
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
        CAVLTree() : m_Memory(128, true), m_pRoot(0) {}

        virtual ~CAVLTree()
        {
            clear();
            delete (m_pRoot);
        }

        bool add(T* pElement)
        {
            if (pElement == 0)
            {
                throw CException();
            }

            if (m_pRoot == 0)
            {
                leaf* newLeaf = m_Memory.newObject();
                *newLeaf = leaf(pElement);
                m_pRoot = new leaf*(newLeaf);
                return true;
            }
            else
            {
                bool added = false;
                insert(*m_pRoot, pElement, added);
                return added;
            }
        }

        bool update(T* pElement)
        {
            if (pElement == 0)
            {
                throw CException();
            }
            return updateFrom(*m_pRoot, pElement);
        }
      
        T* find(const T& pElement)
        {
            if (&pElement == 0)
            {
                throw CException();
            }
            if (m_pRoot == 0)
                return 0;
            return findFrom(*m_pRoot, pElement);
        }

        bool remove(const T& element)
        {
            if (&element == 0)
            {
                throw CException();
            }
            bool removed = false;
            removeFrom(*m_pRoot, element, removed);
            return removed;
        }

        void clear()
        {
            m_Memory.clear();
        }

    private:
        leaf** m_pRoot;
        CMemoryManager<leaf> m_Memory;

        int bfactor(leaf *p)
        {
            return balanceFactor(p->pRight) - balanceFactor(p->pLeft);
        }

        int balanceFactor(leaf *p)
        {
            if (p != 0)
            {
                return p->balanceFactor;
            } else
            {
                return 0;
            }
        }

        leaf *rotateRight(leaf *p)
        {
            leaf *q = p->pLeft;
            p->pLeft = q->pRight;
            q->pRight = p;
            updateBalanceFactor(p);
            updateBalanceFactor(q);

            if (*m_pRoot == q->pRight)
            {
                *m_pRoot = q;
            }
            return q;
        }

        leaf *rotateLeft(leaf *p)
        {
            leaf *q = p->pRight;
            p->pRight = q->pLeft;
            q->pLeft = p;
            updateBalanceFactor(p);
            updateBalanceFactor(q);
            if (*m_pRoot == q->pLeft)
            {
                *m_pRoot = q;
            }
            return q;
        }

        void updateBalanceFactor(leaf *p)
        {
            int balanceLeft = balanceFactor(p->pLeft);
            int balanceRight = balanceFactor(p->pRight);
            if (balanceLeft > balanceRight)
            {
                p->balanceFactor = balanceLeft;
            } else
            {
                p->balanceFactor = balanceRight;
            }
            ++p->balanceFactor;
        }

        leaf *balance(leaf *p)
        {
            updateBalanceFactor(p);
            if (bfactor(p) == 2)
            {
                if (bfactor(p->pRight) < 0)
                {
                    p->pRight = rotateRight(p->pRight);
                }
                return rotateLeft(p);
            }
            if (bfactor(p) == -2)
            {
                if (bfactor(p->pLeft) > 0)
                {
                    p->pLeft = rotateLeft(p->pLeft);
                }
                return rotateRight(p);
            }
            return p;
        }

        leaf* insert(leaf* p, T* k, bool &added)
        {
            if (p == 0)
            {
                leaf *newLeaf = m_Memory.newObject();
                *newLeaf = leaf(k);
                added = true;
                return newLeaf;
            }
            if (Compare(k, p->pData) < 0)
            {
                p->pLeft = insert(p->pLeft, k, added);
            }
            else if (Compare(k, p->pData) > 0)
            {
                p->pRight = insert(p->pRight, k, added);
            }
            else
            {
                added = false;
            }
            return balance(p);
        }

        leaf* findMinimum(leaf* p)
        {
            if (p->pLeft)
            {
                return findMinimum(p->pLeft);
            }
            else
            {
                return p;
            }
        }

        leaf* removeMinimum(leaf* p)
        {
            if (p->pLeft == 0)
            {
                return p->pRight;
            }
            p->pLeft = removeMinimum(p->pLeft);
            return balance(p);
        }

        leaf* removeFrom(leaf* p, const T &k, bool &removed)
        {
            if (p == 0)
            {
                return 0;
            }
            if (Compare(&k, p->pData) < 0)
            {
                p->pLeft = removeFrom(p->pLeft, k, removed);
            }
            else if (Compare(&k, p->pData) > 0)
            {
                p->pRight = removeFrom(p->pRight, k, removed);
            } else
            {
                if ((*m_pRoot)->pRight == 0 && (*m_pRoot)->pLeft == 0)
                {
                    delete m_pRoot;
                    removed = true;
                    m_pRoot = 0;
                    return 0;
                }
                leaf *l = p->pLeft;
                leaf *r = p->pRight;

                bool updateRootNeeded = false;

                if (*m_pRoot == p)
                {
                    updateRootNeeded = true;
                }
                else
                {
                    m_Memory.deleteObject(p);
                }
                removed = true;

                if (r == 0)
                {
                    if (updateRootNeeded)
                    {
                        *m_pRoot = l;
                    }
                    return l;
                }
                leaf* minLeaf = findMinimum(r);
                minLeaf->pRight = removeMinimum(r);
                minLeaf->pLeft = l;
                leaf *res = balance(minLeaf);
                if (updateRootNeeded)
                {
                    *m_pRoot = res;
                }
                return res;
            }
            return balance(p);
        }

        bool updateFrom(leaf* p, T* pElement)
        {
            if (p == 0)
            {
                return false;
            }
            if (Compare(pElement, p->pData) < 0)
            {
                return updateFrom(p->pLeft, pElement);
            }
            else if (Compare(pElement, p->pData) > 0)
            {
                return updateFrom(p->pRight, pElement);
            }
            else
            {
                p->pData = pElement;
                return true;
            }
        }

        T* findFrom(leaf* p, const T &element)
        {
            if (p == 0)
            {
                return 0;
            }
            if (Compare(&element, p->pData) < 0)
            {
                return findFrom(p->pLeft, element);
            }
            else if (Compare(&element, p->pData) > 0)
            {
                return findFrom(p->pRight, element);
            }
            else
            {
                return p->pData;
            }
        }
    };

}; // namespace templates

#endif // #define AVL_HEAD_H_2015_03_31
