#ifndef PYTHON_ARRAY_H
#define PYTHON_ARRAY_H

template <typename ValueType>
class python_array
{
private:
    ValueType * const value_pointer;

public:
    python_array(ValueType * const array)
        : value_pointer(array)
    {}

    python_array(const python_array &array)
        : value_pointer(array.value_pointer)
    {}

    ValueType &operator[](const long index) const {
        return value_pointer[index];
    }

    ValueType getitem(const long key) const {
        return value_pointer[key];
    }

    void setitem(const long key, const ValueType value) {
        value_pointer[key] = value;
    }

};

#endif // PYTHON_ARRAY_H
