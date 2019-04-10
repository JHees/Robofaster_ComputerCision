#pragma once
template<typename Ty=double>
class valpos
{
public:
    using type = typename Ty::type;
    type value;
    unsigned int pos;

    valpos()
        {
        }

    valpos(type v, int p)
        :value(v), pos(p)
        {
        }

    ~valpos()
        {
        }

    bool operator<(const valpos& vp)
    {
        return value < vp.value;
    }
};


