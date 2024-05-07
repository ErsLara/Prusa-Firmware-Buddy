#include "string_view_utf8.hpp"

string_view_utf8::Length string_view_utf8::computeNumUtf8Chars() const {
    if (utf8Length >= 0) {
        return utf8Length;
    }

    Length r = 0;
    StringReaderUtf8 reader(*this);
    while (reader.getUtf8Char()) {
        ++r;
    }

    utf8Length = r;
    return r;
}

unichar string_view_utf8::getFirstUtf8Char() const {
    StringReaderUtf8 reader(*this);
    return reader.getUtf8Char();
}

string_view_utf8 string_view_utf8::substr(size_t pos) const {
    StringReaderUtf8 reader(*this);
    while (pos--) {
        reader.getUtf8Char();
    }

    return reader.remaining_string();
}

size_t string_view_utf8::copyToRAM(char *dst, size_t max_size) const {
    StringReaderUtf8 reader(*this);

    size_t bytesCopied = 0;
    for (size_t i = 0; i < max_size; ++i) {
        *dst = reader.getbyte();
        if (*dst == 0) {
            return bytesCopied;
        }
        ++dst;
        ++bytesCopied;
    }
    *dst = 0; // safety termination in case of reaching the end of the buffer
    return bytesCopied;
}

unichar StringReaderUtf8::getUtf8Char() {
    if (last_read_byte_ == 0xff) { // in case we don't have any character from the last run, get a new one from the input stream
        last_read_byte_ = getbyte();
    }
    unichar ord = last_read_byte_;
    if (!UTF8_IS_NONASCII(ord)) {
        last_read_byte_ = 0xff; // consumed, not available for next run
        return ord;
    }
    ord &= 0x7F;
    for (unichar mask = 0x40; ord & mask; mask >>= 1) {
        ord &= ~mask;
    }
    last_read_byte_ = getbyte();
    while (UTF8_IS_CONT(last_read_byte_)) {
        ord = (ord << 6) | (last_read_byte_ & 0x3F);
        last_read_byte_ = getbyte();
    }
    return ord;
}

uint8_t StringReaderUtf8::getbyte() {
    switch (view_.type) {

    case EType::CPUFLASH:
    case EType::RAM:
        return *view_.cpuflash.utf8raw++; // beware - expecting, that the input string is null-terminated! No other checks are done

    case EType::FILE:
        return FILE_getbyte();

    default:
        return 0;
    }
}

uint8_t StringReaderUtf8::FILE_getbyte() {
    if (!view_.file.f) {
        return '\0';
    }
    uint8_t c;
    // sync among multiple reads from the sameMO file
    if (ftell(view_.file.f) != static_cast<long>(view_.file.offset)) {
        if (fseek(view_.file.f, view_.file.offset, SEEK_SET) != 0) {
            return '\0';
        }
    }

    view_.file.offset++;
    if (fread(&c, 1, 1, view_.file.f) != 1) {
        return '\0';
    }
    return c;
}