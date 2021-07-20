#ifndef MIOSIX_AUDIO_CIRCULAR_BUFFER_H
#define MIOSIX_AUDIO_CIRCULAR_BUFFER_H

#include <array>


/**
 * Collection of structs to be passed as a template argument to CircularBuffer
 * to choose a policy for the overflow behaviour
 */
namespace CircularBufferType
{
    /**
     * When adding an element to a full CircularBuffer, discard it
     */
    struct Discard {};

    /**
     * When adding an element to a full CircularBuffer, overwrite the head
     */
    struct Overwrite {};
}





/**
 * Iterator used by CircularBuffer
 * @tparam CircularBuffer
 */
template <typename CircularBuffer>
class ConstCircularBufferIterator : public std::iterator<std::bidirectional_iterator_tag, typename CircularBuffer::ValueType>
{
public:
    using ValueType = const typename CircularBuffer::ValueType;
    using PointerType = const typename CircularBuffer::PointerType;
    using ReferenceType = const typename CircularBuffer::ReferenceType;

public:
    ConstCircularBufferIterator(CircularBuffer* circularBuffer, size_t startPosition)
            :   _buffer(circularBuffer),
                _position(startPosition) {}
    /**
    * Equals comparison operator
    */
    bool operator== (const ConstCircularBufferIterator& other) const {
        return _position == other._position && _buffer == other._buffer;
    }

    /**
     * Not-equals comparison operator
     * @see operator==(const ConstCircularBufferIterator&) const
     */
    bool operator!= (const ConstCircularBufferIterator& other) const {
        return !(*this == other);
    }

    /**
     * Dereference operator
     * @return the value of the element this iterator is currently pointing at
     */
    const ReferenceType operator*() {
        return (*_buffer)[_position];
    }

    /**
     * Prefix decrement operator (--it)
     */
    ConstCircularBufferIterator &operator--(){
        --_position;
        return *this;
    }

    /**
     * Postfix decrement operator (it--)
     */
    ConstCircularBufferIterator operator--(int){
        // Use operator--()
        const ConstCircularBufferIterator old(*this);
        --(*this);
        return old;
    }

    /**
     * Prefix increment operator (++it)
     */
    ConstCircularBufferIterator &operator++(){\
        ++_position;
        return *this;
    }

    /**
     * Postfix increment operator (it++)
     */
    ConstCircularBufferIterator operator++(int){
        // Use operator++()
        const ConstCircularBufferIterator old(*this);
        ++(*this);
        return old;
    }

private:
    CircularBuffer *_buffer;
    size_t  _position;
};




/**
 * This class implements a circular buffer with different overflow policies
 *
 * @tparam T  Type to be used in the collection
 * @tparam BufferSize Max Buffer size of the circular buffer
 * @tparam OverflowPolicy Defaults to overwrite behaviour @see CircularBufferType for more options
 */
template<typename T, size_t BufferSize, typename OverflowPolicy = CircularBufferType::Overwrite>
class CircularBuffer
{
public:
    using ValueType = T;
    using PointerType = T*;
    using ReferenceType = T&;

    typedef size_t      size_type;
    typedef ptrdiff_t    difference_type;

public:
    /**
     * Constructor
     */
    CircularBuffer() : _head(1), _tail(0), _size(0) {}


    /**
     * Function to get the first element of the CircularBuffer as const
     *
     * @return First element as const
     */
    inline const ReferenceType front()
    {
        return _buffer[_head];
    }

    /**
     * Function to get the last element of the CircularBuffer as const
     *
     * @return Last element as const
     */
    inline const ReferenceType back()
    {
        return _buffer[_tail];
    }

    /**
     * Resets the state of the buffer.
     */
    inline void clear()
    {
        _head = 1;
        _tail = _size = 0;
    }

    /**
     * Get the actual number of elements contained by the buffer
     *
     * @return Number of elements in the buffer
     */
    inline size_type size() const
    {
        return _size;
    }

    /**
     * Maximum number of elements that can be contained by the CircularBuffer
     *
     * @return Maximum number of elements
     */
    inline size_type max_size() const
    {
        return BufferSize;
    }

    /**
     * Checks if the buffer is empty.
     *
     * @return true if the buffer is empty
     */
    inline bool empty() const
    {
        return _size == 0;
    }

    /**
     * Pushes a new element in the buffer.
     * If the buffer overflows the new element
     * it overwrites the oldest element.
     * Overwrite Template version, @see CircularBufferType::Overwrite
     *
     * @param item new element
     */
    template <typename Q = OverflowPolicy>
    typename std::enable_if<std::is_same<Q, CircularBufferType::Overwrite>::value, void>::type
    push(ValueType item)
    {
        if (_size == BufferSize)
        {
            pop();
        }
        advance_tail();
        _buffer[_tail] = item;
    }

    /**
     * Pushes a new element in the buffer.
     * If the buffer overflows the new element
     * it discards the element.
     * Discard Template version, @see CircularBufferType::Discard
     *
     * @param item new element
     */
    template <typename Q = OverflowPolicy>
    typename std::enable_if<std::is_same<Q, CircularBufferType::Discard>::value, void>::type
    push(ValueType item)
    {
        if (_size == BufferSize)
        {
            return;
        }
        advance_tail();
        _buffer[_tail] = item;
    }

    /**
     * Removes the front element from the buffer
     */
    void pop()
    {
        if (_size == 0) return;

        ++_head;
        --_size;
        if (_head == BufferSize)
            _head = 0;
    }


public:

    /**
     * Iterator
     */
    typedef ConstCircularBufferIterator<CircularBuffer> const_iterator;
    friend const_iterator;

    /**
     * Iterator begin
     * @return Begin iterator
     */
    const_iterator begin()
    {
        return const_iterator(this, 0);
    }

    /**
     * Iterator end
     * @return  End iterator
     */
    const_iterator end()
    {
        return const_iterator(this, _size);
    }

private:
    /**
     * Auxiliary method to advance the tail and size pointers,
     * usually used to add new items to the buffer
     */
    void advance_tail()
    {
        ++_tail;
        ++_size;
        if (_tail == BufferSize)
            _tail = 0;
    }

    /**
    * Auxiliary random access operator, undefined behaviour if index is out of bounds
    * @param index Logical index of the chosen element
    * @return Element at logical position index
    */
    const ReferenceType operator[](size_type index)
    {
        size_type i = (_head + index) % BufferSize;
        return _buffer[i];
    }

private:
    /**
     * Underlying buffer to be used as circular
     */
    std::array<T, BufferSize> _buffer;

    /**
     * Head position
     */
    size_t _head;

    /**
     * Tail position
     */
    size_t _tail;

    /**
     * Number of elements actually contained by the buffer
     */
    size_t _size;

};


#endif //MIOSIX_AUDIO_CIRCULAR_BUFFER_H