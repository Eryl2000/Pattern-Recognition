#include "EigenfaceError.h"

EigenfaceError::EigenfaceError() : index(0), error(0) {}

EigenfaceError::EigenfaceError(int _index, float _error) : index(_index), error(_error) {}

EigenfaceError::EigenfaceError(const EigenfaceError & other) : EigenfaceError(other.index, other.error) {}

EigenfaceError & EigenfaceError::operator=(const EigenfaceError & other)
{
    index = other.index;
    error = other.error;

    return *this;
}

bool EigenfaceError::operator <(const EigenfaceError & rhs) const
{
    return error < rhs.error;
}