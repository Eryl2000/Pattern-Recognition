#ifndef EIGENFACE_ERROR_H_
#define EIGENFACE_ERROR_H_

class EigenfaceError
{
    public:
        EigenfaceError();
        EigenfaceError(int _index, float _error = 0);
        EigenfaceError(const EigenfaceError & other);

        EigenfaceError & operator=(const EigenfaceError & other);
        int index;
        float error;

        bool operator <(const EigenfaceError & rhs) const;
};

#endif