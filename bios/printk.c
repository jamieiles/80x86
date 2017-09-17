#include <stddef.h>
#include <stdarg.h>

#include "serial.h"

struct string_specifier {
    int precision;
    int zero_pad;
};

struct string_formatter {
    char *buf;
    size_t len;
    size_t maxlen;
    const char *fmt;
    struct string_specifier specifier;
};

void output_char(struct string_formatter *formatter, int c)
{
    if (formatter->len < formatter->maxlen - 1) {
        *formatter->buf++ = c;
        ++formatter->len;
    }
}

void output_buf_reversed(struct string_formatter *formatter,
                         const char *buf,
                         int nchars)
{
    const char hex_val[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    for (nchars = nchars - 1; nchars >= 0; --nchars) {
        unsigned digit = buf[nchars];
        output_char(formatter, hex_val[digit]);
    }
}

void output_zero_padding(struct string_formatter *formatter, int num_zeroes)
{
    int i;

    if (formatter->specifier.zero_pad && num_zeroes > 0)
        for (i = 0; i < num_zeroes; ++i)
            output_char(formatter, '0');
}

void output_unsigned(struct string_formatter *formatter,
                     unsigned val,
                     unsigned base)
{
    char reversal_buf[32];
    int nchars = 0;

    do {
        reversal_buf[nchars++] = val % base;
        val /= base;
    } while (val);

    output_zero_padding(formatter, formatter->specifier.precision - nchars);
    output_buf_reversed(formatter, reversal_buf, nchars);
}

void output_string(struct string_formatter *formatter, const char *str)
{
    while (*str)
        output_char(formatter, *str++);
}

void output_pointer(struct string_formatter *formatter, const void *ptr)
{
    unsigned val = (unsigned)ptr;

    formatter->specifier.precision = 4;
    formatter->specifier.zero_pad = 1;

    if (!val) {
        output_string(formatter, "(null)");
    } else {
        output_string(formatter, "0x");
        output_unsigned(formatter, val, 16);
    }
}

void output_item(struct string_formatter *formatter, va_list *ap)
{
    switch (*formatter->fmt) {
    case 's': output_string(formatter, va_arg(*ap, const char *)); break;
    case 'u': output_unsigned(formatter, va_arg(*ap, unsigned), 10); break;
    case 'x': output_unsigned(formatter, va_arg(*ap, unsigned), 16); break;
    case 'p': output_pointer(formatter, va_arg(*ap, const void *)); break;
    case 'c':
    default: output_char(formatter, va_arg(*ap, int)); break;
    }

    ++formatter->fmt;
}

void read_precision(struct string_formatter *formatter)
{
    formatter->specifier.precision = 0;

    while (*formatter->fmt >= '0' && *formatter->fmt <= '9') {
        formatter->specifier.precision *= 10;
        formatter->specifier.precision += *formatter->fmt - '0';
        formatter->fmt++;
    }
}

void read_zero_pad(struct string_formatter *formatter)
{
    formatter->specifier.zero_pad = 0;

    if (*formatter->fmt == '0') {
        formatter->specifier.zero_pad = 1;
        formatter->fmt++;
    }
}

void parse_specifier(struct string_formatter *formatter)
{
    formatter->fmt++;

    read_zero_pad(formatter);
    read_precision(formatter);
}

size_t string_format(char *buf, size_t maxlen, const char *fmt, va_list *ap)
{
    struct string_formatter formatter = {
        .buf = buf, .maxlen = maxlen, .fmt = fmt, .len = 0,
    };

    while (formatter.len < maxlen - 1 && *formatter.fmt) {
        if (*formatter.fmt == '%') {
            parse_specifier(&formatter);
            output_item(&formatter, ap);
        } else {
            output_char(&formatter, *formatter.fmt++);
        }
    }

    *formatter.buf = '\0';

    return formatter.len;
}

void printk(const char *fmt, ...)
{
    static char printk_buf[128];
    va_list ap;

    va_start(ap, fmt);
    string_format(printk_buf, sizeof(printk_buf), fmt, &ap);
    va_end(ap);

    putstr(printk_buf);
}
