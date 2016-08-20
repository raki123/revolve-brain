#ifndef REVOLVE_BRAIN_PYTHON_LIST_WRAP_H
#define REVOLVE_BRAIN_PYTHON_LIST_WRAP_H

#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>

/**
 * boost::python::list is not iterable, so we create a wrapper to make it iterable
 */
template < typename ValueType >
class python_list_wrap {
private:
    const boost::python::list *wrapped_list;
    unsigned int list_length; // cached list length value

public:
    python_list_wrap(const boost::python::list *list)
        : wrapped_list(list)
        , list_length(boost::python::len(*list))
    {}

    class iterator
    : public std::iterator< std::bidirectional_iterator_tag, ValueType >
    {
    private:
        const boost::python::list *wrapped_list;
        unsigned int index;

    public:
        iterator(const boost::python::list *list, const unsigned int index)
            : wrapped_list(list)
            , index(index)
        {}

        ValueType operator*() const {
            boost::python::object pyObject = (*wrapped_list)[index];
            return boost::python::extract<ValueType>(pyObject);
        }

        bool operator==(iterator const &rhs) const {
            return (this->wrapped_list == rhs.wrapped_list)
                && (this->index == rhs.index);
        }

        bool operator!=(iterator const &rhs) const {
            return (this->wrapped_list != rhs.wrapped_list)
                || (this->index != rhs.index);
        }

        void operator++() {
            this->index++;
        }

        void operator--() {
            this->index--;
        }
    };

    iterator begin() const {
        return iterator(wrapped_list, 0);
    }

    iterator end() const {
        return iterator(wrapped_list, list_length);
    }
};

#endif // REVOLVE_BRAIN_PYTHON_LIST_WRAP_H
