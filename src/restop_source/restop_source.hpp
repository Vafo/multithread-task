#ifndef RESTOP_SOURCE_H
#define RESTOP_SOURCE_H

#include <atomic>

namespace concurrency {
    
class restop_token {

public:
    restop_token() = default;
    restop_token(const restop_token& other) = default;
    ~restop_token() = default;

    restop_token& operator=(const restop_token& other) = default;

    bool stop_possible()
    { return static_cast<bool>(m_state_ref); }

    bool stop_requested()
    { return stop_possible() && m_state_ref->stop_requested(); }

private:
    friend class restop_source;

    struct _restop_state {
        using value_type = uint32_t;
        static const value_type _S_stop_requested_bit = 1;
        static const value_type _S_locked_bit = 2;
        static const value_type _S_ssrc_counter_inc = 4;

        std::atomic<value_type> owners{1};
        std::atomic<value_type> m_value{_S_ssrc_counter_inc};

        void add_ssrc();
        void remove_ssrc();
        void add_owner();
        void release_ownership();

        void lock();
        void unlock();
        bool try_lock();

        bool request_stop();
        bool stop_requested();

        bool reset_stop();
    }; // struct _restop_state

    struct _restop_state_ref {
        _restop_state_ref() = default;

        _restop_state_ref(const _restop_state_ref& other):
            m_state(other.m_state)
        {
            if(m_state)
                m_state->add_owner(); // TODO: increment owners
        }

        // constructor to allocate restop_state, used by restop_source
        explicit
        _restop_state_ref(const restop_source& ref_in):
            m_state(new _restop_state())
        {}

        _restop_state_ref& operator=(const _restop_state_ref& other) {
            if(m_state != other.m_state) {
                if(other.m_state)
                    other.m_state->add_owner();// TODO: increase owners
                if(m_state)
                    m_state->release_ownership();// TODO: decrease owners
                m_state = other.m_state;
            }
            return *this;
        }

        ~_restop_state_ref() {
            if(m_state)
                m_state->release_ownership(); // TODO: decrease num of owners
        }

        explicit operator bool() const
        { return m_state != nullptr; }

        _restop_state* operator->() const
        { return m_state; }

    private:
        _restop_state* m_state = nullptr;
    }; // struct _restop_state_ref


    // constructor for restop_source
    explicit
    restop_token(const _restop_state_ref& ref_in) {}

    _restop_state_ref m_state_ref;

}; // class restop_token

class restop_source {

public:
    restop_source():
        m_state_ref(*this)
    {}

    restop_source(const restop_source& other) = default;

    restop_source& operator=(const restop_source& other) = default;

    ~restop_source() = default;

    bool stop_possible()
    { return static_cast<bool>(m_state_ref); }

    bool stop_requested()
    { return stop_possible() && m_state_ref->stop_requested(); }

    bool request_stop()
    { return stop_possible() && m_state_ref->request_stop(); }

private:
    restop_token::_restop_state_ref m_state_ref;

}; // class restop_source

} // namespace concurrency

#endif