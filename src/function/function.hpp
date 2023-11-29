#ifndef FUNCTION_H
#define FUNCTION_H

#include <memory>
#include <stdexcept>

namespace func {

template<typename R, typename ...Args>
class function_owner_int {
public:
    virtual R operator()(Args... args) const = 0;
    virtual std::unique_ptr< function_owner_int > clone() const = 0;

    virtual ~function_owner_int() {};
};

template<typename funcType, typename R, typename ...Args>
class function_owner: public function_owner_int<R, Args...> {

public:
    function_owner(funcType func):
        m_func(func)
    {}

    R operator()(Args... args) const {
        return m_func(args...);
    }

    /*compiler does not argue if clone is called from const function_owner*/
    std::unique_ptr< function_owner_int<R, Args...> > clone() const {
        return std::make_unique< function_owner >(*this);
    }

    ~function_owner() {}

private:
    mutable funcType m_func;
};


/*Template declaration*/
template<typename signature_Type>
class function;

/*Template specialization*/
/*Used to extract func sinature argument types and return type*/
template<typename R, typename ...Args>
class function<R(Args...)> {
public:

    /*Default Constructor*/
    function() {/*is there need to assign m_owner_ptr (?)*/}

    /*Constructor with initializer*/
    template<typename func_Type>
    function(
        func_Type target
    ):
        m_owner_ptr( 
            std::make_unique< function_owner<func_Type, R, Args...> >(
                target
            )
        )
    {}

    /*Copy-constructor*/
    function(const function& other) {
        if(other.m_owner_ptr) /*if it is not empty*/
            m_owner_ptr = other.m_owner_ptr->clone();
    }

    /*Assignment operator - object of same kind as receiving object*/
    function& operator=(function other) {
        swap(*this, other);
        return *this;
    }

    /*Assignment operator - object of function for target member*/
    template<typename func_Type>
    function& operator=(func_Type new_target) {
        assign(new_target);
        return *this;
    }

    /*Same as assignment operator*/
    template<typename func_Type>
    void assign(func_Type new_target) {
        m_owner_ptr = std::make_unique< function_owner<func_Type, R, Args...> >(new_target);
    }


    /*Execute target*/
    R operator()(Args... args) const {
        if(!m_owner_ptr) /*if m_owner_ptr does not point to valid data*/
            throw std::logic_error("function target is empty");

        return (*m_owner_ptr)(args...);
    }

    /*Conversion to bool - is it filled with valid func or not*/
    explicit operator bool() const {
        return static_cast<bool>(m_owner_ptr);
    }

    /*Empty the function*/
    void reset() {
        m_owner_ptr.reset();
    }

    // template<> /*is template<> needed (?)*/
    friend void swap(function& a, function& b) {
        using std::swap;

        swap(a.m_owner_ptr, b.m_owner_ptr);
    }

private:
    std::unique_ptr< function_owner_int<R, Args...> > m_owner_ptr;
};

}; // namespace func

#endif