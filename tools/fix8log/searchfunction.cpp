#include "searchfunction.h"

SearchFunction::SearchFunction():id(-9999)
{
}
SearchFunction::SearchFunction(const SearchFunction &sf)
{
    id = sf.id;
    alias = sf.alias;
    function = sf.function;
}
SearchFunction & SearchFunction::operator=( const SearchFunction &rhs)
{
    if (this == &rhs)
        return *this;
    id       = rhs.id;
    alias    = rhs.alias;
    function = rhs.function;
    return *this;
}
bool SearchFunction::operator==(const SearchFunction &rhs)
{
    if (*this == rhs)
        return true;
    if ((id == rhs.id) &&(alias == rhs.alias) && (function== rhs.function))
        return true;
    else
        return false;
}
SearchFunctionList::SearchFunctionList(): QList<SearchFunction *>()
{

}
SearchFunction *SearchFunctionList::findByID(qint32 id)
{
    SearchFunction *sf = 0;
    if (count() < 1)
        return sf;
    QListIterator <SearchFunction *> iter(*this);
    while(iter.hasNext()) {
        sf = iter.next();
        if (sf->id == id)
            return sf;
    }
    return 0;
}

