/*
 * Credit 2005-2011 MaNGOS <http://getmangos.com/>
 * Credit 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Credit 2011-2016 Nostalrius <https://nostalrius.org>
 * Credit 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once
#include "net_common.h"
#include "ByteConverter.h"

class ByteBufferException
{
public:
    ByteBufferException(bool _add, size_t _pos, size_t _esize, size_t _size)
        : add(_add), pos(_pos), esize(_esize), size(_size)
    {
        PrintPosError();
    }

    void PrintPosError() const
    {
        std::cout << "Attempted to %s in ByteBuffer (pos: " << std::endl;
        //sLog.outError("Attempted to %s in ByteBuffer (pos: " SIZEFMTD " size: " SIZEFMTD ") value with size: " SIZEFMTD,
        //    (add ? "put" : "get"), pos, size, esize);
    }
private:
    bool add;
    size_t pos;
    size_t esize;
    size_t size;
};

template<class T>
struct Unused
{
    Unused() {}
};

class ByteBuffer
{
public:
	const static size_t DEFAULT_SIZE = 0x1000;

	ByteBuffer() : _rpos(0), _wpos(0)
	{
		_storage.reserve(DEFAULT_SIZE);
	}

	// constructor
	ByteBuffer(size_t res) : _rpos(0), _wpos(0)
	{
		_storage.reserve(res);
	}

	// copy constructor
	ByteBuffer(const ByteBuffer& buf) : _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage) {}

	// move constructor
	ByteBuffer(ByteBuffer&& buf) : _rpos(buf._rpos), _wpos(buf._wpos), _storage(std::move(buf._storage)) {}

	// move operator
	ByteBuffer& operator=(ByteBuffer&& rhs)
	{
		_rpos = rhs._rpos;
		_wpos = rhs._wpos;
		_storage = std::move(rhs._storage);
		return *this;
	}

	void clear()
	{
		_storage.clear();
		_rpos = _wpos = 0;
	}

	template <typename T> void put(size_t pos, T value)
	{
		EndianConvert(value);
		put(pos, (uint8_t*)&value, sizeof(value));
	}

    ByteBuffer& operator<<(uint8_t value)
    {
        append<uint8_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(uint16_t value)
    {
        append<uint16_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(uint32_t value)
    {
        append<uint32_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(uint64_t value)
    {
        append<uint64_t>(value);
        return *this;
    }

    // signed as in 2e complement
    ByteBuffer& operator<<(int8_t value)
    {
        append<int8_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(int16_t value)
    {
        append<int16_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(int32_t value)
    {
        append<int32_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(int64_t value)
    {
        append<int64_t>(value);
        return *this;
    }

    // floating points
    ByteBuffer& operator<<(float value)
    {
        append<float>(value);
        return *this;
    }

    ByteBuffer& operator<<(double value)
    {
        append<double>(value);
        return *this;
    }

    ByteBuffer& operator<<(const std::string& value)
    {
        append((uint8_t const*)value.c_str(), value.length());
        append((uint8_t)0);
        return *this;
    }

    ByteBuffer& operator<<(const char* str)
    {
        append((uint8_t const*)str, str ? strlen(str) : 0);
        append((uint8_t)0);
        return *this;
    }

    ByteBuffer& operator<<(bool const& val)
    {
        append((uint8_t const*)&val, 1);
        return *this;
    }

    ByteBuffer& operator>>(bool& value)
    {
        value = read<char>() > 0 ? true : false;
        return *this;
    }

    ByteBuffer& operator>>(uint8_t& value)
    {
        value = read<uint8_t>();
        return *this;
    }

    ByteBuffer& operator>>(uint16_t& value)
    {
        value = read<uint16_t>();
        return *this;
    }

    ByteBuffer& operator>>(uint32_t& value)
    {
        value = read<uint32_t>();
        return *this;
    }

    ByteBuffer& operator>>(uint64_t& value)
    {
        value = read<uint64_t>();
        return *this;
    }

    //signed as in 2e complement
    ByteBuffer& operator>>(int8_t& value)
    {
        value = read<int8_t>();
        return *this;
    }

    ByteBuffer& operator>>(int16_t& value)
    {
        value = read<int16_t>();
        return *this;
    }

    ByteBuffer& operator>>(int32_t& value)
    {
        value = read<int32_t>();
        return *this;
    }

    ByteBuffer& operator>>(int64_t& value)
    {
        value = read<int64_t>();
        return *this;
    }

    ByteBuffer& operator>>(float& value)
    {
        value = read<float>();
        return *this;
    }

    ByteBuffer& operator>>(double& value)
    {
        value = read<double>();
        return *this;
    }

    ByteBuffer& operator>>(std::string& value)
    {
        value.clear();
        while (rpos() < size())                         // prevent crash at wrong string format in packet
        {
            char c = read<char>();
            if (c == 0)
                break;
            value += c;
        }
        return *this;
    }

    template<class T>
    ByteBuffer& operator>>(Unused<T> const&)
    {
        read_skip<T>();
        return *this;
    }


    uint8_t operator[](size_t pos) const
    {
        return read<uint8_t>(pos);
    }

    size_t rpos() const { return _rpos; }

    size_t rpos(size_t rpos_)
    {
        _rpos = rpos_;
        return _rpos;
    }

    size_t wpos() const { return _wpos; }

    size_t wpos(size_t wpos_)
    {
        _wpos = wpos_;
        return _wpos;
    }

    template<typename T>
    void read_skip() { read_skip(sizeof(T)); }

    void read_skip(size_t skip)
    {
        if (_rpos + skip > size())
            throw ByteBufferException(false, _rpos, skip, size());
        _rpos += skip;
    }

    void rfinish()
    {
        _rpos = wpos();
    }

    template <typename T> T read()
    {
        T r = read<T>(_rpos);
        _rpos += sizeof(T);
        return r;
    }

    template <typename T> T read(size_t pos) const
    {
        if (pos + sizeof(T) > size())
            throw ByteBufferException(false, pos, sizeof(T), size());
        T val = *((T const*)&_storage[pos]);
        EndianConvert(val);
        return val;
    }

    void read(uint8_t* dest, size_t len)
    {
        if (_rpos + len > size())
            throw ByteBufferException(false, _rpos, len, size());
        memcpy(dest, &_storage[_rpos], len);
        _rpos += len;
    }

    uint64_t readPackGUID()
    {
        uint64_t guid = 0;
        uint8_t guidmark = 0;
        (*this) >> guidmark;

        for (int i = 0; i < 8; ++i)
        {
            if (guidmark & (uint8_t(1) << i))
            {
                uint8_t bit;
                (*this) >> bit;
                guid |= (uint64_t(bit) << (i * 8));
            }
        }

        return guid;
    }

    const uint8_t* contents() const { return &_storage[0]; }

    size_t size() const { return _storage.size(); }
    bool empty() const { return _storage.empty(); }

    void resize(size_t newsize)
    {
        _storage.resize(newsize);
        _rpos = 0;
        _wpos = size();
    }

    void reserve(size_t ressize)
    {
        if (ressize > size())
            _storage.reserve(ressize);
    }

    void append(const std::string& str)
    {
        append((uint8_t const*)str.c_str(), str.size() + 1);
    }

    void append(const std::vector<uint8_t>& src)
    {
        return append(src.data(), src.size());
    }

    void append(const char* src, size_t cnt)
    {
        return append((const uint8_t*)src, cnt);
    }

    template<class T> void append(const T* src, size_t cnt)
    {
        return append((const uint8_t*)src, cnt * sizeof(T));
    }

    void append(const uint8_t* src, size_t cnt)
    {
        if (!cnt)
            return;

        assert(size() < 10000000);

        if (_storage.size() < _wpos + cnt)
            _storage.resize(_wpos + cnt);
        memcpy(&_storage[_wpos], src, cnt);
        _wpos += cnt;
    }

    void append(const ByteBuffer& buffer)
    {
        if (buffer.wpos())
            append(buffer.contents(), buffer.wpos());
    }

    void appendPackGUID(uint64_t guid)
    {
        uint8_t packGUID[8 + 1];
        packGUID[0] = 0;
        size_t size = 1;
        for (uint8_t i = 0; guid != 0; ++i)
        {
            if (guid & 0xFF)
            {
                packGUID[0] |= uint8_t(1 << i);
                packGUID[size] = uint8_t(guid & 0xFF);
                ++size;
            }

            guid >>= 8;
        }

        append(packGUID, size);
    }

    void appendPackXYZ(float x, float y, float z)
    {
        uint32_t packed = 0;
        packed |= ((int)(x / 0.25f) & 0x7FF);
        packed |= ((int)(y / 0.25f) & 0x7FF) << 11;
        packed |= ((int)(z / 0.25f) & 0x3FF) << 22;
        *this << packed;
    }

    void put(size_t pos, const uint8_t* src, size_t cnt)
    {
        if (pos + cnt > size())
            throw ByteBufferException(true, pos, cnt, size());
        memcpy(&_storage[pos], src, cnt);
    }

    void print_storage() const
    {
        //if (!sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))   // optimize disabled debug output
        //    return;

        std::ostringstream ss;
        ss << "STORAGE_SIZE: " << size() << "\n";

        //if (sLog.IsIncludeTime())
            ss << "         ";

        for (size_t i = 0; i < size(); ++i)
            ss << uint32_t(read<uint8_t>(i)) << " - ";

        std::cout << ss.str().c_str() << std::endl;
        //DEBUG_LOG(ss.str().c_str());
    }

    void textlike() const
    {
        //if (!sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))   // optimize disabled debug output
        //    return;

        std::ostringstream ss;
        ss << "STORAGE_SIZE: " << size() << "\n";

        //if (sLog.IsIncludeTime())
            ss << "         ";

        for (size_t i = 0; i < size(); ++i)
            ss << read<uint8_t>(i);

        std::cout << ss.str().c_str() << std::endl;
        //DEBUG_LOG(ss.str().c_str());
    }

    void hexlike() const
    {
        //if (!sLog.HasLogLevelOrHigher(LOG_LVL_DEBUG))   // optimize disabled debug output
        //    return;

        std::ostringstream ss;
        ss << "STORAGE_SIZE: " << size() << "\n";

        //if (sLog.IsIncludeTime())
            ss << "         ";

        size_t j = 1, k = 1;

        for (size_t i = 0; i < size(); ++i)
        {
            if ((i == (j * 8)) && ((i != (k * 16))))
            {
                ss << "| ";
                ++j;
            }
            else if (i == (k * 16))
            {
                ss << "\n";

                //if (sLog.IsIncludeTime())
                    ss << "         ";

                ++k;
                ++j;
            }

            char buf[4];
            snprintf(buf, 4, "%02X", read<uint8_t>(i));
            ss << buf << " ";

        }
        std::cout << ss.str().c_str() << std::endl;
        //DEBUG_LOG(ss.str().c_str());
    }

private:
    // limited for internal use because can "append" any unexpected type (like pointer and etc) with hard detection problem
    template <typename T> void append(T value)
    {
        EndianConvert(value);
        append((uint8_t*)&value, sizeof(T));
    }

protected:
	size_t _rpos, _wpos;	// rpos = position of the read head, wpos = position of the write head
	std::vector<uint8_t> _storage;
};