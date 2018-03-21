/* 
 * Fork: https://github.com/marcmo/bitstream
 * File:   BiteStream.h
 * Edited: Adrian Peniak
 *
 * Created on February 7, 2018, 4:07 PM
 */

#ifndef BITESTREAM_H
#define BITESTREAM_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cassert>

class BiteStream {
public:
    BiteStream()
    : m_data(),
      m_offset(0),
      m_size(0) {
    }
    
    BiteStream(std::vector<uint8_t>&& data)
    : m_data(std::move(data)),
      m_offset(0),
      m_size(m_data.size()*8)
    {
    }
    
    ~BiteStream() = default;
    
    BiteStream(BiteStream&& other)
    : m_data(std::move(other.m_data)), 
      m_offset(other.m_offset),
      m_size(m_data.size()*8)
    {
    }
    
    BiteStream& operator=(BiteStream&& other) {
        m_data = std::move(other.m_data);
        m_offset = other.m_offset;
        m_size = m_data.size()*8;
        return *this;
    }
    
    template <class T>
    void put(T value, size_t size = 0)
    {
        if(size == 0) {
            size = sizeof(value)*8;
        }
        size_t index = (m_offset) / 8;
        size_t offsetInByte = (m_offset) % 8;
        size_t spaceLeftInFirstByte = 8 - offsetInByte;
        size_t bitsForFirstByte = std::min(size, spaceLeftInFirstByte);
        size_t resize = ((size + m_offset)%8) ? (size + m_offset)/8 + 1 : (size + m_offset)/8;
        if(resize != m_data.size()) {
            m_data.resize(resize);
        }
        if (size <= spaceLeftInFirstByte) {
            T mask = 0;
            std::memset(&mask, 255, sizeof(mask));
            mask >>= sizeof(mask)*8 - size;
            m_data[index++] |= (value & mask) << (spaceLeftInFirstByte - size);
            m_offset += size;
        } else {
            int remainingBits = size - bitsForFirstByte;
            uint8_t m = mask(offsetInByte, spaceLeftInFirstByte);
            m_data[index++] |= (value >> (size - spaceLeftInFirstByte)) & m;
            m_offset += spaceLeftInFirstByte;
            while (remainingBits) {
                if (remainingBits <= 8) {
                    m_data[index++] |=
                        (value << (8 - remainingBits)) & mask(0, remainingBits);
                    m_offset += remainingBits;
                    remainingBits = 0;
                } else {
                    m_data[index++] |= value >> (remainingBits - 8);
                    m_offset += 8;
                    remainingBits -= 8;
                }
            }
        }
        m_size = (m_size < m_offset) ? m_offset : m_size;
    }
    
    
    
    template<class T>
    void put(const T begin, const T end) {
        if(m_offset%8) {
            m_data.insert(m_data.end(), begin, end);
            m_size += (std::distance(begin, end)*8);
        } else {
            for(T p = begin; p != end; ++p) {
                put<uint8_t>(*p);
            }
        }
    }
    
    template<class T>
    void putToEnd(const T begin, const T end) {
        m_offset = m_size;
        put(begin, end);
    }
    
    template<class T>
    T get(size_t size = 0)
    {
        T res = 0;
        if(size == 0) {
            size = sizeof(res)*8;
        }
        size_t index = (m_offset) / 8;
        size_t offsetInByte = (m_offset) % 8;
        size_t remainingInFirstByte = 8 - offsetInByte;
        size_t bitsInFirstByte = std::min(size, remainingInFirstByte);
        res |= m_data[index++] & mask(offsetInByte, bitsInFirstByte);
        m_offset += bitsInFirstByte;
        size_t remainingBits = size - bitsInFirstByte;
        if (!remainingBits) {
            res >>= 8 - (size + offsetInByte);
        }
        while (remainingBits > 0) {
            if (remainingBits <= 8) {
                res <<= remainingBits;
                uint8_t masked = m_data[index++] & (mask(0, remainingBits));
                res |= masked >> (8 - remainingBits);
                m_offset += remainingBits;
                remainingBits = 0;
            } else {
                size_t elementSize = 8;
                res <<= std::min(elementSize, remainingBits);
                res |= m_data[index++];
                m_offset += 8;
                remainingBits -= 8;
            }
        }
        return res;
    }
    
    
    template<class T>
    T getWithOffset(size_t offset, size_t size = 0) {
        m_offset = offset;
        return get<T>(size);
    }
    
    std::vector<uint8_t> getRest() {
        size_t rest = 8 - m_offset % 8;
        if(rest != 8) {
            std::vector<uint8_t> ret{};
            ret.push_back(get<uint8_t>(rest));
            ret.insert(ret.end(), m_data.begin() + (m_offset / 8), m_data.end());
            return ret;
        }
        return std::vector<uint8_t>(m_data.begin() + (m_offset / 8), m_data.end());
    }
    
    const std::vector<uint8_t>& getData() const {
        return m_data;
    }
        
    std::vector<uint8_t> releaseData() {
        return std::move(m_data);
    }
    
    void reset() {
        m_offset = 0;
    }
    
    BiteStream(BiteStream&) = delete;
    BiteStream& operator=(BiteStream&) = delete;
    
private:
    uint8_t mask(uint8_t from, uint8_t length)
    {
        assert((from + length) <= 8);
        uint8_t res = 0;
        for (int i = 0; i < length; ++i) {
            res |= 1 << i;
        }
        res = res << (8 - (from + length));
        return res;
    }
    
private:
    std::vector<uint8_t> m_data;
    size_t m_offset;
    size_t m_size;
};

#endif /* BITESTREAM_H */
