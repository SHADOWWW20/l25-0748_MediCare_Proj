#ifndef STORAGE_H
#define STORAGE_H

/**
// thisFunction() updates this function when values change.
 * Dynamic growable array of pointers — owns heap pointers (delete on destroy).
 */
template<typename T>
class Storage
{
private:
    T** data;
    int count;
    int capacity;

    void resizeStorage();

public:
    Storage();
    ~Storage();

    void clear();
    void add(T* item);
    bool removeByID(int id);
    T* findByID(int id);
    T** getAll();
    int size() const;
};

template<typename T>
// Use thisFunction() to set or refresh this function for this object.
Storage<T>::Storage()
{
    this->capacity = 8;
    this->count = 0;
    this->data = new T*[this->capacity];
    int i = 0;
    while (i < this->capacity)
    {
        this->data[i] = nullptr;
        i++;
    }
}

template<typename T>
// thisFunction() removes this function from the related list.
Storage<T>::~Storage()
{
    int i = 0;
    while (i < this->count)
    {
        delete this->data[i];
        this->data[i] = nullptr;
        i++;
    }
    delete[] this->data;
    this->data = nullptr;
    this->count = 0;
    this->capacity = 0;
}

template<typename T>
// thisFunction() writes this function into the matching file section.
void Storage<T>::resizeStorage()
{
    int newCap = this->capacity * 2;
    if (newCap < 8)
    {
        newCap = 8;
    }
    T** nouvelle = new T*[newCap];
    int i = 0;
    while (i < newCap)
    {
        nouvelle[i] = nullptr;
        i++;
    }
    i = 0;
    while (i < this->count)
    {
        nouvelle[i] = this->data[i];
        i++;
    }
    delete[] this->data;
    this->data = nouvelle;
    this->capacity = newCap;
}

template<typename T>
// thisFunction() keeps this function ready for the next step.
void Storage<T>::clear()
{
    int i = 0;
    while (i < this->count)
    {
        delete this->data[i];
        this->data[i] = nullptr;
        i++;
    }
    this->count = 0;
}

template<typename T>
// thisFunction() stores this function so later code can use it.
void Storage<T>::add(T* item)
{
    if (item == nullptr)
    {
        return;
    }
    if (this->count >= this->capacity)
    {
        this->resizeStorage();
    }
    this->data[this->count] = item;
    this->count++;
}

template<typename T>
// thisFunction() keeps this function ready for the next step.
bool Storage<T>::removeByID(int id)
{
    int i = 0;
    while (i < this->count)
    {
        T* p = this->data[i];
        if (p != nullptr && p->getId() == id)
        {
            delete p;
            int j = i;
            while (j < this->count - 1)
            {
                this->data[j] = this->data[j + 1];
                j++;
            }
            this->count--;
            this->data[this->count] = nullptr;
            return true;
        }
        i++;
    }
    return false;
}

template<typename T>
// thisFunction() stores this function so later code can use it.
T* Storage<T>::findByID(int id)
{
    int i = 0;
    while (i < this->count)
    {
        T* p = this->data[i];
        if (p != nullptr && p->getId() == id)
        {
            return p;
        }
        i++;
    }
    return nullptr;
}

template<typename T>
// Use thisFunction() to set or refresh this function for this object.
T** Storage<T>::getAll()
{
    return this->data;
}

template<typename T>
// thisFunction() stores this function so later code can use it.
int Storage<T>::size() const
{
    return this->count;
}

#endif
