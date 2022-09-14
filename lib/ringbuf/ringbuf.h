#pragma once
#include <stdint.h>
#include <strings.h>

namespace RingBufHelper
{
    template <bool fits_in_uint8_t>
    struct Index
    {
        using Type = uint16_t;       /* index of the buffer */
        using BiggerType = uint32_t; /* for intermediate calculation */
    };
    template <>
    struct Index<false>
    {
        using Type = uint8_t;        /* index of the buffer */
        using BiggerType = uint16_t; /* for intermediate calculation */
    };
}

template <
    typename ET,
    size_t S,
    typename IT = typename RingBufHelper::Index<(S > 255)>::Type,
    typename BT = typename RingBufHelper::Index<(S > 255)>::BiggerType>
class RingBuf
{
    /*
     * check the size is greater than 0, otherwise emit a compile time error
     */
    static_assert(S > 0, "RingBuf with size 0 are forbidden");

    /*
     * check the size is lower or equal to the maximum uint16_t value,
     * otherwise emit a compile time error
     */
    static_assert(S <= UINT16_MAX, "RingBuf with size greater than 65535 are forbidden");

private:
    ET mBuffer[S];
    IT mReadIndex;
    IT mSize;

    IT writeIndex();

public:
    /* Constructor. Init mReadIndex to 0 and mSize to 0 */
    RingBuf();
    /* Push a data at the end of the buffer */
    bool push(const ET inElement) __attribute__((noinline));
    /* Push a data at the end of the buffer. Copy it from its pointer */
    bool push(const ET *const inElement) __attribute__((noinline));
    /* Push a data at the end of the buffer with interrupts disabled */
    bool lockedPush(const ET inElement);
    /* Push a data at the end of the buffer with interrupts disabled. Copy it from its pointer */
    bool lockedPush(const ET *const inElement);
    /* Pop the data at the beginning of the buffer */
    bool shift(ET &outElement) __attribute__((noinline));
    /* Pop the data at the beginning of the buffer with interrupt disabled */
    bool lockedShift(ET &outElement);
    /* Return true if the buffer is full */
    bool isFull() { return mSize == S; }
    /* Return true if the buffer is empty */
    bool isEmpty() { return mSize == 0; }
    /* Reset the buffer  to an empty state */
    void clear() { mSize = 0; }
    /* return the size of the buffer */
    IT size() { return mSize; }
    /* return the maximum size of the buffer */
    IT maxSize() { return S; }
    /* access the buffer using array syntax, not interrupt safe */
    ET &operator[](IT inIndex);
};

template <typename ET, size_t S, typename IT, typename BT>
IT RingBuf<ET, S, IT, BT>::writeIndex()
{
    BT wi = (BT)mReadIndex + (BT)mSize;
    if (wi >= (BT)S)
        wi -= (BT)S;
    return (IT)wi;
}

template <typename ET, size_t S, typename IT, typename BT>
RingBuf<ET, S, IT, BT>::RingBuf() : mReadIndex(0),
                                    mSize(0)
{
}

template <typename ET, size_t S, typename IT, typename BT>
bool RingBuf<ET, S, IT, BT>::push(const ET inElement)
{
    if (isFull())
        return false;
    mBuffer[writeIndex()] = inElement;
    mSize++;
    return true;
}

template <typename ET, size_t S, typename IT, typename BT>
bool RingBuf<ET, S, IT, BT>::push(const ET *const inElement)
{
    if (isFull())
        return false;
    mBuffer[writeIndex()] = *inElement;
    mSize++;
    return true;
}

template <typename ET, size_t S, typename IT, typename BT>
bool RingBuf<ET, S, IT, BT>::lockedPush(const ET inElement)
{
    noInterrupts();
    bool result = push(inElement);
    interrupts();
    return result;
}

template <typename ET, size_t S, typename IT, typename BT>
bool RingBuf<ET, S, IT, BT>::lockedPush(const ET *const inElement)
{
    noInterrupts();
    bool result = push(inElement);
    interrupts();
    return result;
}

template <typename ET, size_t S, typename IT, typename BT>
bool RingBuf<ET, S, IT, BT>::shift(ET &outElement)
{
    if (isEmpty())
        return false;
    outElement = mBuffer[mReadIndex];
    mReadIndex++;
    mSize--;
    if (mReadIndex == S)
        mReadIndex = 0;
    return true;
}

template <typename ET, size_t S, typename IT, typename BT>
bool RingBuf<ET, S, IT, BT>::lockedShift(ET &outElement)
{
    noInterrupts();
    bool result = shift(outElement);
    interrupts();
    return result;
}

template <typename ET, size_t S, typename IT, typename BT>
ET &RingBuf<ET, S, IT, BT>::operator[](IT inIndex)
{
    if (inIndex >= mSize)
        return mBuffer[0];
    BT index = (BT)mReadIndex + (BT)inIndex;
    if (index >= (BT)S)
        index -= (BT)S;
    return mBuffer[(IT)index];
}