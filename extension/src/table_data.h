#ifndef MORPHY_TABLE_DATA_CLASS_H
#define MORPHY_TABLE_DATA_CLASS_H

#include "rows.h"

#include <godot_cpp/templates/vector.hpp>

namespace morphy
{

template <typename H, int64_t Z>
class TableData
{
public:
    int64_t num_columns() const
    {
        return headers.size();
    }

    int64_t num_rows() const
    {
        return row_count;
    }

    void add_columns(std::vector<H>&& p_headers)
    {
        for (Rows<Z>& rows : buffers)
        {
            rows.add_columns(p_headers.size());
        }

        headers.reserve(headers.size() + p_headers.size());
        for (const H& header : p_headers)
        {
            headers.push_back(std::move(header));
        }
    }

    void add_columns(const std::vector<H>& p_headers)
    {
        for (Rows<Z>& rows : buffers)
            rows.add_columns(p_headers.size());

        headers.reserve(headers.size() + p_headers.size());
        for (const H& header : p_headers)
            headers.push_back(header);
    }

    void add_rows(int64_t num)
    {
        row_count += num;
        if (buffers.size() * Z < row_count)
        {
            int64_t oldSize = buffers.size();
            buffers.resize((row_count * 2 + Z) / Z);

            for (int64_t i = oldSize; i < buffers.size(); ++i)
                buffers[i].add_columns(headers.size());
        }
    }

    void set_cell(int64_t column, int64_t row, const godot::Variant& value)
    {
        ERR_FAIL_COND(headers.size() <= column);
        ERR_FAIL_COND(buffers.size() * Z <= row);

        buffers[row / Z].set_cell(column, row % Z, value);
    }

    const godot::Variant& get_cell(int64_t column, int64_t row) const
    {
        static const godot::Variant nil;
        ERR_FAIL_COND_V(headers.size() <= column, nil);
        ERR_FAIL_COND_V(buffers.size() * Z <= row, nil);

        return buffers[row / Z].get_cell(column, row % Z);
    }

private:
    std::vector<H> headers;
    std::vector<Rows<Z>> buffers;
    int64_t row_count = 0;
};

} // namespace morphy

#endif // MORPHY_TABLE_DATA_CLASS_H
